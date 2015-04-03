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


#ifndef _Mstar_Boot_Header_
#define _Mstar_Boot_Header_


#if (ENABLE_MODULE_EMMC == 1)
#define BOOT_COMMAND "emmc read.p 0x45000000 recovery 0x00800000; bootm 0x45000000"
#else
#define BOOT_COMMAND "nand read.e 0x45000000 recovery 0x00800000; bootm 0x45000000"
#endif

//#define BOOT_MODE_TMP_BUF_ADDR 0x44000000
#define BOOT_MODE_TMP_BUF_ADDR 0x40040000

#define CMD_BUF 128

#define BOOT_MODE_RECOVERY_STR "boot-recovery\0"
#define BOOT_MODE_UPGRADEBOOTLOADER_STR "update-radio/hboot\0"

struct bootloader_message {
    char command[32];
    char status[32];
    char recovery[1024];
};

typedef enum
{
    EN_BOOT_MODE_UNKNOWN,
    EN_BOOT_MODE_NORMAL,
    EN_BOOT_MODE_RECOVERY,
    EN_BOOT_MODE_UPGRADEBOOTLOADER,
    EN_BOOT_MODE_FORCEUGRADE,
    EN_BOOT_MODE_USB_UPGRADE,
    EN_BOOT_MODE_NET_UPGRADE,
    EN_BOOT_MODE_OAD_UPGRADE, 
}EN_BOOT_MODE;

int do_bootcheck (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

#endif
