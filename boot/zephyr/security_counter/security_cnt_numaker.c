/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 Nuvoton Technology Corporation
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bootutil/security_cnt.h"
#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/linker/linker-defs.h>
#include <soc.h>

#include "bootutil/bootutil_log.h"
#include "bootutil/bootutil.h"
#include "bootutil/fault_injection_hardening.h"

BOOT_LOG_MODULE_DECLARE(mcuboot);

#if defined(CONFIG_SOC_SERIES_M335X)

#define NUMAKER_SECCNT_MAXIMG          16
#define NUMAKER_SECCNT_DFWPROT_BLK_IDX 0
#define NUMAKER_SECCNT_DFWPROT_BLK_MSK BIT(NUMAKER_SECCNT_DFWPROT_BLK_IDX)
#define NUMAKER_SECCNT_BASE                                                                        \
	(FMC_DATA_FLASH_BASE + FMC_DFWPROT_BLK_SIZE * NUMAKER_SECCNT_DFWPROT_BLK_IDX)
#define NUMAKER_SECCNT_MAXNUM INT_MAX

static uint32_t img_security_cnt_arr[NUMAKER_SECCNT_MAXIMG];

/* FIXME: Rewrite FMC_IS_DFWPROT_LCOKED and FMC_LOCK_DFWPROT for
 * multi-block support.
 */

fih_ret boot_nv_security_counter_init(void)
{
	/* Do nothing. */
	return FIH_SUCCESS;
}

fih_ret boot_nv_security_counter_get(uint32_t image_id, fih_int *security_cnt)
{
	uint32_t img_security_cnt_addr;
	uint32_t img_security_cnt_nv;

	if (image_id >= NUMAKER_SECCNT_MAXIMG) {
		BOOT_LOG_ERR("image_id(%d) exceeds max support(%d)", image_id,
			     NUMAKER_SECCNT_MAXIMG);
		return FIH_FAILURE;
	}

	SYS_UnlockReg();
	FMC_Open();
	FMC_ENABLE_DF_UPDATE();

	img_security_cnt_addr = NUMAKER_SECCNT_BASE + sizeof(uint32_t) * image_id;
	img_security_cnt_nv = FMC_Read(img_security_cnt_addr);

	/* Security counter can be invalid without provision. Regard it
	 * as zero for easy development.
	 */
	if (img_security_cnt_nv > NUMAKER_SECCNT_MAXNUM) {
		img_security_cnt_nv = 0;
	}

	FMC_DISABLE_DF_UPDATE();
	/* No paired FMC_Close
	 *
	 * NuMaker implementation of zephyr flash driver relies on flash
	 * device being kept open, or it will break with FMC_Close invoked
	 * here.
	 */
	SYS_LockReg();

	*security_cnt = fih_int_encode(img_security_cnt_nv);

	return FIH_SUCCESS;
}

