## GL Compute Raytracer
A real-time raytracing tech demo I created for my Graphics and Computational Programming module during my third year at Bournemouth University. It uses a pipeline of OpenGL compute shaders to draw to a pixel buffer that is then displayed on the screen. The program uses a single static Bounding Volume Hierarchy calculated at the start of the program.

### Example video
<p>
This is the raytracer running at a resolution of 1920x1080, running fully at around 100fps on my RTX 4070
<video src="docs/videos/GLTracer.mp4" controls></video>
</p>

Most of the performance in this is taken up, predictably, by shadow calculation which uses Monte Carlo sampling and motion vectors per-pixel.