#include"config.h"
using namespace std;
int main() {
    init();
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData); //MAKEWORD（主版本号，副版本号）
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        cout << "初始化套接字DLL失败..." << endl;
        exit(EXIT_FAILURE);
    }
    cout << "初始化套接字DLL成功" << endl;
    //创建服务器端套接字
    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//IPv4地址族，流式套接字，TCP协议
    if (server == INVALID_SOCKET) {
        cout << "套接字创建失败..." << endl;
        exit(EXIT_FAILURE);
    }
    cout << "套接字创建成功！" << endl;
    //绑定服务器地址
    server_addr.sin_family = AF_INET;//地址类型
    server_addr.sin_port = htons(PORT);//端口号
    //将服务器套接字与服务器地址和端口绑定
    if (bind(server, (LPSOCKADDR)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        cout << "绑定失败..." << endl;
        exit(EXIT_FAILURE);
    }
    cout << "绑定成功。端口为： " << PORT << endl;
    //设置监听
    if (listen(server, clients_num) != 0) {
        cout << "监听失败..." << endl;
        exit(EXIT_FAILURE);
    }
    cout << "正在监听..." << endl;
    cout << "服务器准备就绪，现有" << curr_num << "/" << clients_num-1 << "用户在线。" << endl;
    //循环接收客户端请求
    while (1) {
        if (curr_num <= clients_num) {
            int num = find_index();
            int addrlen = sizeof(SOCKADDR);
            clients[num] = accept(server, (sockaddr*)&client_addr[num], &addrlen);//接收客户端请求
            if (clients[num] == SOCKET_ERROR) {
                cout << "用户创建失败。现有" << curr_num << "/" << clients_num-1 << "用户在线。" << endl;
                closesocket(server);
                WSACleanup();
                exit(EXIT_FAILURE);
            }
            client_online[num] = 1;//连接位 置1表示占用
            curr_num++; //当前连接数加1
            //创建时间戳，记录当前通讯时间
            curr();
            cout << "用户Client@" << clients[num] << "已于" << curr_time << "连接。现有" << curr_num << " / " << clients_num-1 << "用户在线。" << endl;
            HANDLE Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CreateThread, (LPVOID)num, 0, NULL);//创建线程
            if (Thread == NULL) {
                cout << "检测到线程创建错误..." << endl;
                exit(EXIT_FAILURE);
            }
            else CloseHandle(Thread);
        }
        else {
            cout << "当前服务器已达上限。" << endl;
            Sleep(1000);
            break;
        }
    }
    closesocket(server);
    WSACleanup();
    return 0;
}

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