int32_t boot_nv_security_counter_update(uint32_t image_id, uint32_t img_security_cnt)
{
	fih_int security_cnt;
	fih_ret rc_fih;
	uint32_t dfwpkeep;
	int i;
	uint32_t img_security_cnt_addr;
	uint32_t *img_security_cnt_pos;
	uint32_t img_security_cnt_old;
	int rc;

	rc_fih = boot_nv_security_counter_get(image_id, &security_cnt);
	if (FIH_NOT_EQ(rc_fih, FIH_SUCCESS)) {
		return -1;
	}

	img_security_cnt_old = (uint32_t)fih_int_decode(security_cnt);
	if (img_security_cnt == img_security_cnt_old) {
		BOOT_LOG_INF("Needn't update image(%d) security counter 0x%08x for being unchanged",
			     image_id, img_security_cnt);
		return 0;
	}

	SYS_UnlockReg();
	FMC_Open();
	FMC_ENABLE_DF_UPDATE();

	dfwpkeep = FMC->DFWPKEEP;
	switch (NUMAKER_SECCNT_DFWPROT_BLK_IDX) {
	case 0:
		dfwpkeep &= 0xffff0000;
		dfwpkeep |= 0xffff;
		break;
	case 1:
		dfwpkeep &= 0xffff;
		dfwpkeep |= 0xffff0000;
		break;
	default:
		__ASSERT_NO_MSG(false);
	}
	FMC->DFWPKEEP = dfwpkeep;

	if (FMC_IS_DFWPROT_LCOKED()) {
		BOOT_LOG_ERR("DFWPROT should be unlocked after reboot. This requires cold reset or "
			     "chip reset.");
		rc = -1;
		goto cleanup;
	}

	FMC_DISABLE_DFWPROT(NUMAKER_SECCNT_DFWPROT_BLK_MSK);
	if (FMC_IS_DFWPROT(NUMAKER_SECCNT_DFWPROT_BLK_MSK)) {
		BOOT_LOG_ERR("Disable DFWPROT failed");
		rc = -1;
		goto cleanup;
	}

	img_security_cnt_addr = NUMAKER_SECCNT_BASE;
	img_security_cnt_pos = img_security_cnt_arr;
	for (i = 0; i < NUMAKER_SECCNT_MAXIMG; i++) {
		*img_security_cnt_pos = FMC_Read(img_security_cnt_addr);

		img_security_cnt_addr += sizeof(uint32_t);
		img_security_cnt_pos++;
	}

	__ASSERT_NO_MSG(image_id < NUMAKER_SECCNT_MAXIMG);
	img_security_cnt_old = img_security_cnt_arr[image_id];
	img_security_cnt_arr[image_id] = img_security_cnt;

	rc = FMC_Erase(NUMAKER_SECCNT_BASE);
	if (rc != 0) {
		BOOT_LOG_ERR("FMC_Erase 0x%08lx failed", NUMAKER_SECCNT_BASE);
		rc = -1;
		goto cleanup;
	}

	img_security_cnt_addr = NUMAKER_SECCNT_BASE;
	img_security_cnt_pos = img_security_cnt_arr;
	for (i = 0; i < NUMAKER_SECCNT_MAXIMG; i += 2) {
		rc = FMC_Write8Bytes(img_security_cnt_addr, *img_security_cnt_pos,
				     *(img_security_cnt_pos + 1));
		if (rc != 0) {
			BOOT_LOG_ERR("FMC_Write8Bytes 0x%08x failed", img_security_cnt_addr);
			rc = -1;
			goto cleanup;
		}

		img_security_cnt_addr += sizeof(uint32_t) * 2;
		img_security_cnt_pos += 2;
	}

	rc = 0;
	BOOT_LOG_INF("Updated image(%d) security counter from 0x%08x to 0x%08x", image_id,
		     img_security_cnt_old, img_security_cnt);

cleanup:
	FMC_DISABLE_DF_UPDATE();
	/* No paired FMC_Close (see above) */
	SYS_LockReg();

	return rc;
}

fih_ret boot_nv_security_counter_is_update_possible(uint32_t image_id, uint32_t img_security_cnt)
{
	fih_int security_cnt;
	fih_ret rc_fih;

	rc_fih = boot_nv_security_counter_get(image_id, &security_cnt);
	if (FIH_NOT_EQ(rc_fih, FIH_SUCCESS)) {
		return rc_fih;
	}

	rc_fih = fih_ret_encode_zero_equality(img_security_cnt <
					      (uint32_t)fih_int_decode(security_cnt));
	if (FIH_NOT_EQ(rc_fih, FIH_SUCCESS)) {
		return rc_fih;
	}

	return FIH_SUCCESS;
}

int32_t boot_nv_security_counter_lock(uint32_t image_id)
{
	int rc;

	if (image_id >= NUMAKER_SECCNT_MAXIMG) {
		BOOT_LOG_ERR("image_id(%d) exceeds max support(%d)", image_id,
			     NUMAKER_SECCNT_MAXIMG);
		return -1;
	}

	SYS_UnlockReg();
	FMC_Open();
	FMC_ENABLE_DF_UPDATE();

	FMC_ENABLE_DFWPROT(NUMAKER_SECCNT_DFWPROT_BLK_MSK);
	if (!FMC_IS_DFWPROT(NUMAKER_SECCNT_DFWPROT_BLK_MSK)) {
		BOOT_LOG_ERR("Enable DFWPROT failed");
		rc = -1;
		goto cleanup;
	}

	FMC_LOCK_DFWPROT();
	if (!FMC_IS_DFWPROT_LCOKED()) {
		BOOT_LOG_ERR("Lock DFWPROT failed");
		rc = -1;
		goto cleanup;
	}

	rc = 0;
	BOOT_LOG_INF("Enabled DFWPROT to protect security counter");

cleanup:
	FMC_DISABLE_DF_UPDATE();
	/* No paired FMC_Close (see above) */
	SYS_LockReg();

	return rc;
}

#else

#error "This NuMaker target doesn't support security counter"

#endif
