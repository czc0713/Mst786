////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (��MStar Confidential Information��) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _REG_GPIO_H_
#define _REG_GPIO_H_

//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------
#define GPIO_UNIT_NUM               198

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define REG_MIPS_BASE              0x1F000000//Use 8 bit addressing

#define REG_ALL_PAD_IN              (0x101ea0)   //set all pads (except SPI) as input
#define REG_LVDS_BASE               (0x103200)
#define REG_LVDS_BANK               REG_LVDS_BASE

#define PAD_PM_GPIO0    0
#define PAD_PM_GPIO1    1
#define PAD_PM_IIC_SDA  2
#define PAD_PM_IIC_SCL  3
#define PAD_PM_MIIC_SDA 4
#define PAD_PM_MIIC_SCL 5
#define PAD_PM_SPI_CK   6
#define PAD_PM_SPI_CZ0  7
#define PAD_PM_SPI_GPIO 8
#define PAD_PM_SPI_DI   9
#define PAD_PM_SPI_DO   10
#define PAD_ONOFF   11
#define PAD_CHRGDET 12
#define PAD_32K_OUT 13
#define PAD_IRIN    14
#define PAD_CEC 15
#define PAD_PM_SD0_CDZ  16
#define PAD_PM_SD1_CDZ  17
#define PAD_PM_SD2_CDZ  18
#define PAD_PM_SD_GPIO  19
#define PAD_HOTPLUG 20
#define PAD_BATOK   21
#define PAD_PMIC_CHIPEN 22
#define PAD_PMIC_STDBYN 23
#define PAD_PMIC_INT    24
#define PAD_SD0_D1  25
#define PAD_SD0_D0  26
#define PAD_SD0_CLK 27
#define PAD_SD0_CMD 28
#define PAD_SD0_D3  29
#define PAD_SD0_D2  30
#define PAD_SD1_D1  31
#define PAD_SD1_D0  32
#define PAD_SD1_CLK 33
#define PAD_SD1_CMD 34
#define PAD_SD1_D3  35
#define PAD_SD1_D2  36
#define PAD_A_GPS_EVENT 37
#define PAD_AFE1_SGN    38
#define PAD_AFE1_MAG    39
#define PAD_RFSPI_CZ1   40
#define PAD_AFE0_SGN    41
#define PAD_AFE0_MAG    42
#define PAD_RFSPI_CLK   43
#define PAD_RFSPI_DATA  44
#define PAD_RFSPI_CZ0   45
#define PAD_BTI_RX_TX   46
#define PAD_BTI_DATA1   47
#define PAD_BTI_DATA0   48
#define PAD_BT_CLK_24M  49
#define PAD_BT_SPI_SCLK 50
#define PAD_BT_SPI_SDATA    51
#define PAD_BT_SPI_SENB 52
#define PAD_BT_CHIP_EN  53
#define PAD_SR_D0   54
#define PAD_SR_D1   55
#define PAD_SR_D2   56
#define PAD_SR_D3   57
#define PAD_SR_D4   58
#define PAD_SR_D5   59
#define PAD_SR_D6   60
#define PAD_SR_D7   61
#define PAD_SR_PCLK 62
#define PAD_SR_HSYNC    63
#define PAD_SR_STROBE   64
#define PAD_SR_VSYNC    65
#define PAD_SR_RST  66
#define PAD_SR_PWRDN    67
#define PAD_GPIO33  68
#define PAD_GPIO32  69
#define PAD_GPIO31  70
#define PAD_GPIO30  71
#define PAD_GPIO29  72
#define PAD_GPIO28  73
#define PAD_GPIO27  74
#define PAD_GPIO26  75
#define PAD_GPIO25  76
#define PAD_GPIO24  77
#define PAD_UART_RX1    78
#define PAD_UART_TX1    79
#define PAD_UART_RX2    80
#define PAD_UART_TX2    81
#define PAD_UART_RX3    82
#define PAD_UART_TX3    83
#define PAD_UART_RX4    84
#define PAD_UART_TX4    85
#define PAD_UART_CTS2   86
#define PAD_UART_RTS2   87
#define PAD_GPIO34  88
#define PAD_GPIO35  89
#define PAD_GPIO36  90
#define PAD_GPIO37  91
#define PAD_GPIO38  92
#define PAD_MPIF_CS1Z   93
#define PAD_MPIF_CS0Z   94
#define PAD_MPIF_D0 95
#define PAD_MPIF_D1 96
#define PAD_MPIF_D2 97
#define PAD_MPIF_D3 98
#define PAD_MPIF_CK 99
#define PAD_MPIF_BUSY   100
#define PAD_MIIC0_SDA   101
#define PAD_MIIC0_SCL   102
#define PAD_GPIO23  103
#define PAD_GPIO22  104
#define PAD_GPIO21  105
#define PAD_GPIO20  106
#define PAD_GPIO19  107
#define PAD_GPIO18  108
#define PAD_GPIO17  109
#define PAD_GPIO16  110
#define PAD_GPIO15  111
#define PAD_GPIO14  112
#define PAD_GPIO13  113
#define PAD_GPIO12  114
#define PAD_GPIO11  115
#define PAD_GPIO10  116
#define PAD_GPIO9   117
#define PAD_GPIO8   118
#define PAD_GPIO7   119
#define PAD_GPIO6   120
#define PAD_GPIO5   121
#define PAD_GPIO4   122
#define PAD_GPIO3   123
#define PAD_GPIO2   124
#define PAD_GPIO1   125
#define PAD_GPIO0   126
#define PAD_TESTPIN 127
#define PAD_SPDIF_OUT   128
#define PAD_IIS_TRX_BCK 129
#define PAD_IIS_TRX_WS  130
#define PAD_IIS_TRX_OUT 131
#define PAD_IIS_TRX_IN  132
#define PAD_UART_RX5    133
#define PAD_UART_TX5    134
#define PAD_UART_CTS1   135
#define PAD_UART_RTS1   136
#define PAD_TCON_GPIO8  137
#define PAD_TCON_GPIO7  138
#define PAD_TCON_GPIO6  139
#define PAD_TCON_GPIO5  140
#define PAD_TCON_GPIO4  141
#define PAD_TCON_GPIO3  142
#define PAD_TCON_GPIO2  143
#define PAD_TCON_GPIO1  144
#define PAD_TCON_GPIO0  145
#define PAD_TTL_GPIO11  146
#define PAD_TTL_GPIO10  147
#define PAD_TTL_GPIO9   148
#define PAD_TTL_GPIO8   149
#define PAD_TTL_GPIO7   150
#define PAD_TTL_GPIO6   151
#define PAD_TTL_GPIO5   152
#define PAD_TTL_GPIO4   153
#define PAD_TTL_GPIO3   154
#define PAD_TTL_GPIO2   155
#define PAD_TTL_GPIO1   156
#define PAD_TTL_GPIO0   157
#define PAD_NAND_WPZ    158
#define PAD_NAND_WEZ    159
#define PAD_NAND_ALE    160
#define PAD_NAND_CLE    161
#define PAD_NAND_CE3Z   162
#define PAD_NAND_CE2Z   163
#define PAD_NAND_CE1Z   164
#define PAD_NAND_CE0Z   165
#define PAD_NAND_REZ    166
#define PAD_NAND_RBZ    167
#define PAD_NAND_DA0    168
#define PAD_NAND_DA1    169
#define PAD_NAND_DA2    170
#define PAD_NAND_DA3    171
#define PAD_NAND_DQS    172
#define PAD_NAND_DA4    173
#define PAD_NAND_DA5    174
#define PAD_NAND_DA6    175
#define PAD_NAND_DA7    176
#define PAD_SD2_D1  177
#define PAD_SD2_D0  178
#define PAD_SD2_CLK 179
#define PAD_SD2_CMD 180
#define PAD_SD2_D3  181
#define PAD_SD2_D2  182

