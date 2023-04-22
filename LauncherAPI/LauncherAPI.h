#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <functional>

#include "json.hpp"

#pragma comment(lib, "wsock32.lib")

using namespace nlohmann;

namespace LauncherAPI
{
	constexpr auto API_UNKNOWN = 0x0;
	constexpr auto API_ABORT = 0x1;
	constexpr auto API_RETRY = 0x2;
	constexpr auto API_IGNORE = 0x3;
	constexpr auto API_CANCEL = 0x4;
	constexpr auto API_TRY = 0x5;
	constexpr auto API_CONTINUE = 0x6;
	constexpr auto API_OK = 0x7;
	constexpr auto API_YES = 0x8;
	constexpr auto API_NO = 0x9;

	class SocketConnection
	{
	public:
		SOCKET _socket;
	public:
		SocketConnection()
		{
			WSAData wsaData;
			WSAStartup(MAKEWORD(2, 2), &wsaData);
			SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			this->_socket = s;
		}
	public:
		int connectToServer(std::string ip, int port)
		{
			SOCKADDR_IN addr;
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);
			addr.sin_addr.s_addr = inet_addr(ip.c_str());

			int result = connect(this->_socket, (SOCKADDR*)&addr, sizeof(addr));
			return result;
		}
		int sendString(std::string str)
		{
			return send(this->_socket, str.c_str(), str.size(), 0);
		}
	public:
		bool receiveRaw(void* buf, int buflen) {
			char* p = (char*)buf;
			while (buflen > 0) {
				int received = recv(this->_socket, p, buflen, 0);
				if (received < 1) return false;
				p += received;
				buflen -= received;
			}
			return true;
		}
		std::string receiveString() {
			char str[1024] = "";
			receiveRaw(&str, sizeof(str));
			return str;
		}
	};

	enum MessageBoxButtons
	{
		AbortRetryIgnore,
		CancelTryContinue,
		OK,
		OKCancel,
		RetryCancel,
		YesNo,
		YesNoCancel
	};

	class MessageBox
	{
	private:
		static std::string ConvertToString(MessageBoxButtons buttons)
		{
			return buttons == AbortRetryIgnore ? "AbortRetryIgnore" :
				buttons == CancelTryContinue ? "CancelTryContinue" :
				buttons == OK ? "OK" :
				buttons == OKCancel ? "OKCancel" :
				buttons == RetryCancel ? "RetryCancel" :
				buttons == YesNo ? "YesNo" :
				buttons == YesNoCancel ? "YesNoCancel" : "";
		}
	public:
		static void Show(std::string title, std::string text, MessageBoxButtons buttons)
		{
			LauncherAPI::SocketConnection connection = LauncherAPI::SocketConnection();
			int result = connection.connectToServer("127.0.0.1", 4260);
			if (result == -1) return;
			connection.sendString("{\"messagebox\":{\"Title\":\"" + title + "\",\"Text\":\"" + text + "\",\"Buttons\":\"" + ConvertToString(buttons) + "\"}}");
		}
	public:
		static void Show(std::string title, std::string text, MessageBoxButtons buttons, std::function<void (int)> onResponse)
		{
			LauncherAPI::SocketConnection connection = LauncherAPI::SocketConnection();
			int result = connection.connectToServer("127.0.0.1", 4260);
			if (result == -1) return;
			connection.sendString("{\"messagebox\":{\"Title\":\"" + title + "\",\"Text\":\"" + text + "\",\"Buttons\":\"" + ConvertToString(buttons) + "\"}}");
			
			std::string jsonString = connection.receiveString();
			auto json = json::parse(jsonString);
			onResponse(json["response"]["id"]);
		}
	};
}