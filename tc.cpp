#include <dlfcn.h>
#include <iostream>

void printCallerFunctionNames() {
    const int maxStackSize = 10;  // Maximum stack trace size
    void* callStack[maxStackSize];
    int stackSize = backtrace(callStack, maxStackSize);

    if (stackSize > 1) {
        Dl_info dlInfo;
        for (int i = 1; i < stackSize; ++i) {
            if (dladdr(callStack[i], &dlInfo) != 0) {
                std::cout << "Caller function " << (stackSize - i) << ": " << dlInfo.dli_sname << std::endl;

                // Check if the address is within the loaded shared library
                if (dlInfo.dli_fbase == libraryHandle) {
                    // Calculate the function offset from the base address
                    uintptr_t functionOffset = reinterpret_cast<uintptr_t>(callStack[i]) - reinterpret_cast<uintptr_t>(dlInfo.dli_saddr);

                    // Iterate over the symbols in the shared library
                    Dl_info symbolDlInfo;
                    for (int j = 0; dladdr(dlInfo.dli_fbase + functionOffset - j, &symbolDlInfo) != 0; ++j) {
                        if (symbolDlInfo.dli_saddr == dlInfo.dli_saddr) {
                            std::cout << "Caller function name: " << symbolDlInfo.dli_sname << std::endl;
                            break;
                        }
                    }
                }
            } else {
                std::cout << "Failed to obtain caller function name." << std::endl;
            }
        }
    } else {
        std::cout << "No caller function information available." << std::endl;
    }
}

int main() {
    // Load the shared library
    void* libraryHandle = dlopen("mylibrary.so", RTLD_LAZY);
    if (libraryHandle == nullptr) {
        std::cout << "Failed to load the shared library." << std::endl;
        return 1;
    }

    // Look up a function within the library
    void (*libraryFunction)() = reinterpret_cast<void (*)()>(dlsym(libraryHandle, "libraryFunction"));
    if (libraryFunction == nullptr) {
        std::cout << "Failed to find the function within the library." << std::endl;
        dlclose(libraryHandle);
        return 1;
    }

    // Call the function
    libraryFunction();

    // Print the caller function names
    printCallerFunctionNames();

    // Unload the shared library
    dlclose(libraryHandle);

    return 0;
}
