#include "../inc/usart.h"

int isPortOpen = PortClose;
HANDLE hSerial = INVALID_HANDLE_VALUE;



int main(void)
{
    int choice;
    while(choice!=5)
    {
        printMeau();
        if(scanf("%d", &choice) != 1){
            while(getchar()!='\n');
            choice = 0;
        }
        while(getchar()!='\n');

        switch (choice){
        case 1:
            openPort();
            break;
        case 2:
            sendData();
            break;
        case 3:
            readData();
            break;
        case 4:
            closePort();
            break;
        case 5:
            closePort();
            printf("Exit\n");
            break;
        default:
            printf("Invalid option, please enter again!\n");
            break;
        }
    }
    return 0;

}

void printMeau()
{
    // system("cls");
    printf("==========================================\n");
    printf("        Serial Port Assistant Win32       \n");
    printf("==========================================\n");

    if(isPortOpen){
        printf("The COM has opened\n");
    }
    else{
        printf("The COM is close\n");
    }

    printf("------------------------------------------\n");
    printf("  1. Open Port\n");
    printf("  2. Send Data (Hex/ASCII)\n");
    printf("  3. Receive Data (Hex/ASCII)\n");
    printf("  4. Close Port\n");
    printf("  5. Exit\n");
    printf("------------------------------------------\n");
}


void openPort(){
    char portName[20];
    char fullPortName[30];
    int baud;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};
    printf("----------------Open Port-----------------\n");
    printf("Input Port Name: ");
    scanf("%s", portName);
    sprintf(fullPortName, "\\\\.\\%s", portName);
    printf("Input baud rate: ");
    scanf("%d", &baud);
    if(getBaudRateCode(baud) == -1){
        printf("Wrong baud rate, it has changed the default baud rate 115200\n");
        baud = 115200;
    }

    hSerial = CreateFileA(fullPortName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if(hSerial == INVALID_HANDLE_VALUE) {
        printf("Error!!! I can't open the COM, I need to check it!\n");
        return ;
    }
    

    printf("The COM has been opened successfully\n");

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if(!GetCommState(hSerial, &dcbSerialParams)){
        printf("I can't get the configuration of the COM\n");
        CloseHandle(hSerial);
        return ;
    }

    // Same as USART of stm32
    dcbSerialParams.BaudRate = getBaudRateCode(baud);
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if(!SetCommState(hSerial, &dcbSerialParams)){
        printf("I can't config the COM\n");
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
        return ;
    }

    printf("COM is configed successfully!\n");
    
    // Some setting about time out
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if(!SetCommTimeouts(hSerial, &timeouts)){
        printf("I can't config the setting about time out\n");
        CloseHandle(hSerial);
        return ;
    }

    isPortOpen = PortOpen;
}

/**
 * @brief: baud rate transform
 */
int getBaudRateCode(int baud) {
    switch(baud) {
        case 9600: return CBR_9600;
        case 19200: return CBR_19200;
        case 38400: return CBR_38400;
        case 57600: return CBR_57600;
        case 115200: return CBR_115200;
        default: return -1;
    }
}

void sendData()
{
    if (!isPortOpen) {
        printf("Error: Please open COM first\n");
        return;
    }
    int choice = 0;
    printf("----------------Send Data-----------------\n");

    while(choice != 3)
    {
        printf("------------------------------------------\n");
        printf("  1. Send separately\n");
        printf("  2. Send continuously\n");
        printf("  3. Go back\n");
        printf("------------------------------------------\n");
        if(scanf("%d", &choice) != 1){
            while(getchar()!='\n');
            choice = 0;
        }
        switch (choice)
        {
        case 1:
            sendSparate();
            break;
        case 2:
            sendContinue();
            break;
        case 3:
            break;
        default:
            printf("Invalid option, please enter again!\n");
            break;
        }
    }
    return ;
    
}
void sendSparate()
{
    char data[256];
    printf("Input string that you want to send (default information: 'Hello'): \n");
    
    // 清空缓冲区，防止之前的输入残留
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    scanf("%255s", data);
    int len = strlen(data);
    loopSend(hSerial, data, len, 1, 1000);
}


void sendContinue()
{
    int nums = 0;
    char data[256];
    printf("How many times do you want to send: ");
    scanf("%d", &nums);

    printf("Input string that you want to send (default information: 'Hello'): \n");
    // 清空缓冲区，防止之前的输入残留
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    scanf("%255s", data);
    int len = strlen(data);
    loopSend(hSerial, data, len, nums, 1000);
    
}

void loopSend(HANDLE hSerial, char* data, int len, int nums, int delay_ms)
{
    DWORD bytesWritten;
    // 如果用户直接回车，发送默认内容
    if (len == 0) { 
        strcpy(data, "Hello\n");
        // WriteFile(hSerial, "1", len, &bytesWritten, NULL);
    }
    for(int i=0; i<nums; i++)
    {
        if(WriteFile(hSerial, data, len, &bytesWritten, NULL)) {
            printf("%d send over (%ld byte): %s\n", i+1, bytesWritten, data);
        } else {
            printf("%d send failed!\n", i+1);
        }
        Sleep(delay_ms);
    }
}

void readData()
{
    if (!isPortOpen) {
        printf("Error: Please open COM first\n");
        return;
    }

    char buffer[256];
    char printBuff[256];
    int pos = 0;
    DWORD bytesRead;
    int ch; // 用于接收键盘按键

    printf("\n--- 实时监控模式 (按 'q' 退出) ---\n");
    printf("正在监听串口数据...\n");

    // 1. 进入死循环
    while (1)
    {
        // ------------------------------------------------
        // A. 检测键盘输入 (非阻塞)
        // ------------------------------------------------
        if (_kbhit()) { // 如果键盘有按键动作
            ch = _getch(); // 获取按下的字符
            
            // 如果按下的是 q 或 Q，则退出循环
            if (ch == 'q' || ch == 'Q') {
                printf("\nExit receive data\n");
                break; 
            }
        }

        // ------------------------------------------------
        // B. 读取串口数据 (非阻塞，依靠超时设置)
        // ------------------------------------------------
        // 注意：要实现“一直等待但不卡死”，依赖的是之前在 openPort 里
        // 设置的 COMMTIMEOUTS (ReadTotalTimeoutConstant = 50)。
        // 这样 ReadFile 如果没有数据，会在 50ms 后自动返回，不会卡死程序。
        
        memset(buffer, 0, sizeof(buffer)); // 清空缓冲区
        
        if (ReadFile(hSerial, buffer, sizeof(printBuff)-2, &bytesRead, NULL)) {
            if (bytesRead > 0) {
                for(int i=0; i<bytesRead; i++)
                {
                    if(buffer[i] == '\n')
                    {
                        printBuff[pos] = '\n';
                        printBuff[pos+1] = '\0';
                        printf("[RX] %s", printBuff);
                        pos = 0;
                        break;
                    }
                    else
                    {
                        printBuff[pos] = buffer[i];
                        pos++;
                    }

                }
                
                memset(buffer, 0, sizeof(buffer));
                 
            }
        }
        
        // 稍微给 CPU 一点休息时间，避免占用率 100%
        Sleep(10); 
    }
}


void closePort()
{
    if (isPortOpen) {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
        isPortOpen = 0;
        printf("The Port has been closed\n");
    } else {
        printf("The Port even not open\n");
    }
}