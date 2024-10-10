#include "OpenVag.h"

int main(int, char**)
{
    OpenVag openVag;
    if (openVag.Create())
        openVag.Run();
}