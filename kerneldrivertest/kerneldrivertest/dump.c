#include "dump.h"

/*
 * Undocumented kernel method - MmCopyVirtualMemory
 * 
 * Enables reading user-mode memory from kernel-mode
 *
 * Resources:
 *  UNOFFICIAL DOCUMENTATION (POOR) : 
 *      https://doxygen.reactos.org/db/de3/ntoskrnl_2include_2internal_2mm_8h.html#a28233ccb0b7c68cf58647de71513e565
 *  
 * NTSTATUS ReadMemory(
 *  PEPROCESS srcProc : PEPROCESS structure for target process to read memory from
 *  PVOID srcAddress  : Virtual memory address (VMAddy) from target process to start reading from
 *  PVOID dstAddress  : Pointer to buffer to output data to 
 *  ULONGLONG size    : Count bytes to read 
 * )
 *
 */
NTSTATUS ReadMemory(PEPROCESS srcProc,
    PVOID srcAddress, PVOID dstAddress,
    ULONGLONG size) {
    PSIZE_T transferred;

    // Returns STATUS_SUCCESS or other NTSTATUSes
    return MmCopyVirtualMemory(srcProc, srcAddress,
        PsGetCurrentProcess(), dstAddress,
        size, 
        KernelMode, // Windows global variable I think it dictates real,protected or long mode for x86_64 processor
                    // Don't really need to mess about with this though
        &transferred);
}

/*
 * Utilises Kernel File I/O to dump process from memory to disk
 *
 * NTSTATUS DumpExe (
 *  PIMAGE_INFO ImageInfo : IMAGE_INFO pointer 
 *  HANDLER ProcessId     : Process id of image to dump from
 *  WCHAR *outFilename    : Filepath to output it to (implicitly assumes current path if no path is passed)
 * )
 *
 * Resources :
 *  https://support.microsoft.com/en-us/topic/how-to-open-a-file-from-a-kernel-mode-device-driver-and-how-to-read-from-or-write-to-the-file-45f7de35-cac4-8d83-9520-a7e480ca404d
 *
 *
 * Since this is well documented I won't do many comments on it.
 */
NTSTATUS DumpExe(PIMAGE_INFO ImageInfo, HANDLE ProcessId, WCHAR *outFilename) {
    UNREFERENCED_PARAMETER(ImageInfo);
    UNREFERENCED_PARAMETER(outFilename);

    UNICODE_STRING unicodeFilename;
    OBJECT_ATTRIBUTES fileObjAttrs;
    
    RtlInitUnicodeString(&unicodeFilename, outFilename);
    InitializeObjectAttributes(&fileObjAttrs, &unicodeFilename,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL, NULL);

    HANDLE hFile;
    NTSTATUS status = STATUS_SUCCESS;
    IO_STATUS_BLOCK ioStatusBlock;

    // Don't perform irql at higher levels
    if (KeGetCurrentIrql() != PASSIVE_LEVEL)
        return STATUS_INVALID_DEVICE_STATE;

    status = ZwCreateFile(&hFile,
        GENERIC_WRITE, 
        &fileObjAttrs, &ioStatusBlock, NULL,
        FILE_ATTRIBUTE_NORMAL,
        0,
        FILE_OVERWRITE_IF,
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL, 0);
    if (!NT_SUCCESS(status))
        goto end;

    // Lookup user mode process
    PEPROCESS userModeProc;
    status = PsLookupProcessByProcessId(ProcessId,
        &userModeProc);

    if (!NT_SUCCESS(status))
        goto end;

    // Allocate kernel memory buffer
    const ULONG tag = 'tag1';
    unsigned char* kernelMemoryBuffer = (unsigned char*)ExAllocatePoolWithTag(NonPagedPool, 
        ImageInfo->ImageSize, 
        tag);

    if (!kernelMemoryBuffer) {
        status = STATUS_MEMORY_NOT_ALLOCATED;
        goto end;
    }

    // Read memory from the process
    status = ReadMemory(userModeProc, 
        ImageInfo->ImageBase, 
        (void*)kernelMemoryBuffer, 
        ImageInfo->ImageSize);

    if (!NT_SUCCESS(status))
        goto free_then_end;

    // Debug output 
    WCHAR output[256] = L"";
    WCHAR format[] = L"%s %2x";
    for (int i = 0; i < 4; ++i)
        RtlStringCchPrintfW(output, 256, format, output, kernelMemoryBuffer[i] & 0xff);
    DebugMessage("\t\tHeader: [%ls]\n", output);

    status = ZwWriteFile(hFile, NULL, NULL, NULL, &ioStatusBlock,
        kernelMemoryBuffer, (ULONG)ImageInfo->ImageSize, NULL, NULL);
    if (!NT_SUCCESS(status)) 
        goto free_then_end;

free_then_end:
    // Free the kernel memory buffer
    ExFreePoolWithTag(kernelMemoryBuffer, tag);
end:
    ZwClose(hFile);
    return STATUS_SUCCESS;
}
