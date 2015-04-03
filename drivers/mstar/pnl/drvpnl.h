////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   drvPNL.h
/// @brief  Panel Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_PNL_H_
#define _DRV_PNL_H_
#include "asm/arch/mach/ms_types.h"
#include "asm/arch/mach/platform.h"
//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define PNL_LIB_VERSION         "000.000.000.000"

// OS related
#define PNL_LOG_DBG     0

// Debug related


//#if PNL_LOG_DBG
//#define PNL_DBG(_dbgSwitch_, _fmt, _args...)      { if((_dbgSwitch_ & _u16PnlDbgSwitch) != 0) printf("PNL:"_fmt, ##_args); }
//#define PNL_ASSERT(_cnd, _fmt, _args...)                                                                            \
//                                    if (!(_cnd)) {                                                                  \
//                                        MS_DEBUG_MSG(printf("PNL ASSERT: %s %d:"_fmt, __FILE__, __LINE__, ##_args));              \
//                                        /* while(1); */                                                             \
//                                    }
//#else
#define PNL_DBG(_dbgSwitch_, _fmt, _args...)      {}
#define PNL_ASSERT(_cnd, _fmt, _args, ...)

//#endif




//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_PNL_FAIL = 0,
    E_PNL_OK = 1,
    E_PNL_GET_BASEADDR_FAIL,            ///< get base address failed when initialize panel driver
    E_PNL_OBTAIN_MUTEX_FAIL,            ///< obtain mutex timeout when calling this function
} PNL_Result;

typedef enum
{
    E_DRVPNL_GAMMA_10BIT         = 1,
    E_DRVPNL_GAMMA_12BIT         = 2,
} DRVPNL_GAMMA_TYPE;

typedef enum
{
    E_DRVPNL_GAMMA_8BIT_MAPPING  = 1,      ///< mapping 1024 to 256 gamma entries
    E_DRVPNL_GAMMA_10BIT_MAPPING = 2,         ///< mapping 1024 to 1024 gamma entries
} DRVPNL_GAMMA_MAPPEING_MODE;

/// Define Panel MISC control index
/// please enum use BIT0 = 0x01, BIT1 = 0x02, BIT2 = 0x04, BIT3 = 0x08, BIT4 = 0x10,
typedef enum
{
    E_DRVPNL_MISC_MFC_ENABLE = 0x0001,

    E_DRVPNL_MISC_MFC_MCP    = 0x0010,
    E_DRVPNL_MISC_MFC_ABChannel = 0x0020,
    E_DRVPNL_MISC_MFC_ACChannel = 0x0040,

} DRVPNL_MISC;

typedef struct
{
    U8 eSupportGammaType;    ///< refer to DRVPNL_GAMMA_TYPE
    U8 eSupportGammaMapLv;   ///< refero to DRVPNL_GAMMA_MAPPEING_LELVE
} PNL_DrvInfo;

typedef struct
{
    U8 bPanel_Initialized;     ///< panel initialized or not
    U8 bPanel_Enabled;         ///< panel enabled or not, if enabled, you can see OSD/Video
} PNL_DrvStatus;

#define PNL_DBGLEVEL_OFF        (0x0000)            ///< turn off debug message, this is default setting
#define PNL_DBGLEVEL_INIT       (0x0001)            ///< Initial function
#define PNL_DBGLEVEL_PANEL_EN   (0x0002)            ///< panel enable function
#define PNL_DBGLEVEL_SSC        (0x0004)            ///< panel SSC setting
#define PNL_DBGLEVEL_GAMMA      (0x0008)            ///< gamma table setting

