


#include "mdrv_gop.h"




U32  OALReadDeviceId(U32 devId)
{
    U16 tmp = 0;
    // need to refine for another SOC,weichun
    // tmp = INREG16(0xBF003C00+0x0000);
    tmp = 0x50;
    return tmp;
}




U16 GOP_MIU_BUS(U16 MIU_BUS)
{
    // A3: MIU Bus = 0x5;
    // C2: MIU Bus = 0x3;
    // C3: MIU Bus = 0x4;
    // Need to check with HW team

    U32 devId=0;

    devId=OALReadDeviceId(devId);

    if(DEVICE_AMBER3==devId)
    {
                GOPDBG("*****   devId is AMBER3   *****\r\n");

        MIU_BUS = 5;
    }
    else if(DEVICE_CEDRIC==devId)
    {
        GOPDBG("*****   devId is CEDRIC    *****\r\n");
        MIU_BUS = 0x4;
    }
    else
    {
        GOPDBG("*****   devId  UNKNOWN   *****\r\n");
        MIU_BUS = 0x4;

    }

    GOPDBG("GOP_MIU_BUS devId is %d BUS is %d \n",devId,MIU_BUS);


    return MIU_BUS;
}

#define MIU0_BUS_ADDR           0x40000000


unsigned long long ms_mem_bus_to_MIU(unsigned long long x)
{

	return ((x) - MIU0_BUS_ADDR);
}


void GOP_Setmode(int Screen,int Panel_Width,int  Panel_Height,U32  FB_Start_Addr )
{



// 1. GOP Hardware initail (need to check Chip top colok)
// 2. GOP fetch framebuffer information
// 3. GOP Display Setting
// 4. Initial Mux for Select GOP and Sclaer(MApi_XC_ip_sel_for_gop)



    U32 FB_add = FB_Start_Addr;
    U16 FB_WIDTH=Panel_Width;
    U16 FB_HEIGHT=Panel_Height;

    U16 DISPLAY_Format=ARGB8888;
    U16 FB_Bytes_Per_Pixel=4;

    U16 u16FrameBuffer_HSize=FB_WIDTH;//LCD_PANEL_WIDTH;
    U16 u16FrameBuffer_VSize= FB_HEIGHT;//mdrvgop_Get_Panel_Height();//mdrvgop_Get_Panel_HeightYRES;//LCD_PANEL_HEIGHT;
    U16 u16FrameBuffer_Bytes_Per_Pixel=FB_Bytes_Per_Pixel;

    U16 u16DispImage_HStart=0;
    U16 u16DispImage_HSize=FB_WIDTH;
    U16 u16DispImage_HEnd=u16DispImage_HStart+u16DispImage_HSize;

    U16 u16DispImage_VStart=000;
    U16 u16DispImage_VSize=FB_HEIGHT;
    U16 u16DispImage_VEnd=u16DispImage_VStart+u16DispImage_VSize;

    U32 s32BuffAddr=0;
    U32 u32TempVal=0;
    U16 MIU_BUS=0x04;

    U16 GOP_block_0=GOP_BANK_G0_0;
    U16 GOP_block_1=GOP_BANK_G0_1;
    U8 RColor=0xFF;
    U8 GColor=0xFF;
    U8 BColor=0xFF;

    FB_add = (U32)ms_mem_bus_to_MIU(FB_add);

    MIU_BUS= GOP_MIU_BUS(MIU_BUS);


    GOPDBG ("GOP_Setmode Screen is %x.\r\n",Screen);

    if (Screen == 0)
    {
        GOP_block_0=GOP_BANK_G0_0;
        GOP_block_1=GOP_BANK_G0_1;
    }
    else if (Screen == 1)
    {
        GOP_block_0=GOP_BANK_G1_0;
        GOP_block_1=GOP_BANK_G1_1;
    }

    GOPDBG ("GOP_Setmode: LCD_PANEL_WIDTH = [%d] \r\n",Panel_Width);
    GOPDBG ("GOP_Setmode: LCD_PANEL_HEIGHT = [%d] \r\n",Panel_Height);
    GOPDBG ("GOP_Setmode: FB_WIDTH = [%d] \r\n",FB_WIDTH);
    GOPDBG ("GOP_Setmode: FB_HEIGHT = [%d] \r\n",FB_HEIGHT);
    GOPDBG ("GOP_Setmode: FB_add = [%x] \r\n",FB_add);
    GOPDBG ("GOP_Setmode: FB_Start_Addr = [%x] \r\n",FB_Start_Addr);

    // GOP Global seting ; destination, Hsync
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_7F,GOP_block_0); // Change to bank GOP2G_0

    //enlarge the gop dma bandwidth & priority, make sure gop can always access mem when kernel calls dma_alloc_coherent
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_19,0xC7F0);

    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_00,GOP_SOFT_RESET|GOP_FIELD_INV); //
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_00,GWIN_display_mode_progress|GOP_HS_MASK|GOP_ALPHA_INV); // PerPixel alpha 0x3F, RGB8888, enable               
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_01,GOP_DST_OP|GOP_REGDMA_INTERVAL_START|GOP_REGDMA_INTERVAL_END);


    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_02,GOP_BLINK_DISABLE);


    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_0F,GOP_HSYNC_PIPE_DLY);//Hsync input pipe delay

	OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_24,((0x0000|(GColor<<2)|(GColor>>3))|(0x0000|(BColor<<3)|(BColor>>2))));
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_25,(0x0000|(RColor<<3)|(RColor>>2)));


 //  GOP : Display Area global seting

    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_30,Panel_Width >>1); // Stretch Window H size (unit:2 pixel)
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_31,Panel_Height); // Stretch window V size
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_32,GOP_STRETCH_HST); // Stretch Window H coordinate
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_34,GOP_STRETCH_VST); // Stretch Window V coordinate
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_35,GOP_STRETCH_HRATIO); // Stretch H ratio
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_36,GOP_STRETCH_VRATIO); // Stretch V ratio
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_38,GOP_STRETCH_HINI); // Stretch H start value
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_39,GOP_STRETCH_VINI); // Stretch V start value



    //1  GOP fetch framebuffer information




    // 1.0 framebuffer Data format
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_7F,GOP_block_1); // Change to bank GOP2G_1
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_00,DISPLAY_Format|GOP0_GWin0_Enable|GOP_PIXEL_ALPHA_EN);


    // 1.1 framebuffer starting address
    s32BuffAddr = FB_add;
    s32BuffAddr=s32BuffAddr&0xFFFFFFFF;
    s32BuffAddr=s32BuffAddr>>MIU_BUS; // 128-bit unit = 16 bytes


    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_01,(s32BuffAddr&0xFFFF)); // Ring block start Low address 0x21:16
    s32BuffAddr=s32BuffAddr>>0x10;
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_02,(s32BuffAddr&0xFFFF)); // Ring block start Hi address 0x22:6, page number

    // 1.2 framebuffer Pitch
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_09,(u16FrameBuffer_HSize*u16FrameBuffer_Bytes_Per_Pixel)>>MIU_BUS); // Ring block 0 line size

    // 1.4 framebuffer Size
    u32TempVal=(u16FrameBuffer_HSize*u16FrameBuffer_VSize*u16FrameBuffer_Bytes_Per_Pixel)>>MIU_BUS;
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_10,u32TempVal&0xFFFF);
    u32TempVal=u32TempVal>>0x10;
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_11,u32TempVal);

    // 1.5 frame buffer V start offset  (line)
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_0C,0);
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_0D,0);

    // 1.6 frame buffer H start offset  (pixel)
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_0E,0);


    //2  GOP Display Setting

    // 2.3 GWIN display area in panel : : H Start postion and end information
    u32TempVal=(u16DispImage_HStart*u16FrameBuffer_Bytes_Per_Pixel)>>MIU_BUS;
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_04,u32TempVal); // H start
    u32TempVal=(u16DispImage_HEnd*u16FrameBuffer_Bytes_Per_Pixel)>>MIU_BUS;
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_05,u32TempVal); // H end

    // 2.4 GWIN  display area in panel : V Start postion and end information
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_06,u16DispImage_VStart); // V start line
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_08,u16DispImage_VEnd); // V end line


    // 2: Display Area setting in Panel

    CLRREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_7E,GOP_GOPG0_MUX_MASK);
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_7E,GOP_Layer1_Sel_GOP_Null|GOP_Layer0_Sel_GOP0);

    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_7F,GOP_BANK_all_bank_double_buffer|GOP_BANK_DOUBLE_WR); // Double Buffer Write

    GOPDBG ("GOP_Setmode: out \r\n");
}

