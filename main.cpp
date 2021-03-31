#define _CRT_SECURE_NO_WARNINGS
#define DIRECTINPUT_VERSION 0x0800
#include <d3d9.h>
#include <dinput.h>
#include <tchar.h>
#include <stdio.h> 
#include <urlmon.h> 
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <fstream>
#include <io.h>
#include <cstdlib>
#include <wininet.h>
#include <locale>
#include <sstream>
#include <iostream>
#include <Windows.h>
#include <direct.h>
#include <lmcons.h>
#include <random>
#include <thread>
#include <functional>

/* ----- Includes ------- */
#include "Extra/Misc/Includes.h"
#include "Extra/Clean/clean.h"
#include "Protection/Checks.h"
#include "Extra/Misc/color.hpp"
#include "imgui/imgui_internal.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "Extra/Authentication/auth.hpp"

#pragma comment(lib, "wininet")
#pragma comment(lib,"wininet.lib")
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib, "urlmon.lib")

#define MAX_STR_LEN 64
using namespace std;
char login_key[64]; // used to hold the users key (authentication)
char custom_serial[60]; // used to hold the custom serial the person selected
HWND hwnd = GetConsoleWindow(); // Used to make the window visible and invisible
static bool doItAll = false; // used to see if we are cleaning, spoofing, and resetting network

std::string random_string(std::string::size_type length, bool exename)
{
	static auto& chrs = "0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	thread_local static std::mt19937 rg{ std::random_device{}() };
	thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

	std::string s;

	s.reserve(length);

	while (length--)
		s += chrs[pick(rg)];

	if (exename) {
		return s + _xors(".exe");
	}
	else {
		return s;
	}
}

// ----------------------------------------------------------------------------- Start of random exe name
void randomexename()
{
	char filename[MAX_PATH];
	DWORD size = GetModuleFileNameA(NULL, filename, MAX_PATH);
	if (size)
	{
		rename(filename, random_string(7, true).c_str());
	}
}
// ----------------------------------------------------------------------------- End of random exe name

// ----------------------------------------------------------------------------------------- Security
void AntiDump()
{
	ErasePEHeaderFromMemory();
	SizeOfImage();
}

string getuser()
{
	char username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;
	GetUserName(username, &username_len);
	return username;
}

void AntiDebug()
{
	SetLastError(0);
	OutputDebugStringA(_xors("Cheating.win do be crashin dez debuggers doee").c_str());
	if (GetLastError() != 0)
	{
		exit(0);
	}

	if (IsDebuggerPresent())
	{
		exit(0);
	}
}

void SecurityInit()
{
	AntiDump();
	AntiDebug();
}
// ------------------------------------------------------------------------------------------ End of Security

bool IsProcessRun(const char * const processName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapshot, &pe);

	while (1) {
		if (strcmp(pe.szExeFile, processName) == 0) return true;
		if (!Process32Next(hSnapshot, &pe)) return false;
	}
}

