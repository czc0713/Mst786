
/******************************************************************************
 *-----------------------------------------------------------------------------
 *
 *    Copyright (c) 2011 MStar Semiconductor, Inc.  All rights reserved.
 *
 *-----------------------------------------------------------------------------
 * FILE NAME      gop_c2_reg.h
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



#define GET_REG8_ADDR(x, y)     (x+(y)*2)
#define GET_REG16_ADDR(x, y)    (x+(y)*4)



/* ========================================================================= */
/* Define HW base address */
#define COLUMBUS_BASE_REG_RIU_PA            (0x1F000000)

#define RIU_BASE                    (0xA0000000)
#define mdrv_BASE_REG_CHIPTOP_PA    (0x101E00)
#define mdrv_BASE_REG_GOP_PA       GET_REG16_ADDR(COLUMBUS_BASE_REG_RIU_PA, 0x080F80) // (0xA0006A00)
//#define mdrv_BASE_REG_GOP_PA        0x080F80*4 // (0xA0006A00)

#define mdrv_BASE_REG_GOP31_PA           GET_REG16_ADDR(COLUMBUS_BASE_REG_RIU_PA, 0x90600)
#define mdrv_BASE_REG_GOP01_PA           GET_REG16_ADDR(COLUMBUS_BASE_REG_RIU_PA, 0x90180)

#define mdrv_REG_GOP31_7E	            GET_REG16_ADDR(mdrv_BASE_REG_GOP31_PA,0x7E)
#define mdrv_REG_GOP31_7F	            GET_REG16_ADDR(mdrv_BASE_REG_GOP31_PA,0x7F)
#define mdrv_REG_GOP01_WIN              GET_REG16_ADDR(mdrv_BASE_REG_GOP01_PA,0x00)
#define mdrv_REG_GOP31_WIN              GET_REG16_ADDR(mdrv_BASE_REG_GOP31_PA,0x00)
#define mdrv_REG_GOP01_ALPHA            GET_REG16_ADDR(mdrv_BASE_REG_GOP01_PA,0x0A)
#define mdrv_REG_GOP01_7F	            GET_REG16_ADDR(mdrv_BASE_REG_GOP01_PA,0x7F)
#define mdrv_REG_GOP31_ALPHA            GET_REG16_ADDR(mdrv_BASE_REG_GOP31_PA,0x0A)

#define     mdrv_GWin0_Disable   (0xFFFE)
#define		mdrv_GWIN0_Enable	(0x1)

#define mdrv_ENABLE_CONST_ALPHA	(~(0x1<<14))

//for rtk
#define CEDRIC_BASE_REG_MBX_PA			GET_REG16_ADDR(COLUMBUS_BASE_REG_RIU_PA, 0x81980)
#define REG_MBX_BASE    				GET_REG16_ADDR(CEDRIC_BASE_REG_MBX_PA,0X40)
#define REG_MBX_RX    					GET_REG16_ADDR(CEDRIC_BASE_REG_MBX_PA,0x42) ////CARBACKING_MBX
#define REG_MBX_TX    					GET_REG16_ADDR(CEDRIC_BASE_REG_MBX_PA,0x41) ////HOSTKEEPER_MBX
#define CAR_BACK_STATUS_SHIFT           2
// 1 for forward, 0 for back
#define CAR_BACK_STATUS_MASK            0x04



/* ======================================================================== */
/* refer to drvop2.h and drvgop.h */
#define BK_REG(reg)                   ((reg) * 4)

/* -------------------------------------------------------------------------
 * CHIPTOP_OFFSET
 * -------------------------------------------------------------------------
 */

// Set odclk         /*h'2A CLK_GOPG0 clock control */

 #define REG_CHIPTOP_2A     BK_REG(0x2A)


//------------------------------------------------------------------------------
// GOP
//------------------------------------------------------------------------------






/* ----------------------------------------------------------------------------
 * GOP_OFFSET
 * ----------------------------------------------------------------------------
 */
#define REG_GOP_00                BK_REG(0x00)
#define REG_GOP_01                BK_REG(0x01)
#define REG_GOP_02                BK_REG(0x02)
#define REG_GOP_03                BK_REG(0x03)
#define REG_GOP_04                BK_REG(0x04)
#define REG_GOP_05                BK_REG(0x05)
#define REG_GOP_06                BK_REG(0x06)
#define REG_GOP_07                BK_REG(0x07)
#define REG_GOP_08                BK_REG(0x08)
#define REG_GOP_09                BK_REG(0x09)
#define REG_GOP_0A                BK_REG(0x0A)
#define REG_GOP_0B                BK_REG(0x0B)
#define REG_GOP_0C                BK_REG(0x0C)
#define REG_GOP_0D                BK_REG(0x0D)
#define REG_GOP_0E                BK_REG(0x0E)
#define REG_GOP_0F                BK_REG(0x0F)


