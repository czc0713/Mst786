// $Change: 449039 $
//------------------------------------------------------------------------------
//
// Copyright (c) 2012 MStar Semiconductor, Inc.  All rights reserved.
//
//------------------------------------------------------------------------------
// FILE
//      columbus_spi.h
//
// DESCRIPTION
//      Define the gdma registers
//
// HISTORY
//      2012.02.21       Lei.qin      Initial Version
//
//------------------------------------------------------------------------------
#ifndef __COLUMBUS_SPI_H
#define __COLUMBUS_SPI_H

#if __cplusplus
extern "C" {
#endif  //__cplusplus

//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------
//#include "columbus_typedefs.h"
//#include "bsp_cfg.h"
#include "asm/arch/mach/ms_types.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"

#include <common.h>
#include <command.h>
#include <malloc.h>

#ifndef DWORD
typedef unsigned long DWORD;
typedef unsigned short WORD;
#endif

//------------------------------------------------------------------------------
//  Macros
//------------------------------------------------------------------------------
#define ROMCODE_START   0x0000
#define ROMCODE_SIZE    0x10000         //64K

#define DEVINFO_OFFSET  0x7800        // 30K-32K
#define DEVINFO_SIZE    0x800           //2*1024         

#define UBOOT_START     0x10000
#define UBOOT_SIZE      0x80000         //64K

#define RTK_START       0X90000
#define RTK_SIZE        0x100000        //1MB

#define BITMAP_START    0x190000


#define SERFLASH_TYPE_MX25L3205     1
#define SERFLASH_TYPE_MX25L6405     2
#define SERFLASH_TYPE_MX25L12805    3
#define SERFLASH_TYPE_S25FL064A     4
#define SERFLASH_TYPE_EN25B64     5
#define SERFLASH_TYPE_EN25B16     6
#define SERFLASH_TYPE_SST25VF010     7

#define SERFLASH_TYPE   SERFLASH_TYPE_MX25L3205
//#define ESMT_F25L016A

/* refer to drvop2.h and drvgop.h */
#define BK_REG(reg)                   ((reg) * 4)
#define GET_REG16_ADDR(x, y)         (x+(y)*4)
#define COLUMBUS_BASE_REG_RIU_PA            (0x1F000000)

#define CEDRIC_BASE_REG_ISP_PA           GET_REG16_ADDR(COLUMBUS_BASE_REG_RIU_PA, 0x000400)
#define CEDRIC_BASE_REG_FSP_PA           GET_REG16_ADDR(COLUMBUS_BASE_REG_RIU_PA, 0x000460)
#define CEDRIC_BASE_REG_QSPI_PA          GET_REG16_ADDR(COLUMBUS_BASE_REG_RIU_PA, 0x000470)
#define CEDRIC_BASE_REG_PM_TOP_PA        GET_REG16_ADDR(COLUMBUS_BASE_REG_RIU_PA, 0x000F00)



//------------------------------------------------------------------------------
//  Constants
//------------------------------------------------------------------------------
// Bootloader version.
#define COLUMBUS_BASE_REG_ISP_PA                (0xA0001000)
#define COLUMBUS_BASE_REG_PIU_PA                (0xA0007800)


//#define REG_ISP_BASE                COLUMBUS_BASE_REG_ISP_PA|COLUMBUS_BASE_REG_RIU_PA
#define REG_ISP_BASE        CEDRIC_BASE_REG_ISP_PA
#define ISP_REG16(addr)             *((volatile U32*)(REG_ISP_BASE+addr*4))
//#define REG_PIU_BASE                COLUMBUS_BASE_REG_PIU_PA|COLUMBUS_BASE_REG_RIU_PA
#define REG_PIU_BASE        (0x1F007800)


#define PIU_REG16(addr)             *((volatile U32*)(REG_PIU_BASE+addr*4))

#define REG_ISP_PASSWORD            0x00 // ISP / XIU read / DMA mutual exclusive
#define REG_ISP_SPI_COMMAND         0x01
#define CMD_WRITE_ENABLE                    0x06
#define CMD_WRITE_DISABLE                   0x04
#define CMD_READ_STATUS_REG                 0x05    // Read Status Register
#define CMD_WRITE_STATUS_REG                0x01    // Write Status Register
#define CMD_READ                            0x03
#define CMD_FAST_READ                       0x0B

#define REG_ISP_SPI_ADDR_L          0x02 // A[15:0]
#define REG_ISP_SPI_ADDR_H          0x03 // A[23:16]
#define REG_ISP_SPI_WDATA           0x04
#define REG_ISP_SPI_RDATA           0x05
#define REG_ISP_SPI_CLKDIV          0x06 // clock = CPU clock / this div

#define SPI_CLK_DIV2                        0x00000000 // BIT0
#define SPI_CLK_DIV4                        0x00000004 // BIT2
#define SPI_CLK_DIV8                        0x00000040 // BIT6
#define SPI_CLK_DIV16                       0x00000080 // BIT7
#define SPI_CLK_DIV32                       0x00000100 // BIT8
#define SPI_CLK_DIV64                       0x00000200 // BIT9
#define SPI_CLK_DIV128                      0x00000400 // BIT10

#define REG_ISP_DEV_SEL             0x07
#define BRAND_PMC                           0x00
#define BRAND_NEXTFLASH                     0x01
#define BRAND_ST                            0x02
#define BRAND_SST                           0x03
#define BRAND_ATMEL                         0x04
#define MAX_SPI_BRAND                       0x05

#define REG_ISP_SPI_CECLR           0x08
#define REG_ISP_SPI_TBP           0x0A
#define REG_ISP_SPI_RDREQ           0x0C
#define REG_ISP_SPI_ENDIAN          0x0F
#define REG_ISP_SPI_RD_DATARDY      0x15
#define REG_ISP_SPI_WR_DATARDY      0x16
#define REG_ISP_SPI_WR_CMDRDY       0x17
#define REG_ISP_SPI_AAI_NUM       0x21
#define REG_ISP_SPI_AAI_REG       0x22
#define REG_ISP_TRIGGER_MODE        0x2A
#define REG_ISP_CHIP_SEL            0x36
#define SFSH_CHIP_SEC_MASK                  0x00FF
#define SFSH_CHIP_SEL_RIU                   0x0080
#define SFSH_CHIP_SEL_XIU                   0x0000

#define REG_PIU_DMA_STATUS          0x10 // [1]done [2]busy [8:15]state
#define REG_PIU_SPI_CLK_SRC         0x26 // SPI clock source  [0]:gate  [1]:inv  [4:2]:clk_sel  000:Xtal clock 001:27MHz 010:36MHz 011:43.2MHz 100:54MHz 101:72MHz 110:86MHz 111:108MHz  [5]:0:xtal 1:clk_Sel
#define REG_PIU_DMA_SPISTART_L      0x70 // [15:0]
#define REG_PIU_DMA_SPISTART_H      0x71 // [23:16]
#define REG_PIU_DMA_DRAMSTART_L     0x72 // [15:0]  in unit of B; must be 8B aligned
#define REG_PIU_DMA_DRAMSTART_H     0x73 // [23:16]
#define REG_PIU_DMA_SIZE_L          0x74 // [15:0]  in unit of B; must be 8B aligned
#define REG_PIU_DMA_SIZE_H          0x75 // [23:16]
#define REG_PIU_DMA_CMD             0x76
#define PIU_DMA_CMD_FIRE                    0x0001
#define PIU_DMA_CMD_LE                      0x0000
#define PIU_DMA_CMD_BE                      0x0020


#ifndef RIU_BASE
#define RIU_BASE                            COLUMBUS_BASE_REG_RIU_PA      // Virtual address
#endif
//#define REG_BANK_CHIPTOP                  0x0F00U // FIXME
#define OSC_CLK_26000000              26000000
#define MASTER_CLOCK_HZ               OSC_CLK_26000000//OSC_CLK_16369000

// CHIPTOP offsets
#define REG_SPI_CS                      0x0A

// CHIPTOP absolute address
#define CHIPTOP_REG_SPI_CS              (RIU_BASE + ((REG_BANK_CHIPTOP + REG_SPI_CS) << 2))

// ISP base address
#define REG_BANK_ISP                    0x0400

// Register offsets of ISP
#define REG_PASSWORD_OFFSET         0x00
#define REG_COMMAND_OFFSET          0x01
#define REG_LO_ADDRESS_OFFSET           0x02
#define REG_HI_ADDRESS_OFFSET           0x03
#define REG_WDATA_OFFSET                0x04
#define REG_RDATA_OFFSET                0x05
#define REG_SPI_CLK_DIV_OFFSET          0x06
#define REG_DEVICE_SELECT_OFFSET        0x07
#define REG_SPI_CE_CLR_OFFSET           0x08
// ...
#define REG_SPI_RD_REQ_OFFSET           0x0C
#define REG_ENDIAN_SEL_SPI_OFFSET       0x0F
// ...
#define REG_RD_DATA_RDY_OFFSET      0x15
#define REG_WR_DATA_RDY_OFFSET      0x16
#define REG_WR_CMD_RDY_OFFSET       0x17
// ...
#define REG_TRIGGER_MODE_OFFSET     0x2A

// Absolute register address
#define ISP_REG_PASSWORD                (RIU_BASE + ((REG_BANK_ISP + REG_PASSWORD_OFFSET) << 2))
#define ISP_REG_COMMAND             (RIU_BASE + ((REG_BANK_ISP + REG_COMMAND_OFFSET) << 2))
#define ISP_REG_ADDRESS_LO              (RIU_BASE + ((REG_BANK_ISP + REG_LO_ADDRESS_OFFSET) << 2))
#define ISP_REG_ADDRESS_HI              (RIU_BASE + ((REG_BANK_ISP + REG_HI_ADDRESS_OFFSET) << 2))
#define ISP_REG_WRITE_DATA          (RIU_BASE + ((REG_BANK_ISP + REG_WDATA_OFFSET) << 2))
#define ISP_REG_READ_DATA               (RIU_BASE + ((REG_BANK_ISP + REG_RDATA_OFFSET) << 2))
#define ISP_REG_SPI_CLK_DIV             (RIU_BASE + ((REG_BANK_ISP + REG_SPI_CLK_DIV_OFFSET) << 2))
#define ISP_REG_DEVICE_SELECT           (RIU_BASE + ((REG_BANK_ISP + REG_DEVICE_SELECT_OFFSET) << 2))
#define ISP_REG_CE_CLR                  (RIU_BASE + ((REG_BANK_ISP + REG_SPI_CE_CLR_OFFSET) << 2))
#define ISP_REG_READ_REQUEST            (RIU_BASE + ((REG_BANK_ISP + REG_SPI_RD_REQ_OFFSET) << 2))
#define ISP_REG_ENDIAN_LITTLE           (RIU_BASE + ((REG_BANK_ISP + REG_ENDIAN_SEL_SPI_OFFSET) << 2))
#define ISP_REG_READ_DATA_READY     (RIU_BASE + ((REG_BANK_ISP + REG_RD_DATA_RDY_OFFSET) << 2))
#define ISP_REG_WRITE_DATA_READY        (RIU_BASE + ((REG_BANK_ISP + REG_WR_DATA_RDY_OFFSET) << 2))
#define ISP_REG_WRITE_CMD_READY     (RIU_BASE + ((REG_BANK_ISP + REG_WR_CMD_RDY_OFFSET) << 2))
#define ISP_REG_TRIGGER_MODE            (RIU_BASE + ((REG_BANK_ISP + REG_TRIGGER_MODE_OFFSET) << 2))

// Supported SPI flash vendors
#define SPIFLASH_VENDOR_PMC                 0
#define SPIFLASH_VENDOR_NEXTFLASH           1
#define SPIFLASH_VENDOR_ST                  2
#define SPIFLASH_VENDOR_SST                 3
#define SPIFLASH_VENDOR_ATMEL               4
#define SPIFLASH_VENDOR_ESMT                5

#define DBG_ISP_FLOW        0
#define RETRY_PRO_CNT       2
#define SPI_SPEED_LEVEL     8
#define SPI_SPEED_GPS       2
#define SPI_SPEED_NOR       2
#define SPI_SPEED_UART      4

#define ST_M25P32        1

// SPI Flash constants
#ifdef ESMT_F25L016A
#define SPIFLASH_ESMT_F25L016A_SECTOR_SIZE           (4096)
#define SPIFLASH_ESMT_F25L016A_TOTAL_NUM_SECTORS     (512)
#define SPIFLASH_ESMT_F25L016A_BLOCK_SIZE           (16 * SPIFLASH_ESMT_F25L016A_SECTOR_SIZE)
#define SPIFLASH_ESMT_F25L016A_TOTAL_NUM_BLOCK (SPIFLASH_ESMT_F25L016A_TOTAL_NUM_SECTORS / 16)
#endif

#ifdef ESMT_F25L016A
#define SPI_FLASH_PAGE_SIZE             SPIFLASH_ESMT_F25L016A_SECTOR_SIZE
#define SPI_FLASH_SECTOR_SIZE          16 /* one sector has 16 Page  */
#define SPI_FLASH_TOTAL_NUM_SECTORS    SPIFLASH_ESMT_F25L016A_TOTAL_NUM_BLOCK
#else
#define SPI_FLASH_PAGE_SIZE             (256)
#define SPI_FLASH_SECTOR_SIZE           g_PagePerSector //(16 )
#define SPI_FLASH_BLOCK_SIZE            g_BlockSize //64*1024
#define SPI_FLASH_TOTAL_NUM_SECTORS     g_SectorCunt//(64)2048
#endif

#define SPI_FLASH_SECTOR_SIZE_SHIFT     (16)
#define SPI_FLASH_SIZE                  (SPI_FLASH_TOTAL_NUM_SECTORS*SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE)


#define SPIFLASH_ST_M25P32_SIZE_OF_RDID     20      // the size of the returned values in bytes
#define SPIFLASH_ST_M25P32_MANUFACTURER_ID  0x20    // manufacturer id
#define SPIFLASH_ST_M25P32_MEMORY_TYPE      0x20    // memory type
#define SPIFLASH_ST_M25P32_MEMORY_CAPACITY  0x16    // memory capacity
#define SPIFLASH_ST_M25P32_UNIQUE_ID        0x10    // unique id, which menas there are 16 bytes of CFI followed the UID

#define SPIFLASH_ESMT_F25L016A_MANUFACTURER_ID 0x8C // manufacturer id
#define SPIFLASH_ESMT_F25L016A_MEMORY_TYPE      0x20    // memory type
#define SPIFLASH_ESMT_F25L016A_MEMORY_CAPACITY  0x15    // memory capacity


// flash manufacturer id, returned by RDID
#ifdef ESMT_F25L016A
#define SPIFLASH_MANUFACTURER_ID    SPIFLASH_ESMT_F25L016A_MANUFACTURER_ID    // manufacturer id
#define SPIFLASH_MEMORY_TYPE        SPIFLASH_ESMT_F25L016A_MEMORY_TYPE            // memory type
#define SPIFLASH_MEMORY_CAPACITY    SPIFLASH_ESMT_F25L016A_MEMORY_CAPACITY     // memory capacity
#else
#define SPIFLASH_MANUFACTURER_ID    SPIFLASH_ST_M25P32_MANUFACTURER_ID  // manufacturer id
#define SPIFLASH_MEMORY_TYPE        SPIFLASH_ST_M25P32_MEMORY_TYPE  // memory type
#define SPIFLASH_MEMORY_CAPACITY    SPIFLASH_ST_M25P32_MEMORY_CAPACITY  // memory capacity
#define SPIFLASH_UNIQUE_ID      SPIFLASH_ST_M25P32_UNIQUE_ID    // unique id, which menas there are 16 bytes of CFI followed the UID
#endif


#define SPIFLASH_ST_M25P32_PROTECT_NONE         0x00    // none
#define SPIFLASH_ST_M25P32_PROTECT_UPPER64TH    0x01    // Sector #63
#define SPIFLASH_ST_M25P32_PROTECT_UPPER32ND    0x02    // Sector #62-63
#define SPIFLASH_ST_M25P32_PROTECT_UPPER16TH    0x03    // Sector #60-63
#define SPIFLASH_ST_M25P32_PROTECT_UPPER8TH     0x04    // Sector #56-63
#define SPIFLASH_ST_M25P32_PROTECT_UPPERQUARTER 0x05    // Sector #48-63
#define SPIFLASH_ST_M25P32_PROTECT_UPPERHALF    0x06    // Sector #32-63
#define SPIFLASH_ST_M25P32_PROTECT_ALL          0x07    // Sector #0-63

#define SPIFLASH_ESMT_F25L016A_PROTECT_NONE         0x00    // none
#define SPIFLASH_ESMT_F25L016A_PROTECT_UPPER32TH    0x01    // Block #31
#define SPIFLASH_ESMT_F25L016A_PROTECT_UPPER16ND    0x02    // Block #30-31
#define SPIFLASH_ESMT_F25L016A_PROTECT_UPPER8TH 0x03    // Block #28-31
#define SPIFLASH_ESMT_F25L016A_PROTECT_UPPERQUARTER 0x04    // Block #24-31
#define SPIFLASH_ESMT_F25L016A_PROTECT_UPPERHALF    0x05    // Block #16-31
#define SPIFLASH_ESMT_F25L016A_PROTECT_ALL          0x07    // Block #0-31

#define SPIFLASH_MXIC_25LXX_PROTECT_NONE          0x00    // none
#define SPIFLASH_MXIC_25LXX_PROTECT_UPPER32TH    0x01    // Block #31
#define SPIFLASH_MXIC_25LXX_PROTECT_UPPER16ND    0x02    // Block #30-31
#define SPIFLASH_MXIC_25LXX_PROTECT_UPPER8TH     0x03    // Block #28-31
#define SPIFLASH_MXIC_25LXX_PROTECT_UPPERQUARTER 0x04    // Block #24-31
#define SPIFLASH_MXIC_25LXX_PROTECT_UPPERHALF    0x05    // Block #16-31
#define SPIFLASH_MXIC_25LXX_PROTECT_ALL          0x08    // Block all


// Protect range
#ifdef ESMT_F25L016A
#define SPIFLASH_PROTECT_NONE           SPIFLASH_ESMT_F25L016A_PROTECT_NONE
/*
//These constants varies from vendor to vendor
#define SPIFLASH_PROTECT_UPPER_EIGHTH   1
#define SPIFLASH_PROTECT_UPPER_QUARTER  2
#define SPIFLASH_PROTECT_UPPER_HALF     3
*/
#define SPIFLASH_PROTECT_ALL            SPIFLASH_ESMT_F25L016A_PROTECT_ALL
#else
#define SPIFLASH_PROTECT_NONE           SPIFLASH_ST_M25P32_PROTECT_NONE
/*
//These constants varies from vendor to vendor
#define SPIFLASH_PROTECT_UPPER_EIGHTH   1
#define SPIFLASH_PROTECT_UPPER_QUARTER  2
#define SPIFLASH_PROTECT_UPPER_HALF     3
*/
#define SPIFLASH_PROTECT_ALL            SPIFLASH_ST_M25P32_PROTECT_ALL
#endif



// Flash command
#define ISP_NORMAL_READ_COMMAND         0x03
#define ISP_FAST_READ_COMMAND           0x0B
#define SPI_FLASH_WRITE_STATUS_REGISTER_ST      0x01
#define SPI_FLASH_PAGE_PROGRAM_COMMAND_ST       0x02
#define SPI_FLASH_READ_COMMAND_ST               0x03
#define SPI_FLASH_WRITE_ENABLE_COMMAND_ST       0x06
#define SPI_FLASH_WRSR_ENABLE_COMMAND_SST       0x50

#define SPI_FLASH_RES_COMMAND_ST                0xAB
#define ISP_BULK_ERASE_COMMAND          0xC7
#define ISP_BLOCK_ERASE_COMMAND          0xD8

#define ISP_READ_ID_COMMAND             0x9F
#define SPI_FLASH_DEEP_POWERDOWN_COMMAND_ST     0xB9
#define ISP_PAGE_PROGRAM_COMMAND_ST     0x02

#ifdef ESMT_F25L016A
#define ISP_BYTE_PROGRAM_COMMAND_ST     0x02
#define ISP_AAI_COMMAND_ST 0xAD
#define ISP_ENABLE_WRITE_STATUS_REGISTER_ST 0x50
#endif

// MStar SPI IP specific command
#define ISP_WRITE_SR_COMMAND            0x8001
#define ISP_PAGE_PROGRAM_COMMAND        0x8002
#define ISP_READ_SR_COMMAND             0x8005
#define ISP_SECTOR_ERASE_COMMAND        0xD8//0x8020

// Endian definitions
#define ENDIAN_TYPE_LITTLE              0
#define ENDIAN_TYPE_BIG                 1

#define SPI_FLASH_ENDIAN                ENDIAN_TYPE_LITTLE
//#define SPI_FLASH_ENDIAN              ENDIAN_TYPE_BIG

#define SPI_FLASH_IS_LITTLE_ENDIAN      (SPI_FLASH_ENDIAN == ENDIAN_TYPE_LITTLE)

// Macro definitions
#define ENABLE_ISP()                                        \
    do {                                                    \
        OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_00, 0xAAAA);\
    } while (0)