void GOP_Pan_Display(U32  FB_Start_Addr )
{

    U32 FB_add = FB_Start_Addr;

    U32 s32BuffAddr=0;
    U16 MIU_BUS=0x04;

    U16 GOP_block_1=GOP_BANK_G0_1;

    FB_add = (U32)ms_mem_bus_to_MIU(FB_add);
    MIU_BUS= GOP_MIU_BUS(MIU_BUS);

    GOPDBG ("GOP_Pan_Display: in \r\n");


    // 1  GOP fetch framebuffer information
    // 1.1 framebuffer starting address
    s32BuffAddr = FB_add;
    s32BuffAddr=s32BuffAddr&0xFFFFFFFF;
    s32BuffAddr=s32BuffAddr>>MIU_BUS; // 128-bit unit = 16 bytes


    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_7F,GOP_block_1); // Change to bank GOP2G_1
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_01,(s32BuffAddr&0xFFFF)); // Ring block start Low address 0x21:16
    s32BuffAddr=s32BuffAddr>>0x10;
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_02,(s32BuffAddr&0xFFFF)); // Ring block start Hi address 0x22:6, page number



    // Double Buffer Write
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_7F,0);
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_7F,GOP_BANK_DOUBLE_WR);
    OUTREG16(mdrv_BASE_REG_GOP_PA+REG_GOP_7F,0);

    GOPDBG ("GOP_Pan_Display: out \r\n");
}

