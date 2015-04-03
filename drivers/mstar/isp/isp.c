//-----------------------------------------------------------------------------
//
// Copyright (c) 2008 MStar Semiconductor, Inc.  All rights reserved.
//
//-----------------------------------------------------------------------------
// FILE
//      isp.c
//
// DESCRIPTION
//      This header file defines the Physical Addresses (PA) of the base
//      registers for the System on Chip (SoC) components.
//
// HISTORY
//      2011.07.27  Lei.qin       Initial Version
//
// DESCRIPTION
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------
#if 0
#include "isp.h"
#ifdef	SPI_DRIVER_ISP
#include "SPI_PRIV.H"
#include "columbus_timer.h"
#endif
#ifdef	EBOOT
#include "loader.h"
#include "boot.h"
#endif
#endif
#include <command.h>
#include "isp.h"

//------------------------------------------------------------------------------
//  Macros
//------------------------------------------------------------------------------
#define DBG_PROG        1
#define DBG_DUMP        0
#define DBG_ERROR       1
#define DBG_MAIN(x)		x

//------------------------------------------------------------------------------
// structure
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------
// The max size of nor flash.
#define MAX_NOR_SIZE    0x1000000     //16M

// The layout of nor flash
//                   SIZE    Physical_Addr
//  ROM_0        30K     0x0000 - 0x7800
//  DEVINFO     2K      0X7800 - 0X8000
//  ROM_1        32K     0X8000 - 0X10000
//  Uboot         512K   0x10000 - 0x90000
//  RTK            1024k   0x90000 -  0x190000
//  BITMAP(UI)             0x190000 -






//#if  (SERFLASH_TYPE == SERFLASH_TYPE_MX25L3205)
    #define NUMBER_OF_SERFLASH_BLOCKS   64          // (1024)
    #define SERFLASH_BLOCK_SIZE         (64 * 1024) // (4*1024)
    #define SERFLASH_PAGE_SIZE          256         // page write size
    #define PAGE_PROG_CMD               0x02
    #define BLOCK_ERASE_CMD             0xD8
    #define CHIP_ERASE_CMD              0xC7

#define PAGE_PROG_CMD_SST               0xAF


#define LOU16(u32Val)   ((U16)(u32Val))
#define HIU16(u32Val)   ((U16)((u32Val) >> 16))

#define SERFLASH_SAFETY_FACTOR      10
#define SER_FLASH_TIME(_stamp, _msec)    { volatile int i; for (i=0; i<100; i++); _stamp =0xFFFFFFF;}
#define SER_FLASH_EXPIRE(_stamp)         ( !(--_stamp) )

//------------------------------------------------------------------------------
//  Variables
//------------------------------------------------------------------------------
//DWORD dwRomCodeLoc;
DWORD dwNorVendor;
DWORD dwSpiClk;
U32 g_PagePerSector;
U32 g_BlockSize;
U32 g_SectorCunt;
U8 g_WPRange;
#if 0
//------------------------------------------------------------------------------
//  Extern Variables
//------------------------------------------------------------------------------
extern volatile PREG_PMTOP_st g_pPmtopRegs;
extern volatile PREG_PMU_st g_pPmuRegs;
extern volatile PREG_ISP_st g_pIspRegs;
extern volatile PREG_FSP_st g_pFspRegs;
extern volatile PREG_QSPI_st g_pQspiRegs;
#ifdef SPI_DRIVER_ISP
extern volatile PREG_PMGPIO_st g_pPmGpio;
extern volatile PREG_PM_SLP_st g_pPmSlp;
#endif
#endif
//------------------------------------------------------------------------------
//  External Functions
//------------------------------------------------------------------------------
extern void CAPDELAY(U32 u32CpuCycle);
extern void MHal_GPIO_Init(void);
extern void MHal_GPIO_Pad_Set(U8 u8IndexGPIO);
extern void MHal_GPIO_Pad_Oen(U8 u8IndexGPIO);
extern void MHal_GPIO_Set_High(U8 u8IndexGPIO);
extern void MHal_GPIO_Set_Low(U8 u8IndexGPIO);

//------------------------------------------------------------------------------
//  Local Functions
//------------------------------------------------------------------------------
static BOOLEAN _MDrv_BusyWaitWriteCompletion(U32 timeout);

void __inline DumpBuff(U8 *dwAddr, DWORD Len)
{
    U32 u32i;

    for(u32i = 0; u32i < Len; u32i++)
        {
        if(u32i % 16 == 0)
            printf("\r\n");

        if(u32i % 1024 == 0)
            printf("%dK\r\n", u32i /1024);

        printf("%02X ", dwAddr[u32i]);
        }

    printf("\r\n");
}
//-------------------------------------------------------------------------------------------------
/// Initialize Serial Flash
/// @return None
/// @note
/// [NONOS_SUPPORT]
//-------------------------------------------------------------------------------------------------
void MDrv_SerFlash_Init(void)
{
    /* Note: Adjust clock source & freq according to the specific flash spec */
    /* the clock of DMA & Read via XIU */
    PIU_REG16(REG_PIU_SPI_CLK_SRC) &= ~(1<<5);  // Xtal
    PIU_REG16(REG_PIU_SPI_CLK_SRC)  = (PIU_REG16(REG_PIU_SPI_CLK_SRC) & ~0x1F) | 0x08; // 36 Mhz
    PIU_REG16(REG_PIU_SPI_CLK_SRC) |= (1<<5);   // clk_sel

    /* the clock of other operations */
    ISP_REG16(REG_ISP_SPI_CLKDIV) = SPI_CLK_DIV8; // (cpu clock / 8)
    ISP_REG16(REG_ISP_CHIP_SEL) = SFSH_CHIP_SEL_XIU | 0x01; // chip_sel, [0]:FLASH0, [1]:FLASH1

    /* SPI brand */
#if ((SERFLASH_TYPE == SERFLASH_TYPE_MX25L3205)||(SERFLASH_TYPE == SERFLASH_TYPE_MX25L6405)||(SERFLASH_TYPE == SERFLASH_TYPE_MX25L12805))
    ISP_REG16(REG_ISP_DEV_SEL) = BRAND_PMC; // PMC flash: MX flash is similar to PMC flash, except RDID command
#elif ((SERFLASH_TYPE == SERFLASH_TYPE_EN25B16)||(SERFLASH_TYPE == SERFLASH_TYPE_EN25B64)||(SERFLASH_TYPE == SERFLASH_TYPE_S25FL064A))
    ISP_REG16(REG_ISP_DEV_SEL) = BRAND_ST;  // ST flash: SPANSION flash is the same as ST flash
#endif
    /* endian */
    ISP_REG16(REG_ISP_SPI_ENDIAN) = 0; // little-endian
}


//-------------------------------------------------------------------------------------------------
// Wait for SPI Write Cmd Ready
// @return TRUE : succeed
// @return FALSE : fail before timeout
//-------------------------------------------------------------------------------------------------
static BOOL _MDrv_SerFlash_WaitWriteCmdRdy(void)
{
    while((INREG16(CEDRIC_BASE_REG_ISP_PA+REG_17) & 0x0001)== 0x00);
    return 1;
}

//-------------------------------------------------------------------------------------------------
// Wait for SPI Read Data Ready
// @return TRUE : succeed
// @return FALSE : fail before timeout
//-------------------------------------------------------------------------------------------------
static BOOL _MDrv_SerFlash_WaitReadDataRdy(void)
{
    while((ISP_REG16(REG_ISP_SPI_RD_DATARDY) & 0x0001) == 0x00);
    return 1;
}

//-------------------------------------------------------------------------------------------------
// Wait for Write/Erase to be done
// @return TRUE : succeed
// @return FALSE : fail before timeout
//-------------------------------------------------------------------------------------------------
static BOOL _MDrv_SerFlash_WaitWriteDone(void)
{
    BOOL bRet = FALSE;

    _MDrv_SerFlash_WaitWriteCmdRdy();

    do
    {
        ISP_REG16(REG_ISP_SPI_COMMAND) = CMD_READ_STATUS_REG;   // RDSR
        ISP_REG16(REG_ISP_SPI_RDREQ) = 0x01;                    // SPI read request

        _MDrv_SerFlash_WaitReadDataRdy();

        if ((ISP_REG16(REG_ISP_SPI_RDATA) & 0x01) == 0)       // WIP == 0, write done
        {
            bRet = TRUE;
            break;
        }
    } while (1);

    if (bRet == FALSE)
    {
        DBG_MAIN(printf("ERROR> wait for write to be done fail !!\r\n"));
    }

    ISP_REG16(REG_ISP_SPI_CECLR) = 1; // SPI CEB dis

    return bRet;
}

static BOOL _MDrv_SerFlash_WaitWriteDone_AAI(void)
{
    BOOL bRet = FALSE;

    _MDrv_SerFlash_WaitWriteCmdRdy();

    do
    {
        ISP_REG16(REG_ISP_SPI_COMMAND) = CMD_READ_STATUS_REG;   // RDSR
        ISP_REG16(REG_ISP_SPI_RDREQ) = 0x01;                    // SPI read request

        _MDrv_SerFlash_WaitReadDataRdy();

        if ((ISP_REG16(REG_ISP_SPI_RDATA) & 0x01) == 0)       // WIP == 0, write done
        {
            bRet = TRUE;
            break;
        }
    } while (1);

    if (bRet == FALSE)
    {
        DBG_MAIN(printf("ERROR> wait for write to be done fail !!\r\n"));
    }

    ISP_REG16(REG_ISP_SPI_CECLR) = 1; // SPI CEB dis

    return bRet;
}

//-------------------------------------------------------------------------------------------------
// Wait for SPI Write Data Ready
// @return TRUE : succeed
// @return FALSE : fail before timeout
//-------------------------------------------------------------------------------------------------
static BOOL _MDrv_SerFlash_WaitWriteDataRdy(void)
{
    while((ISP_REG16(REG_ISP_SPI_WR_DATARDY) & 0x0001) == 0x00);
    return 1;
}


