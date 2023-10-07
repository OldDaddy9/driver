CUR_DIR := ${dir ${abspath ${lastword ${MAKEFILE_LIST}}}}
_ROOT_KMD := $(CUR_DIR)
-include $(ADLA_SW_ROOT)/build/Makefile.kmd.$(USER)
EXTRA_CFLAGS += $(FLAGS_EXTRA)
#define module name
MOUDEL_NAME ?= adla_core
SDK_INSTALL ?= sdk
HAS_PM_DOMAIN ?= 1
DEBUG ?= 0

ifeq ($(DEBUG),1)
EXTRA_CFLAGS += -DCONFIG_ADLAK_DEBUG=1
endif
ifeq ($(HAS_PM_DOMAIN),0)
EXTRA_CFLAGS += -DCONFIG_HAS_PM_DOMAIN=0
endif

SRCS :=


SRCS += ./drv/port/platform/linux/adlak_debugfs.c
SRCS += ./drv/port/platform/linux/adlak_interrupt.c
SRCS += ./drv/port/platform/adlak_hw.c
SRCS += ./drv/port/platform/linux/adlak_io.c
SRCS += ./drv/port/platform/linux/adlak_platform_config.c
SRCS += ./drv/port/platform/linux/adlak_platform_device.c
SRCS += ./drv/port/os/linux/adlak_os.c
SRCS += ./drv/port/os/linux/mm/adlak_mm_os_common.c
SRCS += ./drv/port/os/linux/mm/adlak_mm_mbp.c
SRCS += ./drv/common/mm/adlak_mm_common.c
SRCS += ./drv/common/mm/adlak_mm_smmu.c
SRCS += ./drv/common/mm/adlak_mm_smmu_tlb.c
SRCS += ./drv/common/adlak_dpm.c
SRCS += ./drv/common/adlak_queue.c
SRCS += ./drv/common/adlak_context.c
SRCS += ./drv/common/adlak_profile.c
SRCS += ./drv/common/adlak_mm.c
SRCS += ./drv/common/adlak_device.c
SRCS += ./drv/common/adlak_submit.c

OBJS := $(patsubst %.c,%.o,$(SRCS))

INC := 
INC += -I$(_ROOT_KMD)drv/port/platform/linux
INC += -I$(_ROOT_KMD)drv/port/platform
INC += -I$(_ROOT_KMD)drv/port/os/linux/mm
INC += -I$(_ROOT_KMD)drv/port/os/linux
INC += -I$(_ROOT_KMD)drv/port/os
INC += -I$(_ROOT_KMD)drv/port
INC += -I$(_ROOT_KMD)drv/uapi/linux
INC += -I$(_ROOT_KMD)drv/uapi
INC += -I$(_ROOT_KMD)drv/common/mm
INC += -I$(_ROOT_KMD)drv/common
INC += -I$(_ROOT_KMD)drv


EXTRA_CFLAGS += $(INC)
CONFIG_MODULE_SIG=n

ifneq ($(DEBUG),1)
EXTRA_LDFLAGS += --strip-debug
EXTRA_CFLAGS += -Wno-error
endif

ifeq ($(KERNELRELEASE),)
# KERNELDIR ?= $(ADLA_SW_KERNEL_DIR) 
	KDIR ?= /lib/modules/`uname -r`/build
	PWD :=$(shell pwd)

file_adlak_version := $(_ROOT_KMD)drv/common/adlak_version.h

all: install clean 
build: generate-files
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

install:build
	@mkdir -p $(SDK_INSTALL)
	@mv -fv $(MOUDEL_NAME).ko $(SDK_INSTALL)
	@cp -av $(PWD)/drv/uapi $(SDK_INSTALL)

clean:del-generate-files
	$(shell find $(PWD) -name "*.o" -o -name "*.mod.c"  | xargs rm -f {})
	$(shell find $(PWD) -name "*.symvers" -o -name "*.order" | xargs rm -f {})
	$(shell find $(PWD) -name "*.cmd" -o -name "*.mod"  | xargs rm -f {})
	$(shell find $(PWD) -name ".tmp_versions" | xargs rm -rf {})
	
.PHONY: all

FORCE:

include $(ADLA_SW_ROOT)/VERSION
ADLAK_V_MAJOR := $(strip $(ADLA_DRIVER_V_MAJOR))
ADLAK_V_MINOR := $(strip $(ADLA_DRIVER_V_MINOR))
ADLAK_V_PATCH := $(strip $(ADLA_DRIVER_V_PATCH))


# Generate a temporary adlak_version.h
generate-files: $(file_adlak_version)
$(file_adlak_version):FORCE
	@echo "Generate $@"
	@echo "" > $@
	@echo /\* This file is auto generated\*/ >> $@
	@echo \#ifndef __ADLAK_VERSION_H__ >> $@
	@echo \#define __ADLAK_VERSION_H__ >> $@
	@echo \#define ADLAK_VERSION \"$(ADLAK_V_MAJOR).$(ADLAK_V_MINOR).$(ADLAK_V_PATCH)\" >> $@
	@echo "" >> $@
	@echo \#define ADLAK_VERSION_MAJOR $(ADLAK_V_MAJOR) >> $@
	@echo \#define ADLAK_VERSION_MINOR $(ADLAK_V_MINOR) >> $@
	@echo \#define ADLAK_VERSION_RSV $(ADLAK_V_PATCH) >> $@
	@echo '#define ADLAK_VERSION_VAL  ((ADLAK_VERSION_MAJOR << 16) + ((ADLAK_VERSION_MINOR) << 8) + (ADLAK_VERSION_RSV))' >> $@
	@echo '#endif /* __ADLAK_VERSION_H__ end define*/' >> $@
	@echo "" >> $@
del-generate-files: $(file_adlak_version)
	@echo  "Remove the temporary files...\n $^"
	@rm -fv $^
else
obj-m	:= $(MOUDEL_NAME).o
$(MOUDEL_NAME)-objs = $(OBJS)
endif
