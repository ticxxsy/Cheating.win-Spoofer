#include "Includes.h"
#include "misc.h"

//string xor'ing (skCrypter)
#include "..\..\Protection\xor.hpp"

using namespace std;

// --------------------------------------------------------------------- Start of colors
#define FOREGROUND_WHITE		(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN)
#define FOREGROUND_YELLOW       	(FOREGROUND_RED | FOREGROUND_GREEN)
#define FOREGROUND_CYAN		        (FOREGROUND_BLUE | FOREGROUND_GREEN)
#define FOREGROUND_MAGENTA	        (FOREGROUND_RED | FOREGROUND_BLUE)
#define FOREGROUND_BLACK		0

#define FOREGROUND_INTENSE_RED		(FOREGROUND_RED | FOREGROUND_INTENSITY)
#define FOREGROUND_INTENSE_GREEN	(FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define FOREGROUND_INTENSE_BLUE		(FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define FOREGROUND_INTENSE_WHITE	(FOREGROUND_WHITE | FOREGROUND_INTENSITY)
#define FOREGROUND_INTENSE_YELLOW	(FOREGROUND_YELLOW | FOREGROUND_INTENSITY)
#define FOREGROUND_INTENSE_CYAN		(FOREGROUND_CYAN | FOREGROUND_INTENSITY)
#define FOREGROUND_INTENSE_MAGENTA	(FOREGROUND_MAGENTA | FOREGROUND_INTENSITY)

#define BACKGROUND_WHITE		(BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN)
#define BACKGROUND_YELLOW	        (BACKGROUND_RED | BACKGROUND_GREEN)
#define BACKGROUND_CYAN		        (BACKGROUND_BLUE | BACKGROUND_GREEN)
#define BACKGROUND_MAGENTA	        (BACKGROUND_RED | BACKGROUND_BLUE)
#define BACKGROUND_BLACK		0

#define BACKGROUND_INTENSE_RED		(BACKGROUND_RED | BACKGROUND_INTENSITY)
#define BACKGROUND_INTENSE_GREEN	(BACKGROUND_GREEN | BACKGROUND_INTENSITY)
#define BACKGROUND_INTENSE_BLUE		(BACKGROUND_BLUE | BACKGROUND_INTENSITY)
#define BACKGROUND_INTENSE_WHITE	(BACKGROUND_WHITE | BACKGROUND_INTENSITY)
#define BACKGROUND_INTENSE_YELLOW	(BACKGROUND_YELLOW | BACKGROUND_INTENSITY)
#define BACKGROUND_INTENSE_CYAN		(BACKGROUND_CYAN | BACKGROUND_INTENSITY)
#define BACKGROUND_INTENSE_MAGENTA	(BACKGROUND_MAGENTA | BACKGROUND_INTENSITY)

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

void White()
{
	SetConsoleTextAttribute(h, FOREGROUND_INTENSE_WHITE);
}
void Cyan()
{
	SetConsoleTextAttribute(h, FOREGROUND_CYAN);
}

void Green()
{
	SetConsoleTextAttribute(h, FOREGROUND_INTENSE_GREEN);
}

void Red()
{
	SetConsoleTextAttribute(h, FOREGROUND_RED);
}

void Purple()
{
	SetConsoleTextAttribute(h, FOREGROUND_MAGENTA);
}

void Intensity()
{
	SetConsoleTextAttribute(h, FOREGROUND_INTENSITY);
}
// ---------------------------------------------------------------------------------------- End of Colors

void time()
{
    time_t now = time(0);
    tm* ltm = localtime(&now); Purple();
    cout << _xors(" [");
    cout << ltm->tm_hour << _xors(":");
    cout << ltm->tm_min << _xors(":");
    cout << ltm->tm_sec; Purple();
    cout << _xors("] ");
}

bool FileExists(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}