//-------------------------------------------------------------------------------------------------
/// Write data to Serial Flash
/// @param  u32Addr \b IN: start address (4-B aligned)
/// @param  pu8Data \b IN: data to be written
/// @param  u32Size \b IN: size in Bytes (4-B aligned)
/// @return TRUE : succeed
/// @return FALSE : fail before timeout or illegal parameters
/// @note   Not allowed in interrupt context
/// @note
/// [NONOS_SUPPORT]
//-------------------------------------------------------------------------------------------------
BOOL MDrv_SerFlash_Write(U32 u32Addr, U8 *pu8Data, U32 u32Size)
{
    BOOL bRet = FALSE;
    U32 u32I;
    U32 u32Rem, u32WriteBytes;

    ISP_REG16(REG_ISP_PASSWORD) = 0xAAAA; // enable ISP

    u32Rem = u32Addr % SERFLASH_PAGE_SIZE;
    if (u32Rem)
    {
        u32WriteBytes = SERFLASH_PAGE_SIZE - u32Rem;
        if (u32Size < u32WriteBytes)
        {
            u32WriteBytes = u32Size;
        }

        _MDrv_SerFlash_WaitWriteCmdRdy();

        ISP_REG16(REG_ISP_SPI_COMMAND) = CMD_WRITE_ENABLE; // WREN
        ISP_REG16(REG_ISP_SPI_ADDR_L) = LOU16(u32Addr);
        ISP_REG16(REG_ISP_SPI_ADDR_H) = HIU16(u32Addr);
        _MDrv_SerFlash_WaitWriteCmdRdy();

        ISP_REG16(REG_ISP_SPI_COMMAND) = PAGE_PROG_CMD; // PAGE_PROG

        for (u32I = 0; u32I < u32WriteBytes; u32I++)
        {
            ISP_REG16(REG_ISP_SPI_WDATA) = pu8Data[u32I];
            _MDrv_SerFlash_WaitWriteDataRdy();
        }

        ISP_REG16(REG_ISP_SPI_CECLR) = 1; // SPI CEB dis

        bRet = _MDrv_SerFlash_WaitWriteDone();
        if ( bRet == TRUE )
        {
            u32Addr += u32WriteBytes;
            pu8Data += u32WriteBytes;
            u32Size -= u32WriteBytes;
        }
        else
        {
            DBG_MAIN(printf("INFO> write cmd wait write done fail\r\n"));
            goto MDrv_SerFlash_Write_return;
        }
    }

    while(u32Size)
    {
        if (u32Size > SERFLASH_PAGE_SIZE)
        {
            u32WriteBytes = SERFLASH_PAGE_SIZE; // write EEPROM_WRITE_BYTES_MAX bytes one time
        }
        else
        {
            u32WriteBytes = u32Size;
        }

        bRet = FALSE;
        _MDrv_SerFlash_WaitWriteCmdRdy();

        ISP_REG16(REG_ISP_SPI_COMMAND) = CMD_WRITE_ENABLE; // WREN
        ISP_REG16(REG_ISP_SPI_ADDR_L) = LOU16(u32Addr);
        ISP_REG16(REG_ISP_SPI_ADDR_H) = HIU16(u32Addr);
        _MDrv_SerFlash_WaitWriteCmdRdy();

        ISP_REG16(REG_ISP_SPI_COMMAND) = PAGE_PROG_CMD; // PAGE_PROG

        for (u32I = 0; u32I < u32WriteBytes; u32I++)
        {
            ISP_REG16(REG_ISP_SPI_WDATA) = pu8Data[u32I];
            _MDrv_SerFlash_WaitWriteDataRdy();
        }

        ISP_REG16(REG_ISP_SPI_CECLR) = 1; //SPI CEB dis
        bRet = _MDrv_SerFlash_WaitWriteDone();

        if (bRet == TRUE)
        {
            u32Addr += u32WriteBytes;
            pu8Data += u32WriteBytes;
            u32Size -= u32WriteBytes;
        }
        else
        {
            DBG_MAIN(printf("INFO> write cmd wait write done fail!\r\n"));
            goto MDrv_SerFlash_Write_return;
        }
    }

MDrv_SerFlash_Write_return:
    ISP_REG16(REG_ISP_SPI_CECLR) = 1; //SPI CEB dis
    ISP_REG16(REG_ISP_PASSWORD) = 0x5555; // disable ISP

    return bRet;
}

//------------------------------------------------------------------------------
//  Local Functions
//------------------------------------------------------------------------------
static U32 Read32( U32 u32Address)
{
    return( (U32)( *(volatile U32 *)(u32Address) ) );
}

static void Write32( U32 u32Address, U32 u32Data)
{
    *(volatile U32 *)(u32Address) = u32Data;
}

static U8 Read8( U32 u32Address)
{
    return( (U8)( *(volatile U8 *)(u32Address) ) );
}

static void Write8( U32 u32Address, U8 u32Data)
{
    *(volatile U8 *)(u32Address) = u32Data;
}

void MDrv_SpiSelectClock(U8 mcu_clkdiv)
{
	if (mcu_clkdiv < 2)
		mcu_clkdiv = 2;
	if (mcu_clkdiv > 10)
		mcu_clkdiv = 10;

	//Write32(ISP_REG_SPI_CLK_DIV, 1 << (mcu_clkdiv-2));
	OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_06, 1 << (mcu_clkdiv-2));
	//Write32(ISP_REG_ENDIAN_LITTLE, SPI_FLASH_IS_LITTLE_ENDIAN);
	//OUTREG32(&g_pIspRegs->u16REG_Device_Select, vendor);
}

void MDrv_InitialSPIFlash(U8 vendor, U8 mcu_clkdiv)
{
	// Enable the SPI
	// on-board nor flash
	OUTREG32(CEDRIC_BASE_REG_PM_TOP_PA+REG_0A, 0x01);

	MDrv_SpiSelectClock(mcu_clkdiv);

	OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_07, vendor);
	OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_0F, SPI_FLASH_IS_LITTLE_ENDIAN);
}

void MDrv_SPI_CE(U32 ce)
{
#if defined (AMBER3)
#if 0
	if(E_BD_MST032B_10AFX_A3 == OALReadBoardName())
	{
		//SPI chip select
		//00: Select external #1 SPI Flash
		//01: Select external #2 SPI Flash
		//10: Select external #3 SPI Flash
		//11: Reserved
		if((ce&0x03) == 0x02)
		{
			#ifdef SPI_DRIVER_ISP
			DBG_MAIN(printf("[INFO]CS2 LOW\r\n"));
			OUTREG16(CEDRIC_BASE_REG_QSPI_PA+REG_0A, 0x03);//sw to reserved

			OUTREG16(&g_pPmSlp->u16REG_GPIO_CHK, BIT01);//"reg_spicsz0_gpioreg[0e6a]#1 = 1"
			CLRREG16(&g_pPmGpio->reg_gpio_pm23, BIT00);//set as output
			CLRREG16(&g_pPmGpio->reg_gpio_pm23, BIT01);//pull low
			#endif
			dwSpiClk = SPI_SPEED_UART;
			OUTREG16(CEDRIC_BASE_REG_ISP_PA+REG_37, 0x00);
		}
		else if((ce&0x03) == 0x01)
		{
			DBG_MAIN(printf("[INFO]CS1 LOW\r\n"));
			OUTREG16(CEDRIC_BASE_REG_QSPI_PA+REG_0A, (ce&0x03));
			dwSpiClk = SPI_SPEED_GPS;
			OUTREG16(CEDRIC_BASE_REG_ISP_PA+REG_37, 0x01);
		}
		else
		{
			OUTREG16(CEDRIC_BASE_REG_QSPI_PA+REG_0A, (ce&0x03));
			dwSpiClk = SPI_SPEED_NOR;
			OUTREG16(CEDRIC_BASE_REG_ISP_PA+REG_37, 0x00);
		}
	}
	else if(E_BD_MST145A_D01A1_S == OALReadBoardName())
#endif		
	{
		//SPI chip select
		//00: Select external #1 SPI Flash
		//01: Select external #2 SPI Flash
		//10: Select external #3 SPI Flash
		//11: Reserved
		OUTREG16(CEDRIC_BASE_REG_QSPI_PA+REG_0A, (ce&0x03));
		dwSpiClk = SPI_SPEED_GPS;
	}
#endif
}

void MDrv_SPI_CE_Disable(U32 ce)
{
#if 0//defined (AMBER3)
	if(E_BD_MST032B_10AFX_A3 == OALReadBoardName())
	{
		//SPI chip select
		//00: Select external #1 SPI Flash
		//01: Select external #2 SPI Flash
		//10: Select external #3 SPI Flash
		//11: Reserved
		if((ce&0x03) == 0x02)
		{
			#ifdef SPI_DRIVER_ISP
			DBG_MAIN(printf("[INFO]CS0 HI\r\n"));
			OUTREG16(CEDRIC_BASE_REG_QSPI_PA+REG_0A, 0x03);//sw to reserved

			OUTREG16(&g_pPmSlp->u16REG_GPIO_CHK, BIT01);//"reg_spicsz0_gpioreg[0e6a]#1 = 1"
			CLRREG16(&g_pPmGpio->reg_gpio_pm23, BIT00);//set as output
			SETREG16(&g_pPmGpio->reg_gpio_pm23, BIT01);//pull high
			#endif
		}
		else
		{

		}
	}
	else if(E_BD_MST145A_D01A1_S == OALReadBoardName())
	{
		//SPI chip select
		//00: Select external #1 SPI Flash
		//01: Select external #2 SPI Flash
		//10: Select external #3 SPI Flash
		//11: Reserved

	}
#endif
}

// Wait for bit zero of register u8reg to turn on.
static BOOLEAN __MDrv_BusyWaitBit(U16 *u16reg, U32 timeout)
{
	U32 i;
	U8 v;

	// limit the timeout value in case it's too short and always timeout
	timeout = MAX( timeout, MASTER_CLOCK_HZ >> 9 );

	for (i=0; i<timeout; i++) {
		#ifdef	SPI_DRIVER_ISP
		//SimpleTimerLoopDelay(1);
		#endif
		v = (U8)INREG16(u16reg);
		if (v & 0x01) break;
	}
	//DBG_MAIN(1, (printf("delay%x\r\n"),i));
	return (i<timeout);
}

static BOOLEAN _MDrv_BusyWaitWriteCmdReady(S32 timeout)
{
	return __MDrv_BusyWaitBit(CEDRIC_BASE_REG_ISP_PA+REG_17, timeout);
}

static BOOLEAN _MDrv_BusyWaitReadDataReady(S32 timeout)
{
	return __MDrv_BusyWaitBit(CEDRIC_BASE_REG_ISP_PA+REG_15, timeout);
}

