//
// Created by enzoc on 30/09/2022.
//

#ifndef LUMEN_ENTRYPOINT_HPP
#define LUMEN_ENTRYPOINT_HPP


bool g_ApplicationRunning = true;

namespace Lumen {

    static auto Main(int argc, char **argv) -> int {
        while (g_ApplicationRunning) {
            Lumen::Application *app = Lumen::CreateApplication(argc, argv);
            app->Run();
            delete app;
        }
        return 0;
    }

}// namespace Lumen

#ifdef WL_DIST

#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
  return Lumen::Main(__argc, __argv);
}

#else

auto main(int argc, char **argv) -> int { return Lumen::Main(argc, argv); }

#endif// WL_DIST

#endif// LUMEN_ENTRYPOINT_HPP