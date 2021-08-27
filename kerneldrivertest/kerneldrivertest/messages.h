#pragma once

// Printing kernel debug messages (readable with DebugView sysinternal suite tool)
#define DebugMessage(str, ...) DbgPrintEx(0, 0, str, __VA_ARGS__)