static BOOLEAN _MDrv_BusyWaitWriteDataReady(S32 timeout)
{
	return __MDrv_BusyWaitBit(CEDRIC_BASE_REG_ISP_PA+REG_16, timeout);
}

static BOOLEAN _MDrv_BusyWaitWriteDataDone(S32 timeout)
{
	return __MDrv_BusyWaitBit(CEDRIC_BASE_REG_FSP_PA+REG_0E, timeout);
}

//(3) Read Status Register (RDSR)
U8 GetRDSR()
{
	U8 status;

	ENABLE_ISP();

	if ( !_MDrv_BusyWaitWriteCmdReady(0))
		goto _End_BusyWaitWriteCompletion;

	// Issue 'read status register' command
	Write32(ISP_REG_COMMAND, ISP_READ_SR_COMMAND);

	// Issue request to read status register
	Write32(ISP_REG_READ_REQUEST, 0x0001);  // SPI read request
	if ( !_MDrv_BusyWaitReadDataReady( 0))
		goto _End_BusyWaitWriteCompletion;

	// Check status bit 0 (WIP)
	status = Read8(ISP_REG_READ_DATA);
	printf("[INFO] GetRDSR 0X%02X\r\n",status);

	CHIP_ENABLE_CLEAR();
	DISABLE_ISP();

	return status;

_End_BusyWaitWriteCompletion:
	printf("[ERROR] GetRDSR FAIL\r\n");

	CHIP_ENABLE_CLEAR();
	DISABLE_ISP();

	return 0;
}

BOOLEAN MDrv_ProtectSpiFlash(U8 range)
{
	BOOLEAN bRet = FALSE;

	ENABLE_ISP();

	if (!_MDrv_BusyWaitWriteCmdReady(0))
		goto _End_MDrv_ProtectSpiFlash;

	Write32( ISP_REG_COMMAND, SPI_FLASH_WRITE_ENABLE_COMMAND_ST );
	if (!_MDrv_BusyWaitWriteCmdReady(0))
		goto _End_MDrv_ProtectSpiFlash;

        if(dwNorVendor == SPIFLASH_VENDOR_SST)
        {
            Write32( ISP_REG_COMMAND, SPI_FLASH_WRSR_ENABLE_COMMAND_SST );
            if (!_MDrv_BusyWaitWriteCmdReady(0))
                goto _End_MDrv_ProtectSpiFlash;
        }

	Write32( ISP_REG_COMMAND, ISP_WRITE_SR_COMMAND );

	range &= 0x2F; // for all SPI flash vendors, only 3 bits (at most) are used
	Write32(ISP_REG_WRITE_DATA, (0x8000 | (range<<2)));  //[4:2] protect blocks

	bRet = _MDrv_BusyWaitWriteCompletion(0);

_End_MDrv_ProtectSpiFlash:

	CHIP_ENABLE_CLEAR();
	DISABLE_ISP();

	return bRet;
}

void HW_WP_Status(BOOL bStatus)
{
	U16 u16temp;
	
    MHal_GPIO_Pad_Set(69);
    MHal_GPIO_Pad_Oen(69);

	if(bStatus)
	{
	    MHal_GPIO_Set_High(69);
	}
	else
	{
	    MHal_GPIO_Set_Low(69);
	}
}

BOOL DisableWP(DWORD Len)
{
	BOOL bRet = FALSE;
	U32 u32I;
	U32 u32WriteBytes;

	//Pull high HW Write protection
	HW_WP_Status(1);

	// Initialize the ISP with vendor and clock divisor
	MDrv_InitialSPIFlash((U8)dwNorVendor,(U8)SPI_SPEED_NOR);//(SPIFLASH_VENDOR_ST, i);

	GetRDSR();

	// Unprotect the flash
	MDrv_ProtectSpiFlash( SPIFLASH_PROTECT_NONE );

	printf("[INFO]DisableWP!\r\n");

	GetRDSR();

	return bRet;
}

//****************************************************************************//
//Mode					Status register condition		WP# and SRWD bit status		Memory
//Software protection 	SRWD, BP0-BP1 cann be 			WP#=1 and SRWD bit=0,		cannot
//mode (SPM)			changed							WP#=0 and SRWD bit=0,		be program or erase
//														WP#=1 and SRWD=1
//
//Hardware protection	SRWD, BP0-BP1 cannot be			WP#=0, SRWD bit=1			cannot
//mode (HPM)			changed.													be program or erase
//****************************************************************************//
BOOL EnableWP(DWORD Len)
{
    BOOL bRet = FALSE;
    U32 u32I;
    U32 u32WriteBytes;

    // Initialize the ISP with vendor and clock divisor
    MDrv_InitialSPIFlash((U8)dwNorVendor,(U8)SPI_SPEED_NOR);//(SPIFLASH_VENDOR_ST, i);

	GetRDSR();

    // protect the flash
    MDrv_ProtectSpiFlash( Len );

	GetRDSR();

	printf("[INFO]EnableWP!\r\n");
	//Pull Low ,Enable Write Protection
	HW_WP_Status(0);

    return bRet;
}

static BOOLEAN _MDrv_BusyWaitWriteCompletion(U32 timeout)
{
	BOOLEAN bRet = FALSE;
	U32 i;
	U8 status;

	if ( !_MDrv_BusyWaitWriteCmdReady(timeout))
		goto _End_BusyWaitWriteCompletion;

	// limit the timeout value in case it's too short and always timeout
	timeout = MAX(timeout, MASTER_CLOCK_HZ >> 8);

	for (i=0; i<timeout; i++) {

		// Issue 'read status register' command
		Write32(ISP_REG_COMMAND, ISP_READ_SR_COMMAND);

		// Issue request to read status register
		Write32(ISP_REG_READ_REQUEST, 0x0001);  // SPI read request
		if ( !_MDrv_BusyWaitReadDataReady( timeout-i ))
			goto _End_BusyWaitWriteCompletion;

		// Check status bit 0 (WIP)
		status = Read8(ISP_REG_READ_DATA);
		if ((status & 0x01) == 0) {
			bRet = TRUE;
			break;
		}
	}

    if(i >= timeout)
        DBG_MAIN(printf("INFO:[status:0x%02X i:%d timeout:%d]!\r\n", status, i,timeout));

_End_BusyWaitWriteCompletion:

	return bRet;
}



BOOLEAN MDrv_SpiFlash_TriggerMode_WriteEnable(void)
{
	BOOLEAN bRet = FALSE;

	// Enable the ISP
	ENABLE_ISP();

	// Clear the chip enable for writing command safely
	CHIP_ENABLE_CLEAR();

	// Enter the trigger mode
	ENABLE_TRIGGER_MODE();

	// Issue "Write Enable(WREN)"
	if (!_MDrv_BusyWaitWriteDataReady(0))
		goto _End_MDrv_SpiFlash_TriggerMode_WriteEnable;
	Write32( ISP_REG_WRITE_DATA, SPI_FLASH_WRITE_ENABLE_COMMAND_ST );

	bRet = TRUE;

_End_MDrv_SpiFlash_TriggerMode_WriteEnable:

	DISABLE_TRIGGER_MODE();

	// Clear the chip enable for next command
	CHIP_ENABLE_CLEAR();

	// Disable the ISP
	DISABLE_ISP();

	return bRet;
}


BOOLEAN MDrv_BlockEraseSpiFlash(U32 u32Addr)
{
	BOOLEAN bRet = FALSE;

	// enable the ISP
	ENABLE_ISP();

	// clear the spi chip enable
	CHIP_ENABLE_CLEAR();
	if (!_MDrv_BusyWaitWriteCmdReady(0))
		goto _End_MDrv_BlockEraseSpiFlash;

	Write32(ISP_REG_COMMAND, SPI_FLASH_WRITE_ENABLE_COMMAND_ST);
	if (!_MDrv_BusyWaitWriteCmdReady(0))
		goto _End_MDrv_BlockEraseSpiFlash;

	// enter the trigger mode
	ENABLE_TRIGGER_MODE();

        if(dwNorVendor == SPIFLASH_VENDOR_SST)
        {
            // Issue erase command
            Write32( ISP_REG_WRITE_DATA, 0x60 );
        }
        else
        {
            // Issue erase command
            Write32( ISP_REG_WRITE_DATA, ISP_BLOCK_ERASE_COMMAND );

            // Issue the start address of the nor flash memory
             if (!_MDrv_BusyWaitWriteDataReady(0))
                 goto _End_MDrv_BlockEraseSpiFlash;
             Write32( ISP_REG_WRITE_DATA, ( (U8)( u32Addr >> 16 ) & 0xFF ) );
            
             if (!_MDrv_BusyWaitWriteDataReady(0))
                 goto _End_MDrv_BlockEraseSpiFlash;
             Write32( ISP_REG_WRITE_DATA, ( (U8)( u32Addr >> 8 ) & 0xFF ) );
            
             if (!_MDrv_BusyWaitWriteDataReady(0))
                 goto _End_MDrv_BlockEraseSpiFlash;
             Write32( ISP_REG_WRITE_DATA, ( (U8)( u32Addr ) & 0xFF ) );
        }


	if (!_MDrv_BusyWaitWriteDataReady(0))
		goto _End_MDrv_BlockEraseSpiFlash;

	// Wait for erase completion
	DISABLE_TRIGGER_MODE();

	CHIP_ENABLE_CLEAR();

	if (!_MDrv_BusyWaitWriteCompletion(SPI_FLASH_TOTAL_NUM_SECTORS*SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE*10UL))	// It takes a long time to erase the flash, so, the guard time has to be enough to erase whole flash
		goto _End_MDrv_BlockEraseSpiFlash;

	//SPI_DEBUGINFO( console_printf("-done\n"); );

	bRet = TRUE;

_End_MDrv_BlockEraseSpiFlash:

	CHIP_ENABLE_CLEAR();

	DISABLE_ISP();

	return bRet;
}


