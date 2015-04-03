#include <common.h>
#include <nand.h>
#include "drvNAND.h"

#ifndef CONFIG_NANDDRV_DEBUG
#define CONFIG_NANDDRV_DEBUG 0
#endif

#if 0	//if (CONFIG_NANDDRV_DEBUG)
#define NAND_DEBUG(fmt, args...) printf(fmt, ##args)
#else
#define NAND_DEBUG(fmt, args...)
#endif

#define CRIT_SECT_BEGIN(x)	
#define CRIT_SECT_END(x)	

//-----------------------------------------------------------------------
#define SELF_WRITE_CIS_TEST 0
#if SELF_WRITE_CIS_TEST
static U8 au8_CIS[1024] = 
{
	0x4D, 0x53, 0x54, 0x41, 0x52, 0x53, 0x45, 0x4D, 0x49, 0x55, 0x4E, 0x46, 0x44, 0x43, 0x49, 0x53, 
	0x05, 0xAD, 0xDA, 0x10, 0x95, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x91, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x08, 0x40, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 
	0x01, 0x00, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x48, 0x59, 0x4E, 0x49, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x48, 0x59, 0x32, 0x37, 0x55, 0x46, 0x30, 0x38, 0x32, 0x47, 0x32, 0x42, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  
	/**************************************************************/
	0x27, 0x03, 0x00, 0x00, 0x40, 0x00, 0x00, 0x08, 0x40, 0x00, 0x00, 0x08, 0x07, 0x00, 0x08, 0x00, 
	0x0A, 0x00, 0x04, 0x00, 0x0C, 0x00, 0x02, 0x00, 0x10, 0x00, 0x01, 0x00, 0x0D, 0x00, 0x02, 0x00, 
	0x13, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0x00, 0x16, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0x00, 
	0x19, 0x00, 0x04, 0x00, 0x06, 0x00, 0x02, 0x00, 0x1F, 0x00, 0x04, 0x00, 0x06, 0x00, 0x02, 0x00, 
	0x25, 0x00, 0xDB, 0x07, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};
#endif
//-----------------------------------------------------------------------

static uint8_t scan_ff_pattern[] = { 0xff };

/* struct nand_bbt_descr - bad block table descriptor */
static struct nand_bbt_descr _unfd_nand_bbt_descr = {
	.options		= NAND_BBT_2BIT | NAND_BBT_LASTBLOCK | NAND_BBT_VERSION | NAND_BBT_CREATE | NAND_BBT_WRITE,
	.offs			= 5,
	.len			= 1,
	.pattern		= scan_ff_pattern
};

/*
===========================================
MTD ECC Structure
===========================================
*/
#if 0
static struct nand_ecclayout unfd_nand_oob_16 = {
	.eccbytes		= 10,
	.eccpos			= {6,7,8,9,10,11,12,13,14,15},
	.oobfree		=
	{
		{
			.offset = 0,
			.length = 5
		}
	}
};

static struct nand_ecclayout unfd_nand_oob_64 = {
    .eccbytes		= 40,
    .eccpos			=
    {
    	0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
    	0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
    	0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
    	0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F
    },
    .oobfree		=
    {
    	{
    		.offset = 1,
    		.length = 5
    	},
        {
        	.offset = 0x10,
        	.length = 6
        },
        {
        	.offset = 0x20,
        	.length = 6
        },
        {
        	.offset = 0x30,
        	.length = 6
        }
    }
};

static struct nand_ecclayout unfd_nand_oob_128 = {
    .eccbytes		= 80,
    .eccpos			=
    {
    	0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
    	0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
    	0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
    	0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
    	0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
    	0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
    	0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    	0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F

    },
    .oobfree		=
    {
    	{
    		.offset = 1,
    		.length = 5
    	},
        {
        	.offset = 0x10,
        	.length = 6
        },
        {
        	.offset = 0x20,
        	.length = 6
        },
        {
        	.offset = 0x30,
        	.length = 6
        },
		{
    		.offset = 0x40,
    		.length = 6
    	},
        {
        	.offset = 0x50,
        	.length = 6
        },
        {
        	.offset = 0x60,
        	.length = 6
        },
        {
        	.offset = 0x70,
        	.length = 6
        }
    }
};
#else
static struct nand_ecclayout unfd_nand_oob_custom;
#endif

