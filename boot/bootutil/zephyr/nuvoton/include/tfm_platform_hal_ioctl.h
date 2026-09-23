/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 Nuvoton Technology Corporation
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __TFM_PLATFORM_HAL_IOCTL_H__
#define __TFM_PLATFORM_HAL_IOCTL_H__

#include "tfm_platform_api.h"
#include "tfm_platform_hal_ioctl_api.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Helper to define Nuvoton specific Platform IOCTL service function name
 */
#define NVT_TFM_PLAT_IOCTL_SRV(FUNC) NVT_TFM_PLAT_IOCTL_SRV_##FUNC

/*!
 * \brief Helper to declare Nuvoton specific Platform IOCTL service function
 */
#define NVT_TFM_PLAT_IOCTL_SRV_DECL(FUNC)                                      \
    NVT_TFM_PLAT_IOCTL_SRV_DECL_(NVT_TFM_PLAT_IOCTL_SRV(FUNC))
#define NVT_TFM_PLAT_IOCTL_SRV_DECL_(FUNC)                                     \
    enum tfm_platform_err_t FUNC(struct psa_invec *in_vec,                     \
                                 struct psa_outvec *out_vec)

NVT_TFM_PLAT_IOCTL_SRV_DECL(SYS_LockReg);
NVT_TFM_PLAT_IOCTL_SRV_DECL(SYS_UnlockReg);
NVT_TFM_PLAT_IOCTL_SRV_DECL(SYS_ResetModule_Assert);
NVT_TFM_PLAT_IOCTL_SRV_DECL(SYS_ResetModule_Deassert);
NVT_TFM_PLAT_IOCTL_SRV_DECL(SYS_ResetModule_IsAsserted);
NVT_TFM_PLAT_IOCTL_SRV_DECL(SYS_GPx_MFPx_Read);
NVT_TFM_PLAT_IOCTL_SRV_DECL(SYS_GPx_MFPx_Write);
NVT_TFM_PLAT_IOCTL_SRV_DECL(SYS_GPx_MFOSx_Read);
NVT_TFM_PLAT_IOCTL_SRV_DECL(SYS_GPx_MFOSx_Write);
NVT_TFM_PLAT_IOCTL_SRV_DECL(SYS_USBPHY_Read);
NVT_TFM_PLAT_IOCTL_SRV_DECL(SYS_USBPHY_Write);
NVT_TFM_PLAT_IOCTL_SRV_DECL(CLK_SetModuleClock);
NVT_TFM_PLAT_IOCTL_SRV_DECL(CLK_EnableModuleClock);
NVT_TFM_PLAT_IOCTL_SRV_DECL(CLK_DisableModuleClock);
NVT_TFM_PLAT_IOCTL_SRV_DECL(FMC_Open);
NVT_TFM_PLAT_IOCTL_SRV_DECL(FMC_Close);
NVT_TFM_PLAT_IOCTL_SRV_DECL(FMC_ENABLE_AP_UPDATE);
NVT_TFM_PLAT_IOCTL_SRV_DECL(FMC_DISABLE_AP_UPDATE);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_PLATFORM_HAL_IOCTL_H__ */