#define DISABLE_ISP()                                       \
    do {                                                    \
        OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_00, 0x5555);\
    } while (0)

#define ENABLE_TRIGGER_MODE()                               \
    do {                                                    \
        OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_2A, 0x3333);\
    } while (0)

#define DISABLE_TRIGGER_MODE()                              \
    do {                                                    \
        OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_2A, 0x2222);\
    } while (0)

#define CHIP_ENABLE_CLEAR()                                 \
    do {                                                    \
        OUTREG32(CEDRIC_BASE_REG_ISP_PA+REG_08, 0x0001);\
    } while (0)

#define ISP_WRITE_ADDRESS_REG( addr )                       \
    do {                                                    \
        Write32( ISP_REG_ADDRESS_LO, ((addr) & 0xffff) );   \
        Write32( ISP_REG_ADDRESS_HI, ((addr) >> 16));       \
    } while (0)

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

// PIU base address, ISP uses some registers in PIU
#define REG_BANK_PIU                    0x1E00

// Register offsets of PIU
#define REG_DMA_STATE_OFFSET            0x10
#define REG_DMA_FAST_MODE               0x20
#define REG_DMA_SOURCE_ADDRESS_0        0x70
#define REG_DMA_SOURCE_ADDRESS_1        0x71
#define REG_DMA_DESTINATION_ADDRESS_0   0x72
#define REG_DMA_DESTINATION_ADDRESS_1   0x73
#define REG_DMA_SIZE_0                  0x74
#define REG_DMA_SIZE_1                  0x75
#define REG_DMA_RIU_MODE                0x76