static uint8_t bbt_pattern[] = {'B', 'b', 't', '0' };
static uint8_t mirror_pattern[] = {'1', 't', 'b', 'B' };

static struct nand_bbt_descr _unfd_bbt_main_descr = {
	.options		= NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE |
					  NAND_BBT_2BIT | NAND_BBT_VERSION | NAND_BBT_PERCHIP,
	.offs			= 1,
	.len			= 3,
	.veroffs		= 4,
	.maxblocks		= 4,
	.pattern		= bbt_pattern
};

static struct nand_bbt_descr _unfd_bbt_mirror_descr = {
	.options		= NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE |
					  NAND_BBT_2BIT | NAND_BBT_VERSION | NAND_BBT_PERCHIP,
	.offs			= 1,
	.len			= 3,
	.veroffs		= 4,
	.maxblocks		= 4,
	.pattern		= mirror_pattern
};

static U32 u32_WriteLen = 0;
static U32 u32_CurRow = 0;
static U32 u32_CIFDIdx = 0;
static U32 u32_CurCol = 0;
static U16 u16_ByteIdxofPage = 0;
static U32 u32_UnfdRet = 0; 
static int s32_ECCStatus = 0;
/*
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
======================================UNFD FUNCTION ==============================================
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*/



/*
vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
======================================MTD MAP Function=============================================
vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
*/


static void _unfd_nand_hwcontrol(struct mtd_info *mtdinfo, int cmd, unsigned int ctrl)
{
    NAND_DEBUG("%s()\n", __func__);

	return;
}

static int _unfd_nand_device_ready(struct mtd_info *mtdinfo)
{
    NAND_DEBUG("%s()\n", __func__);
	
	return 1;
}

static void _unfd_nand_write_buf(struct mtd_info *mtd, const u_char *buf, int len)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U8 *pu8_MainBuf = pNandDrv->PlatCtx_t.pu8_PageDataBuf;
	U8 *pu8_SpareBuf = pNandDrv->PlatCtx_t.pu8_PageSpareBuf;
	
	CRIT_SECT_BEGIN(&FCIE3_mutex);
    NAND_DEBUG("%s():%X, %X\n", __func__, buf, len);

	u16_ByteIdxofPage += len;
	/*
	for(u16_i = 0; u16_i < u16_len; u16_i ++)
	{
		printf("%2X ",pu8_buf[u16_i]);
		if((u16_i & 0xf) == 0xf)
			printf("\r\n");
	}
	*/

	if( len >= pNandDrv->u16_PageByteCnt )	// whole page or Main area only
	{
		u32_WriteLen += len;
		if( len > pNandDrv->u16_PageByteCnt )	// whole page
		{
			if(0 /*(U32)buf >= VMALLOC_START && (U32)buf <= VMALLOC_END*/)		//whether buf is from vmalloc ?
			{
				memcpy(pu8_MainBuf, buf, pNandDrv->u16_PageByteCnt);
				memcpy(pu8_SpareBuf, buf+pNandDrv->u16_PageByteCnt, pNandDrv->u16_SpareByteCnt);
			}
			else
			{
				pu8_MainBuf = (U8*)buf;
				pu8_SpareBuf = (U8*)buf + (pNandDrv->u16_PageByteCnt);
			}

			u32_UnfdRet = NC_WritePages(u32_CurRow, pu8_MainBuf, pu8_SpareBuf, 1);
			if(u32_UnfdRet != UNFD_ST_SUCCESS)
				goto END_WRITE_BUF;
		}
		else	// main area only
		{
			memcpy(pu8_MainBuf, buf, len);
		}
	}
	else
	{
		if((u32_WriteLen == 0) && (u16_ByteIdxofPage>=pNandDrv->u16_PageByteCnt)) // mtd skip prepare main area, default all oxff
		{
			u32_WriteLen += pNandDrv->u16_PageByteCnt;
			memset(pu8_MainBuf, 0xFF, pNandDrv->u16_PageByteCnt);
			memset(pu8_SpareBuf, 0xFF, pNandDrv->u16_SpareByteCnt);
		}
		u32_WriteLen += len;
		memcpy(pu8_SpareBuf, buf, len);

		if( u32_WriteLen == (pNandDrv->u16_PageByteCnt + pNandDrv->u16_SpareByteCnt) )
		{
			u32_UnfdRet = NC_WritePages(u32_CurRow, pu8_MainBuf, pu8_SpareBuf, 1);
			if(u32_UnfdRet != UNFD_ST_SUCCESS)
				goto END_WRITE_BUF;			
		}
	}

