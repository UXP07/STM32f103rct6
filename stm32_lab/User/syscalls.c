#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "main.h"

// 如果你使用的是标准外设库，确认包含了 USART 相关头文件
// #include "stm32f10x_usart.h" 

// 用于关闭文件的接口
int _close(int file) {
    return -1;
}

// 用于将数据写入文件的接口 (printf 最终会调用这里！)
int _write(int file, char *ptr, int len) {
    
    // 这里的逻辑是：标准库把数据交给 _write
    // 我们在 _write 里把数据通过 USART1 发出去
    
    // 遍历数据缓冲区，一个字节一个字节发
    for (int i = 0; i < len; i++) {
        // 等待发送寄存器为空
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        
        // 发送数据
        USART_SendData(USART1, ptr[i]);
    }
    
    return len; // 返回发送的长度
}

// 用于从文件读取数据的接口 (scanf 会用到)
int _read(int file, char *ptr, int len) {
    int n;
    int c;
    
    for (n = 0; n < len; n++) {
        // 等待接收到数据
        while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
        
        // 读取数据
        c = USART_ReceiveData(USART1);
        
        // 遇到回车符则停止
        if (c == '\n') {
            ptr[n] = '\0'; // 结束字符串
            break; 
        } else {
            ptr[n] = c;
        }
    }
    
    return n; // 返回读取的长度
}

// 用于移动文件读写指针的接口
int _lseek(int file, int ptr, int dir) {
    return 0;
}

// 获取文件状态的接口
int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

// 判断文件是否是终端设备的接口
int _isatty(int file) {
    return 1;
}

// 进程退出接口
void _exit(int status) {
    while (1); // 单片机没有操作系统退出，只能死循环
}

// 杀死进程接口
int _kill(int pid, int sig) {
    errno = EINVAL;
    return -1;
}

// 获取进程 ID 接口
int _getpid(void) {
    return 1;
}

// ==========================================
// 下面这个是内存管理相关的，非常重要！
// ==========================================
// 它告诉标准库，堆内存的结束地址在哪里
// 这样 malloc, printf 内部缓冲区等才能正常工作
void* _sbrk(int incr) {
    extern char _end; /* Symbol defined in the linker script */
    extern char _estack; /* Symbol defined in the linker script */
    static char *heap_end;
    char *prev_heap_end;

    // 第一次调用时初始化堆指针
    if (heap_end == 0) {
        heap_end = &_end;
    }

    prev_heap_end = heap_end;

    // 检查是否溢出（堆不能超过栈底）
    if (heap_end + incr > &_estack) {
        // 内存溢出！
        while (1); // 或者你可以返回 -1 并设置 errno
    }

    heap_end += incr;
    return (void *)prev_heap_end;
}
