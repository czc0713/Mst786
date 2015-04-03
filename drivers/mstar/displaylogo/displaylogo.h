#ifndef _DISPLAY_MAIN_H
#define _DISPLAY_MAIN_H

#define _RGB16BIT565(r,g,b) (((r&0x00f8)<<8)|((g&0x00fC)<<3)|((b&0x00f8)>>3))
#define _ARGBADJUST(a,r,g,b) (((a|0xFF)<<24)|((r&0xff)<<16)|((g&0xff)<<8)|((b&0xff)))

#define GOP_START_ADDR  0x55400000
#define GOP_DISP_ADDR  0x55800000
#define TS_VALUE_BUF_ADDR 0x405FFC00
#define TS_VALUE_TMP_BUF_ADDR 0x4A000000
#define PQBIN_BUF_ADDR  0x40600000
#define VGA_CALVALUE_ADDR   0X405FFE10
#define YPBPR_CALVALUE_ADDR 0X405FFE00
#define PNL_BUF_ADDR 0X405FFF00 

typedef enum
{
    POWER_MODE_LOW,  ///1.08V
    POWER_MODE_MID,  ////1.15V
    POWER_MODE_HIGH,  ////1.22V
    POWER_MODE_UNKNOW,
}EN_CORE_POWER_MODE;
void logo_display_main();

#endif

