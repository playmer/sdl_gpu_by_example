---
title: Uniform Buffers (Example Done)
description: More interesting rendering will require learning buffers, we'll begin with the simplest type to use, Uniform Buffers.
template: lesson_template.html
collections: ["lessons"]
---

Almost everything we do in graphics requires data. Sometimes this can be generated or stored in-shader, we did the most simplistic version of this in the previous chapter to do pull-style vertex rendering. We just created some constant arrays and based on the VertexID, we were able to index into those arrays for our Vertex positions and colors. 

However, most of the time when doing rendering, you'll need to pass data from the CPU to the GPU, GPU to the CPU, or GPU to the GPU. When we start drawing entire models for example, we'll need to load that data from a file and put it somewhere the GPU can see and use. You might do compute based-culling, where the compute shader is determining what objects to render before getting to the vertex stage, that'll require the vertex data, the positional/rotation data, and a place for the compute shader to output the information needed to render or not render those objects in the vertex stage.

All that said, we want to ease our way into the concept so lets try Uniform Buffers as they're the easiest to get started with in SDL_GPU. In other APIs they're a bit more similar to Storage Buffers, which we'll also get to. 

## Uniform Buffers

We're going to start off with the example from the previous chapter, as we can demonstrate some simple uses of Uniform Buffers it's shaders. Our goals are going to be to move around the Triangle, and change the color of the the black squares in the checkerboard fullscreen effect.

We've not discussed it before in this tutorial, but you may have heard about structs containing "padding" between fields for alignment reasons. When you're just writing in one language, this mostly isn't something you need to concern yourself with, unless you're trying to optimize your layouts. When you're transferring data between languages, or in this case, between languages _and_ processors, you need to ensure both sides agree on the layout of structs and their padding.


{{collapsible-card "Covered in this Section"}}

[OpenGL Specification](https://registry.khronos.org/OpenGL/specs/gl/glspec45.core.pdf#page=159)

{{collapsible-card-end}}
