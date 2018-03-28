#include "dector.h"
#include <thread>
#include "socketclient.h"
#include <unistd.h>
#include <mutex>
#include "serial.h"


Dector dector;
Serial serial;
int clnt_sock;
mutex sLock;
float err_last = 0;
int D_value = 0;
float integral = 0;
float Kp = 0.12,Ki = 0,Kd = 0.0;
unsigned char speed = 150;
volatile bool stop = false;
double lastTime = 0;
void MotroCarControl();
void PIDControl();
void setTimer();
void readFromExpress();
void turn(int time, int type);
int main(int argc, char *argv[]){
    struct sockaddr_in serv_addr;

    clnt_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (clnt_sock < 0) {
        printf("sock() error\n");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("192.168.1.103");
    serv_addr.sin_port = htons(8899);

    if (connect(clnt_sock,
            (struct sockaddr*)&serv_addr,
            sizeof(serv_addr)) == -1) {
        printf("connect() error\n");
        exit(1);
    }
    char * initMsg = "h1e";
    write(clnt_sock, initMsg, strlen(initMsg));
    usleep(500000);
    char * stateMsg = "s1e";
    write(clnt_sock, stateMsg, strlen(stateMsg));


    lastTime = clock();
    thread th1(MotroCarControl);
    thread th2(setTimer);
    thread commThread(readFromExpress);

//    dector.videoTest("/home/nvidia/Documents/images/1.AVI", clnt_sock);//
    dector.cameraTest(clnt_sock);

//    for(int i = 1; i <= 18; i++) {
//        dector.imageTest("/home/nvidia/Documents/images/" + to_string(i) + ".JPG");
//    }
    close(clnt_sock);
    return 0;
}

void readFromExpress(){
    char buf[1];
    char data[50];
    int index = 0;
    cout << "reading" << endl;
    while (1) {
        memset(buf, 0, sizeof(buf));
        ssize_t size = read(clnt_sock, buf, sizeof(buf));

        if (size >= 0) {
            data[index++] = buf[0];
            if(data[index - 1] == 'e') {
//                cout << "rec from express:" << buf[0] << endl;
                for(int i = 0; i < index; i++) {
                    if(data[i] == 'm') {
                        if(data[i + 1] == '0' && data[i + 2] == '2') {
                            cout << "comm:stop" << endl;
                            sLock.lock();
                            stop = true;
                            char buf1[13] = {'&','V','=','+','0','0','0','/','+','0','0','0','&'};
                            QByteArray qb1(QByteArray::fromRawData(buf1, 13));
                            serial.send(qb1);
                            sLock.unlock();
                            char * stateMsg = "s2e";
                            write(clnt_sock, stateMsg, strlen(stateMsg));
                            break;
                        }
                        if(data[i + 1] == '0' && data[i + 2] == '1') {
                            cout << "comm:farword" << endl;
                            stop = false;
                            char * stateMsg = "s1e";
                            write(clnt_sock, stateMsg, strlen(stateMsg));
                            break;
                        }
                    } else if(data[i] == 'r') {
                        dector.nodeIndex = 0;
//                        memset(dector.routeNodes, 0, sizeof(dector.routeNodes));
//                        memset(dector.command, 0, sizeof(dector.command));
                        int count_l = 0;
                        for(int j = i + 1; j < index; j++) {
                            if(data[j] == 'l' || data[j] == 'e') {
                                for(int m = j - count_l; m < j; m++) {
                                    if(data[m] == 'i') {
                                        int value = 0, comm = 0;
                                        for(int n = j -count_l; n < m; n++) {
                                            value += ((data[n] - 48)*pow(10, m - n - 1));
                                        }
                                        if(data[j] == 'e') {
                                            cout << "stop:" << value << data[++m] << data[++m];
                                            dector.stopNum = value;
                                            dector.routeNodes[dector.nodeIndex] = value;
                                            dector.command[dector.nodeIndex] = 5;
                                            dector.nodeIndex++;
                                        } else if(data[j] == 'l') {
                                            comm = data[m + 2] - 48;
                                            dector.routeNodes[dector.nodeIndex] = value;
                                            dector.command[dector.nodeIndex] = comm;
                                            dector.nodeIndex++;
                                        }
                                    }
                                }
                                count_l = 0;
                                cout << endl;
                            } else {
                                count_l++;
                            }
                        }
                        for(int i = 0; i < dector.nodeIndex; i++) {
                            cout << dector.routeNodes[i] << "//" << dector.command[i] << endl;
                        }
                        dector.nodeIndex = 0;
                        break;
                    }
                }
                index = 0;
                memset(data, 0, sizeof(data));
            }
        } else {
            printf("read() error\n");
            break;
        }
    }
}

void MotroCarControl(){
    sleep(1);
    while(true){
        if(dector.readyToTurn) {
            if(dector.centre_y > dector.imageRows*2/3){
                turn(640000, dector.command[dector.nodeIndex]);
                dector.nodeIndex++;
                dector.readyToTurn = false;
            }
            if(dector.centre_y > dector.imageRows/2){
                turn(740000, dector.command[dector.nodeIndex]);
                dector.nodeIndex++;
                dector.readyToTurn = false;
            }
            if(dector.centre_y > dector.imageRows/3){
                turn(880000, dector.command[dector.nodeIndex]);
                dector.nodeIndex++;
                dector.readyToTurn = false;
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

void turn(int time, int type){//4:turnRight 3:turnLeft 1:farword 5:stop
    if(type == 1) {
        return;//farword, noting to do
    }

    usleep(time);

    sLock.lock();
    stop = true;
    char buf1[13] = {'&','V','=','+','0','0','0','/','+','0','0','0','&'};
    QByteArray qb1(QByteArray::fromRawData(buf1, 13));
    serial.send(qb1);
    serial.send(qb1);
    if(type == 5){
        sLock.unlock();    
        char * stateMsg = "s2e";
        write(clnt_sock, stateMsg, strlen(stateMsg));
	cout << "arrive in destination!!!" << endl;
	return;
    }
    cout << "motro car stop to turn!" << endl;

//    usleep(1000000);

    char buf2[13] = {'&','V','=','+','0','7','5','/','-','0','7','5','&'};
    if(type == 4) {
        buf2[3] = '+';
        buf2[8] = '-';
        cout << "car turning right" << endl;
    } else if (type == 3) {
        buf2[3] = '-';
        buf2[8] = '+';
        cout << "car turning left" << endl;
    }

    QByteArray qb2(QByteArray::fromRawData(buf2, 13));
    serial.send(qb2);
    serial.send(qb2);
    sLock.unlock();

    usleep(1300000);
    
    sLock.lock();
    char buf3[13] = {'&','V','=','+','0','0','0','/','+','0','0','0','&'};
    QByteArray qb3(QByteArray::fromRawData(buf3, 13));
    serial.send(qb3);
    serial.send(qb3);
    cout << "car finished turning, now going" << endl;
    sLock.unlock();

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

    sLock.lock();
    if(!stop){
        QByteArray qb(QByteArray::fromRawData(buf, 13));
        serial.send(qb);
    }
    sLock.unlock();
}
