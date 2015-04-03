#include <common.h>
#include "environment.h"
#include "asm/arch/mach/ms_types.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"
#include "ms_version.h"

#ifndef MS_CHANGELIST
#define MS_CHANGELIST {'0', '0', '0', '0', '0', '0', '0', '0'}
#endif

typedef struct
{
    U8                       tag[4];
    U8                       flag[2];
    U8                       lib_type[2];
    U8                       bn[6];
    U8                       changelist[8];
    U8                       product[2];
    U8                       customer[1];
    U8                       reserved[6];
    U8                       checksum;

} MS_Version;

MS_Version version =
{
	{'M','S','V','C'},
	{'0','0'},
	{'L','X'},
	{'_','U','B','O','O','T'},
	MS_CHANGELIST,
	{'C','4'},
	{'0'},
	{'0','0','0','0','0','0'},
    'T'
};


DECLARE_GLOBAL_DATA_PTR;

#define DEV_BUF_ADDR (PHYS_SDRAM_1+0x3F800)


int arch_cpu_init(void)
{



	gd->xtal_clk=XTAL_26000K;



	/* following code is not worked since bd has not allocated yet!!*/
//
//	gd->bd->bi_arch_number = MACH_TYPE_COLUMBUS2;
//
//	/* adress of boot parameters */
//	gd->bd->bi_boot_params = BOOT_PARAMS;


    return 0;
}

// we borrow the DRAN init to do the devinfo setting...
int dram_init(void)
{
#if CONFIG_ARCH_CEDRIC
    DEVINFO_st* devinfo;

	devinfo = (DEVINFO_st*)(DEV_BUF_ADDR);    // start address: 30K, size:  2K
	// check header
	//printf("[CHECK] DEVINFO. header\r\n");

	if (devinfo->header[0]!='D' || devinfo->header[1]!='E' || devinfo->header[2]!='V' || devinfo->header[3]!='I' ||
		devinfo->header[4]!='N' || devinfo->header[5]!='F' || devinfo->header[6]!='O' || devinfo->header[7]!='.')
	{
		printf("***********************************************************\r\n");
		printf("* DEVINFO. header check failed !!!                        *\r\n");
		printf("* Please program DEVINFO section.                         *\r\n");
		printf("***********************************************************\r\n");
		while(1);
	}


    gd->boot_type=(int)devinfo->boot_device;
    gd->board_type=(int)devinfo->board_name;
    gd->ram_size=CONFIG_UBOOT_RAM_SIZE;
    gd->panel_type=(U8)devinfo->panel_type;
    gd->rtk_flag = (int)devinfo->rtk_flag;

    /* following code is not worked since bd has not allocated yet!!*/
    //    gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
    //    gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;
#endif

#if CONFIG_ARCH_CHICAGO
    gd->boot_type=(int)DEVINFO_BOOT_TYPE_EMMC;
    gd->board_type=(int)E_BD_UNKNOWN;
    gd->ram_size=CONFIG_UBOOT_RAM_SIZE;
    gd->panel_type=(U8)E_PANEL_UNKNOWN;
    gd->rtk_flag = (int)0;
#endif
    return 0;
}

DEVINFO_BOOT_TYPE ms_devinfo_boot_type(void)
{
	//return (DEVINFO_BOOT_TYPE)gd->boot_type;
	return (DEVINFO_BOOT_TYPE)DEVINFO_BOOT_TYPE_EMMC;
}

DEVINFO_BOARD_TYPE ms_devinfo_board_type(void)
{
	return (DEVINFO_BOARD_TYPE)gd->board_type;
}

DEVINFO_BOARD_TYPE ms_devinfo_panel_type(void)
{
	return (DEVINFO_PANEL_TYPE)gd->panel_type;
}

DEVINFO_RTK_FLAG ms_devinfo_rtk_flag(void)
{
    return (DEVINFO_RTK_FLAG)gd->rtk_flag;
}


#if (defined(CONFIG_MS_NAND) && defined(CONFIG_MS_EMMC))
extern void mmc_env_relocate_spec(void);
extern void nand_env_relocate_spec(void);
extern int mmc_env_init(void);
extern int nand_env_init(void);
extern int mmc_saveenv(void);
extern int nand_saveenv(void);

env_t *env_ptr;

void env_relocate_spec(void)
{
	if(DEVINFO_BOOT_TYPE_EMMC==ms_devinfo_boot_type())
	{
		return mmc_env_relocate_spec();
	}
	else
	{
		return nand_env_relocate_spec();
	}
}


int env_init(void)
{
	if(DEVINFO_BOOT_TYPE_EMMC==ms_devinfo_boot_type())
	{
		return mmc_env_init();
	}
	else
	{
		return nand_env_init();
	}

}

int saveenv(void)
{
	if(DEVINFO_BOOT_TYPE_EMMC==ms_devinfo_boot_type())
	{
		return mmc_saveenv();
	}
	else
	{
		return nand_saveenv();
	}
}
#endif
