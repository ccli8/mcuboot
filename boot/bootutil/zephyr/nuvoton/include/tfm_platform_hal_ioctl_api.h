/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 Nuvoton Technology Corporation
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __TFM_PLATFORM_HAL_IOCTL_API_H__
#define __TFM_PLATFORM_HAL_IOCTL_API_H__

#include "tfm_platform_api.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Helper to define Nuvoton specific Platform IOCTL request code
 */
#define NVT_TFM_PLAT_IOCTL_REQ(FUNC) NVT_TFM_PLAT_IOCTL_REQ_##FUNC

/*!
 * \brief Helper to define Nuvoton specific Platform IOCTL client function name
 * for Non-Secure
 */
#define NVT_TFM_PLAT_IOCTL_NS(FUNC) NVT_TFM_PLAT_IOCTL_NS_##FUNC

/*!
 * \enum nvt_tfm_platform_ioctl_req_t
 *
 * \brief Nuvoton platform specific IOCTL request code
 */
enum nvt_tfm_platform_ioctl_req_t {
    /* BSP SYS driver API */
    NVT_TFM_PLAT_IOCTL_REQ(SYS_LockReg),
    NVT_TFM_PLAT_IOCTL_REQ(SYS_UnlockReg),
    NVT_TFM_PLAT_IOCTL_REQ(SYS_ResetModule_Assert),
    NVT_TFM_PLAT_IOCTL_REQ(SYS_ResetModule_Deassert),
    NVT_TFM_PLAT_IOCTL_REQ(SYS_ResetModule_IsAsserted),
    NVT_TFM_PLAT_IOCTL_REQ(SYS_GPx_MFPx_Read),
    NVT_TFM_PLAT_IOCTL_REQ(SYS_GPx_MFPx_Write),
    NVT_TFM_PLAT_IOCTL_REQ(SYS_GPx_MFOSx_Read),
    NVT_TFM_PLAT_IOCTL_REQ(SYS_GPx_MFOSx_Write),
    NVT_TFM_PLAT_IOCTL_REQ(SYS_USBPHY_Read),
    NVT_TFM_PLAT_IOCTL_REQ(SYS_USBPHY_Write),

    /* BSP CLK driver API */
    NVT_TFM_PLAT_IOCTL_REQ(CLK_SetModuleClock),
    NVT_TFM_PLAT_IOCTL_REQ(CLK_EnableModuleClock),
    NVT_TFM_PLAT_IOCTL_REQ(CLK_DisableModuleClock),

    /* BSP FMC driver API */
    NVT_TFM_PLAT_IOCTL_REQ(FMC_Open),
    NVT_TFM_PLAT_IOCTL_REQ(FMC_Close),
    NVT_TFM_PLAT_IOCTL_REQ(FMC_ENABLE_AP_UPDATE),
    NVT_TFM_PLAT_IOCTL_REQ(FMC_DISABLE_AP_UPDATE),

    /* Max request code, plays as number of valid request code */
    NVT_TFM_PLAT_IOCTL_REQ(MAX),

    /* Following entry is only to ensure the error code of int32_t size */
    NVT_TFM_PLAT_IOCTL_REQ(INT32_SIZE) = INT32_MAX
};

void NVT_TFM_PLAT_IOCTL_NS(SYS_LockReg)();
void NVT_TFM_PLAT_IOCTL_NS(SYS_UnlockReg)();
void NVT_TFM_PLAT_IOCTL_NS(SYS_ResetModule_Assert)(uint32_t u32ModuleIndex);
void NVT_TFM_PLAT_IOCTL_NS(SYS_ResetModule_Deassert)(uint32_t u32ModuleIndex);
bool NVT_TFM_PLAT_IOCTL_NS(SYS_ResetModule_IsAsserted)(uint32_t u32ModuleIndex);
uint32_t NVT_TFM_PLAT_IOCTL_NS(SYS_GPx_MFPx_Read)(uint32_t reg);
void NVT_TFM_PLAT_IOCTL_NS(SYS_GPx_MFPx_Write)(uint32_t reg, uint32_t reg_val);
uint32_t NVT_TFM_PLAT_IOCTL_NS(SYS_GPx_MFOSx_Read)(uint32_t reg);
void NVT_TFM_PLAT_IOCTL_NS(SYS_GPx_MFOSx_Write)(uint32_t reg, uint32_t reg_val);
uint32_t NVT_TFM_PLAT_IOCTL_NS(SYS_USBPHY_Read)(uint32_t reg_addr);
void NVT_TFM_PLAT_IOCTL_NS(SYS_USBPHY_Write)(uint32_t reg_addr,
                                             uint32_t reg_val);
void NVT_TFM_PLAT_IOCTL_NS(CLK_SetModuleClock)(uint32_t u32ModuleIndex,
                                               uint32_t u32ClkSrc,
                                               uint32_t u32ClkDiv);
void NVT_TFM_PLAT_IOCTL_NS(CLK_EnableModuleClock)(uint32_t u32ModuleIndex);
void NVT_TFM_PLAT_IOCTL_NS(CLK_DisableModuleClock)(uint32_t u32ModuleIndex);
void NVT_TFM_PLAT_IOCTL_NS(FMC_Open)();
void NVT_TFM_PLAT_IOCTL_NS(FMC_Close)();
void NVT_TFM_PLAT_IOCTL_NS(FMC_ENABLE_AP_UPDATE)();
void NVT_TFM_PLAT_IOCTL_NS(FMC_DISABLE_AP_UPDATE)();

#ifdef __cplusplus
}
#endif

#endif /* __TFM_PLATFORM_HAL_IOCTL_API_H__ */
