/*------------------------------------------------------------------------------
	Copyright (c) 2008 MStar Semiconductor, Inc.  All rights reserved.
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
    PROJECT: Columbus

	FILE NAME: drivers/serial/serial_columbus.c

    DESCRIPTION:
        Serial driver

    HISTORY:
        <Date>     <Author>    <Modification Description>
        2008/07/23  Fred Cheng  Initial Created

------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
    Include Files
------------------------------------------------------------------------------*/
#include <common.h>
#include <command.h>
#include "asm/arch/mach/ms_types.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"
#include "serial.h"

DECLARE_GLOBAL_DATA_PTR;

#define UART_BAUDRATE_DEFAULT  CONFIG_BAUDRATE

#define UART_ENABLE_REG (GET_REG_ADDR(MS_BASE_REG_CHIPTOP_PA,0x64))
#define UART_REG8(_x_)  ((U8 volatile *)(UART_BASE))[((_x_) * 4) - ((_x_) & 1)]


#define UART_BASE MS_BASE_REG_UART0_PA
#define UART4_BASE MS_BASE_REG_UART4_PA
#define UART4_REG8(_x_)  ((U8 volatile *)(UART4_BASE))[((_x_) * 4) - ((_x_) & 1)]

/*------------------------------------------------------------------------------
    Function: serial_init

    Description:
        Init UART
    Input:
        None.
    Output:
        None.
    Return:
        already return 0
    Remark:
        None.
-------------------------------------------------------------------------------*/
int serial_init(void)
{


	//i.   Set "reg_mcr_loopback";
	UART_REG8(UART_MCR) |= 0x10;

	//ii.   Poll "reg_usr_busy" till 0;
	while(UART_REG8(UART_USR) & 0x01)
	{
		UART_REG8(UART_IIR)=(UART_REG8(UART_IIR)|(UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT));
	}

    UART_REG8(UART_IER) = 0x00;

    // Reset receiver and transmiter
    UART_REG8(UART_FCR) = UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT | UART_FCR_TRIGGER_1;

    // Set 8 bit char, 1 stop bit, no parity
    UART_REG8(UART_LCR) = UART_LCR_WLEN8 & ~(UART_LCR_STOP2 | UART_LCR_PARITY);

    //i.   Set "reg_mcr_loopback back;
	UART_REG8(UART_MCR) &= ~0x10;

    serial_setbrg();

    return (0);
}


void ms_putc(const char c)
{
    while (!(UART_REG8(UART_LSR) & UART_LSR_THRE));

    UART_REG8(UART_TX) = c;

}

/*------------------------------------------------------------------------------
    Function: serial_putc

    Description:
        Write a character to serial port
    Input:
        c - the character which will be written
    Output:
        None.
    Return:
        None.
    Remark:
        None.
-------------------------------------------------------------------------------*/
void serial_putc (const char c)
{
	if (c == '\n')
		ms_putc('\r');

	ms_putc(c);
}

/*------------------------------------------------------------------------------
    Function: serial_putc

    Description:
        Read a character from UART
    Input: (The arguments were used by caller to input data.)
        None.
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
        c - the character got from UART
    Remark:
        None.
-------------------------------------------------------------------------------*/
int serial_getc (void)
{
    char c;
//
    while(!(UART_REG8(UART_LSR) & UART_LSR_DR));

    c=(char) ( UART_REG8(UART_TX) & 0xff);

    return c;
}

/*------------------------------------------------------------------------------
    Function: serial_tstc

    Description:
        Check if UART's LSR_DR bit
    Input:
        None.
    Output:
        None.
    Return:
        value of UART's LSR_DR bit
    Remark:
        None.
-------------------------------------------------------------------------------*/
int serial_tstc (void)
{

    return  ((UART_REG8(UART_LSR) & UART_LSR_DR));
}