BOOLEAN MDrv_BulkEraseSpiFlash(void)
{
	BOOLEAN bRet = FALSE;

	// enable the ISP
	ENABLE_ISP();

	// clear the spi chip enable
	CHIP_ENABLE_CLEAR();
	if (!_MDrv_BusyWaitWriteCmdReady(0))
		goto _End_MDrv_BulkEraseSpiFlash;

	Write32(ISP_REG_COMMAND, SPI_FLASH_WRITE_ENABLE_COMMAND_ST);
	if (!_MDrv_BusyWaitWriteCmdReady(0))
		goto _End_MDrv_BulkEraseSpiFlash;

	// enter the trigger mode
	ENABLE_TRIGGER_MODE();

        if(dwNorVendor == SPIFLASH_VENDOR_SST)
        {
            // Issue erase command
            Write32( ISP_REG_WRITE_DATA, 0x60 );
        }
        else
        {
            // Issue erase command
            Write32( ISP_REG_WRITE_DATA, ISP_BULK_ERASE_COMMAND );
        }


	if (!_MDrv_BusyWaitWriteDataReady(0))
		goto _End_MDrv_BulkEraseSpiFlash;

	// Wait for erase completion
	DISABLE_TRIGGER_MODE();

	CHIP_ENABLE_CLEAR();

	if (!_MDrv_BusyWaitWriteCompletion(SPI_FLASH_TOTAL_NUM_SECTORS*SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE*100UL))	// It takes a long time to erase the flash, so, the guard time has to be enough to erase whole flash
		goto _End_MDrv_BulkEraseSpiFlash;

	//SPI_DEBUGINFO( console_printf("-done\n"); );

	bRet = TRUE;

_End_MDrv_BulkEraseSpiFlash:

	CHIP_ENABLE_CLEAR();

	DISABLE_ISP();

	return bRet;
}

#define SECTOR2PHYSICAL( sec )   (((U32)sec)<<SPI_FLASH_SECTOR_SIZE_SHIFT)

BOOL MDrv_ShowChipName(U8 jedec_id, U8 memtype, U8 memcap)
{
    BOOL bRet = 1;

    DBG_MAIN((printf("*************************************************\r\n")));
    DBG_MAIN((printf("INFO:RDID [ID:%02x memType:%02x memCap:%02x]\r\n"),jedec_id, memtype, memcap));

    SPI_FLASH_SECTOR_SIZE = 256;
    SPI_FLASH_TOTAL_NUM_SECTORS = 64;
    SPI_FLASH_BLOCK_SIZE = 64*1024;
	g_WPRange = SPIFLASH_PROTECT_ALL;

    if(jedec_id == 0x1c && memtype == 0x31 && memcap == 0x11)
    {
        DBG_MAIN((printf("INFO: Nor chip is EN25F10!\r\n")));
    }
    else if(jedec_id == 0xc2 && memtype == 0x20 && memcap == 0x11)
    {
        DBG_MAIN((printf("INFO: Nor chip is MX25L1005!\r\n")));
    }
    else if(jedec_id == 0xc8 && memtype == 0x40 && memcap == 0x10)
    {
        DBG_MAIN((printf("INFO: Nor chip is GD25Q512!\r\n")));
    }
    else if(jedec_id == 0xc2 && memtype == 0x20 && memcap == 0x10)
    {
        DBG_MAIN((printf("INFO: Nor chip is MX25L512!\r\n")));
    }
    else if(jedec_id == 0x37 && memtype == 0x30 && memcap == 0x10)
    {
        DBG_MAIN((printf("INFO: Nor chip is A25L512!\r\n")));
    }
    else if(jedec_id == 0xc8 && memtype == 0x40 && memcap == 0x11)
    {
        DBG_MAIN((printf("INFO: Nor chip is G25Q10!\r\n")));
    }
    else if(jedec_id == 0xc2 && memtype == 0x20 && memcap == 0x17)
    {
        //2048 equal sectors with 4k byte each//page base 256byte
        SPI_FLASH_SECTOR_SIZE = 16;
        SPI_FLASH_TOTAL_NUM_SECTORS = 2048;
        SPI_FLASH_BLOCK_SIZE = 64*1024; //64K
        DBG_MAIN((printf("INFO: Nor chip is MX25L6406E!\r\n")));
    }
    else if(jedec_id == 0xc2 && memtype == 0x20 && memcap == 0x18)
    {
        //2048 equal sectors with 4k byte each//page base 256byte
        SPI_FLASH_SECTOR_SIZE = 16;
        SPI_FLASH_TOTAL_NUM_SECTORS = 4096;
        SPI_FLASH_BLOCK_SIZE = 64*1024; //64K
        g_WPRange = SPIFLASH_MXIC_25LXX_PROTECT_ALL;
        DBG_MAIN((printf("INFO: Nor chip is MX25L64128E!\r\n")));
    }    
    else if(jedec_id == 0x20 && memtype == 0xba && memcap == 0x17)
    {
        SPI_FLASH_SECTOR_SIZE = 16;
        SPI_FLASH_TOTAL_NUM_SECTORS = 2048;
        DBG_MAIN((printf("INFO: Nor chip is 25Q064A!\r\n")));
    }
	else if(jedec_id == 0xc2 && memtype == 0x20 && memcap == 0x16)
    {
        //2048 equal sectors with 4k byte each//page base 256byte
        //512 equal sectors with 4k byte each
        SPI_FLASH_SECTOR_SIZE = 16;
        SPI_FLASH_TOTAL_NUM_SECTORS = 1024;
        DBG_MAIN((printf("INFO: Nor chip is MX25L3206E!\r\n")));
    }
	else if(jedec_id == 0xc2 && memtype == 0x20 && memcap == 0x15)
    {
        //2048 equal sectors with 4k byte each//page base 256byte
        //512 equal sectors with 4k byte each
        SPI_FLASH_SECTOR_SIZE = 16;
        SPI_FLASH_TOTAL_NUM_SECTORS = 512;
        SPI_FLASH_BLOCK_SIZE = 64*1024;
        DBG_MAIN((printf("INFO: Nor chip is MX25L1606E!\r\n")));
    }
    else if(jedec_id == 0xef && memtype == 0x30 && memcap == 0x11)
    {
        DBG_MAIN((printf("INFO: Nor chip is W25X10!\r\n")));
    }
    else if(jedec_id == 0xbf && memtype == 0x49)
    {
        DBG_MAIN((printf("INFO: Nor chip is SST25VF010!\r\n")));
        bRet = 0;//unsupport now
    }
    else
    {
        DBG_MAIN((printf("INFO: unknow chip!\r\n")));
        bRet = 0;
    }

    DBG_MAIN((printf("***********************************************\r\n")));

    return bRet;
}



// test item #11,#12 in ISP
BOOLEAN MDrv_SpiFlash_NormalMode_Read(DWORD startAddr, DWORD Len, U8 *dstAddr)
{
	BOOLEAN ok;

	U8 i;
	U16 k, l;
	U32 counter;
	U8 u8Ret = 0;
    U8 *dwReadBuff = dstAddr;
    U32 currentPage, maxPage;
    U32 u32i = 0;


	// test for different clock divisors. only 2,4,8 are meaningful, others are the same as 2, observed by LA
	// In real chip, the clock is very fast. Hence, divisor=2 is still too fast for the flash
	for (i=8; i<=8; i*=2)
	{
		// Initialize the ISP with vendor and clock divisor
		MDrv_InitialSPIFlash((U8)dwNorVendor,(U8)SPI_SPEED_NOR);//(SPIFLASH_VENDOR_ST, i);

		// Unprotect the flash
		ok = MDrv_ProtectSpiFlash( SPIFLASH_PROTECT_NONE );
		if (!ok) goto _End_MDrv_SpiFlash_NormalMode_ReadWrite;

		// Read the flash and check if all 0xFF.
		counter = 0;

        currentPage = (startAddr/(SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE));

		maxPage = (Len % (SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE));
		if(maxPage)
			maxPage = (Len / (SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE)) + 1 + currentPage;
		else
			maxPage = (Len / (SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE)) + currentPage;

		for (/*currentPage=0*/; currentPage< maxPage; currentPage++)	// only test the first two sectors
		{
			for (k=0; k<SPI_FLASH_SECTOR_SIZE; k++)
			{
				// Enable the ISP
				ENABLE_ISP();

				// Set the start address for reading the content at the start of each sector
				ISP_WRITE_ADDRESS_REG((currentPage*SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE) + (k*SPI_FLASH_PAGE_SIZE) );

				// Issue the READ request
				Write32(ISP_REG_COMMAND, ISP_NORMAL_READ_COMMAND);
				if (!_MDrv_BusyWaitReadDataReady(0))
					goto _End_MDrv_SpiFlash_NormalMode_ReadWrite;

#ifdef ESMT_F25L016A
				for (l=0; l<1; l++)
#else
				// Read the whole page
				for (l=0; l<SPI_FLASH_PAGE_SIZE; l++)
#endif
				{
					// enable read
					Write32(ISP_REG_READ_REQUEST, 0x0001);
					if (!_MDrv_BusyWaitReadDataReady(0))
						goto _End_MDrv_SpiFlash_NormalMode_ReadWrite;

					// read the received data
					//dwReadBuff[(currentPage*SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE) + (k*SPI_FLASH_PAGE_SIZE)+ l] = Read8((U32)ISP_REG_READ_DATA);
					dwReadBuff[u32i++] = Read8((U32)ISP_REG_READ_DATA);
				}

        			// Clear the chip enable for next command
				CHIP_ENABLE_CLEAR();

				// Disable the ISP
				DISABLE_ISP();
			}
		}
	}

        //DBG_MAIN( (printf("INFO: read from Nor\r\n")));
        //DumpBuff(dstAddr, Len);

    u8Ret = TRUE;

_End_MDrv_SpiFlash_NormalMode_ReadWrite:

	// Leave the trigger mode
	DISABLE_TRIGGER_MODE();

	// Disable the ISP
	DISABLE_ISP();

	return u8Ret;
}
#ifdef	SPI_DRIVER_ISP
BOOLEAN MDrv_Spi_Read(DWORD startAddr, DWORD Len, U8 *dstAddr)
{
	U16 k, l;
	U8 u8Ret = 0;
	U8 *dwReadBuff = dstAddr;
	U32 currentPage, maxPage;

		// Initialize the ISP with vendor and clock divisor
		MDrv_InitialSPIFlash((U8)dwNorVendor,(U8)SPI_SPEED_NOR);//(SPIFLASH_VENDOR_ST, i);

		// Unprotect the flash
		//ok = MDrv_ProtectSpiFlash( SPIFLASH_PROTECT_NONE );
		//if (!ok) goto _End_MDrv_SpiFlash_NormalMode_ReadWrite;

		currentPage = (startAddr/(SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE));

		maxPage = (Len % (SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE));
		if(maxPage)
			maxPage = (Len / (SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE)) + 1;
		else
			maxPage = (Len / (SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE));

		for (/*currentPage=0*/; currentPage<maxPage; currentPage++)	// only test the first two sectors
		{
			for (k=0; k<SPI_FLASH_SECTOR_SIZE; k++)
			{
				// Enable the ISP
				ENABLE_ISP();

				// Set the start address for reading the content at the start of each sector
				ISP_WRITE_ADDRESS_REG((currentPage*SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE) + (k*SPI_FLASH_PAGE_SIZE) );

				// Issue the READ request
				Write32(ISP_REG_COMMAND, ISP_NORMAL_READ_COMMAND);
				if (!_MDrv_BusyWaitReadDataReady(0))
					goto _End_MDrv_SpiFlash_NormalMode_ReadWrite;

				// Read the whole page
				for (l=0; l<SPI_FLASH_PAGE_SIZE; l++)
				{
					// enable read
					Write32(ISP_REG_READ_REQUEST, 0x0001);
					if (!_MDrv_BusyWaitReadDataReady(0))
						goto _End_MDrv_SpiFlash_NormalMode_ReadWrite;

					// read the received data
					dwReadBuff[(currentPage*SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE) + (k*SPI_FLASH_PAGE_SIZE)+ l] = Read8((U32)ISP_REG_READ_DATA);
				}

        			// Clear the chip enable for next command
				CHIP_ENABLE_CLEAR();

				// Disable the ISP
				DISABLE_ISP();
			}
		}

_End_MDrv_SpiFlash_NormalMode_ReadWrite:

	// Leave the trigger mode
	DISABLE_TRIGGER_MODE();

	// Disable the ISP
	DISABLE_ISP();

	return u8Ret;
}

