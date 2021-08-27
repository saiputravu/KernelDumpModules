#pragma once

#include <Windows.h>

// Custom defined control code - same as the number defined for the action you want in kernel driver
#define IO_GET_CLIENT_ADDRESS CTL_CODE(FILE_DEVICE_UNKNOWN,  \
		0x666, \
		METHOD_BUFFERED, \
		FILE_SPECIAL_ACCESS)
