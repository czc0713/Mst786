
/******************************************************************************
 *-----------------------------------------------------------------------------
 *
 *    Copyright (c) 2011 MStar Semiconductor, Inc.  All rights reserved.
 *
 *-----------------------------------------------------------------------------
 * FILE NAME      mdrv_gop.h
 * DESCRIPTION
 *          Including some MACRO needed in msb250xfb.c
 *                    Defined CLRREG16(), SETREG16()
 *          (refer to include/asm-arm/arch-msb25xx/io.h)
 *                    Defined used MASK and setting value
 *                    Defined CHIPTOP, OP2, VOP, GOP Base Addres in Physical Address
 *                    and theirs offset    (drvop2.h and drvgop.h)
 *                    Declared Boot_splash array. the image content shown in boot
 *          (boot_splasy.h)
 *            refering sources list
 *                    drvop2.h and drvgop.h
 *              boot_splasy.h
 *
 * AUTHOR         Chun Fan
 *
 * HISTORY
 *                2008/05/06    Chun    initial version
 *                2008/10/02    Chun    add MSB251x code, add CONFIG_ARCH_MSB251X
 *                                      add some IP base address Macro
 *                                      BK_LPLL_BASE, BK_DAC_BASE,
 *                                      and OP2_DITH_REG_BASE
 *
 *        linux/drivers/video/msb250xfb.h    --    msb250xfb frame buffer device
 ******************************************************************************/

#include "hal/gop_c2_reg.h"
#include "asm/arch/mach/ms_types.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"


#define CHIPTOP_REG_BASE_PA  (0x101E00)
#define SC_REG_BASE_PA       (0x130000)
#define LPLL_REG_BASE_PA     (0x103100)
#define CLKG0_REG_BASE_PA    (0x100B00)
#define MOD_REG_BASE_PA      (0x103200)
#define GPIO0_REG_BASE_PA    (0x102B00)
#define GPIO1_REG_BASE_PA    (0x101A00)

#define GOP_DEBUG 0
#if (GOP_DEBUG==1)
#define GOPDBG(fmt, arg...) printf(fmt, ##arg)
#else
#define GOPDBG(fmt, arg...)
#endif

int mdrvgop_Set_Starting_address(struct fb_var_screeninfo *var, struct fb_info *info);
void GOP_Setmode(int Screen, int Panel_Width, int Panel_Height, U32 FB_Start_Addr);
void GOP_Pan_Display(U32  FB_Start_Addr );




