---
title: An "FPS" Camera (Not Started)
description: Without a dynamic camera, we'll be really struggling when rendering models and scenes, so lets get this out of the way.
template: lesson_template.html
collections: ["lessons"]
---


We could mostly brush off Camera Space and the View Matrix needed when working in 2D, but now that we're in 3D, we'll need to cover some more math. Something we skipped over a bit earlier when discussing coordinate spaces is that all Models have an inherent Matrix that represents where they exist within World Space:

```
    | X_Forward, X_Right, X_Up, X_Position |
M = | Y_Forward, Y_Right, Y_Up, Y_Position |
    | Z_Forward, Z_Right, Z_Up, Z_Position |
    | W_Forward, W_Right, W_Up, W_Position |

```

This isn't particularly interesting because we typically place Models at reasonable positions within Object Space such that they're easy to manipulate in World Space, and then we can Scale/Rotate/Move them around using the matrices covered earlier. So typically this Matrix looks like an implicit Identity Matrix:

```
    | 1, 0, 0, 0 |
M = | 0, 1, 0, 0 |
    | 0, 0, 1, 0 |
    | 0, 0, 0, 1 |
```

That would then get multiplied against some matrix we computed for this _instance_ of the model using the TRS Matrix, or similar.


So this Matrix is intended to transform from Object Space to World Space, but it turns out, that we can reverse that as well. If we have an Object in World Space, we can use the inverse of this Model Matrix



##