void printCallerFunctionNames() {
    const int maxStackSize = 10;  // Maximum stack trace size
    void* callStack[maxStackSize];
    int stackSize = backtrace(callStack, maxStackSize);

    if (stackSize > 1) {
        Dl_info dlInfo;
        for (int i = 1; i < stackSize; ++i) {
            if (dladdr(callStack[i], &dlInfo) != 0) {
                std::cout << "Caller function " << (stackSize - i) << ": " << dlInfo.dli_sname << std::endl;
            } else {
                std::cout << "Failed to obtain caller function name." << std::endl;
            }
        }
    } else {
        std::cout << "No caller function information available." << std::endl;
    }
}
