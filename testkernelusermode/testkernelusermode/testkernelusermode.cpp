#include <iostream>
#include "kernelinterface.h"

int main() {
    // This is pretty straight forward C++

    KernelInterface Driver = KernelInterface("\\\\.\\kerneldrivertest");
    ULONG address = Driver.GetClientAddress();

    std::cout << "Got address: 0x" << std::hex << address << std::endl;

    return 0;
}