#define REG_GOP_10                BK_REG(0x10)
#define REG_GOP_11                BK_REG(0x11)
#define REG_GOP_12                BK_REG(0x12)
#define REG_GOP_13                BK_REG(0x13)
#define REG_GOP_14                BK_REG(0x14)
#define REG_GOP_15                BK_REG(0x15)
#define REG_GOP_16                BK_REG(0x16)
#define REG_GOP_17                BK_REG(0x17)
#define REG_GOP_18                BK_REG(0x18)
#define REG_GOP_19                BK_REG(0x19)
#define REG_GOP_1A                BK_REG(0x1A)
#define REG_GOP_1B                BK_REG(0x1B)
#define REG_GOP_1C                BK_REG(0x1C)
#define REG_GOP_1D                BK_REG(0x1D)
#define REG_GOP_1E                BK_REG(0x1E)
#define REG_GOP_1F                BK_REG(0x1F)

#define REG_GOP_20                BK_REG(0x20)
#define REG_GOP_21                BK_REG(0x21)
#define REG_GOP_22                BK_REG(0x22)
#define REG_GOP_23                BK_REG(0x23)
#define REG_GOP_24                BK_REG(0x24)
#define REG_GOP_25                BK_REG(0x25)
#define REG_GOP_26                BK_REG(0x26)
#define REG_GOP_27                BK_REG(0x27)
#define REG_GOP_28                BK_REG(0x28)
#define REG_GOP_29                BK_REG(0x29)
#define REG_GOP_2A                BK_REG(0x2A)
#define REG_GOP_2B                BK_REG(0x2B)
#define REG_GOP_2C                BK_REG(0x2C)
#define REG_GOP_2D                BK_REG(0x2D)
#define REG_GOP_2E                BK_REG(0x2E)
#define REG_GOP_2F                BK_REG(0x2F)


#define REG_GOP_30                BK_REG(0x30)
#define REG_GOP_31                BK_REG(0x31)
#define REG_GOP_32                BK_REG(0x32)
#define REG_GOP_33                BK_REG(0x33)
#define REG_GOP_34                BK_REG(0x34)
#define REG_GOP_35                BK_REG(0x35)
#define REG_GOP_36                BK_REG(0x36)
#define REG_GOP_37                BK_REG(0x37)
#define REG_GOP_38                BK_REG(0x38)
#define REG_GOP_39                BK_REG(0x39)
#define REG_GOP_3A                BK_REG(0x3A)
#define REG_GOP_3B                BK_REG(0x3B)
#define REG_GOP_3C                BK_REG(0x3C)
#define REG_GOP_3D                BK_REG(0x3D)
#define REG_GOP_3E                BK_REG(0x3E)
#define REG_GOP_3F                BK_REG(0x3F)


#define REG_GOP_40                BK_REG(0x40)
#define REG_GOP_41                BK_REG(0x41)
#define REG_GOP_42                BK_REG(0x42)
#define REG_GOP_43                BK_REG(0x43)
#define REG_GOP_44                BK_REG(0x44)
#define REG_GOP_45                BK_REG(0x45)
#define REG_GOP_46                BK_REG(0x46)
#define REG_GOP_47                BK_REG(0x47)
#define REG_GOP_48                BK_REG(0x48)
#define REG_GOP_49                BK_REG(0x49)
#define REG_GOP_4A                BK_REG(0x4A)
#define REG_GOP_4B                BK_REG(0x4B)
#define REG_GOP_4C                BK_REG(0x4C)
#define REG_GOP_4D                BK_REG(0x4D)
#define REG_GOP_4E                BK_REG(0x4E)
#define REG_GOP_4F                BK_REG(0x4F)


#define REG_GOP_50                BK_REG(0x50)
#define REG_GOP_51                BK_REG(0x51)
#define REG_GOP_52                BK_REG(0x52)
#define REG_GOP_53                BK_REG(0x53)
#define REG_GOP_54                BK_REG(0x54)
#define REG_GOP_55                BK_REG(0x55)
#define REG_GOP_56                BK_REG(0x56)
#define REG_GOP_57                BK_REG(0x57)
#define REG_GOP_58                BK_REG(0x58)
#define REG_GOP_59                BK_REG(0x59)
#define REG_GOP_5A                BK_REG(0x5A)
#define REG_GOP_5B                BK_REG(0x5B)
#define REG_GOP_5C                BK_REG(0x5C)
#define REG_GOP_5D                BK_REG(0x5D)
#define REG_GOP_5E                BK_REG(0x5E)
#define REG_GOP_5F                BK_REG(0x5F)


