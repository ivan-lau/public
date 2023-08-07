#include <execinfo.h>
#include <iostream>

void printCallerFunctionName() {
    void* callStack[2];  // Adjust the size as needed
    int stackSize = backtrace(callStack, 2);  // Adjust the parameter as needed

    if (stackSize >= 2) {
        // Obtain the symbol name of the previous caller function
        char** symbols = backtrace_symbols(callStack + 1, 1);  // Adjust the index and count as needed

        if (symbols != nullptr) {
            std::cout << "Previous caller function: " << symbols[0] << std::endl;
            std::free(symbols);
        } else {
            std::cout << "Failed to obtain caller function name." << std::endl;
        }
    } else {
        std::cout << "No caller function information available." << std::endl;
    }
}

void foo() {
    printCallerFunctionName();
}

int main() {
    foo();
    return 0;
}
