////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#include <common.h>
#include <command.h>
#include <config.h>

#ifndef CONFIG_MS_NO_LOGO

#include <nand.h>
#include "drvNAND.h"
#include "displaylogo.h"
#include "pnlcfg.h"
#include "asm/arch/mach/ms_types.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"
#include "asm/arch/mach/cedric_r2.h"


#define MS_U16 unsigned short
typedef struct
{
    MS_U16 u16RedGain;      ///< ADC red gain
    MS_U16 u16GreenGain;    ///< ADC green gain
    MS_U16 u16BlueGain;     ///< ADC blue gain
    MS_U16 u16RedOffset;    ///< ADC red offset
    MS_U16 u16GreenOffset;  ///< ADC green offset
    MS_U16 u16BlueOffset;   ///< ADC blue offset
} APIXC_AdcGainOffsetSetting;

typedef struct
{
    APIXC_AdcGainOffsetSetting stAdcGainOffsetSetting;
    U8 u8AdcCalOK;
} MS_ADC_SETTING;


extern void* drvNAND_get_DrvContext_address(void);
extern U32 drvNAND_GetPartOffset(int u16_PartType, U32* u32_Offset);
extern U8  MApi_PNL_Init(PanelType *pSelPanelType);
extern U16 _getPanelWidth(void);
extern U16 _getPanelHeight(void);
extern void GOP_Setmode(int Screen,int Panel_Width,int  Panel_Height,U32  FB_Start_Addr );
extern void MHal_GPIO_Init(void);
extern void MHal_GPIO_Pad_Set(U8 u8IndexGPIO);
extern void MHal_GPIO_Pad_Oen(U8 u8IndexGPIO);
extern void MHal_GPIO_Set_High(U8 u8IndexGPIO);
extern void MHal_GPIO_Set_Low(U8 u8IndexGPIO);

static volatile PREG_MBX_st sg_pMbxRegs = NULL;

void adjustCorePower(EN_CORE_POWER_MODE level,U8 gpio1,U8 gpio2)
{
    MHal_GPIO_Pad_Set(gpio1);
    MHal_GPIO_Pad_Oen(gpio1);
    MHal_GPIO_Pad_Set(gpio2);
    MHal_GPIO_Pad_Oen(gpio2);
    switch(level)
    {
		case POWER_MODE_LOW:
			MHal_GPIO_Set_Low(gpio2);
			MHal_GPIO_Set_Low(gpio1);
			break;
		case POWER_MODE_MID:
			MHal_GPIO_Set_High(gpio2);
			MHal_GPIO_Set_Low(gpio1);
			break;
		case POWER_MODE_HIGH:
			MHal_GPIO_Set_High(gpio1);
			MHal_GPIO_Set_Low(gpio2);
			break;
		default:
			break;
    }
}

int readlogofile(size_t u32_Offset, u_char *buf,U32 lengh)
{
	size_t end = u32_Offset + lengh;
	size_t amount_loaded = 0;
	size_t blocksize,len;
	unsigned char *char_ptr;

    len=lengh;
	blocksize = nand_info[0].erasesize;
	if (!blocksize)
		return 1;

	while (amount_loaded < len && u32_Offset < end) {
		if (nand_block_isbad(&nand_info[0], u32_Offset)) {
			u32_Offset += blocksize;
		} else {
			char_ptr = &buf[amount_loaded];
			if (nand_read_skip_bad(&nand_info[0], u32_Offset,
					       &len, char_ptr))
				return 1;
			u32_Offset += blocksize;
			amount_loaded += len;
		}
	}

	if (amount_loaded != len)
		return 1;
	return 0;
}

