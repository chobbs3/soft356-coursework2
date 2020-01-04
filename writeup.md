
# Chloe Hobbs - SOFT365 Write-up  

Visual Studio Version 2019 and OpenGL version 4 is needed for the programe to work. Before starting programme, insure that ‘Project 3’ is selected as the ‘StartUp’ project and that the following packages are installed: 

- GLU 

- Nupengl.core 

- Nupengl.core.redlist 

When producing my code, I used a variety of sources including OpenGl tutorials from  http://www.opengl-tutorial.org/ and https://github.com/SOFT356/SampleCode. My code works by reading the obj file, taking the needed information from it, and putting it in the correct vector depending on if it is a vertex, uv or normal. I then assign buffers and call the loadTexture function for the png file, to give the cube a creeper colour. I can then adjust the position of the camera, the direction the camera is looking and what is considered up, in the display function. i have split the code for loading and desplaying the object file into a seaprate class so i can have two of them side by side.

the scene can be cleared by pressing 'c' and reloaded by pressing 'l'.







