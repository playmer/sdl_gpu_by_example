---
title: Quads and Textures
description: Extending off of Uniform Buffers, we'll learn about Texture Buffers, so we can finally display images beyond colored shapes.
template: lesson_template.html
example_status: Finished
chapter_status: Not Started
collections: ["lessons"]
---


## Textures


## Creating a Buffer


## Transfer Buffers


## Uploads


## The Vertex Shader

### The Quad and an index array

Up until now, we've only stored arrays of vertices, this made sense because we've only been dealing with a single triangle. Once you want to render a texture however, you'll immediately realize that you'd be duplicating two vertices for that quad you want to display it on!

```

 (-1, 1)                 (1, 1)
     0------------------1    1
     |                 /    /|
     |  Triangle 1   /    /  |
     |             /    /    |
     |           /    /      |
     |         /    /        |
     |       /    /          |
     |     /    /            |
     |   /    /  Triangle 2  |
     | /    /                |
     2     2-----------------3
    (-1, -1)              (1, -1)
```

See how the first and second vertex are the same on each triangle? It doesn't look like much now, but when you start rendering models, this would really start increasing the size of them. So lets learn about indexing now, ahead of when we'll need them for some of the more serious 3D applications later on.

Similar to last time, we'll be declaring some static data in the shader, but this time with an added vertex, as well as an array of indices to index with.

```
static const float2 cVertexPositions[4] = {
  {-1.0f,  1.0f},
  { 1.0f,  1.0f},
  {-1.0f, -1.0f},
  { 1.0f, -1.0f},
};

static const uint cVertexIndicies[6] = {
  0, 1, 2, // Triangle 1
  1, 3, 2  // Triangle 2
};
```

We can see our indices align with the diagram above. An astute reader might also notice that these vertices match the dimensions of the screen. Don't worry, we'll learn how to adjust the size of it in this chapter so it won't take up the whole screen!

### UV Coordinates

## The Pixel Shader