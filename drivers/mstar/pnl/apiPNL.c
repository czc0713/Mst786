#ifndef _API_XC_PANEL_C_
#define _API_XC_PANEL_C_

/******************************************************************************/
/*                      Includes                                              */
/******************************************************************************/
#include "apiPNL.h"
#include "drvpnl.h"
#include "hal/cedric/halpnl.h"
#include "include/pnl_hwreg_utility2.h"
#include "asm/arch/mach/io.h"


/******************************************************************************/
/*                      Debug information                                      */
/******************************************************************************/
#define PNL_DBG_MSG(fmt,...)
#define PNL_PRINT_VAR(var)
#define PNL_PRINT_FUN(fun)



#ifndef UNUSED
	#if defined(MSOS_TYPE_NOS)
	#define UNUSED(x) ((x)=(x))
	#else
	#define UNUSED(x)
	#endif
#endif


// OS related
#define PNL_WAIT_MUTEX              300//(TICK_PER_ONE_MS * 300)              // 300 ms

// Mutex function
#define _PNL_ENTRY()
#define _PNL_RETURN()


/******************************************************************************/
/*                      Type Definition                                       */
/******************************************************************************/

typedef struct
{
    // XC need below information do to frame lock
    U16 u16HStart;               ///< ursa scaler, DE H start
    U16 u16VStart;               ///< ursa scaler, DE V start
    U16 u16Width;                ///< ursa scaler, DE H width
    U16 u16Height;               ///< ursa scaler, DE V height
    U16 u16HTotal;               ///< ursa scaler, H total
    U16 u16VTotal;               ///< ursa scaler, V total

    U16 u16DefaultVFreq;         ///< Panel output Vfreq., used in free run

    U8  u8LPLL_Mode;             ///< 0: single mode, 1: dual mode

    // Later need to refine to use Min/Max SET for PDP panel, but for LCD, it maybe no need to check the Min/Max SET
    //U16 u16DefaultHTotal, u16DefaultVTotal;
    //U16 u16MinHTotal, u16DefaultHTotal, u16MaxHTotal;
    //U16 u16MinVTotal, u16DefaultVTotal, u16MaxVTotal;
    //U32 u32MinDCLK, u32MaxDCLK;
    U32 u32MinSET, u32MaxSET;

    // below structures will be removed to panel module, there is no need for XC to initialize MOD, panel module will do this and init the freerun timing
    // so that project can kick off without XC driver

    // output type
    APIPNL_LINK_TYPE        eLPLL_Type;   ///< output type such LVDS, TTL, etc.

    // sync
    U8  u8HSyncWidth;           ///< H sync width
    U16 u16VSyncStart;          ///< V sync start

    // output control
    U16 u16OCTRL;               ///< Output control such as Swap port, etc.
    U16 u16OSTRL;               ///< Output control sync as Invert sync/DE, etc.
    U16 u16ODRV;                ///< Driving current
    U16 u16DITHCTRL;            ///< Dither control

    // MOD
    U16 u16MOD_CTRL0;           ///< MOD_REG(0x40), PANEL_DCLK_DELAY:8, PANEL_SWAP_LVDS_CH:6, PANEL_SWAP_LVDS_POL:5, PANEL_LVDS_TI_MODE:2,
    U16 u16MOD_CTRL9;           ///< MOD_REG(0x49), PANEL_SWAP_EVEN_ML:14, PANEL_SWAP_EVEN_RB:13, PANEL_SWAP_ODD_ML:12, PANEL_SWAP_ODD_RB:11, [7,6] : output formate selction 10: 8bit, 01: 6bit :other 10bit
    U16 u16MOD_CTRLA;           ///< MOD_REG(0x4A), PANEL_INV_HSYNC:12, PANEL_INV_DCLK:4, PANEL_INV_VSYNC:3, PANEL_INV_DE:2, PANEL_DUAL_PORT:1, PANEL_SWAP_PORT:0,
    U8  u8MOD_CTRLB;            ///< MOD_REG(0x4B), [1:0]ti_bitmode=00(10bit)

#if 0
    //titania to URSA
    U8  u8LVDSTxChannel;        ///< ursa scaler
    U8  u8LVDSTxBitNum;         ///< ursa scaler
    U8  u8LVDSTxTiMode;         ///< ursa scaler 40-bit2
    U8  u8LVDSTxSwapOddEven;    ///< ursa scaler
#endif
    U8  u8LVDSTxSwapMsbLsb;     ///< ursa scaler

#if 0
    //URSA to Panel info
    U8  u8PanelVfreq;           ///< ursa scaler
    U8  u8PanelChannel;         ///< ursa scaler
    U8  u8PanelLVDSSwapCH;      ///< ursa scaler
    U8  u8PanelBitNum;          ///< ursa scaler
    U8  u8PanelLVDSShiftPair;   ///< ursa scaler
    U8  u8PanelLVDSTiMode;      ///< ursa scaler
    U8  u8PanelLVDSSwapPol;     ///< ursa scaler
    U8  u8PanelLVDSSwapPair;    ///< ursa scaler
#endif
    // additional for Chakra (Obama doens't have these member yet)
    U8  u8VSyncWidth;           ///< V sync width
    U16 u16LVDSTxSwapValue;
    U8 bManuelVSyncCtrl;      ///< enable manuel V sync control
    U8 u8PanelNoiseDith;        ///<  PAFRC mixed with noise dither disable
    // output type
    APIPNL_LINK_EXT_TYPE      eLPLL_TypeExt;   ///< output type such LVDS, TTL, etc.
    APIPNL_MISC ePNL_MISC;
    U16 u16OutputCFG0_7;
    U16 u16OutputCFG8_15;
    U16 u16OutputCFG16_21;
} XC_PNL_DBInfo;


/******************************************************************************/
/*                      Macro and Define                                      */
/******************************************************************************/
#define MAX_BACKLIGHT      100
#define PANEL_DE_VSTART    0//8

/******************************************************************************/
/*                      Private varialbes                                     */
/******************************************************************************/
static XC_PNL_DBInfo       m_PanelInfo;
static PanelType           m_XCPaneRawData;
//static U8             _bUserDefinePanel = FALSE;
static U8             **m_ppAllGammaTbls;
static U8             _bSkipTimingChange = FALSE;

// Library info
static PNL_ApiInfo         _cstPnl_ApiInfo;
static PNL_ApiStatus       _stPnl_ApiStatus  = {FALSE, FALSE};
static U16              _u16DbgSwitch     = FALSE;

// Mutex & Lock
static S32 _s32PNLMutex = -1;

static U8      g_bForceInitOneShotFlag = FALSE;

/******************************************************************************/
/*                      Function                                              */
/******************************************************************************/


