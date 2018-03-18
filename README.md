# bee
a bee.

## Runtime
Contains a relatively **safe** runtime, which can:
* produce log with timestamp and file/function utils.
* generate dump file (containing callstack info) automatically when the application craches. 
* provide access to your custom log.

### Capturing exceptions
Any exception derived from **`bee::exception::Fatal`** will provide a call stack info. Other exception is also allowed and both of them provides a dump call when crashes.

you may call bee::Runtime::onDump() to set your dump callback.

Capturing signals are also allowed, it won't provide call stack info (currently SEH exceptions, the linux version is under construction) e.g. `Integer div by zero`, `Segment fault` ...

### Producing dump files
If any exception is captured, the dump file will be produced in `log/` dir (by default, you can modify it). The file may seem like this: 
```
**bee** built Jan 28 2018 01:01:55

[2018-01-28 01-03-57] runtime.cpp:74::test(): this is a custom log
[2018-01-28 01-03-58] <Fatal> runtime.cpp:75::test()	

[C:\WINDOWS\System32\KERNELBASE.dll:0x7fffbcc83fb8] RaiseException (No symbols)
[C:\Users\QwQ\Desktop\bee\build\bee.exe:0x403197] runtimeBase.cpp:68::_ZN3bee9exception5FatalC2ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEES7_()
[C:\Users\QwQ\Desktop\bee\build\bee.exe:0x4025b3] runtime.cpp:75::_ZN3bee7Runtime4testEv()
[C:\Users\QwQ\Desktop\bee\build\bee.exe:0x4046aa] testRuntime.cpp:20::main()
[C:\User\QwQ\Desktop\bee\build\bee.exe:0x4013f8] crtexe.c 
[C:\Users\QwQ\Desktop\bee\build\bee.exe:0x40151b] crtexe.c 
[C:\WINDOWS\System32\KERNEL32.DLL:0x7fffbe961fe4] BaseThreadInitThunk (No symbols)
[C:\WINDOWS\SYSTEM32\ntdll.dll:0x7fffc085efb1] RtlUserThreadStart (No symbols)
```

you can check these features in `runtimeBase.h`.

## OpenGL Encapsulation

See the classes hierachy.

## Shader Controllers

In order to implement a shader controller system, it is necessary to add class metadata support. In order to derive from any ShaderController Class, use some macros to add metadata. See `property.h` && `shaderController.h`.

Shader controller class aims at transmitting data from CPU to GPU(shaders). 

It is a great flexibility that **unknown variables will be ignored rather than prompting an error**.

Whenever you add a property, the getter and setter are automatically generated, and the data member will be protected.

## Lighting & Modeling

![](doc/img/phong-chess.png)

A chess demo for the Phong lighting model.