#define REG_GOP_60                BK_REG(0x60)
#define REG_GOP_61                BK_REG(0x61)
#define REG_GOP_62                BK_REG(0x62)
#define REG_GOP_63                BK_REG(0x63)
#define REG_GOP_64                BK_REG(0x64)
#define REG_GOP_65                BK_REG(0x65)
#define REG_GOP_66                BK_REG(0x66)
#define REG_GOP_67                BK_REG(0x67)
#define REG_GOP_68                BK_REG(0x68)
#define REG_GOP_69                BK_REG(0x69)
#define REG_GOP_6A                BK_REG(0x6A)
#define REG_GOP_6B                BK_REG(0x6B)
#define REG_GOP_6C                BK_REG(0x6C)
#define REG_GOP_6D                BK_REG(0x6D)
#define REG_GOP_6E                BK_REG(0x6E)
#define REG_GOP_6F                BK_REG(0x6F)


#define REG_GOP_70                BK_REG(0x70)
#define REG_GOP_71                BK_REG(0x71)
#define REG_GOP_72                BK_REG(0x72)
#define REG_GOP_73                BK_REG(0x73)
#define REG_GOP_74                BK_REG(0x74)
#define REG_GOP_75                BK_REG(0x75)
#define REG_GOP_76                BK_REG(0x76)
#define REG_GOP_77                BK_REG(0x77)
#define REG_GOP_78                BK_REG(0x78)
#define REG_GOP_79                BK_REG(0x79)
#define REG_GOP_7A                BK_REG(0x7A)
#define REG_GOP_7B                BK_REG(0x7B)
#define REG_GOP_7C                BK_REG(0x7C)
#define REG_GOP_7D                BK_REG(0x7D)
#define REG_GOP_7E                BK_REG(0x7E)
#define REG_GOP_7F                BK_REG(0x7F)



// Bank 0
// REG_GOP_00
#define GOP_SOFT_RESET            0x0001
#define GOP_VS_INV                0x0002
#define GOP_HS_INV                0x0004
#define GOP_DISPLAY_MODE          0x0008
#define GOP_FIELD_INV             0x0010
#define GOP_TEST_PATTENR_MODE     0x0040
#define GOP_5541_EN               0x0080  // RGB5541 alpha mask mode enable, only for RGB1555 Data type
#define GOP_REG_OUTPUT_READY      0x0100

#define GOP_GENSHOT_FAST          0x0200
#define GOP_YUVOUT                0x0400
#define GOP_TRANS_EN              0x0800

#define GOP_HS_MASK               0x4000
#define GOP_ALPHA_INV             0x8000

#define GOP_ALPHA_MASK            0x7F00
#define GOP_ALPHA                 0x3F00
#define GOP_PIXEL_ALPHA_EN        0x4000

// REG_GOP_01
#define GOP_DST_OP                0x0002
#define GOP_DST_MVOP              0x0003
#define GOP_DMA_LEN_8             0x0000
#define GOP_DMA_LEN_16            0x0004
#define GOP_DMA_LEN_32            0x0008
#define GOP_DMA_LEN_ALL           0x000C
#define GOP_MIU_TSH_1             0x0010
#define GOP_GWIN1_PINPON_MODE     0x0080
#define GOP_REGDMA_INTERVAL_START 0x0300
#define GOP_REGDMA_INTERVAL_END   0x5000

// REG_GOP_02
#define GOP_BLINK_DISABLE         0x0000
#define GOP_BLINK_ENABLE          0x8000

// REG_GOP_06
#define GOP_REGDMA_END            0x0034

// REG_GOP_07
#define GOP_REGDMA_STR            0x0000

//REG_GOP_0F
//#define GOP_HSYNC_PIPE_DLY        0x000B

// REG_GOP_20
#define GOP_GWIN_HIGH_PRI_0       0x0000
#define GOP_GWIN_HIGH_PRI_1       0x0001


// REG_GOP_32
#ifdef DISP_LCM_KDO35
#define GOP_STRETCH_HST           0x002A
#else
#define GOP_STRETCH_HST           0x0000
#endif

// REG_GOP_34
#define GOP_STRETCH_VST           0x0000

// REG_GOP_35
#define GOP_STRETCH_HRATIO       0x1000

// REG_GOP_36
#define GOP_STRETCH_VRATIO       0x1000

// REG_GOP_38
#define GOP_STRETCH_HINI       0x0000

// REG_GOP_39
#define GOP_STRETCH_VINI       0x0000

// REG_GOP_7E
#define GOP_GOPG0_MUX_MASK       0x0003
#define GOP_GOPG0_MUX_4G       0x0000
#define GOP_GOPG0_MUX_2G       0x0001

#define GOP_Layer0_Sel_GOP0 0x0000
#define GOP_Layer0_Sel_GOP1 0x0001
#define GOP_Layer0_Sel_GOP2 0x0002
#define GOP_Layer0_Sel_GOP3 0x0003
#define GOP_Layer0_Sel_GOP_Null 0x0004


