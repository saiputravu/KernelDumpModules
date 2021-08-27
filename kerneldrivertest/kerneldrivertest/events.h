#pragma once

#include <ntifs.h>
#include <ntddk.h>
#include <ntstrsafe.h>
#include "dump.h"

PLOAD_IMAGE_NOTIFY_ROUTINE ImageLoadCallback(PUNICODE_STRING FullImageName, 
	HANDLE ProcessId,
	PIMAGE_INFO ImageInfo);

