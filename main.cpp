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
int MAX_D_VALUE = 40;
bool finishedTurn = false;
double last_position_err = 0;
int pid_count;
float integral = 0;
float Kp = 0.12,Ki = 0,Kd = 0.0;
int speed = 20;
volatile bool stop = false;
double lastTime = 0;
void MotroCarControl();
void PIDControl();
void setTimer();
void readFromExpress();
void turn(int time, int type);
int main(int argc, char *argv[]){
//    struct sockaddr_in serv_addr;

//    clnt_sock = socket(AF_INET, SOCK_STREAM, 0);
//    if (clnt_sock < 0) {
//        printf("sock() error\n");
//        exit(1);
//    }

//    memset(&serv_addr, 0, sizeof(serv_addr));
//    serv_addr.sin_family = AF_INET;
//    serv_addr.sin_addr.s_addr = inet_addr("192.168.1.109");
//    serv_addr.sin_port = htons(8899);

//    if (connect(clnt_sock,
//            (struct sockaddr*)&serv_addr,
//            sizeof(serv_addr)) == -1) {
//        printf("connect() error\n");
//        exit(1);
//    }
//    char * initMsg = "h1e";
//    write(clnt_sock, initMsg, strlen(initMsg));
//    usleep(500000);
//    char * stateMsg = "s1e";
//    write(clnt_sock, stateMsg, strlen(stateMsg));


    lastTime = clock();
    thread th1(MotroCarControl);
    thread th2(setTimer);
    thread commThread(readFromExpress);

//    dector.videoTest("/home/nvidia/Documents/images/1.AVI", clnt_sock);//
    dector.cameraTest(clnt_sock);

//    for(int i = 1; i <= 18; i++) {
//        dector.imageTest("/home/nvidia/Documents/images/" + to_string(i) + ".JPG");
//    }
    th1.join();
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
			    dector.commandStop = true;
			    dector.stopDecode = true;
                            cout << "comm:stop" << endl;
                            char * stateMsg = "s2e";
                            write(clnt_sock, stateMsg, strlen(stateMsg));
                            break;
                        }
                        if(data[i + 1] == '0' && data[i + 2] == '1') {
                            cout << "comm:farword" << endl;
                            stop = false;
			    dector.stopDecode = false;
                            char * stateMsg = "s1e";
                            write(clnt_sock, stateMsg, strlen(stateMsg));
                            break;
                        }
                    } else if(data[i] == 'r') {
                        dector.nodeIndex = 0;
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
			dector.decode_value = 0;
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
                turn(590000, dector.command[dector.nodeIndex]);
                dector.nodeIndex++;
                dector.readyToTurn = false;
            } else if(dector.centre_y > dector.imageRows/2){
//                turn(740000, dector.command[dector.nodeIndex]);
//                dector.nodeIndex++;
//                dector.readyToTurn = false;
            } else if(dector.centre_y > dector.imageRows/3){
                turn(840000, dector.command[dector.nodeIndex]);
                dector.nodeIndex++;
                dector.readyToTurn = false;
            }
        }
        if(dector.commandStop) {
	    if(dector.centre_y > dector.imageRows*2/3){
		turn(640000, 5);
		dector.commandStop = false;
	    } else if(dector.centre_y > dector.imageRows/2){
		turn(740000, 5);
		dector.commandStop = false;
	    } else if(dector.centre_y > dector.imageRows/3){
		turn(880000, 5);
		dector.commandStop = false;
	    }
	}
    }
}

void setTimer(){
    sleep(1);
    while(true){
        double cur_time = clock();
        if(cur_time - lastTime >= (3*CLOCKS_PER_SEC/100)) {
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
    char send_buf_stop[8]="z 0 0;\r";
    serial.send(send_buf_stop);
    if(type == 5){
        sLock.unlock();    
        char * stateMsg = "s2e";
        write(clnt_sock, stateMsg, strlen(stateMsg));
//	cout << "arrive in destination!!!" << endl;
//	if(dector.nodeIndex > 0 && dector.decode_value != 132 && dector.decode_value != 512 && dector.decode_value == dector.stopNum){
//	    cout << "motor car unload !!!" << endl;
//    	    char buf_unload[13] = {'&','V','=','u','0','0','0','/','u','0','0','0','#'};
//    	    QByteArray qb_unload(QByteArray::fromRawData(buf_unload, 13));
////    	    serial.send(qb_unload);
//	}
	return;
    }
    cout << "motro car stop to turn!" << endl;

//    usleep(1000000);

    char send_buf_turn[10] = {'z',' ','5','0',' ','-','5','0',';','\r'};
    if(type == 4) {

        cout << "car turning right" << endl;
    } else if (type == 3) {

        cout << "car turning left" << endl;
    }
    serial.send(send_buf_turn);

    sLock.unlock();

    usleep(820000);
    
    sLock.lock();

    serial.send(send_buf_stop);
    cout << "car finished turning, now going" << endl;
    sLock.unlock();

//    usleep(1000000);
    stop = false;
    finishedTurn = true;
    dector.turned = true;
}

void PIDControl(){
    double err_gap = last_position_err - dector.position_err;
    last_position_err = dector.position_err;
    if(finishedTurn){
        pid_count++;
        if(pid_count == 60){
            pid_count = 0;
            finishedTurn = false;
        }
        speed = 35;
        Kd = 0;
        Kp = 0.45;
    } else {
        speed = 20;
        Kd = 4;
        Kp = 0.12;
    }

    D_value = (int)(Kp * dector.position_err - Kd * err_gap);
    if(D_value > MAX_D_VALUE){
        D_value = MAX_D_VALUE;
    } else if(D_value < (0 - MAX_D_VALUE)){
        D_value = 0 - MAX_D_VALUE;
    }
    int left_value = speed - D_value/2;
    int right_value = speed + D_value/2;

    char send_buf[10]="z 10 10;\r";
    send_buf[2] = 48 + left_value/10;
    send_buf[3] = 48 + (left_value - (send_buf[2] - 48)*10);
    send_buf[5] = 48 + right_value/10;
    send_buf[6] = 48 + (right_value - (send_buf[5] - 48)*10);

    sLock.lock();
    if(!stop){

//        cout << send_buf << endl;
        serial.send(send_buf);
    }
    sLock.unlock();
}
