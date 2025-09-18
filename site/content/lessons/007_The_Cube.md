---
title: The Cube
description: It's time to discuss vertex and index buffers, we've been able to get by without them until now, but if we want to draw models, it's the most straight forward way. We'll also need to cover depth so that we don't render objects triangles farther away from the camera on top of triangles closer to the camera.
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


## Perspective Projection Matrix
