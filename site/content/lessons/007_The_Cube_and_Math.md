---
title: The Cube and Math (Example Done, Started Outlining)
description: We're not done learning about APIs that can help us in 2D, but it's time to start learning about 3D rendering. We'll be rendering a cube, and along the way learn about properly constructing a model matrix, and expand out our math library.
template: lesson_template.html
collections: ["lessons"]
---

## Vertex and Index arrays in more detail



## Front and Back Face Culling


```
           4---------------------5
          /|                    /|
         / |      +y           / |
        /  |       |          /  |
       /   |       |         /   |
      /    |       |        /    |
     0-----|-------|-------1     |
     |     |       |       |     |
     |     |       | +z    |     |
     |     |       | /     |     |
     |     6-------|/------|-----7
-----|----/--------*-------|----/------+x
     |   /         |       |   /
     |  /          |       |  /
     | /           |       | /
     |/            |       |/
     2---------------------3
                   |
                   |
                   |
                   |

```


### Clockwise


```
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
```

## Model Matrix

It's time to build out some matrix math functionality. If you're already comfortable with these topics, and prefer to just use your own, or copy/paste the one from this sample, that is entirely valid. 

> Note: The implementations here are naive and intended for learning, rather than high performance work. I intend to teach you how to do things effectively and ideally in a fairly performant manner in this series, but I'm not counting CPU cycles or planning to drop down into SIMD.

### Vectors

This functionality isn't really needed yet, but we might as well start introducing them here. We'll be using them in a few chapters.

#### Types

#### Operations

##### Downcasting

##### Subtraction

##### Addition

##### Scalar Addition

##### Scalar Multiplication

##### Scalar Division

##### Dot Product

##### Magnitude

##### Normalization

### Matrices

#### Identity

#### Multiplication

#### Scale

#### Rotation

##### Rotation about X

##### Rotation about Y

##### Rotation about Z

#### Translation

### Bring it together

## Perspective Projection Matrix