// Absolute register address
#define PIU_REG_DMA_STATE_OFFSET            (RIU_BASE + ((REG_BANK_PIU + REG_DMA_STATE_OFFSET)          << 2))
#define PIU_REG_DMA_FAST_MODE               (RIU_BASE + ((REG_BANK_PIU + REG_DMA_FAST_MODE)             << 2))
#define PIU_REG_DMA_SOURCE_ADDRESS_0        (RIU_BASE + ((REG_BANK_PIU + REG_DMA_SOURCE_ADDRESS_0)      << 2))
#define PIU_REG_DMA_SOURCE_ADDRESS_1        (RIU_BASE + ((REG_BANK_PIU + REG_DMA_SOURCE_ADDRESS_1)      << 2))
#define PIU_REG_DMA_DESTINATION_ADDRESS_0   (RIU_BASE + ((REG_BANK_PIU + REG_DMA_DESTINATION_ADDRESS_0) << 2))
#define PIU_REG_DMA_DESTINATION_ADDRESS_1   (RIU_BASE + ((REG_BANK_PIU + REG_DMA_DESTINATION_ADDRESS_1) << 2))
#define PIU_REG_DMA_SIZE_0                  (RIU_BASE + ((REG_BANK_PIU + REG_DMA_SIZE_0)                << 2))
#define PIU_REG_DMA_SIZE_1                  (RIU_BASE + ((REG_BANK_PIU + REG_DMA_SIZE_1)                << 2))
#define PIU_REG_DMA_RIU_MODE                (RIU_BASE + ((REG_BANK_PIU + REG_DMA_RIU_MODE)              << 2))

