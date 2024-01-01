#include <Windows.h>

#include <filesystem>

#include "Sample.h"

static inline constexpr DWORD bufferSize = 1024;
std::string pathToCurrentModule;

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved)  // reserved
{
    // Perform actions based on the reason for calling.
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // Initialize once for each new process.
        // Return FALSE to fail DLL load.

        pathToCurrentModule.resize(bufferSize);

        pathToCurrentModule.resize(GetModuleFileNameA(hinstDLL, pathToCurrentModule.data(), bufferSize));

        pathToCurrentModule = std::filesystem::path(pathToCurrentModule).parent_path().string();

        break;

    case DLL_THREAD_ATTACH:
        // Do thread-specific initialization.
        break;

    case DLL_THREAD_DETACH:
        // Do thread-specific cleanup.
        break;

    case DLL_PROCESS_DETACH:
        if (lpvReserved != nullptr)
        {
            break; // do not do cleanup if process termination scenario
        }

        // Perform any necessary cleanup.
        break;
    }

    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