#endif


U8 MDrv_SpiFlash_TriggerMode_BlockErase(DWORD u32Addr,DWORD u32ByteCnt)
{
    BOOLEAN ok;

    U8 i;
    U16 j, k, l;
    U32 v, counter;
    U32 u32StartAddr = 0;
    U8 u8Ret = 0;
    U32 u32Tem, u32Sector;
    U32 u32Blocknum;

    // test for different clock divisors. only 2,4,8 are meaningful, others are the same as 2, observed by LA
    // In real chip, the clock is very fast. Hence, divisor=2 is still too fast for the flash

    for (i=8; i<=8; i*=2)
    {
        // Initialize the ISP with vendor and clock divisor
        MDrv_InitialSPIFlash((U8)dwNorVendor,(U8)SPI_SPEED_NOR);//(SPIFLASH_VENDOR_ST, i);

        // Unprotect the flash
        ok = MDrv_ProtectSpiFlash( SPIFLASH_PROTECT_NONE );
        if (!ok) goto _End_MDrv_SpiFlash_TriggerMode_BlockErase;

        for (u32StartAddr = u32Addr; u32StartAddr < (u32Addr + u32ByteCnt); u32StartAddr += SPI_FLASH_BLOCK_SIZE)
        {
            // Block erase the flash
            ok = MDrv_BlockEraseSpiFlash(u32StartAddr);
            if (!ok) goto _End_MDrv_SpiFlash_TriggerMode_BlockErase;
        }

        // Read the flash and check if all 0xFF.
        counter = 0;

        //align by sector size,we only check the size which we need.
        u32Tem = (u32Addr % (SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE));
        if(u32Tem)
            u32Sector = (u32Addr / (SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE)) + 1;
        else
            u32Sector = (u32Addr / (SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE));

        //printf("(Sector,SECTOR_SIZE,PAGE_SIZE) = (0x%02x,0x%02x,0x%02x))\r\n",u32Sector,SPI_FLASH_BLOCK_SIZE,SPI_FLASH_PAGE_SIZE);

        for (j=u32Sector;j < (u32Sector + (u32ByteCnt/(SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE))); j++)
        {
            for (k=0; k<SPI_FLASH_SECTOR_SIZE; k++)
            {
                // Enable the ISP
                ENABLE_ISP();

                // Clear the chip enable for writing command safely
                CHIP_ENABLE_CLEAR();

                // Enter the trigger mode
                ENABLE_TRIGGER_MODE();

                // Issue the "READ" command
                if (!_MDrv_BusyWaitWriteDataReady(0))
                    goto _End_MDrv_SpiFlash_TriggerMode_BlockErase;
                Write32( ISP_REG_WRITE_DATA, SPI_FLASH_READ_COMMAND_ST );

                // Issue the start address of the nor flash memory
                if (!_MDrv_BusyWaitWriteDataReady(0))
                    goto _End_MDrv_SpiFlash_TriggerMode_BlockErase;
                Write32( ISP_REG_WRITE_DATA, ( (U8)( ( (j*SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE) + (k*SPI_FLASH_PAGE_SIZE) ) >> 16 ) & 0xFF ) );

                if (!_MDrv_BusyWaitWriteDataReady(0))
                    goto _End_MDrv_SpiFlash_TriggerMode_BlockErase;
                Write32( ISP_REG_WRITE_DATA, ( (U8)( ( (j*SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE) + (k*SPI_FLASH_PAGE_SIZE) ) >> 8 ) & 0xFF ) );

                if (!_MDrv_BusyWaitWriteDataReady(0))
                    goto _End_MDrv_SpiFlash_TriggerMode_BlockErase;
                Write32( ISP_REG_WRITE_DATA, ( (U8)( ( (j*SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE) + (k*SPI_FLASH_PAGE_SIZE) ) ) & 0xFF ) );

                // Read the data
                for (l=0; l<SPI_FLASH_PAGE_SIZE; l++)
                {
                    // enable read
                    Write32(ISP_REG_READ_REQUEST, 0x0001);
                    if (!_MDrv_BusyWaitReadDataReady(0))
                        goto _End_MDrv_SpiFlash_TriggerMode_BlockErase;

                    // read the received data
                    v = Read8((U32)ISP_REG_READ_DATA);
                    if (v != 0xFF)
                    {
                        counter++;
                        DBG_MAIN((printf("INFO:%d Sector %d page erase fail.\r\n"), j, k));
                    }
                }

                // Leave the trigger mode
                if (!_MDrv_BusyWaitWriteDataReady(0))
                    goto _End_MDrv_SpiFlash_TriggerMode_BlockErase;

                // Disable the trigger mode
                DISABLE_TRIGGER_MODE();

                // Clear the chip enable for next command
                CHIP_ENABLE_CLEAR();

                // Disable the ISP
                DISABLE_ISP();

            }
        }

        if (counter > 0)
        {
            DBG_MAIN((printf("INFO:%d bytes are not 0xFF after bulk erasing.\r\n"),counter));
            u8Ret = 0;
        }
        else
        {
            u8Ret = 1;
            if (u32ByteCnt % SPI_FLASH_BLOCK_SIZE == 0)
                u32Blocknum = u32ByteCnt / SPI_FLASH_BLOCK_SIZE;
            else
                u32Blocknum = u32ByteCnt / SPI_FLASH_BLOCK_SIZE + 1;
            printf("INFO:total 0x%02x Blocks erase success\r\n", u32Blocknum);
        }
    }

    _End_MDrv_SpiFlash_TriggerMode_BlockErase:

    // Leave the trigger mode
    DISABLE_TRIGGER_MODE();

    // Disable the ISP
    DISABLE_ISP();

    return u8Ret;
}


U8 MDrv_SpiFlash_TriggerMode_BulkErase(DWORD Len)
{
    BOOLEAN ok;

    U8 i;
    U16 j, k, l;
    U32 v, counter;
    U8 u8Ret = 0;
    U32 u32Tem, u32Sector;

    // test for different clock divisors. only 2,4,8 are meaningful, others are the same as 2, observed by LA
    // In real chip, the clock is very fast. Hence, divisor=2 is still too fast for the flash

    for (i=8; i<=8; i*=2)
    {
        // Initialize the ISP with vendor and clock divisor
        MDrv_InitialSPIFlash((U8)dwNorVendor,(U8)SPI_SPEED_NOR);//(SPIFLASH_VENDOR_ST, i);

        // Unprotect the flash
        ok = MDrv_ProtectSpiFlash( SPIFLASH_PROTECT_NONE );
        if (!ok) goto _End_MDrv_SpiFlash_TriggerMode_BulkErase;

        // Bulk erase the flash
        ok = MDrv_BulkEraseSpiFlash();
        if (!ok) goto _End_MDrv_SpiFlash_TriggerMode_BulkErase;

        // Read the flash and check if all 0xFF.
        counter = 0;

        //align by sector size,we only check the size which we need.
        u32Tem = (Len % (SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE));
        if(u32Tem)
            u32Sector = (Len / (SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE)) + 1;
        else
            u32Sector = (Len / (SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE));

        printf("(Sector,SECTOR_SIZE,PAGE_SIZE) = (0x%02x,0x%02x,0x%02x))\r\n");

        for (j=0; j<u32Sector; j++)
        {
            for (k=0; k<SPI_FLASH_SECTOR_SIZE; k++)
            {
                // Enable the ISP
                ENABLE_ISP();

                // Clear the chip enable for writing command safely
                CHIP_ENABLE_CLEAR();

                // Enter the trigger mode
                ENABLE_TRIGGER_MODE();

                // Issue the "READ" command
                if (!_MDrv_BusyWaitWriteDataReady(0))
                    goto _End_MDrv_SpiFlash_TriggerMode_BulkErase;
                Write32( ISP_REG_WRITE_DATA, SPI_FLASH_READ_COMMAND_ST );

                // Issue the start address of the nor flash memory
                if (!_MDrv_BusyWaitWriteDataReady(0))
                    goto _End_MDrv_SpiFlash_TriggerMode_BulkErase;
                Write32( ISP_REG_WRITE_DATA, ( (U8)( ( (j*SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE) + (k*SPI_FLASH_PAGE_SIZE) ) >> 16 ) & 0xFF ) );

                if (!_MDrv_BusyWaitWriteDataReady(0))
                    goto _End_MDrv_SpiFlash_TriggerMode_BulkErase;
                Write32( ISP_REG_WRITE_DATA, ( (U8)( ( (j*SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE) + (k*SPI_FLASH_PAGE_SIZE) ) >> 8 ) & 0xFF ) );

                if (!_MDrv_BusyWaitWriteDataReady(0))
                    goto _End_MDrv_SpiFlash_TriggerMode_BulkErase;
                Write32( ISP_REG_WRITE_DATA, ( (U8)( ( (j*SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE) + (k*SPI_FLASH_PAGE_SIZE) ) ) & 0xFF ) );

                // Read the data
                for (l=0; l<SPI_FLASH_PAGE_SIZE; l++)
                {
                    // enable read
                    Write32(ISP_REG_READ_REQUEST, 0x0001);
                    if (!_MDrv_BusyWaitReadDataReady(0))
                        goto _End_MDrv_SpiFlash_TriggerMode_BulkErase;

                    // read the received data
                    v = Read8((U32)ISP_REG_READ_DATA);
                    if (v != 0xFF)
                    {
                        counter++;
                        DBG_MAIN((printf("INFO:%d Sector %d page erase fail.\r\n"), j, k));
                    }
                }

                // Leave the trigger mode
                if (!_MDrv_BusyWaitWriteDataReady(0))
                    goto _End_MDrv_SpiFlash_TriggerMode_BulkErase;

                // Disable the trigger mode
                DISABLE_TRIGGER_MODE();

                // Clear the chip enable for next command
                CHIP_ENABLE_CLEAR();

                // Disable the ISP
                DISABLE_ISP();

            }
        }

        if (counter > 0)
            {
            DBG_MAIN((printf("INFO:%d bytes are not 0xFF after bulk erasing.\r\n"),counter));
            u8Ret = 0;
            }
        else
            {
            u8Ret = 1;
            DBG_MAIN((printf("INFO:total %d Sector erase success\r\n"), u32Sector));
            }
    }

    _End_MDrv_SpiFlash_TriggerMode_BulkErase:

    // Leave the trigger mode
    DISABLE_TRIGGER_MODE();

    // Disable the ISP
    DISABLE_ISP();

    return u8Ret;
}