typedef enum
{
    SPI_SUCCESS = 0,
    SPI_CHANNEL_BUSY,
    SPI_INTERRUPT_NOT_TRIGGERED,
    SPI_INTERRUPT_NOT_CLEAR,
    SPI_TRANSMISSION_FAILED,
    SPI_BUSYWAIT_SUCCESS,
    SPI_UNKNOWN_EVENT_TYPE,
    SPI_UNKNOWN_BUS_TYPE,
    SPI_UNKNOWN_SLAVE_ID,
    SPI_FETAL_ERROR,
    SPI_INVALID_PARAMETER,
    SPI_MMIOSPACE_ERROR,
    SPI_OVER_64BYTES_MPIF_BUG,
    SPI_SPI_WRITE_LAST_BYTE_ERROR,
    SPI_UNKNOWN_ERROR,
    SPI_UNKNOWN_MPIF_ID
} SPI_ERROR_CODE_et;

#define READ_ID_LENGTH 3

//input total char
#define BASE_CMD_INPUT_LEN             5
#define INIT_INPUT_LEN                 1
#define READ_ID_INPUT_LEN              1
#define READ_INPUT_LEN                 3
#define WRITE_INPUT_LEN                3
#define ERASE_CHIP_INPUT_LEN           1
#define ERASE_SEC_INPUT_LEN            3
#define WRITE_PRO_INPUT_LEN            2
#define WRITE_PRO_BOUND_INPUT_LEN      1
#define WRITE_BLK_PRO_INPUT_LEN        3
#define WRITE_BUFF_INPUT_LEN           4
#define READ_BUFF_INPUT_LEN            3
#define GET_CHIP_REV_INPUT_LEN         1
#define GET_FLASH_INFO_INPUT_LEN       1
#define READ_STATUS_INPUT_LEN          1
#define DMA_INPUT_LEN                  4
#define READ_CODE_INPUT_LEN            4
#define WRITE_CODE_INPUT_LEN           4
/* ----------------------------------------------------------------------------
 * GOP_OFFSET
 * ----------------------------------------------------------------------------
 */