#define GOP_Layer1_Sel_GOP0 (0x0000)<<3
#define GOP_Layer1_Sel_GOP1 (0x0001)<<3
#define GOP_Layer1_Sel_GOP2 (0x0002)<<3
#define GOP_Layer1_Sel_GOP3 (0x0003)<<3
#define GOP_Layer1_Sel_GOP_Null (0x0004)<<3

#define GOP_Layer2_Sel_GOP0 (0x0000)<<6
#define GOP_Layer2_Sel_GOP1 (0x0001)<<6
#define GOP_Layer2_Sel_GOP2 (0x0002)<<6
#define GOP_Layer2_Sel_GOP3 (0x0003)<<6
#define GOP_Layer2_Sel_GOP_Null (0x0004)<<6


#define GOP_Layer3_Sel_GOP0 (0x0000)<<9
#define GOP_Layer3_Sel_GOP1 (0x0001)<<9
#define GOP_Layer3_Sel_GOP2 (0x0002)<<9
#define GOP_Layer3_Sel_GOP3  (0x0003)<<9
#define GOP_Layer3_Sel_GOP_Null (0x0004)<<9

// REG_GOP_7F
#define GOP_BANK_MASK           0x0007
#define GOP_BANK_4G0           0x0000
#define GOP_BANK_4G1           0x0001
#define GOP_BANK_2G0           0x0003
#define GOP_BANK_2G1           0x0004
#define GOP_BANK_GPD           0x0006
#define GOP_BANK_DOUBLE_WR       0x0100
#define GOP_BANK_FORCE_WR       0x0200

//fomrat define

#define  RGB1555_Blink 0x0000
#define  RGB565  0x0010
#define  ARGB4444  0x0020
#define  _2266     0x0030
#define  _8Bit_Palette 0x0040
#define  ARGB8888 0x0050
#define  ARGB1555  0x0060
#define  ABGR8888  0x0070
#define  RGB1555_UV7Y8 0x0080
#define  UV8Y8 0x0090
#define  RGBA5551  0x00A0
#define  RGBA4444  0x00B0

//------------------------------------------------------------------------------
// GOP
//------------------------------------------------------------------------------
// Bank 0
// REG_GOP_00
#define GOP_SOFT_RESET            0x0001
#define GOP_VS_INV                0x0002
#define GOP_HS_INV                0x0004
#define GOP_DISPLAY_MODE          0x0008
#define GOP_FIELD_INV             0x0010
#define GOP_TEST_PATTENR_MODE     0x0040
#define GOP_5541_EN               0x0080  // RGB5541 alpha mask mode enable, only for RGB1555 Data type
#define GOP_REG_OUTPUT_READY      0x0100

#define GOP_GENSHOT_FAST          0x0200
#define GOP_YUVOUT                0x0400
#define GOP_RGB_Transparent_color_enable              0x0800

#define GOP_HS_MASK               0x4000
#define GOP_ALPHA_INV             0x8000

#define GOP_ALPHA_MASK            0x7F00
#define GOP_ALPHA                 0x3F00
#define GOP_PIXEL_ALPHA_EN        0x4000

#define GWIN_display_mode_progress 0x0008
#define GWIN_display_mode_interlace 0x0000


// REG_GOP_01
#define GOP_DST_IP_MAIN           0x0000
#define GOP_DST_IP_SUB            0x0001
#define GOP_DST_OP                0x0002
#define GOP_DST_MVOP              0x0003
#define GOP_DMA_LEN_8             0x0000
#define GOP_DMA_LEN_16            0x0004
#define GOP_DMA_LEN_32            0x0008
#define GOP_DMA_LEN_ALL           0x000C
#define GOP_MIU_TSH_1             0x0010
#define GOP_REGDMA_INTERVAL_START 0x0300
#define GOP_REGDMA_INTERVAL_END   0x5000

// REG_GOP_02
#define GOP_BLINK_DISABLE         0x0000
#define GOP_BLINK_ENABLE          0x8000

// REG_GOP_06
#define GOP_REGDMA_END            0x0034

// REG_GOP_07
#define GOP_REGDMA_STR            0x0000

//REG_GOP_0F


#define AMBER3_GOP_HSYNC_PIPE_DLY        0x00E5
#define CEDRIC_GOP_HSYNC_PIPE_DLY        0x0070
#define GOP_HSYNC_PIPE_DLY_FOR_IP_SUB    0x0080


#define GOP_HSYNC_PIPE_DLY CEDRIC_GOP_HSYNC_PIPE_DLY




#define GOP_HSYNC_PIPE_DLY_FOR_IP_SUB  0x0080

