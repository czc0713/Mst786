include $(TOPDIR)/config.mk

LIB	:= $(obj)libmdrv.o

$(shell mkdir -p $(obj)pnl/hal/cedric)
$(shell mkdir -p $(obj)emmc/src/api $(obj)emmc/src/common $(obj)emmc/src/config)
$(shell mkdir -p $(obj)unfd/src/api $(obj)unfd/src/common $(obj)unfd/src/config)
$(shell mkdir -p $(obj)displaylogo $(obj)gop $(obj)recovery $(obj)sdmmc $(obj)gpio $(obj)isp)

COBJS-y += displaylogo/displaylogo.o
COBJS-y += emmc/src/api/eMMC_prg.o \
	       emmc/src/common/eMMC_hal.o \
	       emmc/src/common/eMMC_utl.o \
	       emmc/src/config/eMMC_platform.o
COBJS-y += gop/mdrv_gop.o
COBJS-y += pnl/apiPNL.o pnl/drvpnl.o pnl/hal/cedric/halpnl.o
COBJS-y += recovery/recovery.o
COBJS-y += gpio/mhal_gpio.o
COBJS-y += isp/isp.o

COBJS-$(CONFIG_MS_SDMMC) += sdmmc/src/hal_card_regs.o sdmmc/src/hal_card_timer.o sdmmc/src/hal_sdmmc.o

ifeq ($(CONFIG_ARCH_CEDRIC),y)
COBJS-$(CONFIG_MS_SDMMC) += sdmmc/src/hal_card_platform_c3.o 
else ifeq ($(CONFIG_ARCH_CHICAGO),y)
COBJS-$(CONFIG_MS_SDMMC) += sdmmc/src/hal_card_platform_c4.o 
endif
COBJS-$(CONFIG_MS_SDMMC) += sdmmc/ms_sdmmc_ub.o 

COBJS-$(CONFIG_MS_NAND) += unfd/drvNAND.o \
	unfd/src/api/drvNAND_prg.o \
	unfd/src/common/drvNAND_hal.o \
    unfd/src/common/drvNAND_utl.o \
	unfd/src/config/drvNAND_platform.o

COBJS	:= $(COBJS-y)
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(LIB)

$(LIB): $(obj).depend $(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