END_WRITE_BUF:
	CRIT_SECT_END(&FCIE3_mutex);
    return;
}

static void _unfd_nand_read_buf(struct mtd_info *mtd, u_char* const buf, int len)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U8 *pu8_MainBuf = pNandDrv->PlatCtx_t.pu8_PageDataBuf;
	U8 *pu8_SpareBuf = pNandDrv->PlatCtx_t.pu8_PageSpareBuf;
	
	CRIT_SECT_BEGIN(&FCIE3_mutex);

//    NAND_DEBUG("%s():%X, %X\n", __func__, buf, len);
	
	u16_ByteIdxofPage = len;

	if( len >= pNandDrv->u16_PageByteCnt )
	{
		if( len > pNandDrv->u16_PageByteCnt )
		{
			if(0 /*(U32)buf >=VMALLOC_START && (U32)buf <= VMALLOC_END*/)
			{
				u32_UnfdRet = NC_ReadPages(u32_CurRow, pu8_MainBuf, pu8_SpareBuf, 1);
				if(u32_UnfdRet != UNFD_ST_SUCCESS)
				{
					s32_ECCStatus = -1;
					goto END_READ_BUF;		
				}
				NC_CheckECC(&s32_ECCStatus);
				memcpy(buf, pu8_MainBuf, pNandDrv->u16_PageByteCnt);
				memcpy(buf+pNandDrv->u16_PageByteCnt, pu8_SpareBuf, pNandDrv->u16_SpareByteCnt);
			}
			else
			{
				pu8_MainBuf = buf;
				pu8_SpareBuf = (U8*)buf + (pNandDrv->u16_PageByteCnt);
				u32_UnfdRet = NC_ReadPages(u32_CurRow, pu8_MainBuf, pu8_SpareBuf, 1);
				if(u32_UnfdRet != UNFD_ST_SUCCESS)
				{
					s32_ECCStatus = -1;
					goto END_READ_BUF;		
				}
				NC_CheckECC(&s32_ECCStatus);
			}
		}
		else
		{	
			if(0 /*(U32)buf >=VMALLOC_START && (U32)buf <= VMALLOC_END*/)
			{
				u32_UnfdRet = NC_ReadPages(u32_CurRow, pu8_MainBuf, pu8_SpareBuf, 1);
				if(u32_UnfdRet != UNFD_ST_SUCCESS)
				{
					s32_ECCStatus = -1;
					goto END_READ_BUF;		
				}
				NC_CheckECC(&s32_ECCStatus);
				memcpy(buf, pu8_MainBuf, pNandDrv->u16_PageByteCnt);
			}
			else
			{
				pu8_MainBuf = buf;
				u32_UnfdRet = NC_ReadPages(u32_CurRow, pu8_MainBuf, pu8_SpareBuf, 1);
				if(u32_UnfdRet != UNFD_ST_SUCCESS)
				{
					s32_ECCStatus = -1;
					goto END_READ_BUF;		
				}	
				NC_CheckECC(&s32_ECCStatus);
			}
		}			
	}
	else
	{
		memcpy(buf, pu8_SpareBuf, len);
	}	
