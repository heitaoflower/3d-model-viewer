#include "ViewerApp.hpp"
#include "defines.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <unistd.h>
#include <libgen.h>


int main(int argc, char *argv[])
{
    char *dir = dirname(argv[0]);

    if (chdir(dir) != 0)
    {
        Log::Error("Chyba při změně pracovního adresáře.");
        return 1;
    }

    ViewerApp app;
    app.Run((argc > 1) ? argv[1] : std::string());
    return 0;
}