// REG_GOP_20
#define GOP_GWIN_HIGH_PRI_0       0x0000
#define GOP_GWIN_HIGH_PRI_1       0x0001


// REG_GOP_32
#ifdef DISP_LCM_KDO35
#define GOP_STRETCH_HST 0x002B
#else
#define GOP_STRETCH_HST 0x0000
#endif

// REG_GOP_34
#define GOP_STRETCH_VST 0x0000

// REG_GOP_35
#define GOP_STRETCH_HRATIO 0x1000

// REG_GOP_36
#define GOP_STRETCH_VRATIO 0x1000

// REG_GOP_38
#define GOP_STRETCH_HINI 0x0000

// REG_GOP_39
#define GOP_STRETCH_VINI 0x0000

// REG_GOP_7E
#define GOP_GOPG0_MUX_MASK 0x0003
#define GOP_GOPG0_MUX_4G 0x0000
#define GOP_GOPG0_MUX_2G 0x0001

#define GOP_Layer0_Sel_GOP0 0x0000
#define GOP_Layer0_Sel_GOP1 0x0001
#define GOP_Layer0_Sel_GOP2 0x0002
#define GOP_Layer0_Sel_GOP3 0x0003
#define GOP_Layer0_Sel_GOP_Null 0x0004


#define GOP_Layer1_Sel_GOP0 (0x0000)<<3
#define GOP_Layer1_Sel_GOP1 (0x0001)<<3
#define GOP_Layer1_Sel_GOP2 (0x0002)<<3
#define GOP_Layer1_Sel_GOP3 (0x0003)<<3
#define GOP_Layer1_Sel_GOP_Null (0x0004)<<3

#define GOP_Layer2_Sel_GOP0 (0x0000)<<6
#define GOP_Layer2_Sel_GOP1 (0x0001)<<6
#define GOP_Layer2_Sel_GOP2 (0x0002)<<6
#define GOP_Layer2_Sel_GOP3 (0x0003)<<6
#define GOP_Layer2_Sel_GOP_Null (0x0004)<<6


#define GOP_Layer3_Sel_GOP0 (0x0000)<<9
#define GOP_Layer3_Sel_GOP1 (0x0001)<<9
#define GOP_Layer3_Sel_GOP2 (0x0002)<<9
#define GOP_Layer3_Sel_GOP3  (0x0003)<<9
#define GOP_Layer3_Sel_GOP_Null (0x0004)<<9



// REG_GOP_7F
#define GOP_BANK_MASK 0x0007
#define GOP_BANK_4G0 0x0000
#define GOP_BANK_4G1 0x0001
#define GOP_BANK_2G0 0x0003
#define GOP_BANK_2G1 0x0004
#define GOP_BANK_GPD 0x0006
#define GOP_BANK_DOUBLE_WR 0x0100
#define GOP_BANK_FORCE_WR 0x0200

// REG_GOP_7F
#define GOP_BANK_G0_0           0x0000
#define GOP_BANK_G0_1         0x0001
#define GOP_BANK_G1_0           0x0003
#define GOP_BANK_G1_1          0x0004
#define GOP_BANK_G2_0           0x0006
#define GOP_BANK_G2_1           0x0007
#define GOP_BANK_G3_0           0x0009
#define GOP_BANK_G3_1           0x000A
#define GOP_BANK_GPD           0x0006
#define GOP_BANK_DOUBLE_WR       0x0100
#define GOP_BANK_FORCE_WR       0x0200
#define GOP_BANK_all_bank_double_buffer       0x0400

// GOP0 Bank 1
// REG_GOP_00
#define GOP0_GWin0_Enable           0x0001

// REG_GOP_20
#define GOP0_GWin1_Enable           0x0001


//------------------------------------------------------------------------------
// OP2
//------------------------------------------------------------------------------
#define BANK_RESET 0x00
#define BANK_GOPINT 0x00
#define BANK_IP1F2 0x01
#define BANK_IP2F2 0x02
#define BANK_OPM 0x05
#define BANK_DNR 0x06
#define BANK_OP1 0x07
#define BANK_ELA 0x0B
#define BANK_ACE  0x0C
#define BANK_HD 0x0E
#define BANK_S_VOP 0x0F
#define BANK_VOP 0x10
#define BANK_VIP 0x18
#define BANK_VPS 0x19
#define BANK_DLC 0x1A

#ifdef DISP_LCM_KDO35
#define OP2_HSYNC_START (18)
#define OP2_HSYNC_END (16)
#define OP2_VSYNC_START (3)
#define OP2_VSYNC_END (3)
#define OP2_DE_HSTART (3) //(45)
#define OP2_DE_HEND (524)
#define OP2_DE_VSTART (16)
#define OP2_DE_VEND (287)
#define OP2_HTOTAL (524)
#define OP2_VTOTAL (287)
#define OP2_DISP_HSTART (45)
#define OP2_DISP_HEND (524)
#define OP2_DISP_VSTART (16)
#define OP2_DISP_VSIZE (287)

