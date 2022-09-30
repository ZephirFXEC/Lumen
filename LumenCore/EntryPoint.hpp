//
// Created by enzoc on 30/09/2022.
//

#ifndef LUMEN_ENTRYPOINT_HPP
#define LUMEN_ENTRYPOINT_HPP

extern Lumen::Application *Lumen::CreateApplication(int argc, char **argv);

bool g_ApplicationRunning = true;

namespace Lumen {

    int Main(int argc, char **argv) {
        while (g_ApplicationRunning) {
            Lumen::Application *app = Lumen::CreateApplication(argc, argv);
            app->Run();
            delete app;
        }

        return 0;
    }

}

#ifdef WL_DIST

#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    return Lumen::Main(__argc, __argv);
}

#else

int main(int argc, char **argv) {
    return Lumen::Main(argc, argv);
}

#endif // WL_DIST

#endif //LUMEN_ENTRYPOINT_HPP