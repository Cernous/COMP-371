# Assignment 2 COMP 371

40166293 - Clarence Zhen
40263250 - Gorden Quach

## Objective

Alongside with all the requirements of the first assignment, create a scene that includes:
- User input that interacts with the objects in the application 
- Phong shading or better

## High-Level Description (In point form)

- A truck object composed of 3 different models: The body, mirror, and wheel
    - These are loaded and meticulously placed to resemble a truck
    - Mirror needed to be loaded twice while the wheel is loaded 4 times

- Hierarchical animation model involving the truck, wheels and the ground
    - The main body of the truck and the ground is considered as the first level
    - The wheels are the second level as they steer and roll
    - Yes. That means the entire scene rotates on the Y-axis and not the camera

- Phong Model Shaders
    - <!Add your part!> 

- User-controlled scene comes with the relaxed camera requirements
    - The previous camera keybinds are set to control the wheels on the car

- Skybox
    - It's a cube.
    - Using a unlit shader as opposed to the rest of the elements in the scene

## Pitfalls
- Did not have enough time to create the moving ground
    - While it is unintuitive, we wanted to make the ground move in response to the user input.
    - However if the ground moved, new planes would need to be loaded in the event that the car reaches the end of one plane.

## Controls

- W: Makes the wheels roll forward
- S: Makes the wheels roll backwards
- A: Makes the front wheels steer left
- D: Makes the front wheels steer right