int Logo_LoadBinFile(U32 *buf,U32 len)
{
    U32 u32_Offset=0;
	char cmd[128];
	int ret;
    snprintf(cmd, sizeof(cmd)-1, "emmc read.p 0x%08lX bootlogo 0x%08lx", (unsigned long)GOP_START_ADDR,len);
    ret=run_command(cmd, 0);
	if( ret==0)
    {
        printf("\n Load emmc logo bin files success !!\n");
    }
    else
	{
        NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();

        if(drvNAND_GetPartOffset(UNFD_PART_BOOTLOGO,&u32_Offset)!=UNFD_ST_SUCCESS)
        {
            printf("\n get UNFD_PART_BOOTLOGO error");
        }
        readlogofile(u32_Offset,(unsigned char *)buf,len);
    }
}


void Load_TS_YPBPR_VGA_CalValue()
{
	char cmd[128];
	int ret;
	int i;
	char *ptemp;

	MS_ADC_SETTING *stMsAdcSettingOut =NULL;

	APIXC_AdcGainOffsetSetting defaultYPBPRCalValue =
		{
		    5428,
		    3616,
		    5519,
            2048,
            256,
            2048,
	   };

    APIXC_AdcGainOffsetSetting * adcTemp = NULL;

	APIXC_AdcGainOffsetSetting defaultVGACalValue =
		{
		    8043,
		    7985,
		    7963,
            0,
            0,
            0,
	   };


    snprintf(cmd, sizeof(cmd)-1, "emmc read.p 0x%08lX misc 8A0", (unsigned long)TS_VALUE_TMP_BUF_ADDR);
    ret=run_command(cmd, 0);

	if( ret==0)
    {
    	ptemp = (char *)(TS_VALUE_TMP_BUF_ADDR + 0x800 + 0xA0);
    	*ptemp = '\0';

        snprintf((char *)TS_VALUE_BUF_ADDR, 0x80, "%s", (char *)(TS_VALUE_TMP_BUF_ADDR + 0x800));

		//snprintf((char *)YPBPR_CALVALUE_ADDR, 0x10, "%s", (char *)(TS_VALUE_TMP_BUF_ADDR + 0x880));

		stMsAdcSettingOut = (MS_ADC_SETTING *)(TS_VALUE_TMP_BUF_ADDR + 0x880);
		memcpy(YPBPR_CALVALUE_ADDR,&(stMsAdcSettingOut->stAdcGainOffsetSetting),sizeof(MS_ADC_SETTING));

		if(0 == stMsAdcSettingOut->u8AdcCalOK)
			{memcpy(YPBPR_CALVALUE_ADDR,&defaultYPBPRCalValue,sizeof(APIXC_AdcGainOffsetSetting));}


		//snprintf((char *)VGA_CALVALUE_ADDR, 0x10, "%s", (char *)(TS_VALUE_TMP_BUF_ADDR + 0x890));

		stMsAdcSettingOut = (MS_ADC_SETTING *)(TS_VALUE_TMP_BUF_ADDR + 0x890);
		memcpy(VGA_CALVALUE_ADDR,&(stMsAdcSettingOut->stAdcGainOffsetSetting),sizeof(MS_ADC_SETTING));

		if(0 == stMsAdcSettingOut->u8AdcCalOK)
			{memcpy(VGA_CALVALUE_ADDR,&defaultVGACalValue,sizeof(APIXC_AdcGainOffsetSetting));}


        OUTREGMSK16(0x1F206714,0x5AA5,0xFFFF);  ////MAILBOX 45
		OUTREGMSK16(0x1F206718,0x1111,0xFFFF);  ////MAILBOX 46
        //memset(cmd, 0, sizeof(cmd));
        //snprintf(cmd, sizeof(cmd)-1, "md.b 0x%08lX 880", (unsigned long)TS_VALUE_TMP_BUF_ADDR);
        //run_command(cmd, 0);
        printf("\n Load TS_YPBPR_VGA_CalValue files success !!\n");
    }
    else
	{
		OUTREGMSK16(0x1F206714,0x5AA5,0xFFFF);  ////MAILBOX 45
		OUTREGMSK16(0x1F206718,0x0000,0xFFFF);  ////MAILBOX 46
        printf("\n Load TS_YPBPR_VGA_CalValue files fail !!\n");
    }
}