BOOL MDrv_SerFlash_WriteSST(U32 u32Addr, U8 *pu8Data, U32 u32Size)
{
    BOOL bRet = FALSE;
    U32 u32I;
    U32 u32WriteBytes;

    // Initialize the ISP with vendor and clock divisor
    MDrv_InitialSPIFlash((U8)dwNorVendor,(U8)SPI_SPEED_NOR);//(SPIFLASH_VENDOR_ST, i);

    // Unprotect the flash
    MDrv_ProtectSpiFlash( SPIFLASH_PROTECT_NONE );

    ISP_REG16(REG_ISP_PASSWORD) = 0xAAAA; // enable ISP

    CHIP_ENABLE_CLEAR();

    DBG_MAIN((printf("INFO> MDrv_SerFlash_WriteSST start!\r\n")));

    while(u32Size)
    {

        u32WriteBytes = u32Size;
        //ISP_REG16(REG_ISP_SPI_AAI_NUM) = LOU16(u32WriteBytes);

        _MDrv_SerFlash_WaitWriteCmdRdy();

        //500 spi clock
        ISP_REG16(REG_ISP_SPI_TBP) = 0x1f3;
        _MDrv_SerFlash_WaitWriteCmdRdy();

        //ISP_REG16(REG_ISP_SPI_COMMAND) = PAGE_PROG_CMD_SST; // WREN

        DBG_MAIN( (printf("INFO> _MDrv_SerFlash_WaitWriteCmdRdy!\r\n")));

       // ISP_REG16(REG_ISP_SPI_COMMAND) = CMD_WRITE_ENABLE; // WREN
        ISP_REG16(REG_ISP_SPI_ADDR_L) = LOU16(u32Addr);
        ISP_REG16(REG_ISP_SPI_ADDR_H) = HIU16(u32Addr);
        //_MDrv_SerFlash_WaitWriteCmdRdy();

        ISP_REG16(REG_ISP_SPI_COMMAND) = PAGE_PROG_CMD_SST; // WREN

        for (u32I = 0; u32I < u32WriteBytes; u32I++)
        {
            ISP_REG16(REG_ISP_SPI_WDATA) = pu8Data[u32I];
            _MDrv_SerFlash_WaitWriteDataRdy();
        }

        u32Size -= u32WriteBytes;

    }

    ISP_REG16(REG_ISP_SPI_CECLR) = 1; //SPI CEB dis
    bRet = _MDrv_SerFlash_WaitWriteDone_AAI();

    ISP_REG16(REG_ISP_PASSWORD) = 0x5555; // disable ISP
    DBG_MAIN( (printf("INFO> MDrv_SerFlash_WriteSST end!\r\n")));

    return bRet;
}


BOOLEAN MDrv_SpiFlash_TriggerMode_ReadID(void)
{
	U8 j;

	U8 rdid_jedec_id = 0;							// manufacturer id of STM
	U8 rdid_memtype = 0;							// memory type
	U8 rdid_memcap = 0;								// memory capacity
#ifdef ST_M25P32
	U8 rdid_uid = 0;								// length of Common Flash Interface(CFI)
	U8 rdid_cfi[SPIFLASH_UNIQUE_ID] = {0};// common flash interface(CFI)
#endif
	U8 u8Ret = 0;

	DBG_MAIN(printf("\r\nMDrv_SpiFlash_TriggerMode_ReadID\r\n"));

	// test for different clock divisors. only 2,4,8 are meaningful, others are the same as 2, observed by LA
	// In real chip, the clock is very fast. Hence, divisor=2 is still too fast for the flash
	for (dwNorVendor=SPIFLASH_VENDOR_ST; dwNorVendor<=SPIFLASH_VENDOR_ESMT; dwNorVendor++)
	{
		// Initialize the ISP with vendor and clock divisor
		MDrv_InitialSPIFlash((U8)dwNorVendor,(U8)SPI_SPEED_NOR);//(SPIFLASH_VENDOR_ST, i);

		// Enable the ISP
		ENABLE_ISP();

		// Clear the chip enable for writing command safely
		CHIP_ENABLE_CLEAR();

		// Issue write enable
		if (!_MDrv_BusyWaitWriteCmdReady(0))
			goto _End_MDrv_SpiFlash_TriggerMode_ReadID;
		//Write32( ISP_REG_COMMAND, SPI_FLASH_WRITE_ENABLE_COMMAND_ST );
		OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_01, SPI_FLASH_WRITE_ENABLE_COMMAND_ST);

		// Enter the trigger mode
		ENABLE_TRIGGER_MODE();

            if(dwNorVendor == SPIFLASH_VENDOR_SST)
            {
                // Trigger the RDID command
                //Write32( ISP_REG_WRITE_DATA, SPI_FLASH_RES_COMMAND_ST );
                OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_04, SPI_FLASH_RES_COMMAND_ST);

                // Issue the start address of the nor flash memory
                if (!_MDrv_BusyWaitWriteDataReady(0))
                    goto _End_MDrv_SpiFlash_TriggerMode_ReadID;
                //Write32( ISP_REG_WRITE_DATA, 0 );
                OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_04, 0);

                if (!_MDrv_BusyWaitWriteDataReady(0))
                    goto _End_MDrv_SpiFlash_TriggerMode_ReadID;
                //Write32( ISP_REG_WRITE_DATA, 0 );
                OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_04, 0);

                if (!_MDrv_BusyWaitWriteDataReady(0))
                    goto _End_MDrv_SpiFlash_TriggerMode_ReadID;
                //Write32( ISP_REG_WRITE_DATA, 0 );//Manufacture`s ID
                OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_04, 0);

                // Issue read request
                //Write32(ISP_REG_READ_REQUEST, 0x0001);
                OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_0C, 0x0001);
                if (!_MDrv_BusyWaitReadDataReady(0))
                goto _End_MDrv_SpiFlash_TriggerMode_ReadID;

                // Read the return value, the manufacture identification
                //rdid_jedec_id = Read8(ISP_REG_READ_DATA);
                rdid_jedec_id = (U8)INREG16(CEDRIC_BASE_REG_ISP_PA+REG_05);

                // Issue read request
                //Write32(ISP_REG_READ_REQUEST, 0x0001);
                OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_0C, 0x0001);
                if (!_MDrv_BusyWaitReadDataReady(0))
                goto _End_MDrv_SpiFlash_TriggerMode_ReadID;

                // Read the return value, the memory type
                //rdid_memtype = Read8(ISP_REG_READ_DATA);
                rdid_memtype = (U8)INREG16(CEDRIC_BASE_REG_ISP_PA+REG_05);

                // Issue read request
                //Write32(ISP_REG_READ_REQUEST, 0x0001);
                OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_0C, 0x0001);
                if (!_MDrv_BusyWaitReadDataReady(0))
                goto _End_MDrv_SpiFlash_TriggerMode_ReadID;

                // Read the return value, the memory capacity
                //rdid_memcap = Read8(ISP_REG_READ_DATA);
                rdid_memcap = (U8)INREG16(CEDRIC_BASE_REG_ISP_PA+REG_05);

            }
            else
            {
                // Trigger the RDID command
                //Write32( ISP_REG_WRITE_DATA, ISP_READ_ID_COMMAND );
                OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_04, ISP_READ_ID_COMMAND);

                // Issue read request
                //Write32(ISP_REG_READ_REQUEST, 0x0001);
                OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_0C, 0x0001);
                if (!_MDrv_BusyWaitReadDataReady(0))
                goto _End_MDrv_SpiFlash_TriggerMode_ReadID;

                // Read the return value, the manufacture identification
                rdid_jedec_id = (U8)INREG16(CEDRIC_BASE_REG_ISP_PA+REG_05);

                // Issue read request
                //Write32(ISP_REG_READ_REQUEST, 0x0001);
                OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_0C, 0x0001);
                if (!_MDrv_BusyWaitReadDataReady(0))
                goto _End_MDrv_SpiFlash_TriggerMode_ReadID;

                // Read the return value, the memory type
                rdid_memtype = (U8)INREG16(CEDRIC_BASE_REG_ISP_PA+REG_05);

                // Issue read request
                //Write32(ISP_REG_READ_REQUEST, 0x0001);
                OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_0C, 0x0001);
                if (!_MDrv_BusyWaitReadDataReady(0))
                goto _End_MDrv_SpiFlash_TriggerMode_ReadID;

                // Read the return value, the memory capacity
                rdid_memcap = (U8)INREG16(CEDRIC_BASE_REG_ISP_PA+REG_05);
            }

