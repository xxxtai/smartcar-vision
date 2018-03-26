#ifndef SERIAL_H
#define SERIAL_H
#include <QTimer>
#include "posix_qextserialport.h"

class Serial
{
public:
    Serial();
        void send(QByteArray array); //”发送数据”按钮槽函数
            void readMyCom(); //读取串口

private:


    Posix_QextSerialPort *myCom;
    QTimer *readTimer;
};

#endif // SERIAL_H
