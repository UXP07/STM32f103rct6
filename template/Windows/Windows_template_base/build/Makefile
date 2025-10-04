TARGET := template

# cross compiler platform
# Linux

# debug build?
DEBUG = 0;

# rtos?
RTOS = 0;

# optimization
OPT += -Og
OPT += -fsingle-precision-constant
OPT += -fno-common
OPT += -ffunction-sections
OPT += -fdata-sections
# OPT = -O0
# OPT = -O1/2/3
# OPT = -Os


# Path
#######################################################
INC_LIBRARY += ..\Library\inc\ 
SRC_LIBRARY += ..\Library\src

INC_MODULES += ..\Modules\inc\ 
SRC_MODULES += ..\Modules\src

INC_SYSTEM += ..\System\inc\ 
SRC_SYSTEM += ..\System\src

BUILD_DIR = others
TARGET_DIR = object

#######################################################

# cross compiler
#######################################################
PREFIX = arm-none-eabi-
CC  = $(PREFIX)gcc
CXX = $(PREFIX)g++
CP  = $(PREFIX)objcopy
GDB = $(PREFIX)gdb
SZ  = $(PREFIX)size
AS  = $(PREFIX)gcc -x assembler-with-cpp
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
#######################################################


# C_FLAGS
#######################################################

# cpu
CPU = -mcpu=cortex-m3			# 核心的型号
FLASH_START = 0x08000000		# Flash起始地址，stm32F10x系列的标准地址

# fpu
# cortex-m3没有硬件浮点单元
FPU = 

# float-abi
# cortex-m3没有硬件浮点单元
FLOAT-ABI = -mfloat-abi=soft

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# include
C_INCLUDES = \
			-I..\Core\ \
		   	-I..\User\ \
		   	$(addprefix -I, $(INC_LIBRARY))	\
		   	$(addprefix -I, $(INC_MODULES))	\
		   	$(addprefix -I, $(INC_SYSTEM))	\

# source
C_SOURCES = \
			$(wildcard ../Core/*.c)			\
			$(wildcard ../User/*.c)			\
			$(wildcard $(SRC_LIBRARY)/*.c)	\
			$(wildcard $(SRC_MODULES)/*.c)	\
			$(wildcard $(SRC_SYSTEM)/*.c)	

# asm insludes
AS_INCLUDES = 


# asm source
ASM_SOURCES = \
			$(wildcard ../Core/*.s)			\

# c defines
C_DEFS = \
		 -D STM32F10X_HD \
		 -D USE_STDPERIPH_DRIVER \

# asm defines
AS_DEFS = 

# compile gcc flags
AS_FLAGS += $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -c -Wall
C_FLAGS  += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -c -Wall

ifeq ($(DEBUG), 1)
C_FLAGS += -g -gdwarf-2
endif

# generate dependency information
C_FLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

#######################################################

# LDFLAGS
#######################################################

# link script
LDSCRIPT = .\STM32F103RCTx_FLASH.ld

# libraries
ifeq ($(RTOS), 0)
LIBS = -lc -lm -lnosys
LIBDIR = 
LDFLAGS = $(MCU) -specs=nosys.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)\$(TARGET).map,--cref -Wl,--gc-sections,--print-memory-usage
else
LIBS = -lc -lm
LIBDIR = 
LDFLAGS = $(MCU) -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)\$(TARGET).map,--cref -Wl,--gc-sections,--print-memory-usage
# LDFLAGS += -specs=nano.specs -u _printf_float
endif

all: $(TARGET_DIR) $(BUILD_DIR) $(TARGET_DIR)\$(TARGET).elf $(TARGET_DIR)\$(TARGET).hex $(TARGET_DIR)\$(TARGET).bin

#######################################################

# build application
#######################################################

# make build dir
$(BUILD_DIR):
	mkdir $(BUILD_DIR)
$(TARGET_DIR):
	mkdir $(TARGET_DIR)

# list of c program objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) $(C_FLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)\$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) $(C_FLAGS) $< -o $@

$(TARGET_DIR)\$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(TARGET_DIR)\\%.hex: $(TARGET_DIR)\\%.elf | $(TARGET_DIR)
	$(HEX) $< $@
	
$(TARGET_DIR)\\%.bin: $(TARGET_DIR)\\%.elf | $(TARGET_DIR)
	$(BIN) $< $@	
	
#######################################################

# clean up
#######################################################
clean:
	rmdir /s /q $(BUILD_DIR)
	rmdir /s /q $(TARGET_DIR)
#######################################################

# write
#######################################################

write: $(TARGET_DIR)/$(TARGET).bin
	openocd -f .\stlink-v2.cfg	\
			-f .\stm32f1x.cfg	\
			-c "init; reset halt; wait_halt; flash write_image erase $(TARGET_DIR)/$(TARGET).bin $(FLASH_START); reset; shutdown;"
			@echo "Write Completed."
#######################################################

# erase
#######################################################

erase:
	openocd -f .\stlink-v2.cfg	\
			-f .\stm32f1x.cfg	\
			-c "init; reset halt; flash erase_sector 0 0 1; exit;"
			@echo "Erase Completed."

#######################################################

# reset
#######################################################

reset:
	openocd -f .\stlink-v2.cfg	\
			-f .\stm32f1x.cfg	\
			-c "init; reset; exit;"
			@echo "Reset Completed."

#######################################################

# depandencies
#######################################################

-include $(wildcard $(BUILD_DIR)/*.d)

#######################################################