#ifdef ST_M25P32
		// Issue read request
		//Write32(ISP_REG_READ_REQUEST, 0x0001);
              OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_0C, 0x0001);
		if (!_MDrv_BusyWaitReadDataReady(0))
			goto _End_MDrv_SpiFlash_TriggerMode_ReadID;

		// Read the return value, the UID
		rdid_uid = (U8)INREG16(CEDRIC_BASE_REG_ISP_PA+REG_05);

		for (j=0; j<SPIFLASH_UNIQUE_ID; j++)
		{
			// issue read request
			//Write32(ISP_REG_READ_REQUEST, 0x0001);
			OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_0C, 0x0001);
			if (!_MDrv_BusyWaitReadDataReady(0))
				goto _End_MDrv_SpiFlash_TriggerMode_ReadID;

			// read the return value, the first byte of the CFI
			rdid_cfi[j] = (U8)INREG16(CEDRIC_BASE_REG_ISP_PA+REG_05);
		}
#endif
            // Leave the trigger mode
            DISABLE_TRIGGER_MODE();

            // Clear the chip enable for next command
            CHIP_ENABLE_CLEAR();

            // Disable the ISP
            DISABLE_ISP();

            u8Ret = MDrv_ShowChipName(rdid_jedec_id, rdid_memtype, rdid_memcap);
            if(u8Ret)
            {
                break;
            }
	}

_End_MDrv_SpiFlash_TriggerMode_ReadID:

	// Leave the trigger mode
	DISABLE_TRIGGER_MODE();

	// Disable the ISP
	DISABLE_ISP();

	return u8Ret;
}

BOOL BlockEraseFlash(DWORD u32Addr, DWORD u32ByteCnt)
{
    return (MDrv_SpiFlash_TriggerMode_BlockErase(u32Addr,u32ByteCnt));
}


BOOL EraseFlash(DWORD u32ByteCnt)
{
    return (MDrv_SpiFlash_TriggerMode_BulkErase(u32ByteCnt));
}


BOOL BlankFlash(DWORD Len)
{
    return TRUE;
}


BOOL ProgramRomCode(U32 u32Addr, U8 *pu8Data, U32 u32Size)
{
    if(dwNorVendor == SPIFLASH_VENDOR_SST)
    {
        return (MDrv_SerFlash_WriteSST(u32Addr,pu8Data,u32Size));
    }
    else
    {
        return (MDrv_SerFlash_Write(u32Addr,pu8Data,u32Size));
    }        
}

BOOL VerifyProgram(U8 *srcrtAddr, DWORD Len, U8 *readAddr, U32 dwDstAddr)
{
    U32 u32i = 0;
    BOOL bRet = FALSE;

    MDrv_SpiFlash_NormalMode_Read(dwDstAddr, Len, readAddr);
    //DumpBuff(readAddr,0x200);

    for (u32i = 0; u32i < Len ;u32i++)
    {
        if(srcrtAddr[u32i] != readAddr[u32i])
        {
            DBG_MAIN((printf("INFO:Verify  Fail,pos[0x%02x]!\r\n"),u32i));
            break;
        }
    }

    if(u32i == Len)
    {
        DBG_MAIN((printf("*******************\r\n")));
        DBG_MAIN((printf("INFO:Verify  success!\r\n")));
        DBG_MAIN((printf("*******************\r\n")));
        bRet = TRUE;
    }
    else
    {
        DBG_MAIN((printf("*******************\r\n")));
        DBG_MAIN((printf("INFO:Verify  fail!\r\n")));
        DBG_MAIN((printf("*******************\r\n")));
    }
    return bRet;
}


BOOL do_BurnCode(unsigned char *AddrSrc, DWORD Len, DWORD dwDstAddr )
{
    U8  u8TryCnt = 0;
    unsigned char *AddrReadBuf = NULL;
    AddrReadBuf = AddrSrc + MAX_NOR_SIZE; 

RETRY_PROGRAM:

     //get device id, whether it is supported.
    if(u8TryCnt < RETRY_PRO_CNT && MDrv_SpiFlash_TriggerMode_ReadID())
    {
        u8TryCnt ++;
        DisableWP(0);

        //erase,blanking also.            
        printf("INFO : EraseFlash...\r\n");
        if (RTK_START == dwDstAddr)
        {
            if(!BlockEraseFlash(dwDstAddr, RTK_SIZE))
            {
                DBG_MAIN( (printf("INFO:EraseFlash Fail!\r\n")));
                printf("INFO : EraseFlash Fail!! Erase Size = 0x%02x \r\n", RTK_SIZE);
                printf("INFO : retry!\r\n");
                goto RETRY_PROGRAM;
            }
        }
        else
        {
            if(!BlockEraseFlash(dwDstAddr, Len))
            {
                DBG_MAIN( (printf("INFO:EraseFlash Fail!\r\n")));
                printf("INFO : EraseFlash Fail\r\n");
                printf("INFO : retry!\r\n");
                goto RETRY_PROGRAM;
            }
        }

        //Program binary to Nor flash
        printf("INFO : Program Binary...\r\n");
        if(!ProgramRomCode(dwDstAddr, AddrSrc, Len))
        {
            DBG_MAIN((printf("INFO:Program Binary Fail!\r\n")));
            printf("INFO : Program Binary Fail\r\n");
            printf("INFO : retry!\r\n");
            goto RETRY_PROGRAM;
        }

        //Verify Program
        printf("INFO : Verify Program...\r\n");

        if(!VerifyProgram(AddrSrc, Len, AddrReadBuf, dwDstAddr))
        {
            DBG_MAIN( (printf("INFO:Program Verify Fail!\r\n")));
            printf("INFO : Verify Binary Fail\r\n");
            printf("INFO : retry!\r\n");
            goto RETRY_PROGRAM;
        }
        else
        {
            printf("INFO : Program Binary success\r\n");
			EnableWP(g_WPRange|0x20);// (0x20<<2 == 0x80) 1=status register write disable
        }
    }
    else
    {
        printf("INFO : Program Binary fail or no supported, god loves you!\r\n");
        DBG_MAIN( (printf("INFO : Program Binary fail or no supported, god loves you!\r\n")));
        return FALSE;
    }
        
    return TRUE;
}



//------------------------------------------------------------------------------
//
//  Function:   do_BurnUI
//
//  Description
//      Program Rtk to Nor flash
//
//  Parameters
//      N/A
//
//  Return Values
//      true    success
//      false   fail
BOOL do_BurnUI(int argc, char * const argv[])
{
    BOOL ret = FALSE;
    unsigned char *AddrSrc=NULL;
    U32 u32UISize = 0;

	AddrSrc = (unsigned char *) simple_strtol(argv[3], NULL, 16);
	u32UISize = (U32) simple_strtol(argv[4], NULL, 16);
    
    ret = do_BurnCode(AddrSrc, u32UISize, BITMAP_START); //For Size of RTK is not a fixed value.
    
	return ret;
}



//------------------------------------------------------------------------------
//
//  Function:   do_BurnRtk
//
//  Description
//      Program Rtk to Nor flash
//
//  Parameters
//      N/A
//
//  Return Values
//      true    success
//      false   fail
BOOL do_BurnRtk(int argc, char * const argv[])
{
    BOOL ret = FALSE;
    unsigned char *AddrSrc=NULL;
    U32 u32RtkSize = 0;

	AddrSrc = (unsigned char *) simple_strtol(argv[3], NULL, 16);
	u32RtkSize = (U32) simple_strtol(argv[4], NULL, 16);
    
    ret = do_BurnCode(AddrSrc, u32RtkSize, RTK_START); //For Size of RTK is not a fixed value.
    
	return ret;
}

BOOL do_EraseUboot(int argc, char * const argv[])
{
    BOOL ret = FALSE;
    unsigned char *AddrSrc=NULL;

    AddrSrc = (unsigned char *) simple_strtol(argv[3], NULL, 16);

    if(MDrv_SpiFlash_TriggerMode_ReadID())
    {
        ret = TRUE;
        //erase,blanking also.            
        printf("INFO : Eraseuboot...\r\n");
        DisableWP(0);

        if(!BlockEraseFlash(UBOOT_START, UBOOT_SIZE))
        {
            DBG_MAIN( (printf("INFO:Eraseuboot Fail!\r\n")));
            printf("INFO : Eraseuboot Fail\r\n");
            printf("INFO : Eraseubreoot!\r\n");
            ret = FALSE;
        }
		EnableWP(g_WPRange|0x20);// (0x20<<2 == 0x80) 1=status register write disable
		
    }

    return ret;
}

BOOL do_EraseRtkbin(int argc, char * const argv[])
{
    BOOL ret = FALSE;

    unsigned char *AddrSrc=NULL;

    AddrSrc = (unsigned char *) simple_strtol(argv[3], NULL, 16);

    if(MDrv_SpiFlash_TriggerMode_ReadID())
    {
        ret = TRUE;
           //erase,blanking also.            
        printf("INFO : EraseRtkBin...\r\n");
        DisableWP(0);

        if(!BlockEraseFlash(RTK_START, RTK_SIZE))
        {
            DBG_MAIN( (printf("INFO:EraseRtkBin Fail!\r\n")));
            printf("INFO : EraseRtkBin Fail\r\n");
            printf("INFO : EraseRtkBin!\r\n");
            ret = FALSE;
        }
		EnableWP(g_WPRange|0x20);// (0x20<<2 == 0x80) 1=status register write disable
    }

    return ret;
}


//------------------------------------------------------------------------------
//
//  Function:   do_BurnUboot
//
//  Description
//      Program Uboot to Nor flash
//
//  Parameters
//      N/A
//
//  Return Values
//      true    success
//      false   fail
BOOL do_BurnUboot(int argc, char * const argv[])
{
    BOOL ret = FALSE;
    unsigned char *AddrSrc=NULL;

	AddrSrc = (unsigned char *) simple_strtol(argv[3], NULL, 16);
    
    ret = do_BurnCode(AddrSrc, UBOOT_SIZE, UBOOT_START);
    
	return ret;
}




