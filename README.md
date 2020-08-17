# OpenGL Renderer
> A 3D renderer developed as an introduction to real-time rendering.

 This project is a 3D renderer developed as an introduction to real-time rendering.
 It was developed mainly during PUC-Rio's Real-Time Rendering class, and then further improved over winter recess.
 Credits for resources that made this possible are at the bottom of the page.
 
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

