ASSIGNMENT 1

Students:
Rui Botelho - 67077
João Monteiro - 82418

___________________________________________
INSTRUCTIONS

1) open solution using visual studio 2015
2) compile using target x86 debug
3) run with working directory ./Project1

___________________________________________
CONTROLS

Move mouse - change camera direction
W - camera forward
S - camera backward
A - camera strafe left
D - camera strafe right
SPACE - camera move up
Z - camera move down

1 - Rotate at X axis
2 - Rotate at Y axis
3 - Rotate at Z axis

O - change current object
N - change shader(phong, textured, cube mapping, sphere mapping)
C - change current skybox

___________________________________________
DESCRIPTION

Includes four objects: cube, torus, sphere, high resolution sphere, nanosuit;
The first three share the same texture and normal map.
Includes two skyboxes: grand canion and ocean.

There is one light moving along y-axis with time, above the object.


loads objects using library Assimp, which supports formats (PLY, OBJ, .3DS, etc)
