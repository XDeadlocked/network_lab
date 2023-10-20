#pragma once
#include<iostream>
#include<cstring>
#include<cstdlib>
#include<chrono>
#include<WinSock2.h>
#include<WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 8000
#define BUFFER_SIZE 1024
#define MAX_CLIENT 6

using namespace std;
SOCKET* clients;
SOCKET client;
SOCKET server;
SOCKADDR_IN* client_addr;
SOCKADDR_IN server_addr;
SOCKADDR_IN server_ad;
int clients_num = MAX_CLIENT;		//容纳总人数，初始化时候使用，默认为
int* client_online;					//在线人数
int curr_num;					//连接情况
char curr_time[100];

void init() {
	cout << "请设置服务器最大人数（不超过5人）：";
	cin >> clients_num;
	clients_num++;
	if (clients_num > MAX_CLIENT) exit(EXIT_FAILURE);
	clients = new SOCKET[clients_num];
	client_addr = new SOCKADDR_IN[clients_num];
	client_online = new int[clients_num];
	for (int i = 0; i < clients_num; i++) {
		client_online[i] = 0;
	}
	curr_num = 0;
	cout << "设置完成" << endl;
	system("cls");
}


void curr() {
	struct tm t;					//时间
	time_t tmp = chrono::system_clock::to_time_t(chrono::system_clock::now());
	localtime_s(&t, &tmp);			//格式化当前时间
	strftime(curr_time, sizeof(curr_time), "%Y-%m-%d---%H:%M:%S", &t);
}

int find_index(void) {
	for (int i = 0; i < clients_num; i++) {
		if (client_online[i] == 0) return i;
	}
	cout << "搜索失败，当前服务器已达上限..." << endl;
}

DWORD WINAPI CreateThread(LPVOID lpParameter) {
	char BUF_R[BUFFER_SIZE];
	char BUF_S[BUFFER_SIZE];
	//初始化缓冲区
	int R = 0, index;//初始化接受字节数，下标

	while (1) {
		index = int(lpParameter);
		R = recv(clients[index], BUF_R, sizeof(BUF_R), 0);
		if (R) {
			if ((strcmp(BUF_R, "$q") == 0) || (strcmp(BUF_R, "$quit") == 0)) {
				curr();
				closesocket(clients[index]);
				curr_num--;
				client_online[index] = 0;
				cout << "用户Client@" << clients[index] << "已经于" << curr_time << "离线。现有" << curr_num << " / " << clients_num-1 << "用户在线。" << endl;
				return 0;
			}
			curr();
			cout << "[" << curr_time << "]" << "Client@" << clients[index] << ": " << BUF_R << endl;
			sprintf_s(BUF_S, sizeof(BUF_S), "[%s]Client@%d: %s", curr_time, clients[index], BUF_R);
			for (int i = 0; i < clients_num; i++) {
				if (client_online[i] == 1) {
					send(clients[i], BUF_S, sizeof(BUF_S), 0);
				}
			}
			Sleep(1000);
		}
		else {
			cout << "接受失败..." << WSAGetLastError() << endl;
			break;
		}
	}
	Sleep(1000);
	return 0;
}

DWORD WINAPI ReceiveThread() //接收消息线程
{
	while (1) {
		char buffer[BUFFER_SIZE] = {};//接收数据缓冲区
		if (recv(client, buffer, sizeof(buffer), 0) > 0) {
			cout << buffer << endl;
		}
		else if (recv(client, buffer, sizeof(buffer), 0) < 0) {
			cout << "连接失败..." << endl;
			break;
		}
	}
	Sleep(1000);
	return 0;
}