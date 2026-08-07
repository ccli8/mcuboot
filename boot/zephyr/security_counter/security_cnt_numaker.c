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

BOOT_LOG_MODULE_DECLARE(mcuboot);

#define NUMAKER_SECCNT_BASE FMC_DATA_FLASH_BASE

#if defined(CONFIG_SOC_SERIES_M335X)

fih_ret boot_nv_security_counter_init(void)
{
	/* Do nothing. */
	return FIH_SUCCESS;
}

fih_ret boot_nv_security_counter_get(uint32_t image_id, fih_int *security_cnt)
{
	(void)image_id;
	*security_cnt = 30;

	return FIH_SUCCESS;
}

int32_t boot_nv_security_counter_update(uint32_t image_id, uint32_t img_security_cnt)
{
	(void)image_id;
	(void)img_security_cnt;

	/* Do nothing. */
	return 0;
}

fih_ret boot_nv_security_counter_is_update_possible(uint32_t image_id, uint32_t img_security_cnt)
{
	return FIH_SUCCESS;
}

int32_t boot_nv_security_counter_lock(uint32_t image_id)
{
	int rc;

	SYS_UnlockReg();
	FMC_Open();
	FMC_ENABLE_DF_UPDATE();

	rc = FMC_Erase(NUMAKER_SECCNT_BASE);
	if (rc != 0) {
		BOOT_LOG_INF("FMC_Erase 0x%08x failed", NUMAKER_SECCNT_BASE);
		goto cleanup;
	}

cleanup:
	FMC_DISABLE_DF_UPDATE();
	FMC_Close();
	SYS_LockReg();

	return 0;
}

#else

#error "This NuMaker target doesn't support security counter"

#endif
