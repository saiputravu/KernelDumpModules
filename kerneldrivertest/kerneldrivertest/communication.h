#pragma once
#include <ntifs.h>
#include <ntddk.h>
#include <wdm.h>

// Define custom control code - so you know which code represents what action
#define IO_GET_CLIENT_ADDRESS CTL_CODE(FILE_DEVICE_UNKNOWN,  \
		0x666, \
		METHOD_BUFFERED, \
		FILE_SPECIAL_ACCESS)

// Handle communication codes
NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);

// Called when communication when communication from userland is terminated
NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP Irp);

// Called when communication is created
NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP Irp);
