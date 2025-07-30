---
title: Triangle and a "Fullscreen Triangle"
description: Finally you can see a triangle, and then a demonstration on why you might actually use a single triangle later on in your Graphics journey. Will cover the creation of Vertex and Pixel shaders, a simple demonstration of "Vertex Pulling", and then an extension of the triangle to cover the whole screen with a shader effect.
---


# Triangle and a "Fullscreen Triangle"

## Intro 
The big one, it's time to draw a Triangle. And after that, we're going to learn a trick on how to extend this triangle to display over the entire screen. This technique is a small optimization that gets used when you start learning fullscreen effects. We'll be using it in the next chapter to play around with pixel shaders to get a little more comfortable with writing code on the GPU.

I figured it'd be nice to go over these together, because realistically, it's not common that you need to draw a single triangle on the screen. Maybe a Quad, two triangles making the shape of a Rectangle or Square, but that requires textures, transfer buffers, and bindings. That said, if you adjust the positions of that triangle? Suddenly it has a very practical purpose! And it's something we can learn about with only a few additional function calls without needing to get into everything needed to display a texture. Of course, don't worry, we'll get there. But let's build up step by step and try to learn something with each step!

## Vertices

## Pixels (and Fragments)


## Shaders, Pipelines, and Render Passes

When we look at how we do real-time rendering today, it's fundamentally a combination of generally tweakable fixed stages and programmable stages. We got a taste of this fixed functionality when we set-up and executed a RenderPass in the previous chapter. We were able to clear the screen just by tweaking the initialization and target of the RenderPass, with no need to submit geometry to proceed through the rest of the stages.

Insert render pipeline diagram:
Input Assembler -> Vertex Shader -> Tesselation Shader -> Geometry Shader -> Rasterization -> Fragment Shader (Aka Pixel Shaders)

SDL GPU doesn't give us access to every programmable stage, many aren't relevant or performant today and some are too new to have been added for compatability reasons. What it does give us access to are the classics, Vertex Shaders and Fragment Shaders. It also has Compute Shaders to boot, but they're not part of the Render Pipeline, though we'll find they're still very useful for rendering later on. With regards to our concerns here, we'll write Vertex Shaders to output individual vertices, and Fragment Shaders to output colors.

> Note: If you've heard of Mesh Shaders, they effectively replace all of the geometry related stages. What we do in Vertex shaders would likely be sort of the base functionality of learning Mesh Shaders. That said, they're designed to be smarter around data management, letting you implement techniques that we'll have to split between Compute and Vertex shaders when we get there.

So if you're unfamiliar, when I say "programmable" I mean it literally. Shaders are programs that execute on your GPU. Each shader stage tends to have slightly different requirements, you can think about it like function signatures albeit it's not a perfect analogy. Each Graphics API takes a different "native" shader input, Vulkan takes SPIR-V, DirectX takes DXIL, and Metal takes MSL. Technically when you get into extensions or development only situations, there's slightly more flexibility than that, but for our purposes this is good enough to think about. Each of these are an intermediate representation of your program, lying somewhere between the text you wrote in the language you choose, and machine code that can run on your GPU. When implementing the respective Graphics API

There's many languages specifically for shaders, and even general purpose programming languages that can be compiled to 