#include "config.h"
int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		cout << "初始化套接字失败..." << endl;
		Sleep(1000);
		exit(EXIT_FAILURE);
	}
	cout << "初始化套接字成功。" << endl;
	//创建客户端套接字
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client == INVALID_SOCKET) {
		cout << "创建客户端套接字失败..." << endl;
		Sleep(1000);
		exit(EXIT_FAILURE);
	}
	cout << "创建客户端套接字成功。" << endl;
	//绑定服务器地址
	server_ad.sin_family = AF_INET;//地址类型
	server_ad.sin_port = htons(PORT);//端口号
	inet_pton(AF_INET, "127.0.0.1", &(server_ad.sin_addr));
	//向服务器发起请求
	if (connect(client, (SOCKADDR*)&server_ad, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		cout << "连接失败。错误代码为：" << WSAGetLastError() << endl;
		Sleep(1000);
		exit(EXIT_FAILURE);
	}
	else cout << "连接成功。" << endl;


	//创建消息线程
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveThread, NULL, 0, 0);

	char message[BUFFER_SIZE] = {};
	cout << "可输入聊天内容，输入'$q'或'$quit'即可退出。" << endl;
	//发送消息
	while (1) {
		cin.getline(message, sizeof(message));
		send(client, message, sizeof(message), 0);//发送消息
		if ((strcmp(message, "$q") == 0) || (strcmp(message, "$quit") == 0)) {
			break;// $q或者$quit退出
		}
	}
	closesocket(client);
	WSACleanup();
	return 0;
}