END_READ_BUF:	
	CRIT_SECT_END(&FCIE3_mutex);

    return;
}

static u16 _unfd_nand_read_word(struct mtd_info *mtd)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U8 *pu8_SpareBuf = pNandDrv->PlatCtx_t.pu8_PageSpareBuf;
	U16 u16_tmp= 0;
	
	CRIT_SECT_BEGIN(&FCIE3_mutex);
    NAND_DEBUG("%s()\n", __func__);
	u16_tmp = (((U16)pu8_SpareBuf[u32_CIFDIdx+1])<<8) + ((U16)pu8_SpareBuf[u32_CIFDIdx]);
	u32_CIFDIdx+=2;
	CRIT_SECT_END(&FCIE3_mutex);
	
    return u16_tmp;
}

static u_char _unfd_nand_read_byte(struct mtd_info *mtd)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U8 *pu8_SpareBuf = pNandDrv->PlatCtx_t.pu8_PageSpareBuf;
	U8 u8Ret = 0;
	CRIT_SECT_BEGIN(&FCIE3_mutex);
    NAND_DEBUG("%s()\n", __func__);
	u8Ret = pu8_SpareBuf[u32_CIFDIdx];
	u32_CIFDIdx++;
	CRIT_SECT_END(&FCIE3_mutex);
    return (u8Ret);
}

static int _unfd_nand_wait(struct mtd_info *mtd, struct nand_chip *this)
{
	int status = 0;

	CRIT_SECT_BEGIN(&FCIE3_mutex);
    NAND_DEBUG("%s()\n", __func__);
	if(u32_UnfdRet != UNFD_ST_SUCCESS)
		status |= NAND_STATUS_FAIL;
	CRIT_SECT_END(&FCIE3_mutex);
	
	return (status);
}