typedef enum
{
    E_PNL_TYPE_TTL,                      // 0
    E_PNL_TYPE_LVDS,                     // 1
    E_PNL_TYPE_RSDS,                     // 2
    E_PNL_TYPE_MINILVDS,                 // 3 <--  E_XC_PNL_LPLL_MINILVDS_6P_2L,
    E_PNL_TYPE_ANALOG_MINILVDS,          // 4
    E_PNL_TYPE_DIGITAL_MINILVDS,         // 5
    E_PNL_TYPE_MFC,                      // 6
    E_PNL_TYPE_DAC_I,                    // 7
    E_PNL_TYPE_DAC_P,                    // 8
    E_PNL_TYPE_PDPLVDS,                  // 9 ///< For PDP(Vsync use Manually MODE)

    E_PNL_LPLL_EPI34_8P,                 // 10
    E_PNL_LPLL_EPI28_8P,                 // 11
    E_PNL_LPLL_EPI34_6P,                 // 12
    E_PNL_LPLL_EPI28_6P,                 // 13

//  E_PNL_LPLL_MINILVDS_6P_2L,                   ///< replace this with E_PNL_TYPE_MINILVDS
    E_PNL_LPLL_MINILVDS_5P_2L,           // 14
    E_PNL_LPLL_MINILVDS_4P_2L,           // 15
    E_PNL_LPLL_MINILVDS_3P_2L,           // 16
    E_PNL_LPLL_MINILVDS_6P_1L,           // 17
    E_PNL_LPLL_MINILVDS_5P_1L,           // 18
    E_PNL_LPLL_MINILVDS_4P_1L,           // 19
    E_PNL_LPLL_MINILVDS_3P_1L,           // 20

    E_PNL_TYPE_HF_LVDS,                  // 21
    E_PNL_TYPE_HS_LVDS,                  // 22

    E_PNL_TYPE_TTL_TCON,                 // 23
    E_PNL_LPLL_MINILVDS_2CH_3P_8BIT,     // 24 // 2 channel, 3 pari, 8 bits
    E_PNL_LPLL_MINILVDS_2CH_4P_8BIT,     // 25 // 2 channel, 4 pari, 8 bits
    E_PNL_LPLL_MINILVDS_2CH_5P_8BIT,    // 2 channel, 5 pari, 8 bits
    E_PNL_LPLL_MINILVDS_2CH_6P_8BIT,    // 2 channel, 6 pari, 8 bits

    E_PNL_LPLL_MINILVDS_1CH_3P_8BIT,    // 1 channel, 3 pair, 8 bits
    E_PNL_LPLL_MINILVDS_1CH_4P_8BIT,    // 1 channel, 4 pair, 8 bits
    E_PNL_LPLL_MINILVDS_1CH_5P_8BIT,    // 1 channel, 5 pair, 8 bits
    E_PNL_LPLL_MINILVDS_1CH_6P_8BIT,    // 1 channel, 6 pair, 8 bits

    E_PNL_LPLL_MINILVDS_2CH_3P_6BIT,    // 2 channel, 3 pari, 6 bits
    E_PNL_LPLL_MINILVDS_2CH_4P_6BIT,    // 2 channel, 4 pari, 6 bits
    E_PNL_LPLL_MINILVDS_2CH_5P_6BIT,    // 2 channel, 5 pari, 6 bits
    E_PNL_LPLL_MINILVDS_2CH_6P_6BIT,    // 2 channel, 6 pari, 6 bits

    E_PNL_LPLL_MINILVDS_1CH_3P_6BIT,    // 1 channel, 3 pair, 6 bits
    E_PNL_LPLL_MINILVDS_1CH_4P_6BIT,    // 1 channel, 4 pair, 6 bits
    E_PNL_LPLL_MINILVDS_1CH_5P_6BIT,    // 1 channel, 5 pair, 6 bits
    E_PNL_LPLL_MINILVDS_1CH_6P_6BIT,    // 1 channel, 6 pair, 6 bits
} PNL_TYPE;

typedef enum
{
    E_PNL_MODE_SINGLE = 0,          ///< single channel
    E_PNL_MODE_DUAL = 1,            ///< dual channel

    E_PNL_MODE_QUAD = 2,            ///< quad channel
    E_PNL_MODE_QUAD_LR = 3,            ///< quad channel

} PNL_MODE;

typedef enum
{
    E_PNL_OUTPUT_NO_OUTPUT = 0,     ///< even called g_IPanel.Enable(TRUE), still no physical output
    E_PNL_OUTPUT_CLK_ONLY,          ///< after called g_IPanel.Enable(TRUE), will output clock only
    E_PNL_OUTPUT_DATA_ONLY,         ///< after called g_IPanel.Enable(TRUE), will output data only
    E_PNL_OUTPUT_CLK_DATA,          ///< after called g_IPanel.Enable(TRUE), will output clock and data
    E_PNL_OUTPUT_MAX,          ///< after called g_IPanel.Enable(TRUE), will output clock and data
} PNL_OUTPUT_MODE;