/*------------------------------------------------------------------------------
    Function: serial_tstc

    Description:
        set UART's baud rate
    Input:
        None.
    Output:
        None.
    Return:
        None.
    Remark:
        None.
-------------------------------------------------------------------------------*/
void serial_setbrg (void)
{
//    // set baud_rate
//
#if CONFIG_ARCH_CEDRIC
    U16 DLR = ((123000000+8*UART_BAUDRATE_DEFAULT)/ (16 * UART_BAUDRATE_DEFAULT));
#else
#error unknown UART_CLK
#endif
    //i.   Set "reg_mcr_loopback";
    UART_REG8(UART_MCR) |= 0x10;

    //  Clear FIFO Buffer
    UART_REG8(UART_FCR) |= 0x07;

    //ii.   Poll "reg_usr_busy" till 0;
    while(UART_REG8(UART_USR) & 0x01)
    {
    	UART_REG8(UART_IIR)=(UART_REG8(UART_IIR)|(UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT));
    }
//
    UART_REG8(UART_LCR) |= UART_LCR_DLAB;
    UART_REG8(UART_DLL) = (DLR & 0xFF);
    UART_REG8(UART_DLM) = ((DLR >> 8) & 0xFF);
    UART_REG8(UART_LCR) &= ~(UART_LCR_DLAB);
//
    UART_REG8(UART_MCR) &= ~0x10;
}

/*------------------------------------------------------------------------------
    Function: serial_puts

    Description:
        write a string to UART
    Input:
        s - The string which will be written.
    Output:
        None.
    Return:
        None.
    Remark:
        None.
-------------------------------------------------------------------------------*/
void serial_puts (const char *s)
{
    while (*s)
    {
		serial_putc (*s++);
    }
}

/*------------------------------------------------------------------------------
    Function: serial4_init

    Description:
        Init UART4
    Input:
        None.
    Output:
        None.
    Return:
        already return 0
    Remark:
        None.
-------------------------------------------------------------------------------*/
int serial4_init(void)
{

    volatile PREG_CLKGEN0_st        m_pCLKregs;
    volatile PREG_CLKGEN1_st        m_pclkgen1;
    U32 u32ClkBit = 0x02<<2;

	m_pCLKregs =  (PREG_CLKGEN0_st) MS_BASE_REG_CLKGEN0_PA;
	m_pclkgen1 =  (PREG_CLKGEN1_st) MS_BASE_REG_CLKGEN1_PA;
	OUTREG16(&m_pclkgen1->u16REG_CKG_FUART, ((INREG16(&m_pclkgen1->u16REG_CKG_FUART)& 0xFFE0)|(0x1F & u32ClkBit)));
    OUTREG16(&m_pCLKregs->REG_CLKGEN0_17, ((INREG16(&m_pCLKregs->REG_CLKGEN0_17)& 0xE0FF)|(0x1F00 & (u32ClkBit<<8))));
    serial4_setbrg();

	//i.   Set "reg_mcr_loopback";
	UART4_REG8(UART_MCR) |= 0x10;

	//ii.   Poll "reg_usr_busy" till 0;
	while(UART4_REG8(UART_USR) & 0x01)
	{
		UART4_REG8(UART_IIR)=(UART4_REG8(UART_IIR)|(UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT));
	}

    UART4_REG8(UART_IER) = 0x00;

    // Reset receiver and transmiter
    UART4_REG8(UART_FCR) = UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT | UART_FCR_TRIGGER_1;

    // Set 8 bit char, 1 stop bit, no parity
    UART4_REG8(UART_LCR) = UART_LCR_WLEN8 & ~(UART_LCR_STOP2 | UART_LCR_PARITY);

    //i.   Set "reg_mcr_loopback back;
	UART4_REG8(UART_MCR) &= ~0x10;

    OUTREG16(MS_BASE_REG_CHIPTOP_PA + CHIPTOP_REG_54,((INREG16(MS_BASE_REG_CHIPTOP_PA + CHIPTOP_REG_54)& 0xFFF0)|0x0000));
    SETREG16(MS_BASE_REG_CHIPGPIO1_PA + CHIPTOP_REG_03,BIT_5);

    return (0);
}

