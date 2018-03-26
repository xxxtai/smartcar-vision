#include "serial.h"
#include <iostream>
Serial::Serial(){
    myCom = new Posix_QextSerialPort("/dev/ttyTHS2",QextSerialBase::Polling);

    myCom ->open(QIODevice::ReadWrite);
    //以读写方式打开串口
    myCom->setBaudRate(BAUD115200);
    //波特率设置，我们设置为9600
    myCom->setDataBits(DATA_8);
   //数据位设置，我们设置为8位数据位
    myCom->setParity(PAR_NONE);
    //奇偶校验设置，我们设置为无校验
    myCom->setStopBits(STOP_1);
    //停止位设置，我们设置为1位停止位
    myCom->setFlowControl(FLOW_OFF);
    myCom->setTimeout(1000);
}

void Serial::readMyCom(){
    QByteArray temp = myCom->readAll();
    QString str = temp;
    std::cout << "rec : " << str.toStdString() << std::endl;
}

void Serial::send(QByteArray array){
    myCom->write(array); //以ASCII码形式将数据写入串口
}