#define REG_00                BK_REG(0x00)
#define REG_01                BK_REG(0x01)
#define REG_02                BK_REG(0x02)
#define REG_03                BK_REG(0x03)
#define REG_04                BK_REG(0x04)
#define REG_05                BK_REG(0x05)
#define REG_06                BK_REG(0x06)
#define REG_07                BK_REG(0x07)
#define REG_08                BK_REG(0x08)
#define REG_09                BK_REG(0x09)
#define REG_0A                BK_REG(0x0A)
#define REG_0B                BK_REG(0x0B)
#define REG_0C                BK_REG(0x0C)
#define REG_0D                BK_REG(0x0D)
#define REG_0E                BK_REG(0x0E)
#define REG_0F                BK_REG(0x0F)


#define REG_10                BK_REG(0x10)
#define REG_11                BK_REG(0x11)
#define REG_12                BK_REG(0x12)
#define REG_13                BK_REG(0x13)
#define REG_14                BK_REG(0x14)
#define REG_15                BK_REG(0x15)
#define REG_16                BK_REG(0x16)
#define REG_17                BK_REG(0x17)
#define REG_18                BK_REG(0x18)
#define REG_19                BK_REG(0x19)
#define REG_1A                BK_REG(0x1A)
#define REG_1B                BK_REG(0x1B)
#define REG_1C                BK_REG(0x1C)
#define REG_1D                BK_REG(0x1D)
#define REG_1E                BK_REG(0x1E)
#define REG_1F                BK_REG(0x1F)

