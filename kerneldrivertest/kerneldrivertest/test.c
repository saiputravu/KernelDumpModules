#pragma warning (disable : 4100)

#include "test.h"
#include "messages.h"
#include "events.h"
#include "communication.h"

PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING dev, dos;

// ENTRY POINT for the kernel driver
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath) {

    // Kernel drivers take all warnings as errors so you have to add this for any parameters you don't use
	UNREFERENCED_PARAMETER(pRegistryPath);

    // What callback to use on unloading the driver
	pDriverObject->DriverUnload = UnloadDriver;
	DebugMessage("Welcome to first driver!\n");

    // Setting the callback for OnImageLoad
	PsSetLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE)ImageLoadCallback);

	// Initialise driver object paths
	RtlInitUnicodeString(&dev, L"\\Device\\kerneldrivertest");
	RtlInitUnicodeString(&dos, L"\\DosDevices\\kerneldrivertest");

    // Register a /Device/ for this kernel driver
	IoCreateDevice(pDriverObject,
		0,
		&dev,
		FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN,
		FALSE,
		&pDeviceObject);

    // This is super important - kernel device wont be registered properly otherwise 
	IoCreateSymbolicLink(&dos, &dev);

	// Set driver's major func
    // Setting up the IOCTL functionality
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;

    // Flags (use MSDN to understand meaning)
	pDeviceObject->Flags |= DO_DIRECT_IO;
	pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS; 
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject) {
	UNREFERENCED_PARAMETER(pDriverObject);
	DebugMessage("Goodbye!\n");

    // Remove the OnImageLoad callback 
	PsRemoveLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE)ImageLoadCallback);

    // Delete symbolic links and devices (/Device/ , /DosDevices/)
	IoDeleteSymbolicLink(&dos);
	IoDeleteDevice(pDriverObject->DeviceObject);

	return STATUS_SUCCESS;
}