////8 bit define
#define REG_PMGPIO_00  0x0f00
#define REG_PMGPIO_01  0x0f01
#define REG_GPIO1_00   0x101A00////0
#define REG_GPIO1_01   0x101A01
#define REG_GPIO1_02   0x101A02////1
#define REG_GPIO1_03   0x101A03
#define REG_GPIO1_04   0x101A04////2
#define REG_GPIO1_05   0x101A05
#define REG_GPIO1_06   0x101A06////3
#define REG_GPIO1_07   0x101A07
#define REG_GPIO1_20   0x101A20 ////10
#define REG_GPIO1_21   0x101A21
#define REG_GPIO1_22   0x101A22 ////11
#define REG_GPIO1_23   0x101A23
#define REG_GPIO1_24   0x101A24 ////12
#define REG_GPIO1_25   0x101A25
#define REG_GPIO1_26   0x101A26 ////13
#define REG_GPIO1_27   0x101A27
#define REG_GPIO1_2A   0x101A2A ////15
#define REG_GPIO1_2B   0x101A2B
#define REG_GPIO1_2C   0x101A2C ////16
#define REG_GPIO1_2D   0x101A2D




#define GPIO_OEN    0   //set o to nake output
#define GPIO_ODN    1

#define IN_HIGH    1   //input high
#define IN_LOW     0   //input low

#define OUT_HIGH    1   //output high
#define OUT_LOW     0   //output low

#define MHal_GPIO_REG(addr)             (*(volatile U8*)(REG_MIPS_BASE + (((addr) & ~1)<<1) + (addr & 1)))

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

#endif // _REG_GPIO_H_