#define REG_20                BK_REG(0x20)
#define REG_21                BK_REG(0x21)
#define REG_22                BK_REG(0x22)
#define REG_23                BK_REG(0x23)
#define REG_24                BK_REG(0x24)
#define REG_25                BK_REG(0x25)
#define REG_26                BK_REG(0x26)
#define REG_27                BK_REG(0x27)
#define REG_28                BK_REG(0x28)
#define REG_29                BK_REG(0x29)
#define REG_2A                BK_REG(0x2A)
#define REG_2B                BK_REG(0x2B)
#define REG_2C                BK_REG(0x2C)
#define REG_2D                BK_REG(0x2D)
#define REG_2E                BK_REG(0x2E)
#define REG_2F                BK_REG(0x2F)


#define REG_30                BK_REG(0x30)
#define REG_31                BK_REG(0x31)
#define REG_32                BK_REG(0x32)
#define REG_33                BK_REG(0x33)
#define REG_34                BK_REG(0x34)
#define REG_35                BK_REG(0x35)
#define REG_36                BK_REG(0x36)
#define REG_37                BK_REG(0x37)
#define REG_38                BK_REG(0x38)
#define REG_39                BK_REG(0x39)
#define REG_3A                BK_REG(0x3A)
#define REG_3B                BK_REG(0x3B)
#define REG_3C                BK_REG(0x3C)
#define REG_3D                BK_REG(0x3D)
#define REG_3E                BK_REG(0x3E)
#define REG_3F                BK_REG(0x3F)


