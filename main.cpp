#include "Game.h"
#include "SmokeTests.h"

#include <iostream>

#ifdef _WIN32
extern "C" __declspec(dllimport) int __stdcall SetConsoleOutputCP(unsigned int);
static constexpr unsigned int CP_UTF8 = 65001;
#endif

/* -----------------------------------------------------------------------
   main: сначала все тесты, затем игровой цикл
   ----------------------------------------------------------------------- */
int main()
{
    SetConsoleOutputCP(CP_UTF8);

    if (!runSmokeTests(std::cout, std::cerr)) return 1;

    Game game;
    game.run();

    return 0;
}
