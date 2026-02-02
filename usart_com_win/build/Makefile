# ==========================================
# 1. 变量定义 (根据你的项目修改这里)
# ==========================================

# 目标可执行文件名
TARGET = my_serial_tool.exe

# 编译器 (Windows 下通常是 gcc，如果你用了 MinGW)
CC = gcc

# 编译选项
# -Wall: 显示所有警告
# -Iinc: 告诉编译器头文件在 inc 目录下
CFLAGS = -Wall -Iinc

# 链接选项 (这里不需要额外库，所以留空。如果用了 pthread 则加 -lpthread)
LDFLAGS = 

# 源文件目录
SRC_DIR = ..\src
# 头文件目录
INC_DIR = ..\inc
# 构建输出目录
BUILD_DIR = others
TARGET_DIR = target

# 查找源文件 (自动查找 src 目录下所有的 .c 文件)
SRCS = $(wildcard $(SRC_DIR)/*.c)

# 生成目标文件列表 (将 src/main.c 替换为 build/main.o)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# 依赖文件列表 (用于头文件改动时自动重新编译)
DEPS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.d,$(SRCS))

# ==========================================
# 2. 默认目标 (输入 make 时执行)
# ==========================================

.PHONY: all clean

all: $(TARGET_DIR)\$(TARGET)
	@echo "======================================"
	@echo "Build Successful!
	@echo "======================================"

# ==========================================
# 3. 链接规则 (生成 exe)
# ==========================================

# $^: 代表所有依赖文件 (所有的 .o)
# $@: 代表目标文件
$(TARGET_DIR)\$(TARGET): $(OBJS)
	@if not exist $(TARGET_DIR) mkdir $(TARGET_DIR)
	@$(CC) $^ -o $@ $(LDFLAGS)

# ==========================================
# 4. 编译规则 (生成 .o)
# ==========================================

# -MMD -MP: 自动生成 .d 依赖文件，包含头文件依赖关系
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
# 	@echo "Compiling $<..."
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)  # 如果 build 目录不存在则创建
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# ==========================================
# 5. 清理规则 (输入 make clean 时执行)
# ==========================================

clean:
	@echo "Cleaning build directory..."
	@if exist $(BUILD_DIR) (rd /s /q $(BUILD_DIR)) else (echo Directory already clean.)
	@echo "Clean done."

# ==========================================
# 6. 包含依赖文件 (自动处理头文件修改)
# ==========================================

-include $(DEPS)
