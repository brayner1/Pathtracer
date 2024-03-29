# PathTracer
Pathtracer implemented for studying and practicing Physically Based Rendering techniques and the C++ language. This project was also used as the practical part of my Computer Engineering course conclusion work, entitled "Study of Path Tracing and Coherent Ray Tracing Techniques". 

The project architecture and algorithms (including some classes names, for now) are heavily inspired in the (great) book "Physically Based Rendering: From Theory to Implementation", from Matt Pharr, Wenzel Jakob, and Greg Humphreys. Despite this, the book is used only as a reference source, and all the code was written by me.

This project also uses some open-source third party libraries for linear algebra (Eigen), 3D object loading (Assimp), image loading (stb headers), and image denoising (OpenImageDenoise).

For now, there is a lack of support for more complex materials and microfacet models since my focus is on designing the system and achieving a good performance. But these will be introduced soon.

## Sample Rendered Image:
Rendering of the Sponza scene, provided online by Marko Dabrovic.
![sponza_denoised](https://user-images.githubusercontent.com/18320203/162642351-e4dc3199-3c13-4bb3-acdd-17ad7d05e3ad.png)


## External Libs: 
* [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)
* [Assimp](https://www.assimp.org/index.php)
* [Intel Open Image Denoise](https://openimagedenoise.github.io/)
* [Stb headers](https://github.com/nothings/stb)
