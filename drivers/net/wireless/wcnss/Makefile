
# Makefile for WCNSS triple-play driver
ifdef CONFIG_MACH_WARPLTE
EXTRA_CFLAGS = -DWLAN_BIN_COMPATIBLE
endif

wcnsscore-objs += wcnss_wlan.o wcnss_riva.o qcomwlan_secif.o

obj-$(CONFIG_WCNSS_CORE) += wcnsscore.o
obj-$(CONFIG_WCNSS_MEM_PRE_ALLOC) += wcnss_prealloc.o
