#pragma once

#include <ntifs.h>
#include <ntddk.h>
#include <ntstrsafe.h>
#include <wdm.h>
#include "messages.h"

NTSTATUS NTAPI MmCopyVirtualMemory(PEPROCESS srcProcess, PVOID srcAddress,
	PEPROCESS dstProcess, PVOID dstAddress,
	SIZE_T bufferSize,
	KPROCESSOR_MODE kernelProcessorMode,
	PSIZE_T* returnSize);

NTSTATUS ReadMemory(PEPROCESS srcProc, 
	PVOID srcAddress, PVOID dstAddress, 
	ULONGLONG size);

NTSTATUS DumpExe(PIMAGE_INFO ImageInfo, HANDLE ProcessId, WCHAR *outFilename);

