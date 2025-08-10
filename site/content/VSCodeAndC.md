---
title: VSCode and using it for C and C++ Development
description: A place to go with links to books, sites, blogs, and communities built around learning Graphics Programming.
template: post_template.html
---

A lot of folks early into their use of C or C++ will decide they want to use Visual Studio Code as a development environment. Is is achievable, and you can potentially get really good results doing so, but depending on how you do it, it can be incredibly easy to break. And the thing is, there's probably dozens of permuations here, and everyone tries something different, and they run into different but similar problems.

Consider this a living document in my trying to tell folks how they can configure VSCode, depending on their situation. If you run into problems, please contact me.


## Problem Statment

> "I want to compile my code, get autocomplete, and launch the debugger from VSCode"

These are three distinct problems, and while there are _some_ unified solutions (which we'll discuss), often folks are not using one of those, and failing to configure one or more of these correctly.

First we need to discuss what VSCode actually is and how it works. At it's most simple, VSCode is a text editor. That said, you can turn it into a full blown IDE for many languages with the right extensions and configurations. With C and C++, you're _generally_ relying on tools that will allow you to reduce some of this configuration, but without them you've got a lot of work to do:
 - [tasks.json](https://code.visualstudio.com/docs/debugtest/tasks)
   - This works out of the box and is the most basic thing VSCode can do. This is how you'll build your code, although you can create tasks for a variety of tasks. You'll be giving VSCode a command to execute, which includes everything you'd need if you were in your terminal. Environment variables, path to the compiler if it's not already in your PATH, the working directory where you want to exectue it, and most important for us: all the compiler flags you need. Include and Link directories, libraries to link to, Defines, so on and so forth. You can run the compiler directly, or you can write a makefile, script, or whatever to run. We'll try to address these all later.
 - [launch.json](https://code.visualstudio.com/docs/debugtest/debugging-configuration)
   - If you want to debug, you'll need both a debugger, and an extension to make use of it. This file is how you'll tell VSCode to launch that debugger. It's similar to the above in that you'll need to ensure VSCode (and the extension you're using) knows how to find your debugger.
 - [c_cpp_properties.json](https://code.visualstudio.com/docs/cpp/customize-cpp-settings)
   - To get autocomplete, you'll need an extension, if you choose the C/C++ one (discussed below), this is how you configure it. This requires duplicating basically all of the stuff in your `tasks.json` file regarding compiling your files.
 - [compile_commands.json/compile_flags.txt](https://clang.llvm.org/docs/JSONCompilationDatabase.html)
   - On the other hand, if you use clangd for autocomplete, you'll need to configure one of these files. Generally you'll want to use something such as CMake to write `compile_commands.json` for you, as you need to explicitly tell it the compile command for every file you're compiling. If your project is simple (every file you compile use the same flags) you can use `compile_flags.txt`. And again, this is duplicated work of whats in your tasks.json file.

With all that said, lets talk about the different options you'll need to decide on. Perhaps you're coming to this guide with those decisions made, if so please skip the Permutations section and move on to the appropriate configuration sections.

## The Permutations

### Platform Toolchains

This guide is almost exclusively focussed on helping beginners understand how this works and getting them up and running on Desktop platforms. It won't be covering cross compiling or targeting things like phones. If you're relying on this guide, then you really shouldn't start by running on hardware different than your Host.

With that said, you need to choose a compiler toolchain before proceeding.

#### Windows

 - MSVC (Visual Studio)
 - clang for Windows (Visual Studio/LLVM)
 - The many mingw offshoots
   - [gcc (mingw-w64 MSYS2)](https://www.mingw-w64.org/getting-started/msys2/)
   - [clang (mingw-w64 MSYS2)](https://www.mingw-w64.org/getting-started/msys2-llvm/)
   - [gcc (w64devkit)](https://github.com/skeeto/w64devkit)
   - [gcc (mingw)](https://sourceforge.net/projects/mingw/)


#### MacOS

 - AppleClang (Xcode)
 - gcc/clang (Homebrew)

Like Windows, the preferred toolchain here should be the publishers own, Xcode. It's a firmer recommendation here, as it's going to be much more difficult to ship software you build to other users portably with toolchains provided by homebrew. That said, it should be noted that it's difficult even with Xcode, and as a beginner it's entirely likely you won't be shipping software for awhile. Feel free to choose whichever.

#### Linux

 - gcc
 - clang

Generally speaking, there's actually many toolchains possible, including cross compilers like mingw that targets Windows, but as we're just talking about running on your host platform, we'll assume you're going to just install your typical toolchain and preferred compiler from your package manager. Most Linux distros prefer gcc with clang as an alternative compiler, but there are some that have this reversed. I'll assume you can comfortably compile a normal "hello world" style sample from the command line here just by invoking `gcc` or `clang` as-is.


### Debugging

 - [C/C++ by Microsoft](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
   - This extension can speak to just about any debugger, and is generally pretty stable with all of them, but given it's by Microsoft, I'm sure the .
 - [CodeLLDB by Vadim Chugunov](https://marketplace.visualstudio.com/items?itemName=vadimcn.vscode-lldb)
   - This extension exclusively uses the LLDB debugger provided by LLVM. This has the downside of having worse support for MSVC and Clang for Windows based binaries, as LLDB has less support for their Debug Information Format (PDB) in comparison to the Dwarf format the mingw and other platforms use. That said, for the most part it should still work there, you can read their docs on [Windows support here.](https://github.com/vadimcn/codelldb/wiki/Windows)

### Autocomplete/Intellisense Providers

 - [C/C++ extension by Microsoft](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
 - [clangd extension by LLVM](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd)

There's two main solutions for autocomplete/Intellisense. The  and the . 

These are pretty much one or the other, if you have them both installed and enabled, clangd will complain endlessly until you turn the C/C++ one off. That said, you might want or need the C/C++ extension for debugging, thankfully if you want to use clangd for autocomplete, you can still use the C/C++ extension for debugging by setting the following: `C_Cpp.intelliSenseEngine="Disabled"`.

Before writing this guide I've only ever used the C/C++ extension, which worked well enough for me, but for completions sake I wanted to account for clangd, as I've heard that it's quite good.




## Accessing your Tools

This is less of an issue on MacOS and Linux, but we _must_ discuss the PATH environment variable on Windows.

If you're unfamiliar, the `PATH` environment variable is used for command lookup. Essentially when you're in a shell typing in a program name, or a program is attempting to launch another program, they have to be able to find what's being invoked. `PATH` is a semicolon delimited list of paths, for which the various OS APIs for launching processes will search for the programs requested.

## Configuring Separately

### Compiling

#### tasks.json

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "type": "cppbuild",
            "label": "C/C++: g++ build active file",
            "command": "w64devkit\\bin\\gcc",
            "args": [
                "-g",
                "${workspaceFolder}\\src\\*.c",
                "-o",
                "${workspaceFolder}\\bin\\hello.exe",
                "-I",
                "SDL3-3.2.20\\x86_64-w64-mingw32\\include",
                "-L",
                "SDL3-3.2.20\\x86_64-w64-mingw32\\lib",
                "-lSDL3",
            ],
            "options": {
                "env": {
                    "PATH": "w64devkit\\bin;${env:PATH}"
                },
                "cwd": "${workspaceFolder}",
            },
            "problemMatcher": [
                "$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "detail": "compiler: ${workspaceFolder}/w64devkit/bin/gcc"
        }
    ]
}
```


### Debugger

#### C/C++

```json
{
    "configurations": [{
        "name": "C/C++: gcc.exe build and debug active file",
        "type": "cppdbg",
        "request": "launch",
        "program": "${workspaceFolder}/bin/hello.exe",
        "args": [],
        "stopAtEntry": false,
        "cwd": "${fileDirname}",
        "environment": [],
        "externalConsole": false,
        "MIMode": "gdb",
        "miDebuggerPath": "${workspaceFolder}/w64devkit/bin/gdb.exe",
        "setupCommands": [{
            "description": "Enable pretty-printing for gdb",
            "text": "-enable-pretty-printing",
            "ignoreFailures": true
        },]
    }],
    "preLaunchTask": "C/C++: gcc.exe build active file"
}
```

#### CodeLLDB

TBD


### Autocomplete/Intellisense Providers


#### C/C++

```json
{
  "configurations": [
    {
      "name": "Win32",
      "includePath": ["SDL3-3.2.20\\x86_64-w64-mingw32\\include"],
      "defines": ["_DEBUG", "UNICODE", "_UNICODE"],
      "windowsSdkVersion": "10.0.22621.0",
      "cStandard": "c17",
      "cppStandard": "c++17",
      "intelliSenseMode": "${default}",
      "compilerPath": "${workspaceFolder}\\w64devkit\\bin\\gcc.exe"
    }
  ],
  "version": 4
}
```