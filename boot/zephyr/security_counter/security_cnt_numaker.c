/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 Nuvoton Technology Corporation
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bootutil/security_cnt.h"
#include <stdint.h>

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
	return 0;
}

#endif
