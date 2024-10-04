#include "OpenVag.h"

# if defined(_WIN32)
#     define NOMINMAX
#     define WIN32_LEAN_AND_MEAN
#     include <windows.h>
#     include <stdlib.h> // __argc, argv
# endif

int Main(int argc, char** argv) {

    OpenVag openVag;
    if (openVag.Create())
        openVag.Run();
    return 0;
}

# if defined(_WIN32) && !defined(_CONSOLE)
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    return Main(__argc, __argv);
}
# else
int main(int, char**)
{
    return Main(argc, argv);
}
# endif

