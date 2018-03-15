#ifndef TX2UART_H
#define TX2UART_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> //文件控制定义
#include <termios.h>//终端控制定义
#include <errno.h>
#include <string.h>

#define DEVICE "/dev/ttyS0"
#define S_TIMEOUT 1


class TX2UART
{
public:
    TX2UART();
    int init_serial(void);
    int WriteData(char *data, int datalen);
    int uart_recv(int fd, char *data, int datalen);

private:
    int serial_fd = 0;
    unsigned int total_send = 0 ;
    unsigned int total_length = 0 ;
};

#endif // TX2UART_H