/* Control Signal Inverse */
#define OP2_OUTPUT_CLK_EN (0x0006)  /* Bit_3- DE inv, Bit_2- Hsync inv, Bit_1- Vsync inv , Bit_0- CLK inv */
/* LPLL setting */
#define OP2_LPLL_DIV_RATION_CTL (0x0101)
#define OP2_LPLL_SET_60MHZ_1 (0x0000)
#define OP2_LPLL_SET_60MHZ_2 (0x0090)

#define PNL_FREQ (900UL) // 9.00 MHZ

#endif
#ifdef DISP_LCM_NANO35
#define OP2_HSYNC_START (0)
#define OP2_HSYNC_END (16)
#define OP2_VSYNC_START (0)
#define OP2_VSYNC_END (3)
#define OP2_DE_HSTART (45)
#define OP2_DE_HEND (524)
#define OP2_DE_VSTART (16)
#define OP2_DE_VEND (287)
#define OP2_HTOTAL (524)
#define OP2_VTOTAL (287)
#define OP2_DISP_HSTART (45)
#define OP2_DISP_HEND (524)
#define OP2_DISP_VSTART (16)
#define OP2_DISP_VSIZE (287)

/* Control Signal Inverse */
#define OP2_OUTPUT_CLK_EN (0x0000)  /* Bit_3- DE inv, Bit_2- Hsync inv, Bit_1- Vsync inv , Bit_0- CLK inv */
/* LPLL setting */
#define OP2_LPLL_DIV_RATION_CTL (0x0101)
#define OP2_LPLL_SET_60MHZ_1 (0x0000)
#define OP2_LPLL_SET_60MHZ_2 (0x0090)

#define PNL_FREQ (900UL) // 9.00 MHZ

#endif

#ifdef DISP_LCM_HANSTAR50   /* 5 inch */
#define PANEL_HSYNC_WIDTH         48
#define PANEL_HSYNC_BACK_PORCH    40

#define PANEL_VSYNC_WIDTH         3
#define PANEL_VSYNC_BACK_PORCH    29

#define PANEL_HSTART             ((U16)PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
#define PANEL_VSTART            0 // ((U16)PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)
#define PANEL_WIDTH             800
#define PANEL_HEIGHT            480
#define PANEL_HTOTAL            928
#define PANEL_VTOTAL            525

#define OP2_HSYNC_END (PANEL_HSYNC_WIDTH -1)
#define OP2_VSYNC_START (PANEL_HEIGHT + 13 - 1)  /* put back */
#define OP2_VSYNC_END (OP2_VSYNC_START + PANEL_VSYNC_WIDTH -1)
#define OP2_DE_HSTART (PANEL_HSTART )
#define OP2_DE_HEND (PANEL_HSTART + PANEL_WIDTH -1)
#define OP2_DE_VSTART (PANEL_VSTART)
#define OP2_DE_VEND (PANEL_VSTART + PANEL_HEIGHT -1)
#define OP2_HTOTAL (PANEL_HTOTAL - 1)
#define OP2_VTOTAL (PANEL_VTOTAL - 1)
#define OP2_DISP_HSTART (OP2_DE_HSTART)
#define OP2_DISP_HSIZE (OP2_DE_HEND)
#define OP2_DISP_VSTART (OP2_DE_VSTART)
#define OP2_DISP_VSIZE (OP2_DE_VEND)

/* Control Signal Inverse */
#define OP2_OUTPUT_CLK_EN (0x0000)  /* Bit_3- DE inv, Bit_2- Hsync inv, Bit_1- Vsync inv , Bit_0- CLK inv */
/* LPLL setting */
#define OP2_LPLL_DIV_RATION_CTL (0x0103)
#define OP2_LPLL_SET_60MHZ_1 (0x3333)
#define OP2_LPLL_SET_60MHZ_2 (0x0073)  //(0x005B)

#define PNL_FREQ (900UL) // 9.00 MHZ

#endif /* #ifdef DISP_LCM_HANSTAR50 */

#ifdef DISP_LCM_NANO43
#define OP2_HSYNC_START (0)
#define OP2_HSYNC_END (16)
#define OP2_VSYNC_START (0)
#define OP2_VSYNC_END (3)
#define OP2_DE_HSTART (45)
#define OP2_DE_HEND (524)
#define OP2_DE_VSTART (16)
#define OP2_DE_VEND (287)
#define OP2_HTOTAL (524)
#define OP2_VTOTAL (287)
#define OP2_DISP_HSTART (45)
#define OP2_DISP_HEND (524)
#define OP2_DISP_VSTART (16)
#define OP2_DISP_VSIZE (287)

