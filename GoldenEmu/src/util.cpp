#include "util.h"

#include <Windows.h>

std::string Util::GetUserInputFile()
{
    char szFileNameIN[MAX_PATH]{};
    char szFileNameOUT[MAX_PATH]{};

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Any File\0*.*\0";
    ofn.lpstrFile = szFileNameOUT;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFileTitle = szFileNameIN;
    ofn.nMaxFileTitle = MAX_PATH;
    ofn.lpstrTitle = "Select a ROM to load";
    ofn.Flags = OFN_FILEMUSTEXIST;
    if (GetOpenFileNameA(LPOPENFILENAMEA(&ofn)))
    {
        return szFileNameOUT;
    }
    return "";
}
