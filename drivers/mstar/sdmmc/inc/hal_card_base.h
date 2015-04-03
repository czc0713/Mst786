/***************************************************************************************************************
 *
 * FileName hal_card_base.h
 *     @author jeremy.wang (2010/10/14)
 * Desc:
 *	   This header file is independent.
 * 	   We could put different hal_card_base.h in different build code folder but the same name.
 *	   We want to distinguish between this and others settings but the same project.
 * 	   Specific Define is freedom for each project, but we need to put it at inner code of project define.
 *
 *	   For Base Setting:
 *     (1) BASE Project/OS/CPU and Specific Option Define
 * 	   (2) BASE TYPE Define
 * 	   (3) BASE OS/CPU Define and include file
 * 	   (4) BASE Specific Define for Each Project
 * 	   (5) BASE Project Define
 * 	   (6) BASE Debug System



 *
 * 	   P.S.	D_XX for define and D_XX__ two under line("__") to distinguish define and its define option.
 *
 ***************************************************************************************************************/

#ifndef __HAL_CARD_BASE_H
#define __HAL_CARD_BASE_H


//***********************************************************************************************************
// (1) BASE Project/OS/CPU and Specific Option Define
//***********************************************************************************************************

// Project Option Define
//-----------------------------------------------------------------------------------------------------------
#define D_PROJECT__C3	  1
#define D_PROJECT__C4	  2

// OS Option
//-----------------------------------------------------------------------------------------------------------
#define D_OS__LINUX       1
#define D_OS__UBOOT       2
#define D_OS__WINCE       3
#define D_OS__EBOOT       4

// CPU Option
//-----------------------------------------------------------------------------------------------------------
#define D_CPU__ARM        1
#define D_CPU__MIPS       2

// IC Version Option
//-----------------------------------------------------------------------------------------------------------
#define D_ICVER_00      0
#define D_ICVER_01      1
#define D_ICVER_02      2
#define D_ICVER_03      3
#define D_ICVER_04      4
#define D_ICVER_05      5
#define D_ICVER_06      6
#define D_ICVER_07      7
#define D_ICVER_08      8
#define D_ICVER_09      9

//***********************************************************************************************************
// (2) BASE Type Define
//***********************************************************************************************************

typedef unsigned char	BOOL_T;								  // 1 bytes
typedef unsigned char	U8_T;								  // 1 bytes
typedef unsigned short  U16_T;                                // 2 bytes
typedef unsigned int    U32_T;                                // 4 bytes
typedef unsigned long   ULONG_T;                              // 4 or 8 bytes
typedef signed char     S8_T;                                 // 1 byte
typedef signed short    S16_T;                                // 2 bytes
typedef signed int      S32_T;                                // 4 bytes
typedef signed long     SLONG_T;                              // 4 or 8 bytes

#define FALSE	0
#define TRUE	1

#define BIT00_T 0x0001
#define BIT01_T 0x0002
#define BIT02_T 0x0004
#define BIT03_T 0x0008
#define BIT04_T 0x0010
#define BIT05_T 0x0020
#define BIT06_T 0x0040
#define BIT07_T 0x0080
#define BIT08_T 0x0100
#define BIT09_T 0x0200
#define BIT10_T 0x0400
#define BIT11_T 0x0800
#define BIT12_T 0x1000
#define BIT13_T 0x2000
#define BIT14_T 0x4000
#define BIT15_T 0x8000
#define BIT16_T 0x00010000
#define BIT17_T 0x00020000
#define BIT18_T 0x00040000
#define BIT19_T 0x00080000
#define BIT20_T 0x00100000
#define BIT21_T 0x00200000
#define BIT22_T 0x00400000
#define BIT23_T 0x00800000
#define BIT24_T 0x01000000
#define BIT25_T 0x02000000
#define BIT26_T 0x04000000
#define BIT27_T 0x08000000
#define BIT28_T 0x10000000
#define BIT29_T 0x20000000
#define BIT30_T 0x40000000
#define BIT31_T 0x80000000

typedef enum
{
	EV_IP_FCIE1     = 0,
	EV_IP_FCIE2     = 1,
	EV_IP_FCIE3     = 2,

} IPEmType;

typedef enum
{
	EV_REG_BANK    = 0,
	EV_CIFC_BANK   = 1,
	EV_CIFD_BANK   = 2,

} IPBankEmType;

typedef enum
{
	EV_PORT_SD     = 0,
	EV_PORT_SDIO1  = 1,
	EV_PORT_SDIO2  = 2,
	EV_PORT_MS     = 3,
	EV_PORT_SMXD   = 4,
	EV_PORT_CF     = 5,
	EV_PORT_NANDT  = 6,

} PortEmType;

typedef enum
{
	EV_PAD1,
	EV_PAD2,
	EV_PAD3,
	EV_PAD4,
	EV_PAD5,
	EV_PAD6,
	EV_NPAD,

} PADEmType;

typedef enum
{
	EV_OK	= 0,
	EV_FAIL	= 1,

} RetEmType;


//***********************************************************************************************************
// (3) BASE OS/CPU Define and include file
//***********************************************************************************************************
#define D_OS                D_OS__UBOOT
#define D_CPU               D_CPU__ARM

//###########################################################################################################
#if (D_OS == D_OS__LINUX)
//###########################################################################################################
#include <linux/kernel.h>
//###########################################################################################################
#elif (D_OS == D_OS__UBOOT)
//###########################################################################################################
#include <common.h>
//###########################################################################################################
#endif
//###########################################################################################################


//***********************************************************************************************************
// (4) BASE Specific Define for Each Project
//***********************************************************************************************************
#define D_ICVER             D_ICVER_00

//***********************************************************************************************************
// (5) BASE Project Define
//***********************************************************************************************************

#if defined(CONFIG_ARCH_CEDRIC)
#define D_PROJECT           D_PROJECT__C3
#elif defined(CONFIG_ARCH_CHICAGO)
#define D_PROJECT           D_PROJECT__C4
#endif



//***********************************************************************************************************
// (6) BASE Debug System
//***********************************************************************************************************

//###########################################################################################################
#if (D_OS == D_OS__LINUX)
//###########################################################################################################
#define prtstring(s)	printk(s)
#define prtUInt(v) 		printk("%u", v)
#define prtU8(v)		printk("0x%02X", v)
#define prtU8Hex(v)		printk("0x%02X", v)
#define prtU16Hex(v)	printk("0x%04X", v)
#define prtU32Hex(v)	printk("0x%08X", v)
//###########################################################################################################
#elif (D_OS == D_OS__UBOOT)
//###########################################################################################################
#define prtstring(s)	printf(s)
#define prtUInt(v) 		printf("%u", v)
#define prtU8(v)		printf("0x%02X", v)
#define prtU8Hex(v)		printf("0x%02X", v)
#define prtU16Hex(v)	printf("0x%04X", v)
#define prtU32Hex(v)	printf("0x%08X", v)
//###########################################################################################################
#endif
//###########################################################################################################










#endif //End of __HAL_CARD_BASE_H


