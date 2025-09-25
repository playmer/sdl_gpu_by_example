---
title: Resources
description: A place to go with links to books, sites, blogs, and communities built around learning Graphics Programming.
template: post_template.html
---

## Additional Resources

### Blogs/Documentation

 - [SDL_GPU Documentation](https://wiki.libsdl.org/SDL3/CategoryGPU) 
 - [The Graphics Programming Discord Website](https://graphics-programming.org/blog)
    - They've got links to both their own content and interesting Graphics Blogs!
 - [SDL GPU API Concepts: Data Transfer and Cycling](https://moonside.games/posts/sdl-gpu-concepts-cycling/)
    - Written by Evan Hemsley, this article explains the GPU API concept called cycling. We'll get there eventually, but this is written from the source, highly recommended! 
 - [SDL GPU API Concepts: Sprite Batcher](https://moonside.games/posts/sdl-gpu-sprite-batcher/)
    - Written by Evan Hemsley as his answer to folks who want shaders but are coming from the [SDL_Renderer](https://wiki.libsdl.org/SDL3/CategoryRender). Fantastic article on the design of a simple sprite batcher to solve the main things people use the built-in renderer for. We'll try to build up to examples like this, but we likely won't speak about the design of a pipeline at this length.

### Repositories

 - [SDL_gpu_examples repo](https://github.com/TheSpydog/SDL_gpu_examples/)

### Books

Whenever I've needed to relearn some of the base operations that we do in the tutorials so far, I've gone and read through the relevant sections from the Foundation of Game Engine Development books. They've been a fantastic resource for me, and they inspired the sections on covering the infinite projection matrix as well as using reverse depth for your projection matrixes/depth buffer for greater precision far away from the camera. Both techniques I had heard of, but never implemented or used myself. If you're interested in the deeper math behind how all of this works, or even just want to skim for things like the formulas and code snippets, I can't recommend them enough.

I'm not sure if we'll cover the topics, but towards the end of Volume 1, it touches on Grassman Algebra. I won't pretend to understand it myself, but it's a more natural way to get to the same end state as Quaternions. At time of writing I've not decided if I want to cover Rotors, which is the primary part of the section we'd be interested in, and have never studied them previously. If we do end up covering them, or you're interesting yourself, make sure to take a look at the end of Volume 1 here. If you'd like to go _very_ in-depth, that's where I'd recommend the Projective Geometric Algebra Illuminated book. Not something I've looked at myself yet, but for the more mathy among you, it may be of interest! 

I'll try to add some good online resources as well, it's not as though I've _never_ looked at any, but I wanted to mention at least the first 2 books which have been a one stop shop for re-learning math I've not done in about 8 years.

 - Eric Lengyels Books:
   - [Foundations of Game Engine Development, Volume 1: Mathematics](https://www.amazon.com/dp/0985811749)
   - [Foundations of Game Engine Development, Volume 2: Rendering](https://www.amazon.com/dp/0985811757)
   - [Projective Geometric Algebra Illuminated](https://www.amazon.com/dp/B0CXY8C72T)

## Get help:
 - [SDL Discord](https://discord.gg/BwpFGBWsv8)
    - We can help with both SDL and SDL_GPU questions here. Though don't expect to get incredibly high end 3D graphics advice here. We're still building out that section of the community now that we have SDL_GPU.
 - [SDL Forum](https://discourse.libsdl.org/)
 - [The Graphics Programming Discord](https://discord.graphics-programming.org/)
    - There's even an SDL_GPU channel! These folks are probably where you want to go to with the more serious Graphics questions. 