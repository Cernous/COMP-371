# Assignment 1 COMP 371

40166293 - Clarence Zhen
40263250 - Gorden Quach

## Objective

Create a scene with custom assets or differently arranged assets (must be different from the tutorials) and use moving lights.

## High-Level Description (In point form)

- Adds 2 extra custom meshes (Loaded through OBJloader/OBJloaderV2 from tutorial 5)
    - Meshes are made and exported from Blender
        - Suzanne is a prefab that comes with blender
        - Table is a custom made mesh that is poorly optimized
            - Applied a subdivider and then triangulate modifier which skyrocketed the vertex count
    - Meshes have custom textures (Loaded through `loadTexture` function from tutorial 4)
- Arrange 4 cubes (Textured vertex array data taken from tutorial 4) in different locations
    - 2 of which are moving *ambient lights* orbiting the scene at an offset
    - 2 of which are static and shows off the light shader
- Modified both the texturedShaderProgram (from tutorial 4) and the original shaderProgram (from tutorial 3) to illuminate parts of the mesh that are exposed to the emitters

## Pitfalls
- Unable to make the normalShaderProgram (from tutorial 5) to work properly with the emitters
    - It was cool to look at
- Unable to load `*.glsl` files
    - We ended up resorting to just use function returns to make the shaders
- Did not use EBO to load the meshes
    - It would have been more efficient
- Unable to cast shadows of the Suzanne Model on to the table using dynamic lighting
    - We resorted to keeping emitters and shaderProgram as they are for now.

## Controls

- W: translates the camera forward from where it is facing 
- S: translates the camera backwards from where it is facing 
- A: translates the camera left of where it is facing
- D: translates the camera right of where it is facing
- LEFT_SHIFT: increases the camera's movement speed
- SPACE: translates the camera up the y-axis
- LEFT_CTRL: translates the camera down the y-axis

## Conclusion
We applied a lot of what we learned from the tutorials and some more from researching a lot about shader programs and 3D modeling. Although, there are a lot of things we could have done differently, such as using ASSIMP and maybe spend more time on figuring out why loading shader files was not working, we feel that we have accomplished the objective of this assignment, which was creating a scene that was different from what's given by the tutorials and include 2 moving lights.