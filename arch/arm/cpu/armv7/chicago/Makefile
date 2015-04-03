include $(TOPDIR)/config.mk

LIB	= $(obj)lib$(SOC).o

COBJS	= timer.o serial.o chicago.o
SOBJS	= reset.o

SRCS	:= $(START:.o=.S) $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS) $(SOBJS))
START	:= $(addprefix $(obj),$(START))

all:	$(obj).depend $(LIB)

$(LIB):	$(OBJS)
	$(call cmd_link_o_target, $(OBJS))

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend
