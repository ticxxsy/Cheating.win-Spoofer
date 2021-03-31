#include "..\Misc\Includes.h"
#include "..\Misc\misc.h"
#include "clean.h"

//string xor'ing (skCrypter)
#include "..\..\Protection\xor.hpp"

using namespace std;

void closeEpicWindows()
{
    system(_xors("TASKKILL /F /IM UnrealCEFSubProcess.exe").c_str());
    system(_xors("TASKKILL /F /IM EpicGamesLauncher.exe").c_str());
    system(_xors("TASKKILL /F /IM FortniteClient-Win64-Shipping.exe").c_str());
    system(_xors("TASKKILL /F /IM FortniteLauncher.exe").c_str());
    system(_xors("TASKKILL /F /IM FortniteClient-Win64-Shipping_BE.exe").c_str());
    system(_xors("TASKKILL /F /IM FortniteClient-Win64-Shipping_EAC.exe").c_str());
}





void ResetNetwork()
{
    system(_xors("netsh winsock reset").c_str());
    system(_xors("netsh int ip reset").c_str());
    system(_xors("ipconfig /release").c_str());
    system(_xors("ipconfig /renew").c_str());
    system(_xors("ipconfig /flushdns").c_str());
    system(_xors("netsh interface set interface ""Wi-Fi"" disable").c_str());
    system(_xors("netsh interface set interface ""Ethernet"" disable").c_str());
    system(_xors("netsh interface set interface ""Wi-Fi"" enable").c_str());
    system(_xors("netsh interface set interface ""Ethernet"" enable").c_str());
    system(_xors("wmic path win32_networkadapter where PhysicalAdapter=True call disable").c_str());
    system(_xors("wmic path win32_networkadapter where PhysicalAdapter=True call enable").c_str());
    system(_xors("netsh advfirewall reset").c_str());
}