static void _unfd_nand_cmdfunc(struct mtd_info *mtd, unsigned command, int column, int page_addr)
{
	NAND_DRIVER * pNandDrv =  (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	U8 *pu8_SpareBuf = pNandDrv->PlatCtx_t.pu8_PageSpareBuf;
	
	CRIT_SECT_BEGIN(&FCIE3_mutex);	

	switch (command) {
		case NAND_CMD_READ0:
    		//NAND_DEBUG("%s(READ0):%X, %X\n", __func__, column, page_addr);
			u32_CurRow = page_addr;
			u32_CurCol = column;
			break;

		case NAND_CMD_READ1:
    		NAND_DEBUG("%s(READ1):%X, %X\n", __func__, column, page_addr);
			printf("\033[31mUNFD not support READ1(CMD 0x01) now!!!\033[m\n");
			nand_die();
			break;

		case NAND_CMD_READOOB:
    		NAND_DEBUG("%s(READOOB):%X, %X\n", __func__, column, page_addr);
			u32_CIFDIdx = 0;
			u16_ByteIdxofPage = 0;
			u32_UnfdRet = NC_Read_RandomIn(page_addr, column + pNandDrv->u16_PageByteCnt, pu8_SpareBuf, pNandDrv->u16_SpareByteCnt-column);
			break;

		case NAND_CMD_READID:
    		NAND_DEBUG("%s(READID)\n", __func__);
			u32_CIFDIdx = 0;
			u32_UnfdRet = NC_ReadID();
			memcpy((void *) pu8_SpareBuf, (const void *) pNandDrv->au8_ID, NAND_ID_BYTE_CNT);
			break;

		case NAND_CMD_PAGEPROG:
			/* sent as a multicommand in NAND_CMD_SEQIN */
    		NAND_DEBUG("%s(PAGEPROG):%X, %X\n", __func__, column, page_addr);
			break;

		case NAND_CMD_ERASE1:
    		NAND_DEBUG("%s(ERASE1):%X, %X\n", __func__, column, page_addr);
			u32_UnfdRet = NC_EraseBlk(page_addr);
			break;

		case NAND_CMD_ERASE2:
    		NAND_DEBUG("%s(ERASE2):%X, %X\n", __func__, column, page_addr);
			break;

		case NAND_CMD_SEQIN:
			/* send PAGE_PROG command(0x1080) */
    		NAND_DEBUG("%s(SEQIN):%X, %X\n", __func__, column, page_addr);
			u32_CurRow = page_addr;
            u32_CurCol = column;
			u16_ByteIdxofPage = column;
			u32_WriteLen = 0;
			break;

		case NAND_CMD_STATUS:
    		NAND_DEBUG("%s(STATUS)\n", __func__);
			u32_CIFDIdx = 0;
			u32_UnfdRet = NC_ReadStatus();
			break;

		case NAND_CMD_RESET:	
    		NAND_DEBUG("%s(RESET)\n", __func__);
			u32_UnfdRet = NC_ResetNandFlash();
			break;

		case NAND_CMD_STATUS_MULTI:
    		NAND_DEBUG("%s(STATUS_MULTI)\n", __func__);
			u32_CIFDIdx = 0;
			u32_UnfdRet = NC_ReadStatus();
			break;

		case NAND_CMD_READSTART:
    		NAND_DEBUG("%s(READSTART):%X, %X\n", __func__, column, page_addr);
			break;

		case NAND_CMD_CACHEDPROG:
    		NAND_DEBUG("%s(CACHEDPROG):%X, %X\n", __func__, column, page_addr);
			printf("\033[31mUNFD not support CACHEPROG (CMD 0x15) now!!!\033[m\n");
			while(1);
			break;

		default:
			printf("_unfd_nand_cmdfunc: error, unsupported command.\n");
			break;
	}

	CRIT_SECT_END(&FCIE3_mutex);
	return;
}

static void _unfd_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{
	//NAND_DEBUG("%s()\n", __func__);

    // default enable
}

static int _unfd_nand_calculate_ecc(struct mtd_info *mtd, const u_char *dat, u_char *ecc_code)
{
	//NAND_DEBUG("%s()\n", __func__);

	return 0;
}

static int _unfd_nand_correct_data(struct mtd_info *mtd, u_char *dat, u_char *read_ecc, u_char *calc_ecc)
{
	int s32ECCStatus = 0;
	
	CRIT_SECT_BEGIN(&FCIE3_mutex);
	NAND_DEBUG("%s()\n", __func__);
	s32ECCStatus = s32_ECCStatus;
	CRIT_SECT_END(&FCIE3_mutex);

	return s32ECCStatus;
}

U32 drvNAND_Init(void)
{
	U32 u32_Err = 0;
	NC_PlatformInit();

	u32_Err = drvNAND_ProbeReadSeq();
	if(u32_Err != UNFD_ST_SUCCESS)
	{
		return u32_Err;
	}
	//return 1;
	u32_Err = drvNAND_SearchCIS();

	return u32_Err; 
		
}


/*
 * Board-specific NAND initialization.
 * - hwcontrol: hardwarespecific function for accesing control-lines
 * - dev_ready: hardwarespecific function for  accesing device ready/busy line
 * - eccmode: mode of ecc, see defines
 */
 
int nand_unfd_init(void)
{
	NAND_DRIVER *pNandDrv = (NAND_DRIVER*)drvNAND_get_DrvContext_address();
	pNandDrv->pPartInfo = drvNAND_get_DrvContext_PartInfo();
	memset(pNandDrv->pPartInfo, '\0', NAND_PARTITAION_BYTE_CNT);
	U16 u16_i;
	U32 u32_Err;
	u32_Err = drvNAND_Init();
	
	if(UNFD_ST_ERR_NO_CIS == u32_Err)
	{	
		printf("NAND No CIS found\n");

		printf("NAND ID:");
		for(u16_i = 0; u16_i < pNandDrv->u8_IDByteCnt; u16_i++)
		printf("0x%X ", pNandDrv->au8_ID[u16_i]);
		printf("\n");
		
		#if defined(FCIE4_DDR) && FCIE4_DDR
		if(pNandDrv->u16_Reg58_DDRCtrl & BIT_DDR_MASM)
		{
			printf("NAND Error: Detect DDR NAND but have no CIS in NAND\n");
			printf("Please use command, \"ncisbl\", to write cis\n");
			return -1;
		}
		#if 0
		if(pNandDrv->u16_Reg58_DDRCtrl & BIT_DDR_TOGGLE)
		{
			NC_ReadToggleParamPage();
			NC_ConfigContext();
			NC_ReInit();
			NC_Config();
		}
		else if(pNandDrv->u16_Reg58_DDRCtrl & BIT_DDR_ONFI)
		{
			NC_ReadONFIParamPage();
			NC_ConfigContext();
			NC_ReInit();
			NC_Config();
		}
		else
		#endif
		#endif
		{
			REG_WRITE_UINT16(NC_PATH_CTL, 0x20);

			NC_PlatformInit();

			NC_ResetFCIE(); // Reset FCIE3

			nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "NC_ResetFCIE done\n");


			// We need to config Reg 0x40 first because we we need to get nand ID first
		//	pNandDrv->u16_Reg40_Signal =
		//	(BIT_NC_WP_AUTO | BIT_NC_WP_H | BIT_NC_CE_AUTO | BIT_NC_CE_H) &
		//	~(BIT_NC_CHK_RB_EDGEn | BIT_NC_CE_SEL_MASK);
			REG_WRITE_UINT16(NC_SIGNAL, pNandDrv->u16_Reg40_Signal);


#if defined(ENABLE_NAND_INTERRUPT_MODE) && ENABLE_NAND_INTERRUPT_MODE
			nand_enable_intr_mode();
#endif
		
			NC_ResetNandFlash(); // Reset NAND flash

			nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "NC_ResetNandFlash done\n");

			NC_ReadID();

			nand_debug(UNFD_DEBUG_LEVEL_HIGH,1, "NC_ReadID done\n");

			if( drvNAND_CHECK_FLASH_TYPE() != 0)
			{
				printf("[%s]\tUnsupported NAND Type, Need to update CIS or modify code\n",__func__);
				return -1;
			}

			nand_debug(UNFD_DEBUG_LEVEL_HIGH,1,"drvNAND_CHECK_FLASH_TYPE done\n");

			NC_ConfigContext();
			NC_Init();
			
		}
	}
	else if(UNFD_ST_SUCCESS != u32_Err)
	{
		printf("[%s]\tdrvNAND_Init Error : %lX", __func__, u32_Err);
		return -1;
	}
	/*Config clock timing either for DDR NAND or SDR NAND */
	nand_config_clock(pNandDrv->u16_tRC);

	return 0;


}
 
 
int board_nand_init(struct nand_chip *nand)
{
	NAND_INFO_t *pNandInfo_t;
	U16 u16_tmp;
	U32 u32_Err;
	U16 u16_oob_poi;
	U16 u16_i;
	U16 u16_SectorSpareByteCnt;
	
	NAND_DEBUG("NANDDrv_BOARD_NAND_INIT().\n");

	printf("board_nand_init here\r\n");
/*
	u32_Err = drvNAND_Init();  
	if (u32_Err != UNFD_ST_SUCCESS)
	{
		printf("NAND Init fail, ErrCode: %x\n",u32_Err);
	#if SELF_WRITE_CIS_TEST
		u32_Err = drvNAND_WriteCIS(au8_CIS);
		printf("drvNAND_WriteCIS : %x\n",u32_Err);	
	#else
		return -1;	
	#endif
	}
*/
	if( nand_unfd_init() != 0 ) return -1;

	/* please refer to include/linux/nand.h for more info. */
	pNandInfo_t = (NAND_INFO_t*)malloc(sizeof(NAND_INFO_t));
	if(!pNandInfo_t)
	{
		printf("no memory for NAND INFO buffer\n");
		return -1;
	}
	drvNAND_GetNandInfo(pNandInfo_t);

	nand->dev_ready						= _unfd_nand_device_ready;
    nand->cmd_ctrl						= _unfd_nand_hwcontrol;
    nand->ecc.mode						= NAND_ECC_HW;

	nand->ecc.size						= pNandInfo_t->u16_PageByteCnt;
	nand->ecc.bytes						= pNandInfo_t->u16_ECCCodeByteCnt * pNandInfo_t->u16_PageSectorCnt;

#if 0	
	if( pNandInfo_t->u16_PageByteCnt == 4096 )
	{	
        nand->ecc.layout				= &unfd_nand_oob_128;
    }
	else if( pNandInfo_t->u16_PageByteCnt == 2048 )
	{	
        nand->ecc.layout				= &unfd_nand_oob_64;
    }
    else if( pNandInfo_t->u16_PageByteCnt == 512 )
    {
		nand->ecc.layout				= &unfd_nand_oob_16;
    }
#else
		u16_SectorSpareByteCnt = (pNandInfo_t->u16_SpareByteCnt/pNandInfo_t->u16_PageSectorCnt);
		unfd_nand_oob_custom.eccbytes = pNandInfo_t->u16_ECCCodeByteCnt * pNandInfo_t->u16_PageSectorCnt;
		for(u16_tmp=0 ; u16_tmp<pNandInfo_t->u16_PageSectorCnt ; u16_tmp++)
		{
			u16_oob_poi = ((u16_tmp+1)*u16_SectorSpareByteCnt)-pNandInfo_t->u16_ECCCodeByteCnt;
			for(u16_i=0 ; u16_i<pNandInfo_t->u16_ECCCodeByteCnt ; u16_i++)
			{
				unfd_nand_oob_custom.eccpos[(u16_tmp*pNandInfo_t->u16_ECCCodeByteCnt)+u16_i] = u16_oob_poi++;
			}

			if( u16_tmp == 0)
			{
				unfd_nand_oob_custom.oobfree[u16_tmp].offset = 2;
				unfd_nand_oob_custom.oobfree[u16_tmp].length= u16_SectorSpareByteCnt - pNandInfo_t->u16_ECCCodeByteCnt - 2;
			}
			else
			{
				unfd_nand_oob_custom.oobfree[u16_tmp].offset = u16_tmp*u16_SectorSpareByteCnt;
				unfd_nand_oob_custom.oobfree[u16_tmp].length= u16_SectorSpareByteCnt - pNandInfo_t->u16_ECCCodeByteCnt;
			}
		}
		nand->ecc.layout = &unfd_nand_oob_custom;
		//dump_mem((unsigned char *) &unfd_nand_oob_custom, sizeof(unfd_nand_oob_custom));		
#endif
	//bad block marker in cis
	_unfd_nand_bbt_descr.offs			= (pNandInfo_t->u32_Config>>1) & 0x07;

	if( pNandInfo_t->u16_PageByteCnt > 512 )
		nand->options					= NAND_NO_AUTOINCR;

	if(pNandInfo_t->u8_WordMode == 1)
		nand->options |= NAND_BUSWIDTH_16;
	else
		nand->options &= (~NAND_BUSWIDTH_16);
	
	nand->waitfunc		= _unfd_nand_wait;
	nand->read_byte		= _unfd_nand_read_byte;
	nand->read_word		= _unfd_nand_read_word;
	nand->read_buf		= _unfd_nand_read_buf;
	nand->write_buf		= _unfd_nand_write_buf;
	nand->chip_delay	= 20;	//@FIXME: unite: us, please refer to nand_base.c 20us is default.

    nand->ecc.hwctl						= _unfd_nand_enable_hwecc;
    nand->ecc.correct					= _unfd_nand_correct_data;
    nand->ecc.calculate					= _unfd_nand_calculate_ecc;
    nand->cmdfunc						= _unfd_nand_cmdfunc;
    nand->badblock_pattern				= &_unfd_nand_bbt_descr; //using default badblock pattern.
    nand->bbt_td						= &_unfd_bbt_main_descr;
    nand->bbt_md						= &_unfd_bbt_mirror_descr;

    free(pNandInfo_t);
	
	printf("board_nand_init end\r\n");
	
	return 0;
}

