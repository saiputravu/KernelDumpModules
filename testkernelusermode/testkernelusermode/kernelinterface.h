#pragma once

#include "communication.h"

class KernelInterface {
public: 
	HANDLE hDriver;

    // Constructor
	KernelInterface(LPCSTR RegistryPath) {

        // Set attribute to be an open file descriptor to the kernel driver /Device/
		hDriver = CreateFileA(RegistryPath, 
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			0, 
			OPEN_EXISTING, 
			0, 0);

	}

	DWORD GetClientAddress() {
		if (hDriver == INVALID_HANDLE_VALUE)
			return -1;

		ULONG Address;
		DWORD Bytes;

        // Send control code 
		if (DeviceIoControl(hDriver,
			IO_GET_CLIENT_ADDRESS,
			&Address, sizeof(Address),
			&Address, sizeof(Address),
			&Bytes, NULL)) {
            // Expect return value in address buffer
            // If you look at kernel driver code this should be 0xdeadbeef
			return Address;
		}
		return 0;
	}

};
