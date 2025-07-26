---
title: Triangle and a "Fullscreen Triangle"
description: Finally you can see a triangle, and then a demonstration on why you might actually use a single triangle later on in your Graphics journey. Will cover the creation of Vertex and Pixel shaders, a simple demonstration of "Vertex Pulling", and then an extension of the triangle to cover the whole screen with a shader effect.
---

# Triangle and a "Fullscreen Triangle"

The big one, we're finally going to draw a Triangle. And after that, we're going to learn a trick on how to extend this triangle to display over the entire screen. This technique is a small optimization that gets used when you start learning fullscreen effects. We'll be using it in the next chapter to play around with pixel shaders to get a little more comfortable with writing code on the GPU.

I figured it'd be nice to go over these together, because realistically, it's not common that you need to draw a single triangle on the screen. Maybe a Quad, two triangles making the shape of a Rectangle or Square, but that requires textures, transfer buffers, and bindings. That said, if you adjust the positions of that triangle? Suddenly it has a very practical purpose! And it's something we can learn about with only a few additional function calls without needing to get into everything needed to display a texture. Of course, don't worry, we'll get there. But let's build up step by step and try to learn something with each step!

