#include "file_selector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32

char *select_file() {
    OPENFILENAME ofn;
    char szFile[MAX_PATH] = {0};

    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Binary Files\0*.bin\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileName(&ofn)) {
        char *selected_file = strdup(szFile);
        if (!selected_file) {
            fprintf(stderr, "Memory allocation failed!\n");
            return NULL;
        }
        return selected_file;
    } else {
        fprintf(stderr, "File selection was canceled or an error occurred.\n");
        return NULL;
    }
}

#else

char* select_file() {
fprintf(stderr, "The file selection function is only supported on the Windows platform.\n");
    return NULL;
}

#endif
