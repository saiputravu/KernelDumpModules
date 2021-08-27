# KernelDumpModules

This is part of my project that I did at my internship at F-Secure, reversing Valorant's Vanguard Anti-Cheat. This is a result of me learning about kernel development.

## Description
Kernel driver which utilises OnImageLoad callback kernel functionality to dump any selected module loaded to disk. There is also a user-mode application to show interaction with IOCTLs 

To customise this, you probably should change the OnImageLoad callback from matching vgc.exe to something else


## Folders

- Kerneldrivertest   : Kernel driver (Entry in test.c)
- Testkernelusermode : User-mode application  

## Compilation
Open .sln in Visual Studio 2019+. Make sure to have the windows 10 SDK + WDK installed. Press build.

## Signing and running the kernel driver 

Inside cmd.exe as an Administrator type the following. (service\_name and path are to be inputted by you). 

```

sc create <service_name> type= kernel binpath="path/to/driver.sys"
bcdedit /set testisigning on
```

Then reboot. On the bottom left side of your windows desktop, you should see "Test mode"

## Start/Stop kernel service

Start
```
sc start <service_name>
```

Stop
```
sc stop <service_name>
```

## Additional tools required

- DebugView (sysinternal suite) : Allows you to see debug messages from kernel 
