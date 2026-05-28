## GL Compute Raytracer
A real-time raytracing tech demo I created for my Graphics and Computational Programming module during my third year at Bournemouth University. It uses a pipeline of OpenGL compute shaders to draw to a pixel buffer that is then displayed on the screen. The program uses a single static Bounding Volume Hierarchy calculated at the start of the program.

The three main shaders used are for Object Intersection, Shadow Calculation, and Material Shading. Object intersection traverses the pre-made BVH to check for any pixel hits and calculates motion vectors for the whole pass. The shadow calulation shader uses the Monte Carlo sampling method alongisde confidence values in the motion vector data to keep soft shadows smooth between frames when in motion.

### Example video
<p>
This is the raytracer running at a resolution of 1920x1080, running fully at around 100fps on my RTX 4070
<video src="https://github.com/user-attachments/assets/a8961d84-a9f1-467f-afb7-42763be9c082" controls></video>
</p>
