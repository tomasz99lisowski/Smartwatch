#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/simplelink_cc13x0_sdk_4_20_02_07/source;C:/ti/simplelink_cc13x0_sdk_4_20_02_07/kernel/tirtos/packages
override XDCROOT = C:/ti/xdctools_3_51_03_28_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/simplelink_cc13x0_sdk_4_20_02_07/source;C:/ti/simplelink_cc13x0_sdk_4_20_02_07/kernel/tirtos/packages;C:/ti/xdctools_3_51_03_28_core/packages;..
HOSTOS = Windows
endif