typedef struct
{
    // Output timing
    U16 u16HStart;               ///< DE H start
    U16 u16VStart;               ///< DE V start
    U16 u16Width;                ///< DE H width
    U16 u16Height;               ///< DE V height
    U16 u16HTotal;               ///< H total
    U16 u16VTotal;               ///< V total

    U16 u16DefaultVFreq;         ///< Panel output Vfreq., used in free run

    // output type
    PNL_TYPE eLPLL_Type;            ///< 0: LVDS type, 1: RSDS type
    PNL_MODE eLPLL_Mode;            ///< 0: single mode, 1: dual mode

    // sync
    U8  u8HSyncWidth;            ///< H sync width
    U16 u16VSyncStart;           ///< V sync start = Vtotal - backporch - VSyncWidth
    U8  u8VSyncWidth;            ///< V sync width
    U8 bManuelVSyncCtrl;       ///< enable manuel V sync control

    // output control
    U16 u16OCTRL;                ///< Output control such as Swap port, etc.
    U16 u16OSTRL;                ///< Output control sync as Invert sync/DE, etc.
    U16 u16ODRV;                 ///< Driving current
    U16 u16DITHCTRL;             ///< Dither control

    // MOD
    U16 u16MOD_CTRL0;            ///< MOD_REG(0x40), PANEL_DCLK_DELAY:8, PANEL_SWAP_LVDS_CH:6, PANEL_SWAP_LVDS_POL:5, PANEL_LVDS_TI_MODE:2,
    U16 u16MOD_CTRL9;            ///< MOD_REG(0x49), PANEL_SWAP_EVEN_ML:14, PANEL_SWAP_EVEN_RB:13, PANEL_SWAP_ODD_ML:12, PANEL_SWAP_ODD_RB:11, [7,6] : output formate selction 10: 8bit, 01: 6bit :other 10bit
    U16 u16MOD_CTRLA;            ///< MOD_REG(0x4A), PANEL_INV_HSYNC:12, PANEL_INV_DCLK:4, PANEL_INV_VSYNC:3, PANEL_INV_DE:2, PANEL_DUAL_PORT:1, PANEL_SWAP_PORT:0,
    U8  u8MOD_CTRLB;             ///< MOD_REG(0x4B), [1:0]ti_bitmode=00(10bit)

    // Other
    U16 u16LVDSTxSwapValue;      ///< swap PN setting
    U8  u8PanelNoiseDith;        ///<  PAFRC mixed with noise dither disable
    U32 u32PNL_MISC;
    U16 u16OutputCFG0_7;
    U16 u16OutputCFG8_15;
    U16 u16OutputCFG16_21;

} PNL_InitData;



//HW LVDS Reserved Bit to L/R flag Info
typedef struct
{
    U32 u32pair; // pair 0: BIT0, pair 1: BIT1, pair 2: BIT2, pair 3: BIT3, pair 4: BIT4, etc ...
    U16 u16channel; // channel A: BIT0, channel B: BIT1,
    U8 bEnable;
} PNL_DrvHW_LVDSResInfo;


//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
//extern U8 _u8MOD_LVDS_Pair_Shift;       ///< 0:default setting, LVDS pair Shift
//extern U8 _u8MOD_LVDS_Pair_Type;        ///< 0:default setting, LVDS data differential pair

#ifdef _DRVPNL_C
PNL_InitData _stPnlInitData;
#else
extern PNL_InitData _stPnlInitData;
#endif



//extern PNL_InitData _stPnlInitData;
extern U8 g_bDrvPnl_UseTConTable_Mod;

// must have functions
extern const U8 * MDrv_PNL_GetLibVer(void);                  ///< Get version (without Mutex protect)
extern U8 MDrv_PNL_GetStatus(PNL_DrvStatus *pDrvStatus);   ///< Get panel current status
extern U8 MDrv_PNL_SetDbgLevel(U16 u16DbgSwitch);       ///< Set debug level (without Mutex protect)

