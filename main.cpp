#include "dector.h"
//#include <windows.h>
//#include "stdafx.h"
//#include "SerialPort.h"
#include <thread>
#include "tx2uart.h"
#include <unistd.h>

Dector dector;
TX2UART mySerialPort;
float err_last = 0;
int D_value = 0;
float integral = 0;
float Kp = 0.12,Ki = 0,Kd = 0.0;
unsigned char speed = 150;
bool stop = false;
double lastTime = 0;
void MotroCarControl();
void PIDControl();
void setTimer();
void turnRight();
int main(int argc, char *argv[]){

    mySerialPort.init_serial();



//    if (!mySerialPort.InitPort(5, CBR_115200)){
//        std::cout << "initPort fail !" << std::endl;
//    }else{
//        std::cout << "initPort success !" << std::endl;
//    }

//    if (!mySerialPort.OpenListenThread()){
//        std::cout << "OpenListenThread fail !" << std::endl;
//    }else{
//        std::cout << "OpenListenThread success !" << std::endl;
//    }


    lastTime = clock();
    thread th1(MotroCarControl);
    thread th2(setTimer);

//    dector.videoTest("/home/nvidia/Documents/images/1.AVI");//
//    dector.cameraTest();

    for(int i = 1; i <= 18; i++) {
        dector.imageTest("C:\\Users\\xxxta\\Desktop\\images\\" + to_string(i) + ".JPG");
    }
    return 0;
}

void MotroCarControl(){
    sleep(1);
    while(true){
//        cout << "angle_err:" << dector.angle_err << endl;
//        cout << "position_err:" << dector.position_err << endl;

    //    integral += dector.position_err;
        int value = dector.decode_value;
//	cout << "control" << endl;
        if((dector.decode_value == 55 || value == 3534 || value == 439 || value == 3422) && dector.centre_y > dector.imageRows*2/3) {
//	    cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@turn right" << "centre_y:" << dector.centre_y << "imageRows:" << dector.imageRows << endl;
            turnRight();
	    dector.decode_value = 0;
        }
    }

}

void setTimer(){
    sleep(1);
    while(true){
        double cur_time = clock();
        if(cur_time - lastTime >= (CLOCKS_PER_SEC/10)) {
            lastTime = cur_time;
            PIDControl();
        }
    }
}

void turnRight(){
    usleep(600000);
    stop = true;
    char buf1[13] = {'&','V','=','+','0','0','0','/','+','0','0','0','&'};
    mySerialPort.WriteData(buf1, 13);
//    cout << "motro car stop!!!!!!" << endl;

//    usleep(1000000);

    char buf2[13] = {'&','V','=','+','0','7','5','/','-','0','7','5','&'};
    mySerialPort.WriteData(buf2, 13);
//    cout << "motro car turn right" << endl;

    usleep(1270000);

    char buf3[13] = {'&','V','=','+','0','0','0','/','+','0','0','0','&'};
    mySerialPort.WriteData(buf3, 13);
//    cout << "motro car stop!!!!!!" << endl;
//    usleep(1000);
    stop = false;

}

void PIDControl(){
    if(stop) {
        return;
    }
    D_value = (int)(Kp * dector.position_err);
//    cout << "D_value:" << D_value << endl;
    unsigned char left_value = speed - D_value/2;
    unsigned char right_value = speed + D_value/2;
    string left_value_str;
    string right_value_str;
    if(left_value < 100) {
        left_value_str = "0" + to_string(left_value);
    } else {
        left_value_str = to_string(left_value);
    }

    if(right_value < 100) {
        right_value_str = "0" + to_string(right_value);
    } else {
        right_value_str = to_string(right_value);
    }

    string msg = "&V=" + left_value_str + "/" + right_value_str + "&";

    char buf[13] = {'&','V','=','+','0','0','0','/','+','0','0','0','&'};
    buf[4] = 48 + left_value/100;
    buf[5] = 48 + (left_value - (buf[4] - 48)*100)/10;
    buf[6] = 48 + (left_value - (buf[4] - 48)*100 - (buf[5] - 48)*10);
    buf[9] = 48 + right_value/100;
    buf[10] = 48 + (right_value - (buf[9] - 48)*100)/10;
    buf[11] = 48 + (right_value - (buf[9] - 48)*100 - (buf[10] - 48)*10);

//    cout << "                     msg:";
//    for(int i = 0; i < 13; i++) {
//        cout << buf[i];
//    }
//    cout << endl;
    mySerialPort.WriteData(buf, 13);
}