/*------------------------------------------------------------------------------
    Function: serial4_tstc

    Description:
        set UART's baud rate
    Input:
        None.
    Output:
        None.
    Return:
        None.
    Remark:
        None.
-------------------------------------------------------------------------------*/
void serial4_setbrg (void)
{
//    // set baud_rate
//
    U16 DLR = ((147333333+8*UART_BAUDRATE_DEFAULT)/ (16 * UART_BAUDRATE_DEFAULT));

    //i.   Set "reg_mcr_loopback";
    UART4_REG8(UART_MCR) |= 0x10;

    //  Clear FIFO Buffer
    UART4_REG8(UART_FCR) |= 0x07;

    //ii.   Poll "reg_usr_busy" till 0;
    while(UART4_REG8(UART_USR) & 0x01)
    {
    	UART4_REG8(UART_IIR)=(UART4_REG8(UART_IIR)|(UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT));
    }
//
    UART4_REG8(UART_LCR) |= UART_LCR_DLAB;
    UART4_REG8(UART_DLL) = (DLR & 0xFF);
    UART4_REG8(UART_DLM) = ((DLR >> 8) & 0xFF);
    UART4_REG8(UART_LCR) &= ~(UART_LCR_DLAB);

    UART4_REG8(UART_LCR) = UART_LCR_WLEN8 & (~(UART_LCR_STOP2 | UART_LCR_PARITY));
//
    UART4_REG8(UART_MCR) &= ~0x10;
}


void serial4_ms_putc(const char c)
{
    while (!(UART4_REG8(UART_LSR) & UART_LSR_THRE));
    //while (!(UART4_REG8(UART_USR) & 0x02));

    UART4_REG8(UART_TX) = c;

}

/*------------------------------------------------------------------------------
    Function: serial4_putc

    Description:
        Read a character from UART
    Input: (The arguments were used by caller to input data.)
        None.
    Output: (The arguments were used by caller to receive data.)
        None.
    Return:
        c - the character got from UART
    Remark:
        None.
-------------------------------------------------------------------------------*/
int serial4_getc (void)
{
    char c;
//
    while(!(UART4_REG8(UART_LSR) & UART_LSR_DR));

    c=(char) ( UART4_REG8(UART_TX) & 0xff);

    return c;
}

/*------------------------------------------------------------------------------
    Function: serial4_putc

    Description:
        Write a character to serial port
    Input:
        c - the character which will be written
    Output:
        None.
    Return:
        None.
    Remark:
        None.
-------------------------------------------------------------------------------*/
void serial4_putc (const char c)
{
	if (c == '\n')
		serial4_ms_putc('\r');

	serial4_ms_putc(c);
}

/*------------------------------------------------------------------------------
    Function: serial4_tstc

    Description:
        Check if UART4's LSR_DR bit
    Input:
        None.
    Output:
        None.
    Return:
        value of UART4's LSR_DR bit
    Remark:
        None.
-------------------------------------------------------------------------------*/
int serial4_tstc (void)
{

    return  ((UART4_REG8(UART_LSR) & UART_LSR_DR));
}


/*------------------------------------------------------------------------------
    Function: serial4_puts

    Description:
        write a string to UART
    Input:
        s - The string which will be written.
    Output:
        None.
    Return:
        None.
    Remark:
        None.
-------------------------------------------------------------------------------*/
void serial4_puts (const char *s,unsigned int num)
{
    while (num--)
    {
		serial4_putc (*s++);
    }
}


void uboot_TellMCU_OpenBackLight()
{

    U32 u32LogMask = 0;
    U16 u16Baud = 0;
    U8  u8PadSel =0;
    char	i,j;
    U32 u32ClkBit = 0x02<<2;

    char tell_mcu[] = {0x0F,0x03,0x12,0x24,0x00,0x39};
    serial4_init();
    printf("czc  :uboot_TellMCU_OpenBackLight Start!!\r\n");

	//uart4
	for(j=0;j<3;j++)
	{
	    serial4_puts(tell_mcu,sizeof(tell_mcu));
        printf("czc  :uboot_TellMCU_OpenBackLight %d times\r\n",j+1);
	}
    printf("czc  :uboot_TellMCU_OpenBackLight End------\r\n");

}