void Load_PQbin()
{
	char cmd[128];
	int ret;
	char *ptemp;

    int i;
	ptemp = (char *)(PQBIN_BUF_ADDR + 0x200000);
    *ptemp = '\0';

    snprintf(cmd, sizeof(cmd)-1, "emmc read.p 0x%08lX pqbin 0x200000", (unsigned long)PQBIN_BUF_ADDR);
    ret=run_command(cmd, 0);

	if( ret==0)
    {
        printf("\n Load PQbin files success !!\n");
    }
    else
	{
        printf("\n Load PQbin files fail !!\n");
    }
}

void logo_display_main()
{
    int panelWidth,panelHeight;
	U32 buffersize=0;
    U8 *pData;
    U32 *pDisBuff;
	int XSize,YSize;
    int DisX,DisY;
    int x,y;
    U32 Data;
    U8 Offset, CDepth;
    int XOffset,YOffset;


	Load_TS_YPBPR_VGA_CalValue();
	Load_PQbin();

	if(DEVINFO_PANEL_HSD_070I_3459M == ms_devinfo_panel_type())
	{
        MApi_PNL_Init(tPanelHSD_070I_3459__MST786_HZCAR);
		memcpy(PNL_BUF_ADDR,tPanelHSD_070I_3459__MST786_HZCAR,sizeof(PanelType));
        printf("[PNL-1] tPanelHSD_070I_3459__MST786_HZCAR \n");
	}
	else if(DEVINFO_PANEL_HSD_070I_3210B == ms_devinfo_panel_type())
	{
        MApi_PNL_Init(tPanelHSD_070I_3210__MST786_HZCAR);
		memcpy(PNL_BUF_ADDR,tPanelHSD_070I_3210__MST786_HZCAR,sizeof(PanelType));
        printf("[PNL] tPanelHSD_070I_3210__MST786_HZCAR \n");
	}
	else if(DEVINFO_PANEL_HSD_062I_DW1 == ms_devinfo_panel_type())
	{
		MApi_PNL_Init(tPanelHSD_062I_DW1__MST786_SZDEMO);
		memcpy(PNL_BUF_ADDR,tPanelHSD_062I_DW1__MST786_SZDEMO,sizeof(PanelType));
		printf("[PNL] tPanelHSD_062I_DW1__MST786_SZDEMO \n");
	}
	else if(DEVINFO_PANEL_HSD_070I_DW2 == ms_devinfo_panel_type())
	{
        MApi_PNL_Init(tPanelHSD_070I_DW2_MST786_SZDEMO);
		memcpy(PNL_BUF_ADDR,tPanelHSD_070I_DW2_MST786_SZDEMO,sizeof(PanelType));
        printf("[PNL] tPanelHSD_070I_DW2_MST786_SZDEMO \n");
	}
	else if(DEVINFO_PANEL_HSD_LVDS_800480 == ms_devinfo_panel_type())
	{
        MApi_PNL_Init(tPanelHSD_070P_LVDS_800480);
		memcpy(PNL_BUF_ADDR,tPanelHSD_070P_LVDS_800480,sizeof(PanelType));
        printf("[PNL] tPanelHSD_070P_LVDS_800480 \n");
	}
	else if(DEVINFO_PANEL_N070ICG_LD1 == ms_devinfo_panel_type())
	{
        MApi_PNL_Init(tPanelN070ICG_LD1_MST154A);
		memcpy(PNL_BUF_ADDR,tPanelN070ICG_LD1_MST154A,sizeof(PanelType));
        printf("[PNL] tPanelN070ICG_LD1_MST154A \n");
	}
	else if(DEVINFO_PANEL_HSD_062I_DW2 == ms_devinfo_panel_type())
	{
        MApi_PNL_Init(tPanelHSD_062I_DW2__MST786_SZDEMO);
		memcpy(PNL_BUF_ADDR,tPanelHSD_062I_DW2__MST786_SZDEMO,sizeof(PanelType));
        printf("[PNL] tPanelHSD_062I_DW2 \n");
	}
	else if(DEVINFO_PANEL_HSD_070I_MODE2 == ms_devinfo_panel_type())
	{
        MApi_PNL_Init(tPanelHSD_070I_DW1__MST786_MODE2);
		memcpy(PNL_BUF_ADDR,tPanelHSD_070I_DW1__MST786_MODE2,sizeof(PanelType));
        printf("[PNL] tPanelHSD_070I_DW1__MST786_MODE2 \n");
	}
	else if(DEVINFO_PANEL_HSD_070P_FW3 == ms_devinfo_panel_type())
	{
        MApi_PNL_Init(tPanelHSD_070P_FW3_MST154A);
		memcpy(PNL_BUF_ADDR,tPanelHSD_070P_FW3_MST154A,sizeof(PanelType));
        printf("[PNL] tPanelHSD_070P_FW3_MST154A \n");
	}
	else
	{
        MApi_PNL_Init(tPanelHSD_070I_DW1_MST154A_D02A);
		memcpy(PNL_BUF_ADDR,tPanelHSD_070I_DW1_MST154A_D02A,sizeof(PanelType));
        printf("[PNL] tPanelHSD_070I_DW1_MST154A_D02A \n");
	}

    sg_pMbxRegs = (PREG_MBX_st)IO_ADDRESS(REG_MBX_BASE);
	sg_pMbxRegs->pq_bin_loaded = 1;

	panelWidth=_getPanelWidth();
	panelHeight=_getPanelHeight();

	buffersize=panelWidth*panelHeight*4+0xFF;///generally speaking,windows bmp offset value is ox36
	GOP_Setmode(0,panelWidth,panelHeight, GOP_START_ADDR);
    Logo_LoadBinFile((unsigned char *)GOP_START_ADDR,buffersize);
    DisX = panelWidth;
    DisY = panelHeight;

    pDisBuff = (U32 *)(GOP_DISP_ADDR);
    pData = (U8 *)(GOP_START_ADDR);

	    //get bmp info
    XSize = *(short *)&pData[0x12];  // this is DORD type but it is not DWORD align,
    YSize = *(short *)&pData[0x16];  // so i use short tye( val is less then short)
    Offset = *(short *)&pData[0x0a];
    CDepth = *(short *)&pData[0x1c];

    //will display offset,
    if( XSize < DisX )
        XOffset = ( DisX - XSize )/2;
    else
        XOffset = 0;

    if( YSize < DisY )
        YOffset =( DisY - YSize )/2;
    else
        YOffset = 0;

    memset( pDisBuff,0,DisX*DisY*4);
    pData +=Offset;
    pDisBuff += DisX*(DisY-YOffset);

    if(32 == CDepth)
	{
		for( y = 0; y <YSize ;y++ )
		{
			pDisBuff -= DisX;
			pDisBuff += XOffset;
			for( x = 0; x <XSize; x++ )
			{
				Data = _ARGBADJUST(*(pData+3),*(pData+2), *(pData+1), *pData);

				*pDisBuff++ = (U32)Data;
				pData +=4;
			}

			pDisBuff -=XOffset;
			pDisBuff -= XSize;
		}
	}
	else
	{
        printf("\n error logo format !!\n");
	}

	GOP_Pan_Display(GOP_DISP_ADDR);

	if(DEVINFO_BD_MST786_SZDEMO == ms_devinfo_board_type())
	{
		OUTREGMSK16(0x1F205610,0x0100,0x0100);  ////GPIO0
	}
	else if(DEVINFO_BD_MST786_CUSTOM10 == ms_devinfo_board_type())
	{
		OUTREGMSK16(0x1F205658,0x0001,0x0001);  ////GPIO35
	}
	else
	{
		OUTREGMSK16(0x1F205610,0x0100,0x0100);  ////GPIO0
	}

}

#endif