//------------------------------------------------------------------------------
//
//  Function:   do_BurnRomDevCode
//
//  Description
//      Program cedric_Rom.bin & DEVINFO to Nor flash
//
//  Parameters
//      N/A
//
//  Return Values
//      true    success
//      false   fail
BOOL do_BurnRomDevCode(int argc, char * const argv[])
{
    BOOL ret = FALSE;
    unsigned char *AddrSrc=NULL, *AddrDevinfo=NULL;

	AddrSrc = (unsigned char *) simple_strtol(argv[3], NULL, 16);
    AddrDevinfo = AddrSrc + DEVINFO_OFFSET;

    //printf("(AddrSrc,AddrDevinfo,AddrReadBuf, AddrNewDevinfo) = (0x%02x,0x%02x,0x%02x)\r\n", (U32)AddrSrc,(U32)AddrReadBuf,(U32)AddrDevinfo, (U32)(AddrSrc + (MAX_NOR_SIZE<<1)));

#if DBG_ISP_FLOW
    printf("----------orgin-------------");
    DumpBuff(AddrDevinfo, DEVINFO_SIZE); // 30K ~ 32K devinfo
    printf("----------new one-------------");
    DumpBuff((U8 *)(AddrSrc + (MAX_NOR_SIZE << 1)), DEVINFO_SIZE);
#endif

    memcpy((void *)AddrDevinfo,(void *)(AddrSrc + (MAX_NOR_SIZE<<1)), (size_t)DEVINFO_SIZE);

#if DBG_ISP_FLOW
    printf("----------after replace-------------");
    DumpBuff(AddrDevinfo, DEVINFO_SIZE); // 30K ~ 32K devinfo
#endif

    ret = do_BurnCode(AddrSrc, ROMCODE_SIZE, ROMCODE_START);

	return ret;
}



//------------------------------------------------------------------------------
//
//  Function:   do_BurnRomCode
//
//  Description
//      Program cedric_Rom.bin to Nor flash
//
//  Parameters
//      N/A
//
//  Return Values
//      true    success
//      false   fail
BOOL do_BurnRomCode(int argc, char * const argv[])
{
    BOOL ret = FALSE;
    unsigned char *AddrSrc=NULL, *AddrDevinfo=NULL;

	AddrSrc = (unsigned char *) simple_strtol(argv[3], NULL, 16);
    AddrDevinfo = AddrSrc + DEVINFO_OFFSET;

    ret = do_BurnCode(AddrSrc, ROMCODE_SIZE, ROMCODE_START);

	return ret;
}


//------------------------------------------------------------------------------
//
//  Function:   do_BurnDevInfo
//
//  Description
//      Program DEVINFO to Nor flash
//
//  Parameters
//      N/A
//
//  Return Values
//      true    success
//      false   fail
BOOL do_BurnDevInfo(int argc, char * const argv[])
{
    U8  u8TryCnt = 0;
    BOOL ret = FALSE;

    unsigned char *AddrSrc = NULL, *AddrNewDevinfo = NULL, *AddrDevinfo = NULL;

	AddrNewDevinfo = (unsigned char *) simple_strtol(argv[3], NULL, 16);   //new devinfo
    AddrSrc = AddrNewDevinfo - (MAX_NOR_SIZE << 1);           //rom code
    AddrDevinfo = AddrSrc + DEVINFO_OFFSET;

    //printf("(AddrSrc,AddrDevinfo,AddrReadBuf,AddrNewDevinfo) = (0x%02x,0x%02x,0x%02x,0x%02x)\r\n", (U32)AddrSrc,(U32)AddrDevinfo,(U32)AddrReadBuf,(U32)AddrNewDevinfo);

RETRY_PROGRAM:

        //get device id, whether it is supported.
        if(u8TryCnt < RETRY_PRO_CNT && MDrv_SpiFlash_TriggerMode_ReadID())
        {
            u8TryCnt++;
            
            //read romcode in nor flash and renew devinfo
            printf("INFO : Load RomCode from nor flash...\r\n");
            if (!MDrv_SpiFlash_NormalMode_Read(ROMCODE_START, ROMCODE_SIZE, AddrSrc))
            {
                DBG_MAIN( (printf("INFO:ReadFlash Fail!\r\n")));
                printf("INFO : ReadFlash Fail\r\n");
                printf("INFO : retry!\r\n");
                goto RETRY_PROGRAM;
            }
            printf("INFO : Replace with New Devinfo...\r\n");
            memcpy((void *)AddrDevinfo,(void *)AddrNewDevinfo, (size_t)DEVINFO_SIZE);
            
            ret = do_BurnCode(AddrSrc, ROMCODE_SIZE, ROMCODE_START);
        }

        return ret;
}



int do_nor(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	if (argc < 2)
		return CMD_RET_USAGE;


    if(strcmp(argv[1], "id") == 0)
    {
        MDrv_SpiFlash_TriggerMode_ReadID ();
        return 0;
    }
    else if(strcmp(argv[1], "burn") == 0)
    {
        if (strcmp(argv[2], "romdev") == 0)
        {
            if (argc < 4) 
            {
                return CMD_RET_USAGE;
            }

            printf("----------Burn RomCode & DEVINFO Start-------------\r\n");
            do_BurnRomDevCode(argc,argv);
            printf("----------Burn RomCode & DEVINFO Done--------------\r\n");
        }
        else if (strcmp(argv[2], "rom") == 0)
        {
            if (argc < 4) 
            {
                return CMD_RET_USAGE;
            }

            printf("----------Burn RomCode & DEVINFO Start-------------\r\n");
            do_BurnRomCode(argc,argv);
            printf("----------Burn RomCode & DEVINFO Done--------------\r\n");
        }
        else if (strcmp(argv[2], "devinfo") == 0)
        {
            if (argc < 4) 
            {
                return CMD_RET_USAGE;
            }

            printf("--------------Burn DEVINFO Start-------------------\r\n");
            do_BurnDevInfo(argc,argv);
            printf("--------------Burn DEVINFO Start-------------------\r\n");
        }
        else if (strcmp(argv[2], "uboot") == 0)
        {
            if (argc < 4) 
            {
                return CMD_RET_USAGE;
            }

            printf("--------------Burn Uboot Start---------------------\r\n");
            do_BurnUboot(argc,argv);
            printf("--------------Burn Uboot Done----------------------\r\n");
        }
        else if (strcmp(argv[2], "rtkbin") == 0)
        {
            if (argc < 5)
            {
                return CMD_RET_USAGE;
            }

            printf("--------------Burn RtkBin Start-------------------\r\n");
            do_BurnRtk(argc,argv);
            printf("--------------Burn RtkBin Done--------------------\r\n");
        }
        else if ((strcmp(argv[2], "UI") == 0) || (strcmp(argv[2], "ui") == 0))
        {
            if (argc < 5)
            {
                return CMD_RET_USAGE;
            }

            printf("--------------Burn bitmap(UI) Start--------------\r\n");
            do_BurnUI(argc,argv);
            printf("--------------Burn bitmap(UI) Done---------------\r\n");
        }
 	}
    else if(strcmp(argv[1], "erase") == 0)
    {
        if (strcmp(argv[2], "uboot") == 0)
        {
            printf("--------------erase uboot Start--------------\r\n");
            do_EraseUboot(argc,argv);
            printf("--------------erase uboot done---------------\r\n");
        }
        if (strcmp(argv[2], "rtkbin") == 0)
        {
            printf("--------------erase uboot Start--------------\r\n");
            do_EraseRtkbin(argc,argv);
            printf("--------------erase uboot done---------------\r\n");
        }
    }
	else
	{
		return CMD_RET_USAGE;
	}

	return 1;

}


BOOL do_devinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    #define TEMPADDR 0x45000000
    BOOL ret = FALSE;
    U16 u16Offset = 0;
    U16 u16Length = 0;
    U8 u8Newvalue = 0;
    unsigned char *AddrSrc = NULL, *AddrDevinfo = NULL;    
    U8  u8TryCnt = 0;

	if ((argc < 3) || (argc > 3))
		return CMD_RET_USAGE;
    if(strcmp(argv[1], "view") == 0)
    {
        u16Length = (U16) simple_strtoul(argv[2], NULL, 16);
    }
    else
    {
        u16Offset = (U16) simple_strtoul(argv[1], NULL, 16);
        u8Newvalue = (U8) simple_strtoul(argv[2], NULL, 16);

        if (u16Offset >= 0x800)
            return CMD_RET_USAGE;
    }

        
    AddrSrc = (unsigned char *)TEMPADDR;           //rom code
    AddrDevinfo = AddrSrc + DEVINFO_OFFSET;
    
RETRY_PROGRAM:

    //get device id, whether it is supported.
    if(u8TryCnt < RETRY_PRO_CNT && MDrv_SpiFlash_TriggerMode_ReadID())
    {
        u8TryCnt++;

        //read romcode in nor flash and renew devinfo
        printf("INFO : Load RomCode from nor flash...\r\n");
        if (!MDrv_SpiFlash_NormalMode_Read(ROMCODE_START, ROMCODE_SIZE, AddrSrc))
        {
            DBG_MAIN( (printf("INFO:ReadFlash Fail!\r\n")));
            printf("INFO : ReadFlash Fail\r\n");
            printf("INFO : retry!\r\n");
            goto RETRY_PROGRAM;
        }
        if(strcmp(argv[1], "view") == 0)
        {
            DumpBuff(AddrDevinfo, u16Length);
            return TRUE;
        }
        else
        {
            printf("INFO : Modify Devinfo...\r\n");
            *(AddrDevinfo + u16Offset) = u8Newvalue;
        }
    }

    ret = do_BurnCode(AddrSrc, ROMCODE_SIZE, ROMCODE_START);
    return ret;
}



U_BOOT_CMD(
	nor,  CONFIG_SYS_MAXARGS,	1,	do_nor,
	"Nor Program Function",
	"\n"
	"nor id - read spi nor flash id\n"
	"nor burn rom Src_Address - burn rom code.\n"
	"nor burn romdev Src_Address - burn rom code and devinfo, devinfo must be loaded in Src_Address + 0x2000000\n"	
	"nor burn devinfo Src_Address - burn devinfo only, keep old rom code\n"
	"nor burn uboot Src_Address - burn uboot\n"
	"nor burn rtkbin Src_Address Src_Length - burn rtk binary\n"
	"nor burn ui Src_Address Src_Length - burn rtk bitmap\n"
);


U_BOOT_CMD(
	devinfo,  CONFIG_SYS_MAXARGS,	1,	do_devinfo,
	"devinfo editor",
	"\n"
	"devinfo view Length\n"
	"devinfo Offset_Addr New_Value - edit devinfo in nor flash , Rom_Code & Devinfo Default Load to 0x45000000\n"
);