/* Control Signal Inverse */
#define OP2_OUTPUT_CLK_EN (0x0000)  /* Bit_3- DE inv, Bit_2- Hsync inv, Bit_1- Vsync inv , Bit_0- CLK inv */
/* LPLL setting */
#define OP2_LPLL_DIV_RATION_CTL (0x0101)
#define OP2_LPLL_SET_60MHZ_1 (0xEA36)
#define OP2_LPLL_SET_60MHZ_2 (0x005B)

#define PNL_FREQ (900UL) // 9.00 MHZ

#endif  /* #ifdef DISP_LCM_NANO43 */

#ifdef DISP_LCM_INNO50
#define PANEL_HSYNC_WIDTH         58
#define PANEL_HSYNC_BACK_PORCH    28

#define PANEL_VSYNC_WIDTH         3
#define PANEL_VSYNC_BACK_PORCH    29

#define PANEL_HSTART             ((U16)PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
#define PANEL_VSTART            0 // ((U16)PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)
#define PANEL_WIDTH             800
#define PANEL_HEIGHT            480
#define PANEL_HTOTAL            1006//920
#define PANEL_VTOTAL            510

#define OP2_HSYNC_START (0)
#define OP2_HSYNC_END (PANEL_HSYNC_WIDTH -1)
#define OP2_VSYNC_START (PANEL_HEIGHT + 13 - 1)  /* put back */
#define OP2_VSYNC_END (OP2_VSYNC_START + PANEL_VSYNC_WIDTH -1)
#define OP2_DE_HSTART (PANEL_HSTART )
#define OP2_DE_HEND (PANEL_HSTART + PANEL_WIDTH -1)//(PANEL_HSTART + PANEL_HTOTAL -1)
#define OP2_DE_VSTART (PANEL_VSTART)
#define OP2_DE_VEND (PANEL_VSTART + PANEL_HEIGHT -1)
#define OP2_HTOTAL (PANEL_HTOTAL - 1)
#define OP2_VTOTAL (PANEL_VTOTAL - 1)
#define OP2_DISP_HSTART (OP2_DE_HSTART)
#define OP2_DISP_HEND (OP2_DE_HEND)
#define OP2_DISP_HSIZE (OP2_DE_HEND)
#define OP2_DISP_VSTART (OP2_DE_VSTART)
#define OP2_DISP_VSIZE (OP2_DE_VEND)

/* Control Signal Inverse */
#define OP2_OUTPUT_CLK_EN (0x0001)  /* Bit_3- DE inv, Bit_2- Hsync inv, Bit_1- Vsync inv , Bit_0- CLK inv */
/* LPLL setting */
#define OP2_LPLL_DIV_RATION_CTL (0x0103)
#define OP2_LPLL_SET_60MHZ_1 (0x3333)
#define OP2_LPLL_SET_60MHZ_2 (0x0073)  //(0x005B)
#define PNL_FREQ (3000UL) // 30.00 MHZ
#endif //DISP_LCM_INNO50

#define OP2_OUTPUT_FREERUN_EN (0x0100)
#define OP2_DITH (0x0000)
#define OP2_PIP (0x0000)
#define OP2_DISP_MISC (0x0000)


#define OP2_LPLL_CONF (0x0043)
#define OP2_TEST_PATTERN (0xA686)
#define OP2_GOP_PATH_EN (0x0001)
#define OP2_DITH_2C (0x835A)

#define OP2_CKG_OP (0x0040)
#define SC_VOP_10 (0x0001)

#define HVSP_422TO444_MD    (0x0003)
#define HVSP_PAT_SEL        (0x0003)

#define DISP_CM_OFFSET  (0x8080)
#define DISP_CM_EN  (0x1001)
#define DISP_CM_11  (0x0CC4)
#define DISP_CM_12  (0x0950)
#define DISP_CM_13  (0x3FFE)
#define DISP_CM_21  (0x397E)
#define DISP_CM_22  (0x0950)
#define DISP_CM_23  (0x3CDE)
#define DISP_CM_31  (0x3FFE)
#define DISP_CM_32  (0x0950)
#define DISP_CM_33  (0x1024)

//------------------------------------------------------------------------------
// HVSP
//------------------------------------------------------------------------------
#define HVSP_HSC_LINEAR_EN              (0x01)
#define HVSP_VSC_LINEAR_EN              (0x01<<1)
#define HSVP_HSD_422TO444_DUPLICATE     (0x01<<9)
#define HSVP_HSD_422TO444_CENTER        (0x02<<9)
#define HSVP_HSD_422TO444_YC_COSIDE     (0x03<<9)

