#include "dector.h"
#include <thread>

#include <unistd.h>
#include <mutex>
#include "serial.h"

Dector dector;
Serial serial;
mutex sLock;
float err_last = 0;
int D_value = 0;
float integral = 0;
float Kp = 0.12,Ki = 0,Kd = 0.0;
unsigned char speed = 150;
volatile bool stop = false;
bool readyToTurn = false;
double lastTime = 0;
void MotroCarControl();
void PIDControl();
void setTimer();
void turnRight(int time);
int main(int argc, char *argv[]){
    lastTime = clock();
    thread th1(MotroCarControl);
    thread th2(setTimer);

    dector.videoTest("/home/nvidia/Documents/images/1.AVI");//
//    dector.cameraTest();

//    for(int i = 1; i <= 18; i++) {
//        dector.imageTest("/home/nvidia/Documents/images/" + to_string(i) + ".JPG");
//    }
    return 0;
}

void MotroCarControl(){
    sleep(1);
    while(true){
//        cout << "angle_err:" << dector.angle_err << endl;
//        cout << "position_err:" << dector.position_err << endl;

    //    integral += dector.position_err;
        int value = dector.decode_value;
        if(value == 544 || value == 2810 || value == 2570 || value == 2815) {
            readyToTurn = true;
	}
	if(readyToTurn) {
	    if(dector.centre_y > dector.imageRows*2/3){
                turnRight(640000);
	        readyToTurn = false;
                dector.decode_value = -1;
            }
            if(dector.centre_y > dector.imageRows/2){
                turnRight(740000);
	        readyToTurn = false;
                dector.decode_value = -1;
            }
            if(dector.centre_y > dector.imageRows/3){
                turnRight(880000);
	        readyToTurn = false;
                dector.decode_value = -1;
            }
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

void turnRight(int time){
    usleep(time);
    sLock.lock();
    stop = true;
    char buf1[13] = {'&','V','=','+','0','0','0','/','+','0','0','0','&'};
    QByteArray qb1(QByteArray::fromRawData(buf1, 13));
    serial.send(qb1);
    cout << "motro car turn!!!!!!" << endl;

//    usleep(1000000);

    char buf2[13] = {'&','V','=','+','0','7','5','/','-','0','7','5','&'};
    QByteArray qb2(QByteArray::fromRawData(buf2, 13));
    serial.send(qb2);
    cout << "motro car turn right" << endl;
    sLock.unlock();

    usleep(1300000);
    
    sLock.lock();
    char buf3[13] = {'&','V','=','+','0','0','0','/','+','0','0','0','&'};
    QByteArray qb3(QByteArray::fromRawData(buf3, 13));
    serial.send(qb3);
    sLock.unlock();
//    cout << "motro car stop!!!!!!" << endl;
//    usleep(1000000);
    stop = false;
    dector.turned = true;
}

void PIDControl(){
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
    sLock.lock();
    if(!stop){
        QByteArray qb(QByteArray::fromRawData(buf, 13));
        serial.send(qb);
    }
    sLock.unlock();
}