#define REG_40                BK_REG(0x40)
#define REG_41                BK_REG(0x41)
#define REG_42                BK_REG(0x42)
#define REG_43                BK_REG(0x43)
#define REG_44                BK_REG(0x44)
#define REG_45                BK_REG(0x45)
#define REG_46                BK_REG(0x46)
#define REG_47                BK_REG(0x47)
#define REG_48                BK_REG(0x48)
#define REG_49                BK_REG(0x49)
#define REG_4A                BK_REG(0x4A)
#define REG_4B                BK_REG(0x4B)
#define REG_4C                BK_REG(0x4C)
#define REG_4D                BK_REG(0x4D)
#define REG_4E                BK_REG(0x4E)
#define REG_4F                BK_REG(0x4F)


#define REG_50                BK_REG(0x50)
#define REG_51                BK_REG(0x51)
#define REG_52                BK_REG(0x52)
#define REG_53                BK_REG(0x53)
#define REG_54                BK_REG(0x54)
#define REG_55                BK_REG(0x55)
#define REG_56                BK_REG(0x56)
#define REG_57                BK_REG(0x57)
#define REG_58                BK_REG(0x58)
#define REG_59                BK_REG(0x59)
#define REG_5A                BK_REG(0x5A)
#define REG_5B                BK_REG(0x5B)
#define REG_5C                BK_REG(0x5C)
#define REG_5D                BK_REG(0x5D)
#define REG_5E                BK_REG(0x5E)
#define REG_5F                BK_REG(0x5F)