//------------------------------------------------------------------------------
// IPW
//------------------------------------------------------------------------------
#define IPW_MSYNC_2FB_EN        (0x01)
#define IPW_MSYNC_2FB_TOG       (0x00<<1)
#define IPW_MSYNC_2FB_BT656_FD  (0x01<<1)
#define IPW_MSYNC_2FB_FD_INV    (0x01<<8)
#define IPW_MSYNC_RBK_MD_R_W    (0x00<<12)
#define IPW_MSYNC_RBK_MD_R_NW   (0x01<<12)
#define IPW_MSYNC_RBK_MD_R_NR   (0x02<<12)

#define IPW_FORCE_OFF       (0x01<<2)
#define IPW_WBE_MSK         (0x01<<3)
#define IPW_INTERLACE_EN    (0x01<<7)
#define IPW_444_EN          (0x01<<8)
#define IPW_DATA_ORDER      (0x01<<10)
#define IPW_WRQ_RST         (0x01<<15)

//------------------------------------------------------------------------------
// IPM
//------------------------------------------------------------------------------
#define IPM_TRIG_SEL        (0x01<<0)
#define IPM_ENG_EN          (0x01<<1)
#define IPM_INTERLACE_OUT   (0x01<<7)
#define IPM_444_EN          (0x01<<8)
#define IPM_2FB_EN          (0x01<<15)
#define IPM_YC_422_ORDER    (BIT02)
//IPM REG19
#define IMP_TRIG_READ    BIT8//Trigger read request
#define IMP_EOF_STATUS    BIT9//Read : end of frame status
#define IMP_SW_SYNC_EN    BIT12//Enable Software synchronization mechanism

//------------------------------------------------------------------------------
// SC0 gating setting
//------------------------------------------------------------------------------
#define SC0_IPW_GATE_EN     (0x01)
#define SC0_IPM_GATE_EN     (0x01<<1)
#define SC0_IPM2_GATE_EN    (0x01<<2)
#define SC0_VIP_GATE_EN     (0x01<<3)
#define SC0_TCON_GATE_EN    (0x01<<4)
#define SC0_CPUIF_GATE_EN   (0x01<<5)
// SC0 REG02 setting
#define SC0_HVSP2DISP       (0x01)



//------------------------------------------------------------------------------
// Scaling ratio formular
//------------------------------------------------------------------------------
#define H_SD_Ratio(Input, Output)        ( (((U32)(Output)) * 1048576ul)/ ((U32)Input) + 1 )
#define V_SP_Ratio(Input, Output)        ( ((U32)(Input)-1) * 1048576ul / (((U32)Output)-1) + 1 )
#define H_SP_Ratio(Input, Output)        ( ((U32)(Input)) * 1048576ul / ((U32)Output) + 1 )


//------------------------------------------------------------------------------
// LPLL Post div
//------------------------------------------------------------------------------
#define LPLL_POST_DIV(x)        (1UL<<x)
#ifdef DISP_LCM_INNO50
#define LPLL_KP                 (3)
#else   //DISP_LCM_INNO50
#define LPLL_KP                 (5)
#endif  //DISP_LCM_INNO50

#if defined(DISP_CPUIF_AUO800x600) || defined(DISP_CPUIF_EINK800x600)
//------------------------------------------------------------------------------
// CPUIF register enable
//------------------------------------------------------------------------------
#define CPUIF_CS_ENABLE					0x0000
#define CPUIF_CS_DISABLE				0x0001

#define CPUIF_DC_COMMAND				0x0000
#define CPUIF_DC_DATA					0x0001

#define CPUIF_PHASE_COMMAND		       0x0000
#define CPUIF_PHASE_IMAGE				0x0002

#define CPUIF_BUS_RGB666_18				0x0000
#define CPUIF_BUS_RGB666_9				0x0001
#define CPUIF_BUS_RGB666_6				0x0002
#define CPUIF_BUS_RGB565_16				0x0003
#define CPUIF_BUS_RGB565_8				0x0004
#define CPUIF_BUS_RGB888_24				0x0005
#define CPUIF_BUS_RGB888_8				0x0006

#define CPUIF_CR_C0_R					0x0000
#define CPUIF_CR_C0_G					0x0001
#define CPUIF_CR_C0_B					0x0002
#define CPUIF_CR_C1_R					0x0000
#define CPUIF_CR_C1_G					0x0004
#define CPUIF_CR_C1_B					0x0008
#define CPUIF_CR_C2_R					0x0000
#define CPUIF_CR_C2_G					0x0010
#define CPUIF_CR_C2_B					0x0020

#define CPUIF_INIT_TRIG					0x0001

#define CPUIF_PATTERN_GEN_EN			0x0010
#endif /* #ifdef DISP_CPUIF_AUO800x600 */


// Chip Device ID
#define DEVICE_COLUMBUS2            (0x2E)
#define DEVICE_AMBER3               (0x47)
#define DEVICE_CEDRIC               (0x50)

