CSE 169: Computer Animation

Sample Code
-----------
This is some sample code to get you started with a simple GL application. It
should render a spinning cube and allows manipulation of the camera with the
mouse.


Controls
--------
The left mouse button rotates the camera, and the right mouse button zooms in
and out.

The 'R' key resets.


Solution & Project Files
------------------------
There is a solution & project file set up for VisualStudio 2017. If you are
using a different compiler, it should be simple to set up a project- just make
sure that all of the .cpp and .h files are included in the project.


GLM
---
The sample code includes a copy of the 'GLM' library. GLM uses headers only,
so should not have any trouble compiling or linking.


GLEW & GLUT
-----------
To build & run the sample code, you must have GL installed and have GLEW &
GLUT in a place where the compiler can find them. The code is set up to
look in a 'GL' subdirectory for both "glut.h" and "glew.h". These #include
lines are in Core.h and can be modified to fit with different system set ups.
