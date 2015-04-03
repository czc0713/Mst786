/*################################################################################
#
# Copyright (c) 2012 MStar Semiconductor, Inc.
# All rights reserved.
#
# Unless otherwise stipulated in writing, any and all information contained
# herein regardless in any format shall remain the sole proprietary of
# MStar Semiconductor Inc. and be kept in strict confidence
# (A!A¡±MStar Confidential InformationA!A¡L) by the recipient.
# Any unauthorized act including without limitation unauthorized disclosure,
# copying, use, reproduction, sale, distribution, modification, disassembling,
# reverse engineering and compiling of the contents of MStar Confidential
# Information is unlawful and strictly prohibited. MStar hereby reserves the
# rights to any and all damages, losses, costs and expenses resulting therefrom.
#
################################################################################*/
#include <common.h>
#include <command.h>
#include <exports.h>
#include "recovery.h"


extern int snprintf(char *str, size_t size, const char *fmt, ...);

int boot_mode_recovery(void)
{
    char *s = NULL;
    char cmd[128];
    memset(cmd, 0, 128);

    s = getenv("recoverycmd");
    if ((s == NULL) || (strncmp(s, BOOT_COMMAND, 4) != 0))
    {
        snprintf(cmd, sizeof(cmd)-1, BOOT_COMMAND);
    }
    else
    {
        snprintf(cmd, sizeof(cmd)-1, s);
    }
    printf("%s: at %d\n", __func__, __LINE__);
	dcache_enable();
	icache_enable();
    return run_command(cmd, 0);
}

int boot_mode_upgradebootloader(void)
{
    /*TODO: wait spec.*/
    printf("%s: at %d\n", __func__, __LINE__);
    return 0;
}

#define ENABLE_MODULE_ENV_BOOT 1
#if (ENABLE_MODULE_ENV_BOOT == 1)
EN_BOOT_MODE get_boot_mode_from_env(void)
{

    return EN_BOOT_MODE_NORMAL;
}
#endif
    
#if (ENABLE_MODULE_BOOT_IR == 1)
EN_BOOT_MODE get_boot_mode_from_ir(void)
{
     return EN_BOOT_MODE_UNKNOWN;
}
#endif

#if (ENABLE_MODULE_ANDROID_BOOT == 1 )

EN_BOOT_MODE get_boot_mode_from_mtd0(void)
{
    EN_BOOT_MODE mode = EN_BOOT_MODE_UNKNOWN;
    struct bootloader_message *p_msg = NULL;
    p_msg = (struct bootloader_message*)BOOT_MODE_TMP_BUF_ADDR;
    char cmd[128];
    memset(cmd, 0, sizeof(cmd));
#if (ENABLE_MODULE_EMMC == 1)
    snprintf(cmd, sizeof(cmd)-1, "emmc read.p 0x%08lX misc 20", (unsigned long)BOOT_MODE_TMP_BUF_ADDR);
#else
    snprintf(cmd, sizeof(cmd)-1, "nand read.e 0x%08lX misc 20", (unsigned long)BOOT_MODE_TMP_BUF_ADDR);
#endif
    //printf("\n %s: '%s'  at %d\n", __func__, cmd, __LINE__);

    if(-1 != run_command(cmd, 0))
    {
        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd)-1, "md.b 0x%08lX 20", (unsigned long)BOOT_MODE_TMP_BUF_ADDR);
        run_command(cmd, 0);
        //printf("\n  %s: '%s'  at %d \n", __func__, cmd, __LINE__);
		if(0 == strncmp(p_msg->command, BOOT_MODE_RECOVERY_STR, strlen(BOOT_MODE_RECOVERY_STR)))
        {
            mode = EN_BOOT_MODE_RECOVERY;
        }
        else if(0 == strncmp(p_msg->command, BOOT_MODE_UPGRADEBOOTLOADER_STR, strlen(BOOT_MODE_UPGRADEBOOTLOADER_STR)))
        {
            mode = EN_BOOT_MODE_UPGRADEBOOTLOADER;
        }
        /*else {do nothing;}*/
    }
    else
    {
        printf("%s: '%s' fails, at %d\n", __func__, cmd, __LINE__);
    }
    return mode;
}

#endif


#if (ENABLE_MODULE_BOOT_KEYPAD == 1)
EN_BOOT_MODE get_boot_mode_from_keypad(void)
{
     return EN_BOOT_MODE_UNKNOWN;
}
#endif

int do_bootcheck (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
     int ret=0;
     EN_BOOT_MODE BootMode;
     int BootFlag;
     BootMode = EN_BOOT_MODE_UNKNOWN;
     BootFlag=0; // decide the boot mode


#if (ENABLE_MODULE_ANDROID_BOOT == 1)
    // NOTE: read BCB in mtd0 (misc) to decide what boot mode to go
    if(BootFlag == 0)
    {
        BootMode=get_boot_mode_from_mtd0();
        if(BootMode!=EN_BOOT_MODE_UNKNOWN)
        {
            //BootMode via MTD
            BootFlag=1;
        }
    }
#endif

    switch(BootMode)
    {
        case EN_BOOT_MODE_RECOVERY:
            ret = boot_mode_recovery();
            break;  		
        case EN_BOOT_MODE_UNKNOWN:
        default:
            //normal booting according bootcmd in main.c
            break;
    }
    return ret;
}

U_BOOT_CMD(
    bootcheck, CONFIG_SYS_MAXARGS, 1,    do_bootcheck,
    "bootcheck   - Do boot check",
    NULL
);