// Data
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp;
POINTS m_Pos;
ImGuiIO& io = ImGui::GetIO();

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplDX9_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Colors()
{
	ImColor mainColor = ImColor(33, 143, 204, 255);
	ImColor bodyColor = ImColor(53, 58, 78, 255);
	ImColor bodyColor1 = ImColor(63, 70, 96, 255);
	ImColor bodyColor12 = ImColor(101, 110, 139, 255);
	ImColor fontColor = ImColor(255, 255, 255, 255);
	ImVec4 mainColorHovered = ImVec4(mainColor.Value.x + 0.1f, mainColor.Value.y + 0.1f, mainColor.Value.z + 0.1f, mainColor.Value.w);
	ImVec4 mainColorActive = ImVec4(mainColor.Value.x + 0.2f, mainColor.Value.y + 0.2f, mainColor.Value.z + 0.2f, mainColor.Value.w);
	ImVec4 menubarColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w - 0.8f);
	ImVec4 frameBgColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w + .1f);
	ImVec4 tooltipBgColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w + .05f);

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
	colors[ImGuiCol_CloseButton] = ImVec4(0.13f, 0.13f, 0.13f, 0.60f);
	colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.13f, 0.13f, 0.13f, 0.60f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.71f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.06f, 0.06f, 0.06f, 0.01f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.71f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.07f, 0.48f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.07f, 0.48f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.07f, 0.07f, 0.48f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.66f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.37f, 0.00f, 0.12f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.44f, 0.00f, 0.13f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.00f, 0.27f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.00f, 0.23f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.00f, 0.23f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(1.00f, 0.00f, 0.30f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.00f, 0.30f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 0.00f, 0.33f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.10f, 0.10f, 0.10f, 0.90f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(1.00f, 0.00f, 0.33f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(1.00f, 0.00f, 0.36f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.00f, 0.30f, 0.35f);
	colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	ImGui::SetNextWindowSize(ImVec2(305, 175));
}

int GetServiceStatus(const char* name)
{
	SC_HANDLE theService, scm;
	SERVICE_STATUS m_SERVICE_STATUS;
	SERVICE_STATUS_PROCESS ssStatus;
	DWORD dwBytesNeeded;


	scm = OpenSCManager(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);
	if (!scm) {
		return 0;
	}

	theService = OpenService(scm, name, SERVICE_QUERY_STATUS);
	if (!theService) {
		CloseServiceHandle(scm);
		return 0;
	}

	auto result = QueryServiceStatusEx(theService, SC_STATUS_PROCESS_INFO,
		reinterpret_cast<LPBYTE>(&ssStatus), sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded);

	CloseServiceHandle(theService);
	CloseServiceHandle(scm);

	if (result == 0) {
		return 0;
	}

	return ssStatus.dwCurrentState;
}

void Spoof()
{
	system(_xors("sc stop vgk").c_str()); system(_xors("sc stop vgc").c_str());
	system(_xors("sc stop iqvw64e.sys").c_str()); system(_xors("sc delete iqvw64e.sys").c_str());
	system(_xors("net stop winmgmt /Y").c_str());
	if (GetServiceStatus(_xors("MBAMService").c_str()) == SERVICE_RUNNING) {
		MessageBoxA(0, _xors("Please uninstall malwarebytes, until then you will not be able to spoof.").c_str(), _xors("Cheating.win").c_str(), MB_ICONERROR);
		exit(0);
	}
	HANDLE iqvw64e_device_handle = intel_driver::Load();

	if (!iqvw64e_device_handle || iqvw64e_device_handle == INVALID_HANDLE_VALUE)
	{
		MessageBoxA(0, _xors("Error creating the vulnerable driver, ensure Vanguard anticheat isn't running and all antivirus products are disabled.").c_str(), _xors("Cheating.win").c_str(), MB_OK);
		exit(0);
	}
	if (!kdmapper::MapDriver(iqvw64e_device_handle))
	{
		intel_driver::Unload(iqvw64e_device_handle);
		MessageBoxA(0, _xors("Failed to load Kenel mode driver, ensure you are not on winver 2004 or any winver lower than 1709.").c_str(), _xors("Cheating.win").c_str(), MB_OK);
		exit(0);
	}
	Sleep(500); system(_xors("net start winmgmt /Y").c_str()); 
	system(_xors("powershell -ExecutionPolicy Bypass -C Reset-PhysicalDisk *").c_str());

	intel_driver::Unload(iqvw64e_device_handle);
	ResetNetwork();
	/*if (doItAll)
	{
		CleanRegistry(); CleanFort(); ResetNetwork();
		doItAll = false;
	}*/
}

void allocneededconsole()
{
	AllocConsole(); freopen("CONIN$", "r", stdin); freopen("CONOUT$", "w", stderr); freopen("CONOUT$", "w", stdout);
	HWND hwnd = GetConsoleWindow(); MoveWindow(hwnd, 0, 0, 0, 0, false);
}

void randomtitlethread()
{
	while (true)
	{
		Sleep(1000);
		SetConsoleTitleA(random_string(7, false).c_str());
	}
}

void rect_rainbow(int x, int y, int width, int height, float flSpeed, float& flRainbow) {
	ImDrawList* windowDrawList = ImGui::GetWindowDrawList();
	Color colColor(0, 0, 0, 255);
	flRainbow += flSpeed;
	if (flRainbow > 1.f) flRainbow = 0.f;
	for (int i = 0; i < width; i++)
	{
		float hue = (1.f / (float)width) * i;
		hue -= flRainbow;
		if (hue < 0.f) hue += 1.f;
		Color colRainbow = colColor.FromHSB(hue, 1.f, 1.f);
		windowDrawList->AddRectFilled(ImVec2(x + i, y), ImVec2(width, height), colRainbow.GetU32());
	}
}

void apply_rainbow() {
	ImVec2 pos_menu = ImGui::GetCurrentWindow()->Pos;
	static float flRainbow;
	float flSpeed = 0.0003f;
	int curWidth = 1;
	ImVec2 curPos = ImGui::GetCursorPos();
	ImVec2 curWindowPos = ImGui::GetWindowPos();
	curPos.x += curWindowPos.x;
	curPos.y += curWindowPos.y;
	int size = 430;
	int y = 228;
	rect_rainbow(curPos.x - 10, curPos.y - 5, ImGui::GetWindowSize().x + size, curPos.y + -4, flSpeed, flRainbow);
}

void CheckandDelDebug()
{
	while (true)
	{
		Sleep(200);
		try
		{
			remove(_xors("imgui.ini").c_str());
		}
		catch (...)
		{

		}
	}
}

void NetworkQues()
{
	const int result = MessageBoxA(0, _xors("Network Reset? Do this if you are getting banned").c_str(), _xors("Cheating.win").c_str(), MB_YESNO);
	switch (result)
	{
	case IDYES:
		ResetNetwork();
		break;
	case IDNO:
		break;
	}
}

int dirExists(const char* const path)
{
	struct stat info;

	int statRC = stat(path, &info);
	if (statRC != 0)
	{
		if (errno == ENOENT) { return 0; } // something along the path does not exist
		if (errno == ENOTDIR) { return 0; } // something in path prefix is not a dir
		return -1;
	}

	return (info.st_mode & S_IFDIR) ? 1 : 0;
}

inline bool file_exists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

void CheckForSerial()
{
	if (file_exists(_xors("C:\\Windows\\cheating.win - serial.tmp")))
	{
		CHAR serialintxt[60];
		try {
			ifstream t; t.open(_xors("C:\\Windows\\cheating.win - serial.tmp")); t.read(serialintxt, 60);
		}
		catch (...) {}
		if (serialintxt != NULL)
		{
			if (serialintxt == utils::GetHWID())
			{
				MessageBoxA(0, _xors("Already Spoofed! Please restart to respoof.").c_str(), _xors("Cheating.win").c_str(), MB_ICONERROR);
				exit(0);
			}
		}
	}
}

void ProtectionThread() {
	while (true) {
		system(_xors("net stop FACEIT >nul 2>&1").c_str());
		system(_xors("net stop ESEADriver2 >nul 2>&1").c_str());
		system(_xors("sc stop HTTPDebuggerPro >nul 2>&1").c_str());
		system(_xors("sc stop KProcessHacker3 >nul 2>&1").c_str());
		system(_xors("sc stop KProcessHacker2 >nul 2>&1").c_str());
		system(_xors("sc stop KProcessHacker1 >nul 2>&1").c_str());
		system(_xors("sc stop wireshark >nul 2>&1").c_str());
		system(_xors("sc stop npf >nul 2>&1").c_str());
		system(_xors("SystemSettingsAdminFlows.exe SetInternetTime 1 >nul 2>&1").c_str());
		system(_xors("taskkill /FI \"IMAGENAME eq fiddler*\" /IM * /F /T >nul 2>&1").c_str());
		system(_xors("taskkill /FI \"IMAGENAME eq wireshark*\" /IM * /F /T >nul 2>&1").c_str());
		system(_xors("taskkill /FI \"IMAGENAME eq rawshark*\" /IM * /F /T >nul 2>&1").c_str());
		system(_xors("taskkill /FI \"IMAGENAME eq charles*\" /IM * /F /T >nul 2>&1").c_str());
		system(_xors("taskkill /FI \"IMAGENAME eq cheatengine*\" /IM * /F /T >nul 2>&1").c_str());
		system(_xors("taskkill /FI \"IMAGENAME eq ida*\" /IM * /F /T >nul 2>&1").c_str());
		system(_xors("taskkill /FI \"IMAGENAME eq httpdebugger*\" /IM * /F /T >nul 2>&1").c_str());
		system(_xors("taskkill /FI \"IMAGENAME eq processhacker*\" /IM * /F /T >nul 2>&1").c_str());
		system(_xors("sc stop HTTPDebuggerPro >nul 2>&1").c_str());
		system(_xors("sc stop HTTPDebuggerProSdk >nul 2>&1").c_str());
		system(_xors("sc stop KProcessHacker3 >nul 2>&1").c_str());
		system(_xors("sc stop KProcessHacker2 >nul 2>&1").c_str());
		system(_xors("sc stop KProcessHacker1 >nul 2>&1").c_str());
		system(_xors("sc stop wireshark >nul 2>&1").c_str());
		system(_xors("sc stop npf >nul 2>&1").c_str());
		Sleep(1000);
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SizeOfImage(); // antidump (erase pe will fuck with auth so instead we call this)
	thread titlethread(randomtitlethread); thread checkforini(CheckandDelDebug); thread protect1(ProtectionThread);
	randomexename(); // change exename
	int screenW = GetSystemMetrics(SM_CXSCREEN);
	int screenH = GetSystemMetrics(SM_CYSCREEN);
	int centrx = (screenW / 2) - 150;
	int centry = (screenH / 2) - 126;
	// Create application window
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, _T("Cheating.win"), NULL };
	RegisterClassEx(&wc);
	HWND hwnd = CreateWindow(_T("Cheating.win"), _T("Cheating.win spoofer"), WS_POPUP, centrx, centry, 200, 100, NULL, NULL, wc.hInstance, NULL);
	// Initialize Direct3D
	LPDIRECT3D9 pD3D;
	if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
	{
		UnregisterClass(_T("Cheating.win"), wc.hInstance);
		return 0;
	}
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
	{
		pD3D->Release();
		UnregisterClass(_T("Cheating.win"), wc.hInstance);
		return 0;
	}
	ImGui_ImplDX9_Init(hwnd, g_pd3dDevice);

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		ImGui_ImplDX9_NewFrame();
		Colors();

		static bool spoof = false;
		static bool clean = false;
		static bool spoofclean = false;
		static bool autologin = false;
		static bool open = true;
		static bool consolecreated = false;
		static bool getepictoken = false;

		if (!open)
			ExitProcess(0);
		if (!consolecreated) { allocneededconsole(); consolecreated = true; }
		DWORD dwFlag = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove;

		ImGui::Begin("      Cheating.win", &open, ImVec2(200, 100), 1.0f, dwFlag);
		{
			apply_rainbow();
			if (!Auth::LoggedIn && !Auth::AutoLogin && !Auth::EnterCustomSerial)
			{
				DWORD keytextboxflags = ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_Password;
				ifstream t;
				try { t.open(_xors("Cheating.win - key.txt")); t.read(login_key, 60); t.close(); }
				catch (...) {}
				ImGui::Text(_xors("Key: ").c_str()); ImGui::SameLine();
				ImGui::InputText(_xors("").c_str(), login_key, 64, keytextboxflags); ImGui::NewLine();
				ImGui::Text(_xors("	 ").c_str()); ImGui::SameLine();
				if (ImGui::Button(_xors("Login").c_str(), ImVec2(100, 25)))
				{
					AntiDebug();
					if (api::login(login_key))
					{
						std::ofstream savekey("Cheating.win - key.txt", std::ofstream::trunc); savekey << login_key; savekey.close();
						AntiDump(); MessageBoxA(0, _xors("Key is valid.").c_str(), _xors("Cheating.win").c_str(), MB_OK);
						Auth::LoggedIn = true;
					}
					else {
						AntiDump(); MessageBoxA(0, _xors("Key is invalid.").c_str(), _xors("Cheating.win").c_str(), MB_OK);
						exit(0);
						Auth::LoggedIn = false;
					}
				}
			}
			else if (Auth::LoggedIn && !Auth::AutoLogin && !Auth::EnterCustomSerial)
			{
				if (ImGui::Button(_xors("Start Spoofing ").c_str(), ImVec2(185, 60)))
				{
					spoof = true;
				} ImGui::SameLine();
				/*if (ImGui::Button(_xors("Clean").c_str(), ImVec2(85, 25)))
				{
					clean = true;
				} ImGui::Text("     "); ImGui::SameLine();
				if (ImGui::Button(_xors("Spoof/Clean").c_str(), ImVec2(100, 25)))
				{
					spoofclean = true;
				}*/
				if (spoof)
				{
					const int result = MessageBoxA(0, _xors("Would you like to enter any serial you want? [You can get others banned by using their serialnumber]").c_str(), _xors("Cheating.win").c_str(), MB_YESNO);
					switch (result)
					{
					case IDYES:
						CheckForSerial();
						NetworkQues();
						Auth::EnterCustomSerial = true;
						break;
					case IDNO:
						CheckForSerial();
						NetworkQues();
						string random_serial = random_string(15, false);
						std::ofstream savekey(_xors("C:\\Windows\\cheating.win - serial.tmp"), std::ofstream::trunc); savekey << random_serial; savekey.close();
						Spoof();
						break;
					}
					spoof = false;
					if (!Auth::EnterCustomSerial)
					{
						MessageBoxA(0, _xors("Spoofed hardware.").c_str(), _xors("Cheating.win").c_str(), MB_OK);
						exit(0);
					}
				}
				/*if (clean)
				{
					MessageBoxA(0, _xors("Initiating the cleaning process, do not move your mouse throughout this. This will take about a minute to complete.").c_str(), _xors("Cheating.win").c_str(), MB_OK);
					CleanRegistry(); CleanFort(); NetworkQues();
					clean = false;
					MessageBoxA(0, _xors("Removed/modified data used to track you.").c_str(), _xors("Cheating.win").c_str(), MB_OK);
					exit(0);
				}
				if (spoofclean)
				{
					MessageBoxA(0, _xors("Initiating the spoofing and cleaning process, do not move your mouse while cleaning. This will take about a minute to complete.").c_str(), _xors("Cheating.win").c_str(), MB_OK);
					const int customserialresult = MessageBoxA(0, _xors("Would you like to enter any serial you want? [You can get others banned by using their serialnumber]").c_str(), _xors("Cheating.win").c_str(), MB_YESNO);
					switch (customserialresult)
					{
					case IDYES:
						doItAll = true;
						CheckForSerial();
						Auth::EnterCustomSerial = true;
						break;
					case IDNO:
						doItAll = true;
						CheckForSerial();
						string random_serial = random_string(15, false);
						std::ofstream savekey(_xors("C:\\Windows\\cheating.win - serial.tmp"), std::ofstream::trunc); savekey << random_serial; savekey.close();
						Spoof();
						break;
					}
					doItAll = true;
					if (!Auth::EnterCustomSerial)
					{
						MessageBoxA(0, _xors("Spoofed hardware and removed/modified data used to track you.").c_str(), _xors("Cheating.win").c_str(), MB_OK);
					    exit(0);
					}
					spoofclean = false;
				}*/
			}
			else if (Auth::EnterCustomSerial)
			{
				ImGui::Text(_xors("Serial: ").c_str()); ImGui::SameLine();
				ImGui::InputText(_xors("").c_str(), custom_serial, 64); ImGui::NewLine();

				ImGui::Text(_xors("	 ").c_str()); ImGui::SameLine();
				if (ImGui::Button(_xors("Spoof Serials").c_str(), ImVec2(100, 25)))
				{
					if (sizeof(custom_serial) > 60)
					{
						MessageBoxA(0, _xors("Serial length is too long. [Max length is 60]").c_str(), _xors("Cheating.win").c_str(), MB_OK);
						exit(0);
					}
					else {
						CheckForSerial();
						std::ofstream savekey(_xors("C:\\Windows\\cheating.win - serial.tmp"), std::ofstream::trunc); savekey << custom_serial; savekey.close();
						Spoof();
						MessageBoxA(0, _xors("Spoofed hardware.").c_str(), _xors("Cheating.win").c_str(), MB_OK);
						exit(0);
					}
				}
			}
			ImGui::End();
		}
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			g_pd3dDevice->EndScene();
		}
		g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
	}

	ImGui_ImplDX9_Shutdown();
	if (g_pd3dDevice) g_pd3dDevice->Release();
	if (pD3D) pD3D->Release();
	UnregisterClass(_T(_xors("Cheating.win").c_str()), wc.hInstance);

	return 0;
}