#define REG_60                BK_REG(0x60)
#define REG_61                BK_REG(0x61)
#define REG_62                BK_REG(0x62)
#define REG_63                BK_REG(0x63)
#define REG_64                BK_REG(0x64)
#define REG_65                BK_REG(0x65)
#define REG_66                BK_REG(0x66)
#define REG_67                BK_REG(0x67)
#define REG_68                BK_REG(0x68)
#define REG_69                BK_REG(0x69)
#define REG_6A                BK_REG(0x6A)
#define REG_6B                BK_REG(0x6B)
#define REG_6C                BK_REG(0x6C)
#define REG_6D                BK_REG(0x6D)
#define REG_6E                BK_REG(0x6E)
#define REG_6F                BK_REG(0x6F)


#define REG_70                BK_REG(0x70)
#define REG_71                BK_REG(0x71)
#define REG_72                BK_REG(0x72)
#define REG_73                BK_REG(0x73)
#define REG_74                BK_REG(0x74)
#define REG_75                BK_REG(0x75)
#define REG_76                BK_REG(0x76)
#define REG_77                BK_REG(0x77)
#define REG_78                BK_REG(0x78)
#define REG_79                BK_REG(0x79)
#define REG_7A                BK_REG(0x7A)
#define REG_7B                BK_REG(0x7B)
#define REG_7C                BK_REG(0x7C)
#define REG_7D                BK_REG(0x7D)
#define REG_7E                BK_REG(0x7E)
#define REG_7F                BK_REG(0x7F)
//------------------------------------------------------------------------------
//  Data Types
//------------------------------------------------------------------------------
BOOLEAN MDrv_SpiFlash_TriggerMode_ReadID(void);
BOOL BurnRomCode();
SPI_ERROR_CODE_et
SPISlaveDataRead(
    U8 u8CmdLength,
    U32 u32DataLength,
    U32 u32Cmd,
    U8 *pu8ReadValue,
    U32 CE
);

SPI_ERROR_CODE_et
SPISlaveDataWrite(
    U8 u8CmdLength,
    U32 u32DataLength,
    U32 u32Cmd,
    U8 *pu8ReadValue,
    U32 CE
);

SPI_ERROR_CODE_et
FSPSlaveDataRead(
    U8 u8CmdLength,
    U32 u32DataLength,
    U32 u32Cmd,
    U8 *pu8ReadValue,
    U32 CE
);

SPI_ERROR_CODE_et
FSPSlaveDataWrite(
    U8 u8CmdLength,
    U32 u32DataLength,
    U32 u32Cmd,
    U8 *pu8ReadValue,
    U32 CE
);

#if __cplusplus
}
#endif  //__cplusplus


#endif