/******************************************************************************/
/*                      Private functions                                     */
/******************************************************************************/
//
// set params from user defined panel

static U8 _SavePanelParams_User(PanelType *pSelPanelType)
{

    /*  Calculate panel info according to panel_raw_data.
    *   can be factor out to diff functions.
    */
    // output Sync timing
    m_PanelInfo.u16HTotal = pSelPanelType->m_wPanelHTotal;//devPanel_HTOTAL();  // output htotal
    m_PanelInfo.u16VTotal = pSelPanelType->m_wPanelVTotal;//devPanel_VTOTAL();  // output vtotal
    m_PanelInfo.u32MinSET = pSelPanelType->m_dwPanelMinSET;//devPanel_MinSET();  // output MinSET
    m_PanelInfo.u32MaxSET = pSelPanelType->m_dwPanelMaxSET;//devPanel_MaxSET();  // output MaxSET
    //#if (ENABLE_LVDSTORGB_CONVERTER == ENABLE)
   // panel_Info.u8HSyncWidth = 0x8D + 1;  // hsync width
   // #else
    m_PanelInfo.u8HSyncWidth = pSelPanelType->m_ucPanelHSyncWidth;//pnlGetHSyncWidth();  // hsync width
   // #endif

    m_PanelInfo.u16VSyncStart = pSelPanelType->m_ucPanelVBackPorch + pSelPanelType->m_ucPanelVSyncWidth;//pSelPanelType->m_wPanelVStart;      //devPanel_VSTART();  // vsync start
    m_PanelInfo.u8VSyncWidth  = pSelPanelType->m_ucPanelVSyncWidth; //PANEL_VSYNC_WIDTH;  // vsync width

    // display timing
    m_PanelInfo.u16HStart = pSelPanelType->m_wPanelHStart;//devPanel_HSTART(); // DE H start
    m_PanelInfo.u16Width  = pSelPanelType->m_wPanelWidth;  // DE H width
    m_PanelInfo.u16VStart = PANEL_DE_VSTART;   // devPanel_VSTART(); // DE V start
    m_PanelInfo.u16Height = pSelPanelType->m_wPanelHeight;//devPanel_HEIGHT(); // DE V height

    // output control
    m_PanelInfo.u16OCTRL = ((U16)((pSelPanelType->m_bPanelDualPort)<<8) |
            ( pSelPanelType->m_bPanelSwapEven_RB) );

    //devPanel_OCTRL();
    m_PanelInfo.u16OSTRL = ((U16)(0xF7<<8) |
            ( pSelPanelType->m_ucPanelDCLKDelay<< 4 ) |
            ( pSelPanelType->m_bPanelInvHSync));

    // output driving current
    m_PanelInfo.u16ODRV = ((pSelPanelType->m_ucPanelDCKLCurrent<< 6) |
            ( pSelPanelType->m_ucPanelDECurrent << 4) |
            ( pSelPanelType->m_ucPanelODDDataCurrent<< 2) |
            ( pSelPanelType->m_ucPanelEvenDataCurrent)); //devPanel_ODRV();

    // dither control
    m_PanelInfo.u16DITHCTRL = (pSelPanelType->m_bPanelDither)?0x2D05 : 0x2D00;

    //LPLL setting
    //panel_Info.u16LPLL_InputDiv = (U16)((PANEL_LPLL_INPUT_DIV_2nd<<8)  | PANEL_LPLL_INPUT_DIV_1st);
    //panel_Info.u16LPLL_LoopDiv = (U16)((PANEL_LPLL_LOOP_DIV_2nd<<8)   | PANEL_LPLL_LOOP_DIV_1st);
    //panel_Info.u16LPLL_OutputDiv = (U16)((PANEL_LPLL_OUTPUT_DIV_2nd<<8) | PANEL_LPLL_OUTPUT_DIV_1st);

    //for MOD
    m_PanelInfo.u8LPLL_Mode = pSelPanelType->m_bPanelDoubleClk ? 1:0; // 0: single clock mode, 1: double clock mode
    // !! it should not use the different type to assign
    m_PanelInfo.eLPLL_Type = pSelPanelType->m_ePanelLinkType;

    //printf("@@ %s, %d, pSelPanelType->m_ePanelLinkType=%u\n", __FUNCTION__, __LINE__, pSelPanelType->m_ePanelLinkType);
    //printf("@@ %s, %d, m_PanelInfo.eLPLL_TypeExt=%u\n", __FUNCTION__, __LINE__, m_PanelInfo.eLPLL_TypeExt);

    //we should set VSync manually to satisfy HDMITx's requirement.
    //auto-Vsync will have several-vertical-line error.
    switch(pSelPanelType->m_ePanelLinkType)
    {
        case LINK_DAC_I:
        case LINK_DAC_P:
        case LINK_PDPLVDS:
        case LINK_EXT:
            m_PanelInfo.bManuelVSyncCtrl = TRUE;
            break;
        default:
            m_PanelInfo.bManuelVSyncCtrl = FALSE;
            break;
    }
    //MOD_REG(0x40)
    //PANEL_DCLK_DELAY:8, PANEL_SWAP_LVDS_CH:6, PANEL_SWAP_LVDS_POL:5, PANEL_LVDS_TI_MODE:2,
    m_PanelInfo.u16MOD_CTRL0 = (( /*PANEL_DCLK_DELAY*/pSelPanelType->m_ucPanelDCLKDelay & 0xf) << 8)  |
            ((/* PANEL_SWAP_LVDS_CH */   pSelPanelType->m_bPanelSwapLVDS_CH)? BIT(6):0) |
            ((/* ~PANEL_SWAP_LVDS_POL */ !pSelPanelType->m_bPanelSwapLVDS_POL)?BIT(5):0)|
            ((/* PANEL_LVDS_PDP_10BIT */ pSelPanelType->m_bPanelPDP10BIT)?BIT(3):0) |
            ((/* PANEL_LVDS_TI_MODE */   pSelPanelType->m_bPanelLVDS_TI_MODE)? BIT(2):0) ;
    //MOD_REG(0x49)
    //PANEL_SWAP_EVEN_ML:14, PANEL_SWAP_EVEN_RB:13, PANEL_SWAP_ODD_ML:12, PANEL_SWAP_ODD_RB:11
    // [7,6] : output formate selction 10: 8bit, 01: 6bit :other 10bit
    m_PanelInfo.u16MOD_CTRL9 = ((/*  PANEL_SWAP_EVEN_ML */ pSelPanelType->m_bPanelSwapEven_ML)? BIT(14):0)|
                                ((/* PANEL_SWAP_EVEN_RB */ pSelPanelType->m_bPanelSwapEven_RB)? BIT(13):0)|
                                ((/* PANEL_SWAP_ODD_ML */  pSelPanelType->m_bPanelSwapOdd_ML)? BIT(12):0) |
                                ((/* PANEL_SWAP_ODD_RB */  pSelPanelType->m_bPanelSwapOdd_RB)? BIT(11):0) |
                                ((/* PANEL_OUTPUT_FORMAT */pSelPanelType->m_ucOutputFormatBitMode) << 6 ) |
                                ((/* PANEL_SWAP_EVEN_RG */  pSelPanelType->m_bPanelSwapEven_RG)? BIT(5):0) |
                                ((/* PANEL_SWAP_EVEN_GB */  pSelPanelType->m_bPanelSwapEven_GB)? BIT(4):0) |
                                ((/* PANEL_SWAP_ODD_RG */  pSelPanelType->m_bPanelSwapOdd_RG)? BIT(3):0) |
                                ((/* PANEL_SWAP_ODD_GB */  pSelPanelType->m_bPanelSwapOdd_GB)? BIT(2):0) ;
    //MOD_REG(0x4A)
    //PANEL_INV_HSYNC:12, PANEL_INV_DCLK:4, PANEL_INV_VSYNC:3, PANEL_INV_DE:2, PANEL_DUAL_PORT:1, PANEL_SWAP_PORT:0,

    m_PanelInfo.u16MOD_CTRLA = ((pSelPanelType->m_bPanelInvHSync)? BIT(12):0)   |
                                ((pSelPanelType->m_bPanelInvDCLK)? BIT(4):0)     |
                                ((pSelPanelType->m_bPanelInvVSync)? BIT(3):0)    |
                                ((pSelPanelType->m_bPanelInvDE)? BIT(2):0)       |
                                ((pSelPanelType->m_bPanelDualPort) ? BIT(1):0)   |
                                ((pSelPanelType->m_bPanelSwapPort) ? BIT(0):0)    ;

    //MOD_REG(0x4B)
    //[1:0]ti_bitmode=00(10bit)
    m_PanelInfo.u8MOD_CTRLB = pSelPanelType->m_ucTiBitMode;

#define MOVE_OUTSIDE TRUE
#if MOVE_OUTSIDE
    /*printf( "Move board related params to (custimized panel file)Panel.c  (LVDSTxSwapValue)%d\n",
             pSelPanelType->m_u16LVDSTxSwapValue
    );*/
    m_PanelInfo.u16LVDSTxSwapValue  = pSelPanelType->m_u16LVDSTxSwapValue;

#else
    #if defined(ENABLE_LVDS_PN_SWAP)
    m_PanelInfo.u16LVDSTxSwapValue  = (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L;
    #else
    m_PanelInfo.u16LVDSTxSwapValue  = 0;
    #endif

#endif
    //m_PanelInfo.eOutTimingMode   = pSelPanelType->m_ucOutTimingMode;
    m_PanelInfo.u16DefaultVFreq  = (pSelPanelType->m_dwPanelDCLK*10000000)/(pSelPanelType->m_wPanelHTotal*pSelPanelType->m_wPanelVTotal);
    if(m_PanelInfo.u16DefaultVFreq > 1100)
    {
        m_PanelInfo.u16DefaultVFreq = 1200;
    }
    else if(m_PanelInfo.u16DefaultVFreq > 900)
    {
        m_PanelInfo.u16DefaultVFreq = 1000;
    }
    else if(m_PanelInfo.u16DefaultVFreq > 550)
    {
        m_PanelInfo.u16DefaultVFreq = 600;
    }
    else if(m_PanelInfo.u16DefaultVFreq > 350)
    {
        m_PanelInfo.u16DefaultVFreq = 500;
    }
    else
    {
        //Frame Rate(VFreq) 24, 25, 30 or others
        //do nothing
    }

    //In FRC 120Hz Panel case, the DCLK should be double
    if(MDrv_PNL_Is_SupportFRC() && (m_PanelInfo.ePNL_MISC == E_APIPNL_MISC_MFC_ENABLE  ))
        m_PanelInfo.u16DefaultVFreq *= 2;

    m_PanelInfo.u8PanelNoiseDith = pSelPanelType->m_bPanelNoiseDith;

    memcpy(&m_XCPaneRawData, pSelPanelType, sizeof(PanelType));

    // for Panel lib compatibility

    return TRUE;

}
//
//  Save panel params
//
static U8 _SetPanelParams(void)
{
    PNL_InitData stPanelInitData;
    U16 u16Type_enum = (U16)m_PanelInfo.eLPLL_Type;
    U16 u16TypeExt_enum = (U16)m_PanelInfo.eLPLL_TypeExt;

    // Output timing
    stPanelInitData.u16HStart = m_PanelInfo.u16HStart;      // DE H start
    stPanelInitData.u16VStart = m_PanelInfo.u16VStart;
    stPanelInitData.u16Width  = m_PanelInfo.u16Width;
    stPanelInitData.u16Height = m_PanelInfo.u16Height;
    stPanelInitData.u16HTotal = m_PanelInfo.u16HTotal;
    stPanelInitData.u16VTotal = m_PanelInfo.u16VTotal;

    stPanelInitData.u16DefaultVFreq = m_PanelInfo.u16DefaultVFreq;
    // output type
    switch(u16Type_enum)
    {
        case LINK_TTL:
            stPanelInitData.eLPLL_Type = E_PNL_TYPE_TTL;
            break;

        case LINK_RSDS:
            stPanelInitData.eLPLL_Type = E_PNL_TYPE_RSDS;
            break;

        case LINK_MINILVDS:
        case LINK_ANALOG_MINILVDS:
        case LINK_DIGITAL_MINILVDS:
            stPanelInitData.eLPLL_Type = E_PNL_TYPE_MINILVDS;
            break;


        case LINK_DAC_I:
            stPanelInitData.eLPLL_Type = E_PNL_TYPE_DAC_I;
            break;
        case LINK_DAC_P:
            stPanelInitData.eLPLL_Type = E_PNL_TYPE_DAC_P;
            break;

        default:
        case LINK_LVDS:
        case LINK_PDPLVDS:
            stPanelInitData.eLPLL_Type = E_PNL_TYPE_LVDS;
            break;
        case LINK_EXT:
        {
            switch(u16TypeExt_enum)
            {
                default:
                case LINK_MINILVDS_5P_2L:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_5P_2L;
                    break;

                case LINK_MINILVDS_4P_2L:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_4P_2L;
                    break;

                case LINK_MINILVDS_3P_2L:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_3P_2L;
                    break;

                case LINK_MINILVDS_6P_1L:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_6P_1L;
                    break;

                case LINK_MINILVDS_5P_1L:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_5P_1L;
                    break;

                case LINK_MINILVDS_4P_1L:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_4P_1L;
                    break;

                case LINK_MINILVDS_3P_1L:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_3P_1L;
                    break;

                case LINK_HF_LVDS:
                    stPanelInitData.eLPLL_Type = E_PNL_TYPE_HF_LVDS;
                    break;

                case LINK_HS_LVDS:
                    stPanelInitData.eLPLL_Type = E_PNL_TYPE_HS_LVDS;
                    break;

                case LINK_TTL_TCON:
                    stPanelInitData.eLPLL_Type = E_PNL_TYPE_TTL_TCON;
                    break;

                case LINK_MINILVDS_1CH_3P_6BIT:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_1CH_3P_6BIT;
                    break;

                case LINK_MINILVDS_1CH_4P_6BIT:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_1CH_4P_6BIT;
                    break;

                case LINK_MINILVDS_1CH_5P_6BIT:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_1CH_5P_6BIT;
                    break;

                case LINK_MINILVDS_1CH_6P_6BIT:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_1CH_6P_6BIT;
                    break;

                case LINK_MINILVDS_2CH_3P_6BIT:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_2CH_3P_6BIT;
                    break;

                case LINK_MINILVDS_2CH_4P_6BIT:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_2CH_4P_6BIT;
                    break;

                case LINK_MINILVDS_2CH_5P_6BIT:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_2CH_5P_6BIT;
                    break;

                case LINK_MINILVDS_2CH_6P_6BIT:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_2CH_6P_6BIT;
                    break;

                case LINK_MINILVDS_1CH_3P_8BIT:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_1CH_3P_8BIT;
                    break;

                case LINK_MINILVDS_1CH_4P_8BIT:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_1CH_4P_8BIT;
                    break;

                case LINK_MINILVDS_1CH_5P_8BIT:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_1CH_5P_8BIT;
                    break;

                case LINK_MINILVDS_1CH_6P_8BIT:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_1CH_6P_8BIT;
                    break;

                case LINK_MINILVDS_2CH_3P_8BIT:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_2CH_3P_8BIT;
                    break;

                case LINK_MINILVDS_2CH_4P_8BIT:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_2CH_4P_8BIT;
                    break;

                case LINK_MINILVDS_2CH_5P_8BIT:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_2CH_5P_8BIT;
                    break;

                case LINK_MINILVDS_2CH_6P_8BIT:
                    stPanelInitData.eLPLL_Type = E_PNL_LPLL_MINILVDS_2CH_6P_8BIT;
                    break;
            }
        }
        break;
    }
    //printf("@@ %s, %d, stPanelInitData.eLPLL_Type=%u\n", __FUNCTION__, __LINE__, stPanelInitData.eLPLL_Type);

    stPanelInitData.eLPLL_Mode = m_PanelInfo.u8LPLL_Mode == 0 ? E_PNL_MODE_SINGLE : E_PNL_MODE_DUAL;

    // sync
    stPanelInitData.u8HSyncWidth = m_PanelInfo.u8HSyncWidth;
    stPanelInitData.u16VSyncStart = m_PanelInfo.u16VSyncStart;
    stPanelInitData.u8VSyncWidth = m_PanelInfo.u8VSyncWidth;
    stPanelInitData.bManuelVSyncCtrl = m_PanelInfo.bManuelVSyncCtrl;

    // output control
    stPanelInitData.u16OCTRL = m_PanelInfo.u16OCTRL;
    stPanelInitData.u16OSTRL = m_PanelInfo.u16OSTRL;
    stPanelInitData.u16ODRV = m_PanelInfo.u16ODRV;
    stPanelInitData.u16DITHCTRL = m_PanelInfo.u16DITHCTRL;

    // MOD
    stPanelInitData.u16MOD_CTRL0 = m_PanelInfo.u16MOD_CTRL0;
    stPanelInitData.u16MOD_CTRL9 = m_PanelInfo.u16MOD_CTRL9;
    stPanelInitData.u16MOD_CTRLA = m_PanelInfo.u16MOD_CTRLA;
    stPanelInitData.u8MOD_CTRLB = m_PanelInfo.u8MOD_CTRLB;

    // Others
    stPanelInitData.u16LVDSTxSwapValue = m_PanelInfo.u16LVDSTxSwapValue;
    stPanelInitData.u8PanelNoiseDith= m_PanelInfo.u8PanelNoiseDith;
    stPanelInitData.u32PNL_MISC = (U32) m_PanelInfo.ePNL_MISC;
    stPanelInitData.u16OutputCFG0_7   = m_PanelInfo.u16OutputCFG0_7;
    stPanelInitData.u16OutputCFG8_15  = m_PanelInfo.u16OutputCFG8_15;
    stPanelInitData.u16OutputCFG16_21 = m_PanelInfo.u16OutputCFG16_21;


    if(MDrv_PNL_Is_SupportFRC())
    {
        if(m_PanelInfo.ePNL_MISC == E_APIPNL_MISC_MFC_ENABLE)
        {
            stPanelInitData.eLPLL_Mode = E_PNL_MODE_QUAD;
        }

    }
    _PNL_ENTRY();

    memcpy(&_stPnlInitData, &stPanelInitData, sizeof(PNL_InitData));

    {

        MDrv_PNL_SetPanelType(&stPanelInitData);
    }

#if 0
    if(stPanelInitData.eLPLL_Type == E_PNL_TYPE_MFC)
    {
        MDrv_PNL_MFC(TRUE);
    }
#endif

    _PNL_RETURN();
    return TRUE;
}

//
//  DumpData
//
static void _DumpData( XC_PNL_DBInfo* panel_info)
{
    //PNL_PRINT_VAR(panel_info);
    PNL_PRINT_VAR(panel_info->u16HTotal            );
    PNL_PRINT_VAR(panel_info->u16VTotal            );

    PNL_PRINT_VAR(panel_info->u8HSyncWidth         );
    PNL_PRINT_VAR(panel_info->u16Width             );
    PNL_PRINT_VAR(panel_info->u16Height            );
    PNL_PRINT_VAR(panel_info->u16HStart            );
    PNL_PRINT_VAR(panel_info->u16VStart            );

    PNL_PRINT_VAR(panel_info->u16OCTRL             );
    PNL_PRINT_VAR(panel_info->u16OSTRL             );

    PNL_PRINT_VAR(panel_info->u16ODRV              );

    PNL_PRINT_VAR(panel_info->u16DITHCTRL          );

    PNL_PRINT_VAR(panel_info->u8LPLL_Mode          );
    PNL_PRINT_VAR(panel_info->eLPLL_Type           );

    PNL_PRINT_VAR(panel_info->u16MOD_CTRL0         );
    PNL_PRINT_VAR(panel_info->u16MOD_CTRL9         );

    PNL_PRINT_VAR(panel_info->u16MOD_CTRLA         );

    PNL_PRINT_VAR(panel_info->u8MOD_CTRLB          );

    PNL_PRINT_VAR(panel_info->u16LVDSTxSwapValue   );
    //PNL_PRINT_VAR(panel_info->enPnl_Out_Timing_Mode);
    PNL_PRINT_VAR(panel_info->u16DefaultVFreq      );
    //PNL_PRINT_VAR(panel_info->u16DefaultHTotal     );
    //PNL_PRINT_VAR(panel_info->u16DefaultVTotal     );
    //PNL_PRINT_VAR(panel_info->u16MinHTotal         );
    //PNL_PRINT_VAR(panel_info->u16MaxHTotal         );
    //PNL_PRINT_VAR(panel_info->u16MinVTotal         );
    //PNL_PRINT_VAR(panel_info->u16MaxVTotal         );
    //PNL_PRINT_VAR(panel_info->u32MinDCLK           );
    //PNL_PRINT_VAR(panel_info->u32MaxDCLK           );

    PNL_PRINT_VAR(panel_info->u16VSyncStart         );
    PNL_PRINT_VAR(panel_info->u8VSyncWidth          );
    PNL_PRINT_VAR(panel_info->u8PanelNoiseDith      );
}

//
//  Dump m_panel data
//
static void _MApi_XC_DumpPanelData(void)
{
    _DumpData(&m_PanelInfo);
}
//
//
void _SetSSC(U16 u16Fmodulation, U16 u16Rdeviation, U8 bEnable)
{
    _PNL_ENTRY();
    MDrv_PNL_SetSSC(u16Fmodulation, u16Rdeviation, bEnable);
    _PNL_RETURN();
}
//
//
static void  _EmptyMethod(U8 bEnable)
{
    UNUSED(bEnable);
    PNL_DBG_MSG(" Error!! <==============  Calling a non-registered function!!");
}
//
//
static U16 _getPanelHstart(void) {return m_PanelInfo.u16HStart;}
static U16 _getPanelVstart(void) {return m_PanelInfo.u16VStart;}
U16 _getPanelWidth(void) {return m_PanelInfo.u16Width;}
U16 _getPanelHeight(void) {return m_PanelInfo.u16Height;}
static U16 _getPanelhtotal(void) {return m_PanelInfo.u16HTotal;}
static U16 _getPanelVtotal(void) {return m_PanelInfo.u16VTotal;}
static U32 _getPanelMinSET(void) {return m_PanelInfo.u32MinSET;}
static U32 _getPanelMaxSET(void) {return m_PanelInfo.u32MaxSET;}

static U16 _getPanelDefVFreq(void) {return m_PanelInfo.u16DefaultVFreq;}
static U8 _getPanel_LPLL_Mode(void) {return m_PanelInfo.u8LPLL_Mode;}
static U8 _getPanel_LPLL_Type(void) {return m_PanelInfo.eLPLL_Type;}

static U8 _getPanelHSyncWidth(void) {return m_XCPaneRawData.m_ucPanelHSyncWidth;}
static U8 _getPanelHSyncBackPorch(void) {return m_XCPaneRawData.m_ucPanelHSyncBackPorch;}
static U8 _getPanelVSyncBackPorch(void) {return m_XCPaneRawData.m_ucPanelVBackPorch;}

static U16 _getPanelOnTiming(APIPNL_POWER_TIMING_SEQ seq)
{
    if(E_APIPNL_POWER_TIMING_1 == seq)
    {
        return m_XCPaneRawData.m_wPanelOnTiming1;
    }
    else if(E_APIPNL_POWER_TIMING_2 == seq)
    {
        return m_XCPaneRawData.m_wPanelOnTiming2;
    }
    else
    {
        return E_APIPNL_POWER_TIMING_NA;
    }
}

static U16 _getPanelOffTiming(APIPNL_POWER_TIMING_SEQ seq)
{
    if(E_APIPNL_POWER_TIMING_1 == seq)
    {
        return m_XCPaneRawData.m_wPanelOffTiming1;
    }
    else if(E_APIPNL_POWER_TIMING_2 == seq)
    {
        return m_XCPaneRawData.m_wPanelOffTiming2;
    }
    else
    {
        return E_APIPNL_POWER_TIMING_NA;
    }
}

static U8 _getPanelDimCTRL(APIPNL_DIMMING_CTRL dim_type)
{
    if(E_APIPNL_DIMMING_MAX == dim_type )
    {
        return m_XCPaneRawData.m_ucMaxPWMVal;
    }
    else if(E_APIPNL_DIMMING_MIN == dim_type )
    {
        return m_XCPaneRawData.m_ucMinPWMVal;
    }
    else
    {
        return m_XCPaneRawData.m_ucDimmingCtl;
    }
}


static  const char*         _getPanelName(void)     {return m_XCPaneRawData.m_pPanelName;}
static  E_PNL_ASPECT_RATIO  _getPanel_ARC(void)     {return m_XCPaneRawData.m_ucPanelAspectRatio;}
static  U8**             _getGammaTbl(void)      {return m_ppAllGammaTbls;}
static  APIPNL_OUT_TIMING_MODE _getOutTimingMode(void) {return m_XCPaneRawData.m_ucOutTimingMode;}

static U8 _EnablePanel(U8 bPanelOn)
{
    U8 bResult = FALSE;
    U8 bCalEn = ENABLE;
    _PNL_ENTRY();

    if( !_stPnl_ApiStatus.bPanel_Initialized )
    {
        PNL_PRINT_VAR(_stPnl_ApiStatus.bPanel_Initialized);
        PNL_DBG_MSG("Usage of uninitialized function.!!");
    }

    if(m_PanelInfo.ePNL_MISC == E_APIPNL_MISC_SKIP_CALIBRATION)
        bCalEn = DISABLE;
    if ( E_PNL_OK == MDrv_PNL_En(bPanelOn, bCalEn) )
    {
        _stPnl_ApiStatus.bPanel_Enabled = bPanelOn;
        bResult = TRUE;
    }
    else
    {
        bResult = FALSE;
    }
    _PNL_RETURN();
    return bResult;
}


static U8 _SetGammaTbl(    APIPNL_GAMMA_TYPE eGammaType,
                                U8* pu8GammaTab[3],
                                APIPNL_GAMMA_MAPPEING_MODE Gamma_Map_Mode)
{
    U8 bResult = FALSE;
    m_ppAllGammaTbls = pu8GammaTab;
    _PNL_ENTRY();

   // bResult = ( E_PNL_OK == MDrv_PNL_SetGammaTbl(
    //(DRVPNL_GAMMA_TYPE)eGammaType,
    //pu8GammaTab,
    //Gamma_Map_Mode == E_APIPNL_GAMMA_8BIT_MAPPING ? E_DRVPNL_GAMMA_8BIT_MAPPING : E_DRVPNL_GAMMA_10BIT_MAPPING ));

    if(bResult != TRUE)
    {
        //PNL_ASSERT(0, "[%s] fails: ( APIPNL_GAMMA_TYPE: %d, APIPNL_GAMMA_MAPPEING_MODE %d) \n.", __FUNCTION__, eGammaType, Gamma_Map_Mode);
    }
    _PNL_RETURN();
    return bResult;
}

static U8 _SetGammaValue(U8 u8Channel, U16 u16Offset, U16 u16GammaValue)
{
    U8 bResult = FALSE;

    //_PNL_ENTRY();

    //bResult = ( E_PNL_OK == MDrv_PNL_SetGammaValue(u8Channel, u16Offset, u16GammaValue));

    //_PNL_RETURN();
    return bResult;
}

#if 1

XC_PNL_OBJ g_IPanel =
{
    //
    //  Data
    //
     _getPanelName,
     _getPanelHstart,
    _getPanelVstart,
    _getPanelWidth,
    _getPanelHeight,
    _getPanelhtotal,
    _getPanelVtotal,
    _getPanelHSyncWidth,
    _getPanelHSyncBackPorch,
    _getPanelVSyncBackPorch,
    _getPanelDefVFreq,
    _getPanel_LPLL_Mode,            /* 0: single, 1: dual mode */
    _getPanel_LPLL_Type,
    _getPanel_ARC,
    _getPanelMinSET,
    _getPanelMaxSET,
    // Manipulation
    //
    _SetSSC,
    _EnablePanel,
    _SetGammaTbl,
    _getGammaTbl,
    _MApi_XC_DumpPanelData,
    _getPanelDimCTRL,
    _getPanelOnTiming,
    _getPanelOffTiming,
    //
    // Custimized methods, can be provided by clinets.
    //
    _EmptyMethod,
    _getOutTimingMode,
    _SetGammaValue,
};

#else
XC_PNL_OBJ g_IPanel =
{
    //
    //  Data
    //
    .Name            = _getPanelName,
    .HStart          = _getPanelHstart,
    .VStart          = _getPanelVstart,
    .Width           = _getPanelWidth,
    .Height          = _getPanelHeight,
    .HTotal          = _getPanelhtotal,
    .VTotal          = _getPanelVtotal,
    .HSynWidth       = _getPanelHSyncWidth,
    .HSynBackPorch   = _getPanelHSyncBackPorch,
    .VSynBackPorch   = _getPanelVSyncBackPorch,
    .DefaultVFreq    = _getPanelDefVFreq,
    .LPLL_Mode       = _getPanel_LPLL_Mode,            /* 0: single, 1: dual mode */
    .LPLL_Type       = _getPanel_LPLL_Type,
    .AspectRatio     = _getPanel_ARC,
    .MinSET          = _getPanelMinSET,
    .MaxSET          = _getPanelMaxSET,
    .OutTimingMode   = _getOutTimingMode,
    // Manipulation
    //
    .SetSSC          = _SetSSC,
    .Enable          = _EnablePanel,
    .SetGammaTbl     = _SetGammaTbl,
    .GammaTab        = _getGammaTbl,
    .Dump            = _MApi_XC_DumpPanelData,
    .DimCtrl         = _getPanelDimCTRL,
    .OnTiming        = _getPanelOnTiming,
    .OffTiming       = _getPanelOffTiming,
    //
    // Custimized methods, can be provided by clinets.
    //
    .TurnBackLightOn = _EmptyMethod,
    .SetGammaValue   = _SetGammaValue,
};
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief Get current panel status
/// @param[in] pPnlStatus @ref PNL_ApiStatus
U8 MApi_PNL_GetStatus(PNL_ApiStatus *pPnlStatus)
{
    pPnlStatus->bPanel_Initialized = _stPnl_ApiStatus.bPanel_Initialized;
    pPnlStatus->bPanel_Enabled = _stPnl_ApiStatus.bPanel_Enabled;

    return TRUE;
}

/// Set debug level (without Mutex protect)
U8 MApi_PNL_SetDbgLevel(U16 u16DbgSwitch)
{
    _u16DbgSwitch = u16DbgSwitch;

	UNUSED(_u16DbgSwitch);

    MDrv_PNL_SetDbgLevel(u16DbgSwitch);

    return TRUE;
}

U8 MApi_PNL_IOMapBaseInit(void)
{
    U8 bResult = FALSE;
#if 0
    if (-1!=_s32PNLMutex)
    {
        MS_CRITICAL_MSG(printf("[MAPI PNL][%06d] Initialization more than once\n", __LINE__));
        return FALSE;
    }

    _s32PNLMutex = MsOS_CreateMutex(E_MSOS_FIFO, "_XC_Mutex", MSOS_PROCESS_SHARED);
    if (-1 == _s32PNLMutex)
    {
        MS_CRITICAL_MSG(printf("[MAPI PNL][%06d] create mutex fail\n", __LINE__));
    }

    if (_s32PNLMutex == -1)
    {
        //(printf("[MAPI PNL][%06d] create mutex fail\n", __LINE__));
        return FALSE;
    }
#endif
    bResult =(MDrv_PNL_Init() == E_PNL_OK);

    return bResult;
}

//////////////////////////////////////////////////////////////////////
/**
*
* @brief  Give some options for panel init.
*
* @param[in] pSelPanelType  a given panel type choosed by user.
*
*/
U8  MApi_PNL_PreInit(E_PNL_PREINIT_OPTIONS eInitParam)
{
    if ( eInitParam == E_PNL_NO_OUTPUT)
    {
        MDrv_PNL_PreInit( E_PNL_OUTPUT_NO_OUTPUT );
    }
    else if ( eInitParam == E_PNL_CLK_ONLY)
    {
        MDrv_PNL_PreInit( E_PNL_OUTPUT_CLK_ONLY);
    }
    else if ( eInitParam == E_PNL_CLK_DATA)
    {
        MDrv_PNL_PreInit(E_PNL_OUTPUT_CLK_DATA);
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////
/**
*
* @brief  Initialize the global panel object with a given PanelType
*
* @param[in] pSelPanelType  a given panel type choosed by user.
*
*/
U8 MApi_PNL_Init(PanelType *pSelPanelType/* <in > */)
{

    if(pSelPanelType){
        _SavePanelParams_User(pSelPanelType);
    }
    else{
        return FALSE;
    }

    if(MDrv_PNL_Init() != E_PNL_OK)
    {
        printf(" MApi_PNL_Init fail!!");
        return FALSE;
    }

    if(pSelPanelType->m_ePanelLinkType == LINK_TTL)
    {
        MDrv_BD_LVDS_Output_Type(4);
    }
    else
    {
	    MDrv_BD_LVDS_Output_Type(1);
	}

    _SetPanelParams();
    _stPnl_ApiStatus.bPanel_Initialized = TRUE;

    MDrv_Mod_Calibration_Init(0x00, 0x00);
    MDrv_PNL_En(ENABLE,ENABLE);
	//!! clear one-shot forced init. flag !!//
	g_bForceInitOneShotFlag             = FALSE;

    return TRUE;
}


void  MApi_PNL_SetForceInitOneShotFlag(U8 bForced)
{
	g_bForceInitOneShotFlag = bForced;
}

U8 MApi_PNL_Control_Out_Swing(U16 u16Swing_Level)
{
    return MDrv_PNL_Control_Out_Swing(u16Swing_Level);
}

void MApi_PNL_SetOutput(APIPNL_OUTPUT_MODE eOutputMode)
{
    PNL_OUTPUT_MODE ePnlOutputMode;

    switch(eOutputMode)
    {
        case E_APIPNL_OUTPUT_NO_OUTPUT:
            ePnlOutputMode = E_PNL_OUTPUT_NO_OUTPUT;
            break;

        case E_APIPNL_OUTPUT_CLK_ONLY:
            ePnlOutputMode = E_PNL_OUTPUT_CLK_ONLY;
            break;

        case E_APIPNL_OUTPUT_DATA_ONLY:
            ePnlOutputMode = E_PNL_OUTPUT_DATA_ONLY;
            break;

        case E_APIPNL_OUTPUT_CLK_DATA:
        default:
            ePnlOutputMode = E_PNL_OUTPUT_CLK_DATA;
            break;
    }

    _PNL_ENTRY();
    if(_stPnl_ApiStatus.bPanel_Initialized == TRUE)
    {
        MDrv_PNL_SetOutputType(ePnlOutputMode, _stPnlInitData.eLPLL_Type);
        MDrv_PNL_MISC_Control(m_PanelInfo.ePNL_MISC);
    }
    else
    {
        printf("Must call MApi_PNL_Init() first\n");
    }
    _PNL_RETURN();
}

U8 MApi_PNL_ChangePanelType(PanelType *pSelPanelType)
{
    if (_stPnl_ApiStatus.bPanel_Initialized == FALSE)
    {
        return FALSE;
    }

    if(pSelPanelType == NULL)
    {
        return FALSE;
    }
    _SavePanelParams_User(pSelPanelType);
    _SetPanelParams();

    return TRUE;
}

U8 MApi_PNL_TCONMAP_DumpTable(U8 *pTCONTable, U8 u8Tcontype)
{
    switch(u8Tcontype)
    {
        case E_APIPNL_TCON_TAB_TYPE_GENERAL:
        case E_APIPNL_TCON_TAB_TYPE_GPIO:
            MDrv_PNL_TCON_DumpGENRegTab(pTCONTable);
            break;
        case E_APIPNL_TCON_TAB_TYPE_SCALER:
            MDrv_PNL_TCON_DumpSCRegTab(pTCONTable);
            break;
        case E_APIPNL_TCON_TAB_TYPE_MOD:
            MDrv_PNL_TCON_DumpMODRegTab(pTCONTable);
            break;

        case E_APIPNL_TCON_TAB_TYPE_POWER_SEQUENCE_ON:
        case E_APIPNL_TCON_TAB_TYPE_POWER_SEQUENCE_OFF:
            //_MDrv_PNL_TCON_DumpPSRegTab(pTCONTable);
            break;

        case E_APIPNL_TCON_TAB_TYPE_GAMMA:
            break;

        default:
            printf("GetTable: unknown tcon type=%u> \n", u8Tcontype);
            break;
    }

    return TRUE;
}

U8 MApi_PNL_TCONMAP_Power_Sequence(U8 *pTCONTable, U8 bEnable)
{
// Move to APP
#if 0
    MDrv_PNL_TCONMAP_Power_Sequence(pTCONTable, bEnable);
#endif
    printf("\n\n!![Alert]Don't ued this function. It is removed!!\n\n");
    return FALSE;
}

void MApi_PNL_SetOutputPattern(U8 bEnable, U16 u16Red , U16 u16Green, U16 u16Blue)
{
    MDrv_PNL_SetOutputPattern(bEnable,u16Red,u16Green,u16Blue);
}
//-------------------------------------------------------------------------------------------------
/// Setup TCON Count Reset ON/OFF
/// @param  bEnable      \b IN: Enable or Disable
//-------------------------------------------------------------------------------------------------
void MApi_PNL_TCON_Count_Reset ( U8 bEnable )
{
    MDrv_PNL_TCON_Count_Reset(bEnable);
}

void MApi_PNL_TCON_Init( void )
{
    MDrv_PNL_TCON_Init();
}


U8 MApi_PNL_GetDstInfo(MS_PNL_DST_DispInfo *pDstInfo, U32 u32SizeofDstInfo)
{
    PNL_InitData PNL_Data;
    U8 bReturn = FALSE;

    _PNL_ENTRY();

    if(NULL == pDstInfo)
    {
        PNL_DBG_MSG("MApi_PNL_GetDstInfo():pDstInfo is NULL\n");
        bReturn = FALSE;
    }
    else if(u32SizeofDstInfo != sizeof(MS_PNL_DST_DispInfo))
    {
        PNL_DBG_MSG("MApi_PNL_GetDstInfo():u16SizeofDstInfo is different from the MS_PNL_DST_DispInfo defined, check header file!\n");
        bReturn = FALSE;
    }
    else if(MDrv_PNL_GetDataFromRegister(&PNL_Data))
    {
        pDstInfo->VDTOT = PNL_Data.u16VTotal;
        pDstInfo->HDTOT = PNL_Data.u16HTotal;
        pDstInfo->DEHST = PNL_Data.u16HStart;
        pDstInfo->DEHEND= PNL_Data.u16HStart + PNL_Data.u16Width;
        pDstInfo->DEVST= PNL_Data.u16VStart;
        pDstInfo->DEVEND= PNL_Data.u16VStart + PNL_Data.u16Height;
        pDstInfo->bInterlaceMode = FALSE;
        pDstInfo->bYUVOutput = ((PNL_Data.eLPLL_Type == E_PNL_TYPE_DAC_I)||
                (PNL_Data.eLPLL_Type == E_PNL_TYPE_DAC_P))? TRUE:FALSE;
        bReturn = TRUE;
    }
    else
    {
        pDstInfo->VDTOT = 0;
        pDstInfo->HDTOT = 0;
        pDstInfo->DEHST = 0;
        pDstInfo->DEHEND= 0;
        pDstInfo->DEVST= 0;
        pDstInfo->DEVEND= 0;
        pDstInfo->bInterlaceMode = FALSE;
        pDstInfo->bYUVOutput = FALSE;
        bReturn = FALSE;
    }

    _PNL_RETURN();
    return bReturn;
}


void MApi_Mod_Calibration_Setting(U8 U8MOD_CALI_TARGET, S8 S8MOD_CALI_OFFSET)
{

    MDrv_Mod_Calibration_Init( U8MOD_CALI_TARGET, S8MOD_CALI_OFFSET);

}

U8 MApi_Mod_Do_Calibration(void)
{
    U8 bResult = FALSE;
    _PNL_ENTRY();
    if ( E_PNL_OK == MDrv_PNL_MOD_Calibration() )
        bResult = TRUE;
    else
        bResult = FALSE;
    _PNL_RETURN();
    return bResult;
}

void MApi_BD_LVDS_Output_Type(U16 Type)
{

    MDrv_BD_LVDS_Output_Type(Type);

}

int MApi_PNL_SetSSC_En(U8 bEnable)
{

#if 1
    _PNL_ENTRY();
    MDrv_PNL_SetSSC_En(bEnable);
    _PNL_RETURN();
    return E_PNL_OK;
#endif

}

int MApi_PNL_SetSSC_Fmodulation(U16 u16Fmodulation)
{

#if 1

    _PNL_ENTRY();

    MDrv_PNL_SetSSC_Fmodulation(u16Fmodulation);

    _PNL_RETURN();
    return E_PNL_OK;
#endif

}

int MApi_PNL_SetSSC_Rdeviation(U16 u16Rdeviation)
{
#if 1
    _PNL_ENTRY();
    MDrv_PNL_SetSSC_Rdeviation(u16Rdeviation);
    _PNL_RETURN();
    return E_PNL_OK;
#endif

}

void MApi_PNL_SetLPLLTypeExt(APIPNL_LINK_EXT_TYPE eLPLL_TypeExt)
{
    m_PanelInfo.eLPLL_TypeExt = eLPLL_TypeExt;
}

void MApi_PNL_Init_MISC(APIPNL_MISC ePNL_MISC)
{
    m_PanelInfo.ePNL_MISC = ePNL_MISC;
}

void MApi_PNL_MOD_OutputConfig_User(U32 u32OutputCFG0_7, U32 u32OutputCFG8_15, U32 u32OutputCFG16_21)
{
    m_PanelInfo.u16OutputCFG0_7 = (U16)u32OutputCFG0_7;
    m_PanelInfo.u16OutputCFG8_15 = (U16)u32OutputCFG8_15;
    m_PanelInfo.u16OutputCFG16_21 = (U16)u32OutputCFG16_21;

}

//-------------------------------------------------------------------------------------------------
/// Set Flag to bypass timing change in MApi_PNL_Init()
/// @param  bFlag               \b IN: TRUE: initialize XC by skipping SW reset; FALSE: initialize XC in normal case
/// @return APIPNL_Result
//-------------------------------------------------------------------------------------------------
APIPNL_Result MApi_PNL_SkipTimingChange(U8 bFlag)
{
    U8 bCap = MDrv_PNL_SkipTimingChange_GetCaps();

    if( (bFlag == TRUE) && (bCap == TRUE) )
    {
        _bSkipTimingChange = TRUE;
        return E_APIPNL_OK;
    }
    else
    {
        //else, not supported
        _bSkipTimingChange = FALSE;
        return E_APIPNL_FAIL;
    }
}

void MApi_PNL_HWLVDSReservedtoLRFlag(MS_PNL_HW_LVDSResInfo lvdsresinfo)
{
    PNL_DrvHW_LVDSResInfo drv_lvdsinfo;
    drv_lvdsinfo.u32pair = lvdsresinfo.u32pair;
    drv_lvdsinfo.u16channel = lvdsresinfo.u16channel;
    drv_lvdsinfo.bEnable = lvdsresinfo.bEnable;
    MDrv_PNL_HWLVDSReservedtoLRFlag(drv_lvdsinfo);
}

////////////////////////////////////////////////////////////////////////
// Turn OD function
////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
/// OverDrive Enable
/// @param  bEnable               \b IN: TRUE: Enable OverDrive; FALSE: Disable OverDrive
/// @return E_APIPNL_OK or E_APIPNL_FAIL
//-------------------------------------------------------------------------------------------------
APIPNL_Result MApi_PNL_OverDriver_Enable(U8 bEnable)
{
    _PNL_ENTRY();
    MDrv_PNL_OverDriver_Enable(bEnable);
    _PNL_RETURN();
    return E_APIPNL_OK;
}


void MApi_MOD_PVDD_Power_Setting(U8 bIs2p5)
{
    _PNL_ENTRY();
    MDrv_MOD_PVDD_Power_Setting(bIs2p5);
    _PNL_RETURN();

}

//-------------------------------------------------------------------------------------------------
///-obosolte!! use MApi_PNL_Control_Out_Swing instead
//-------------------------------------------------------------------------------------------------
U8 MApi_PNL_SetDiffSwingLevel(U8 u8Swing_Level)
{
    return FALSE;
}

//-------------------------------------------------------------------------------------------------
/// Get real LPLL type from driver
/// @return U8
//-------------------------------------------------------------------------------------------------
U8 MApi_Pnl_Get_LPLL_Type(void)
{
    U8 u8value;
    if(m_PanelInfo.eLPLL_Type >= LINK_EXT)
        u8value = m_PanelInfo.eLPLL_TypeExt;
    else
        u8value = m_PanelInfo.eLPLL_Type;

    return u8value;
}

//} // Marked it by coverity_0553

U8 MApi_PNL_SetGammaTbl(    APIPNL_GAMMA_TYPE eGammaType,
                                 U8* pu8GammaTab[3],
                                APIPNL_GAMMA_MAPPEING_MODE Gamma_Map_Mode)
{
    U8 bResult = FALSE;
    m_ppAllGammaTbls = pu8GammaTab;

    //bResult = ( E_PNL_OK == MDrv_PNL_SetGammaTbl(
    //(DRVPNL_GAMMA_TYPE)eGammaType,
    //pu8GammaTab,
    //Gamma_Map_Mode == E_APIPNL_GAMMA_8BIT_MAPPING ? E_DRVPNL_GAMMA_8BIT_MAPPING : E_DRVPNL_GAMMA_10BIT_MAPPING ));

    if(bResult != TRUE)
    {
        PNL_ASSERT(0, "[%s] fails: ( APIPNL_GAMMA_TYPE: %d, APIPNL_GAMMA_MAPPEING_MODE %d) \n.", __FUNCTION__, eGammaType, Gamma_Map_Mode);
    }

    return bResult;
}


#endif
