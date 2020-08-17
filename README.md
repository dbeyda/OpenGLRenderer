# OpenGL Renderer
> A 3D renderer developed as an introduction to real-time rendering.

 This project is a 3D renderer developed as an introduction to real-time rendering.
 It was developed mainly during PUC-Rio's Real-Time Rendering class, and then further improved over winter recess.
 Credits for people and materials that help me are at the bottom of the page.
 
 The Depth of Field implementation was a final assignment for the class. [Here you can check out the final report](https://github.com/dbeyda/OpenGLRenderer/blob/master/David%20Beyda%20-%20Depth%20Of%20Field%20Report.pdf) and more information about the techniques used. 
 
 ## Features:
 * Load `.obj` files for meshes.
 * Load `.png` and `.jpg` for textures.
 * Load `.mtl` for material properties.
 * Importing diffuse, specular and ambient maps.
 * Support for **multiple lights and models**.
 * Basic camera movements.
 * Forward rendering.
 * **Spotlights**.
 * **Phong Lighting Model**, with diffuse, speccular and ambient lighting.
 * **Shadow Mapping** with soft shadows using jittered offsets, as seen [here](https://developer.nvidia.com/gpugems/gpugems2/part-ii-shading-lighting-and-shadows/chapter-17-efficient-soft-edged-shadows-using).
 * **Bump Mapping**, with vertices tangents calculated on mesh import.
 * **Depth of Field** (and bokeh) that can be toggled dinamically during runtime.

## Images
Default configurations example:

![](https://github.com/dbeyda/OpenGLRenderer/blob/master/images/NoDoF.png)

Depth of Field example:

![](https://github.com/dbeyda/OpenGLRenderer/blob/master/images/DoF.png)

Bokeh when the scene is visualized from afar:

![](https://github.com/dbeyda/OpenGLRenderer/blob/master/images/bokeh.png)

One light only soft shadow example:

![](https://github.com/dbeyda/OpenGLRenderer/blob/master/images/Shadow.jpg)

## Dependencies
* GLEW
* GLFW 3.3
* OpenGL 4+

## Credits and Thanks
[The Cherno's](https://www.youtube.com/channel/UCQ-W1KE9EYfdxhL6S4twUNw) youtube channel, for the Introduction to OpenGL Tutorials.

Joey De Vries' [Learn OpenGL Tutorials](https://learnopengl.com/) for the incredible explanations and step-by-step guides.

Waldemar Celes, for the teaching of Real-Time Rendering class at PUC-Rio.

[Bernard Lupiac](https://github.com/blupiac) for the guidance and help with debugging.


