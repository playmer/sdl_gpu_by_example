---
title: Introduction and the Template
description: An introduction to this series, what you should expect, choices that have been made, additional resources, and a demonstration of setting up the template project if you'd prefer that over rolling your own.
---

# Introduction

So if you've come here, you're interested in learning SDL_GPU. As background, I should first say that I myself am not an expert in the topic of Graphics, or in SDL_GPU itself. I've written a few relatively simple renderers for games, imgui projects, and similar dabbling. I've mostly focused on the CPU side of things, and only recently have I been learning about some of the more "modern" approaches to rendering. When [Evan Hemsley](https://moonside.games/pages/about/) (also known as cosmonaut), pricipal designer of SDL_GPU, finishes his graphics book, please go buy it and read it!

## What this tutorial is not, what what it is
So all that being said don't expect scene graphs, PBR models, raytracing, or any of that sort of thing. This isn't a replacement for formal graphics coursework or in-depth learning of everything you might need to know to read technical papers on the above topics. It's intent is to get beginners comfortable with the basics so they can _begin_ to learn some of that outside of a formal setting. I can't give you a complete understanding of Linear Algebra, or the foundational knowledge to understand the rendering equation. 

In the more broad sense, it's also not an SDL3 or C tutorial. Anytime I do something I consider somewhat interesting in SDL3 for a beginner, I'll try to briefly cover it. Those asides however, are not a replacement for the documentation. On the other hand, I will spend no time on C as a language or it's features. It's too difficult to try to combine learning a language, a library, _and_ the basics of Graphics all at the same time. I wouldn't encourage it, and this tutorial would become much too heavy if it also had to cover pointers, arrays, macros, memory management, and the like. Similarly, while I expect the setup portion of this guide to increase in size, ultimately this is not a replacement for understanding your chosen tools, hence the assumption that you have a handle on the basics of Git, CMake, and your C/C++ toolchain.

The goal of this guide is to help you understand the API surface of SDL_GPU. Ideally we get you to the point where you're comfortable reading about techniques written in other APIs and applying them to this API. Most of these APIs do look at least roughly similar. OpenGL is a state machine, but a lot of that state is stuff you'd put into pipelines in SDL_GPU or the other lower level APIs. If you've used Vulkan, DX, or Metal, you'll see echoes of those in SDL_GPU.

## Language Choice
As mentioned about, the CPU side code examples will be written in relatively simple C99 so that both C and C++ programmers can copy and paste and it works in both. For folks who love C, this does mean that I won't be using designated initializers, so apologies for that. Feel free to use them in your own code. It'll assume you know your language of choice and are familiar with the tooling. Regarding shaders, we'll be using HLSL by way of SDL_shadercross. As of writing, it's not stable yet, so things might change a bit, I'll endeavor to keep it up to date.

If you decide to use the template, there will be some short setup involving CMake, git, and some tools we'll download and install down below. I know CMake isn't for everyone, but for the sake of this tutorial I'll assume you're using it, or otherwise have your tooling figured out. I'll try to give you a pretty simple framework to work with in CMake so that you can just copy and paste it, and things will be easy from then on.

When we get there, I'll briefly explain how to use SDL_shadercross to compile shaders, but from then on will assume that you're comfortable with that. The template has this entirely automated on Windows, further work is needed for Mac, and some manual setup will be required from Linux. 

## How to read this Tutorial and some Resources

First off, if you're already comfortable with the basics from another API, and just want to get a handle on SDL_GPU with some additional context beyond the [documentation](https://wiki.libsdl.org/SDL3/CategoryGPU) and [samples repo](https://github.com/TheSpydog/SDL_gpu_examples/), feel free to jump around. If you already know SDL, please skip the sections that cover new-to-beginners SDL calls.

Every major section of a chapter will have some notes at the bottom on API calls with links to the documentation for each of them. This supplements the text, and it's intended that you go look at the documentation yourself as well, even just to get used to looking it over. These notes might also sometimes be highly technical for folks interested in that sort of thing. If you find your eyes glossing over, maybe just skip the note for now and take a look at the documentation.

I cannot cover every aspect of every function hence this documentation reading encouragement. SDL is a living API, and while we likely won't cover them _much_, many of the APIs have alternate functions that take an SDL_PropertiesID for additional tweaking. These properties will evolve with time, so there may be new ones added after time of writing that I can't know about. We'll be using this functionality mostly for creating our own functions for making Graphics resources to ensure they're all named for the sake of debugging in tools like RenderDoc.

### Resources

 - [SDL_GPU Documentation](https://wiki.libsdl.org/SDL3/CategoryGPU) 
 - [SDL_gpu_examples repo](https://github.com/TheSpydog/SDL_gpu_examples/)
 - [SDL Discord](https://discord.gg/BwpFGBWsv8)
    - We can help with both SDL and SDL_GPU questions here. Though don't expect to get incredibly high end 3D graphics advice here. We're still building out that section of the community now that we have SDL_GPU.
 - [SDL Forum](https://discourse.libsdl.org/)
 - [The Graphics Programming Discord](https://discord.graphics-programming.org/)
    - There's even an SDL_GPU channel! These folks are probably where you want to go to with the more serious Graphics questions. 
 - [The Graphics Programming Discord Website](https://graphics-programming.org/)
    - They've got links to both their own content and interesting Graphics Blogs!
 - [SDL GPU API Concepts: Data Transfer and Cycling](https://moonside.games/posts/sdl-gpu-concepts-cycling/)
    - Written by Evan Hemsley, this article explains the GPU API concept called cycling. We'll get there eventually, but this is written from the source, highly recommended! 
 - [SDL GPU API Concepts: Sprite Batcher](https://moonside.games/posts/sdl-gpu-sprite-batcher/)
    - Written by Evan Hemsley as his answer to folks who want shaders but are coming from the [SDL_Renderer](https://wiki.libsdl.org/SDL3/CategoryRender). Fantastic article on the design of a simple sprite batcher to solve the main things people use the built-in renderer for. We'll try to build up to examples like this, but we likely won't speak about the design of a pipeline at this length.

# The Setup

So first, go install some stuff if you haven't already:
 - [git](https://git-scm.com/):
 - [CMake](https://cmake.org/)
 - [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)
    - This is mostly for the Vulkan Validation Layers. SDL_GPU itself does as much checking of your API usage as it can when you've created your GPU Device with the debug flag, but it can't hold a candle to the work people have put into these layers. The errors you get from this will be Vulkan specific, but it should be relatively easy to trace them to your SDL_GPU usage. We'll be wrapping the creation of resources to ensure that we name everything we create to make this much easier.
    - On MacOS this also installs a shim between the native graphics API (Metal) and Vulkan, so that you can use these layers. In practice, you should likely be using the Metal backend of SDL_GPU, but for the sake of learning, we'll generally assume the Vulkan backend.
 - Some C or C++ toolchain
 - [RenderDoc](https://renderdoc.org/)
    - Note that if you're on Linux and using Wayland, you'll need to force X11, as RenderDoc doesn't support Wayland and your application will fail to create a GPU device when launched from it.
    - Also there's _many_ Graphics debugging tools, if you're already comfortable and familiar with them, feel free to use them instead. When debugging comes up though, I will assume you're using RenderDoc.


To make things as simple as possible, I've made a sample repo for you to be able to start working immediately, assuming you have all your tools configured.

```bash
git clone --recurse-submodules -j8  https://github.com/playmer/sdl_gpu_by_example.git
cd sdl_gpu_by_example/code
cmake -B build
cmake --build build
```

You should now have an exceedingly basic SDL program within the bin directory, as well as compiled versions of all our examples. Ideally you should execute it from the root of the repository. This will become more important when we start using assets. The CMake build command should have told you toward the end of execution where it built the executable. For example, with the Visual Studio generator I would execute it like so:
```bash
bin\Debug\sdl_by_example.exe
```

Were I using the make or Ninja generators it would likely look like this:
```bash
bin\sdl_by_example.exe
```

On Linux and MacOS, the executables wouldn't have the `.exe` extension.

Once executed you should see that it prints "Everything is working." when executed. If you don't see that, there should at least be an error that goes like "Couldn't initialize SDL: [Some error reported by SDL]". If you don't see _that_ then there's likely something wrong with accessing the SDL shared library. Possibly an RPATH issue if you're on Linux or similar.

To briefly explain the anatomy of the repo you've cloned and built:
 - The top level contains a CMakeLists.txt file that tells CMake how to build our project. This is also where we expect to run most of our CMake commands and execute our programs. 
    - The CMake Scripts are fairly well annotated, intended for you to read them example by example, line by line with explanations whenever we introduce a new concept, property, or technique. At time of writing, the helper script hasn't yet been annotated, as it starts to get into some of the more difficult techniques.
    - Despite the above, reading them is essentially entirely optional. Later on when you want to seperate some of your code out, you'll want to read up on [`target_sources`](https://cmake.org/cmake/help/latest/command/target_sources.html), but it should be as easy as adding in some files to the template folder and adding the c/cpp (and optionally headers as well, it helps with IDE projects), files to the target_sources call inside of the CMakeLists.txt with the folder.
 - There are folders for:
    - `external` libraries (at time of writing, just SDL itself)
    - `source`, where we'll put the code, Shaders, and CMake scripts we use to build the template you just executed, and the examples seen in later chapters.
        - Most relevant to you at this moment are the contents of `source/01_template`. The intent is for you to use this as a workspace to work through the examples. You can also easily create copies of this folder within the `source` folder with different names, so that you may work through each example, keeping your old code around.If you do that you'll need to make sure to add your folder to the list of [`add_subdirectory`](https://cmake.org/cmake/help/latest/command/add_subdirectory.html) calls inside of `source/CMakeLists.txt`
    - `Assets` where we'll place things like images, models, and compiled shaders.
    - `bin`, a folder for ease of use, this is where we've placed built executables and libraries via some CMake variables.
    - `tools`, a folder where we'll place some binaries of our tools, at time of writing, I only really expect to store SDL_shadercross in here, but you never know.


Finally, lets take a look at one of the most basic SDL programs we can make, which if you chose to follow along with the examples and use the template, was one of the things you compiled up above.

## The Template and SDL Basics
So lets pull up the code we just compiled in the template and take a look at it:

```c
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

int main(int argc, char** argv)
{
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return 1;
  }
  
  SDL_Log("Everything is working.");

  SDL_Quit();
  return 0;
}
```

Now, this isn't a tutorial on SDL itself, but I'll try to briefly cover things as we add them. 

`SDL3/SDL.h` is the canonical way to include SDL in version 3, and we do indeed generally recommend that, because it includes almost everything SDL provides. We can see that all we're really doing is trying to initialize SDL via [`SDL_Init`](https://wiki.libsdl.org/SDL3/SDL_Init) with the Video system in particular active. If it fails, we print out an error and return out, otherwise we print a message to let the user know things are okay. And then finally we call `SDL_Quit` to tear down anything we or SDL has left over.

 > ### A note on SDL_main and the callbacks
 > One of the few headers `SDL3/SDL.h` doesn't include is `SDL3/SDL_main.h`, which is a header only library that implements `int main(int argc, char** argv)` for you. Now you may notice that _we've_ defined that function. Well `SDL_main.h` also provides a macro that turns `main` into [`SDL_main`](https://wiki.libsdl.org/SDL3/SDL_main), so that's what we're actually implementing, and what the `main` that SDL implements calls. You can read more about it in SDLs [main functions README](https://wiki.libsdl.org/SDL3/README-main-functions). For simplicities sake, we're not currently using the [callbacks system](https://wiki.libsdl.org/SDL3/README-main-functions#main-callbacks-in-sdl3) provided by SDL_main so as not to confuse folks from non C and C++ languages reading this tutorial, but know that it's highly encouraged for good reason.

Now that we've covered the basics, lets get us a Window and an Event loop.

> ### Covered in this Section
> - [`SDL_Init`](https://wiki.libsdl.org/SDL3/SDL_Init)
>   - Generally the first think you call in an SDL using application. This initializes the subsystems you require of SDL. Not technically required, as SDL will lazily initialize subsystems when you call functions that require them, but it's recommended you call it, as things like events won't come in from some subsystems if they're not initialized.
> - [`SDL_Quit`](https://wiki.libsdl.org/SDL3/SDL_Quit)
>   - Conversely, generally the last thing you call. It lets SDL close out every resource it's internally created and that you've created with it, even if you haven't yet. It's not safe to use SDL resources you've previously created after this call.
> - [`SDL_GetError`](https://wiki.libsdl.org/SDL3/SDL_GetError)
>   - Built in functionality for SDL to retrieve the last error encounter by SDL. Keep in mind that it's _only_ valid to check directly after a call into an SDL function that reported an error. SDL itself sets errors and calls this function internally, so you cannot simply call this and expect the error string it returns to be relevant to you unless SDL told you there was an error.
> - [`SDL_Log`](https://wiki.libsdl.org/SDL3/SDL_Log)
>   - Really just using this as a drop in replacement of printf because `SDL.h` hands it to us, but it's technically more portable for some situations and platforms. Were we in C++, I'd probably consider [`std::format`](https://en.cppreference.com/w/cpp/utility/format/format.html) or the [`fmt`](https://github.com/fmtlib/fmt) library of which it's based due to it's supirior formatting options.
> - [`SDL_main`](https://wiki.libsdl.org/SDL3/SDL_main) and the [main functions](https://wiki.libsdl.org/SDL3/README-main-functions)
>   - Already fairly covered above, but I encourage you again to read the documentation on SDL_main itself and on the main functions.
>   - Regarding the [callbacks system](https://wiki.libsdl.org/SDL3/README-main-functions#main-callbacks-in-sdl3) I'll formulate these tutorials such that it won't be difficult to use them instead. When we're a little further along I'll make a short demonstration of how that would look instead.