// initial functions
PNL_Result MDrv_PNL_PreInit(PNL_OUTPUT_MODE eParam);
PNL_Result MDrv_PNL_Init(void);       ///< initialize panel driver
PNL_Result MDrv_PNL_Close(void);                                                            ///< close panel driver and release resources

//extern void MDrv_PNL_TCONMAP_DumpTable(U8 *pTCONTable, U8 u8Tcontype);
//extern void MDrv_PNL_TCONMAP_Power_Sequence(U8 *pTCONTable, U8 bEnable);
void MDrv_PNL_TCON_DumpSCRegTab(U8* pu8TconTab);
void MDrv_PNL_TCON_DumpMODRegTab(U8* pu8TconTab);
void MDrv_PNL_TCON_DumpGENRegTab(U8* pu8TconTab);


extern void MDrv_PNL_TCON_Count_Reset( U8 bEnable );
extern void MDrv_PNL_TCON_Init(void);
extern U8 MDrv_PNL_GetDataFromRegister(PNL_InitData *PNL_Data);

// control & setting functions
extern PNL_Result MDrv_PNL_En(U8 bPanelOn, U8 bCalEn);                            ///< bPanelOn = TRUE to show up OSD & video

#define MDrv_PNL_MOD_Calibration  MHal_PNL_MOD_Calibration
extern PNL_Result MDrv_PNL_MOD_Calibration(void);

extern PNL_Result MDrv_PNL_SetGammaTbl(DRVPNL_GAMMA_TYPE eGammaType, U8* pu8GammaTab[3], DRVPNL_GAMMA_MAPPEING_MODE GammaMapMode ); ///< set gamma table
extern PNL_Result MDrv_PNL_SetGammaValue(U8 u8Channel, U16 u16Offset, U16 u16GammaValue);
extern PNL_Result MDrv_PNL_SetSSC(U16 u16Fmodulation, U16 u16Rdeviation, U8 bEnable);
extern void MDrv_PNL_MFC(U8 bIsMFC);
extern void MDrv_PNL_SetOutputPattern(U8 bEnable, U16 u16Red , U16 u16Green, U16 u16Blue);
extern void MDrv_PNL_SetOutputType(PNL_OUTPUT_MODE eOutputMode, PNL_TYPE eLPLL_Type);
extern PNL_Result MDrv_PNL_SetPanelType(PNL_InitData *pstPanelInitData);

extern void MDrv_PNL_SetSSC_Rdeviation(U16 u16Rdeviation);
extern void MDrv_PNL_SetSSC_Fmodulation(U16 u16Fmodulation);
extern void MDrv_PNL_SetSSC_En(U8 bEnable);

extern void MDrv_Mod_Calibration_Init(U8 U8MOD_CALI_TARGET, S8 S8MOD_CALI_OFFSET);
extern void MDrv_BD_LVDS_Output_Type(U16 Type);
extern U8 MDrv_PNL_Control_Out_Swing(U16 u16Swing_Level);
extern U8 MDrv_PNL_SkipTimingChange_GetCaps(void);
extern void MDrv_PNL_HWLVDSReservedtoLRFlag(PNL_DrvHW_LVDSResInfo lvdsresinfo);
extern void MDrv_PNL_OverDriver_Init(U32 u32OD_MSB_Addr, U32 u32OD_LSB_Addr, U8 u8ODTbl[1056]);
extern void MDrv_PNL_OverDriver_Enable(U8 bEnable);
extern U32 MDrv_PNL_CalculateLPLLSETbyDClk(U32 ldHz);

#define MDrv_MOD_PVDD_Power_Setting  MHal_MOD_PVDD_Power_Setting
extern void MDrv_MOD_PVDD_Power_Setting(U8 bIs2p5);

#define MDrv_PNL_MISC_Control  MHal_PNL_MISC_Control
extern void MDrv_PNL_MISC_Control(U32 u32PNL_MISC);

extern U8 MDrv_PNL_Is_SupportFRC(void);
extern int MDrv_PNL_Get_Width();
extern int MDrv_PNL_Get_Height();
#endif // _DRV_PNL_H_

