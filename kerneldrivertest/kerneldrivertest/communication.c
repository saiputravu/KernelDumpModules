#include "communication.h"
#include "messages.h"

/*
 * For this section, it is really recommended for anyone trying to understand this code to read up on the I/O manager
 * 
 * https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/overview-of-the-windows-i-o-model
 * https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/windows-kernel-mode-i-o-manager
 */

// On CreateCall
NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	// Caller has completeed all processing for IO Request
	// Returning IRP back to I/O manager
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	DebugMessage("CreateCall was called, connection established!\n");

	return STATUS_SUCCESS;
}

NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	// Caller has completeed all processing for IO Request
	// Returning IRP back to I/O manager
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	DebugMessage("CloseCall was called, connection terminated!\n");

	return STATUS_SUCCESS;
}

NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);

	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	ULONG ByteIo = 0;

    // Each IRP has its own stack implementation
	PIO_STACK_LOCATION Stack = (PIO_STACK_LOCATION)IoGetCurrentIrpStackLocation(Irp);

    // Get the control code the packet requested
	ULONG ControlCode = Stack->Parameters.DeviceIoControl.IoControlCode;

    // Compare for all control codes your kernel driver implements
	if (ControlCode == IO_GET_CLIENT_ADDRESS) {
		PULONG Output = (PULONG)Irp->AssociatedIrp.SystemBuffer; // System space buffer1
		*Output = 0xdeadbeef;
	
		DebugMessage("IO Code IO_GET_CLIENT_ADDRESS requested\n");

		Status = STATUS_SUCCESS;
		ByteIo = sizeof(*Output);
	} else {
		ByteIo = 0;
	}

	Irp->IoStatus.Status = Status;
	Irp->IoStatus.Information = ByteIo; // Num bytes transferred
    
    // Caller has completed all processing for IO Request
    // Returning IRP back to I/O manager
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return Status;
}
