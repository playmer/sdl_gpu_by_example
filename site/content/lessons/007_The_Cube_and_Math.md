---
title: The Cube and Math
description: We're not done learning about APIs that can help us in 2D, but it's time to start learning about 3D rendering. We'll be rendering a cube, and along the way learn about properly constructing a model matrix, and expand out our math library.
template: lesson_template.html
example_status: Finished
chapter_status: Outlined
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

### Matrices

Unlike with direct vector math, with matrices, most math is going to be done either with 3x3 for 2D operations or 4x4 for 3D operations. We'll show this math in 3D and thus 4x4 matrices.

##### Operations

###### Matrix * Vector Multiplication

###### Matrix * Matrix Multiplication

##### Transformations

###### Identity

The identity matrix is generally a "base" matrix. Multiplying against it will return the same matrix you started with. It's simply a series of 1s along it's diagonal.

Math:

$$ \begin{pmatrix} 1 & 0 & 0 & 0 \\ 0 & 1 & 0 & 0 \\ 0 & 0 & 1 & 0 \\ 0 & 0 & 0 & 1  \end{pmatrix} $$

Code:

```cpp

float4x4 IdentityMatrix() {
  float4x4 toReturn;
  SDL_zero(toReturn);

  toReturn.data[0][0] = 1.0f;
  toReturn.data[1][1] = 1.0f;
  toReturn.data[2][2] = 1.0f;
  toReturn.data[3][3] = 1.0f;

  return toReturn;
}
```

###### Translation


Math:

$$ \begin{pmatrix} 1 & 0 & 0 & x \\ 0 & 1 & 0 & y \\ 0 & 0 & 1 & z \\ 0 & 0 & 0 & 1  \end{pmatrix} $$

Code:

```cpp
float4x4 TranslationMatrix(float4 aPosition) {
  float4x4 toReturn = IdentityMatrix();

  toReturn.data[3][0] = aPosition.x;
  toReturn.data[3][1] = aPosition.y;
  toReturn.data[3][2] = aPosition.z;

  return toReturn;
}
```
###### Scale

Math:

$$ \begin{pmatrix} x & 0 & 0 & 0 \\ 0 & y & 0 & 0 \\ 0 & 0 & z & 0 \\ 0 & 0 & 0 & 1  \end{pmatrix} $$

Code:

```cpp
float4x4 ScaleMatrix(float4 aScale) {
  float4x4 toReturn = IdentityMatrix();

  toReturn.data[0][0] = aScale.x;
  toReturn.data[1][1] = aScale.y;
  toReturn.data[2][2] = aScale.z;

  return toReturn;
}
```

###### Rotation

####### Rotation about X

Math:

$$ \begin{pmatrix} 1 & 0 & 0 & 0 \\ 0 & \cos{a} & -\sin{a} & 0 \\ 0 & \sin{a} & \cos{a} & 0 \\ 0 & 0 & 0 & 1  \end{pmatrix} $$

Code:

```cpp
float4x4 RotationMatrixX(float aAngle) {
  float4x4 toReturn = IdentityMatrix();

  toReturn.data[1][1] = SDL_cosf(aAngle);
  toReturn.data[1][2] = SDL_sinf(aAngle);
  toReturn.data[2][1] = -SDL_sinf(aAngle);
  toReturn.data[2][2] = SDL_cosf(aAngle);

  return toReturn;
}
```


####### Rotation about Y

Math:

$$ \begin{pmatrix} \cos{a} & 0 & \sin{a} & 0 \\ 0 & 1 & 0 & 0 \\ -\sin{a} & 0 & \cos{a} & 0 \\ 0 & 0 & 0 & 1  \end{pmatrix} $$

Code:

```cpp
float4x4 RotationMatrixY(float aAngle) {
  float4x4 toReturn = IdentityMatrix();

  toReturn.data[0][0] = SDL_cosf(aAngle);
  toReturn.data[0][2] = -SDL_sinf(aAngle);
  toReturn.data[2][0] = SDL_sinf(aAngle);
  toReturn.data[2][2] = SDL_cosf(aAngle);

  return toReturn;
}
```


####### Rotation about Z

Math:

$$ \begin{pmatrix} \cos{a} & -\sin{a} & 0 & 0 \\ \sin{a} & \cos{a} & 0 & 0 \\ 0 & 0 & 1 & 0 \\ 0 & 0 & 0 & 1  \end{pmatrix} $$

Code:

```cpp
float4x4 RotationMatrixZ(float aAngle) {
  float4x4 toReturn = IdentityMatrix();

  toReturn.data[0][0] = SDL_cosf(aAngle);
  toReturn.data[0][1] = SDL_sinf(aAngle);
  toReturn.data[1][0] = -SDL_sinf(aAngle);
  toReturn.data[1][1] = SDL_cosf(aAngle);

  return toReturn;
}
```

### Bring it together

## Perspective Projection Matrix
