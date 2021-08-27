#include "events.h"
#include "messages.h"

/*
 *
 * Resources:
 *  https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nc-ntddk-pload_image_notify_routine
 *
 */
PLOAD_IMAGE_NOTIFY_ROUTINE ImageLoadCallback(PUNICODE_STRING FullImageName,
	HANDLE ProcessId,
	PIMAGE_INFO ImageInfo) {
	UNREFERENCED_PARAMETER(ProcessId);
	UNREFERENCED_PARAMETER(ImageInfo);
    
    // Check for vgc.exe in the image that has been loaded
	if (wcsstr(FullImageName->Buffer, L"vgc.exe")) {

        // Standard ImageInfo structure variables 
		PVOID baseAddress = ImageInfo->ImageBase;
		HANDLE hProc = ProcessId;
		SIZE_T imageSize = ImageInfo->ImageSize;

        // Output of the filename
		WCHAR outFilename[256];
		size_t outFilenameMaxSize = 256;

        // Format it so that it has image name, PID, base address 
		LPCWSTR pszFormat = L"\\DosDevices\\C:\\Users\\User\\Desktop\\%s_%d_%llx.exe.donotrun";
        // Use printf formating (same as snprintf)
		NTSTATUS status =
			RtlStringCchPrintfW(outFilename, outFilenameMaxSize, pszFormat, 
				L"vgc.exe", hProc, baseAddress);
		
		DebugMessage("ImageLoaded: %ls \n", FullImageName->Buffer);
		DebugMessage("\t\tProcessId: %lld\n", hProc);
		DebugMessage("\t\tBaseAddress: 0x%llx\n", baseAddress);
		DebugMessage("\t\tImageSize: 0x%llx\n", imageSize);

        // Dump image by calling DumpExe (check dump.c)
		if (NT_SUCCESS(status)) {
			DebugMessage("\t\tDumping image to: %ls\n", outFilename);
			NTSTATUS stat = DumpExe(ImageInfo, hProc, outFilename);
			if (!NT_SUCCESS(stat))
				DebugMessage("\t\t[!] Error dumping image\n");
		}
		else {
			DebugMessage("\t\tUnable to dump image to: %ls\n", outFilename);
			return (PLOAD_IMAGE_NOTIFY_ROUTINE)status;
		}
	}

	return (PLOAD_IMAGE_NOTIFY_ROUTINE)STATUS_SUCCESS;
}
