#ifndef PTracer_RenderHeader_h
#define PTracer_RenderHeader_h

//#include "Threading/ThreadingManager.h"
#include "Loader/AssimpLoader.h"
#include "Loader/SDLLoader.h"
#include "Light/Light.h"
#include "Light/DirectLight.h"
#include "Light/PointLight.h"
#include "Light/PrimitiveLight.h"
#include "Camera/PinholeCamera.h"
#include "Rendering/Integrator/Integrator.h"
#include "Rendering/Ray.h"
#include "Rendering/Scene.h"
#include "Rendering/FrameManager.h"
#include "Rendering/Utils/Spectrum.h"
#include "Rendering/NodeHierarchy.h"
#include "Rendering/FlatTree.h"
#include "Rendering/BVHTree.h"
#include "Rendering/RenderingUtils.h"
#include "Object/Object.h"
#include "Object/GroundPlane.h"
#include "Object/Mesh.h"
#include "Shading/Textures/TextureManager.h"
#include "Shading/Textures/RGBTexture.h"
#include "Shading/Material.h"
#include "Shading/DiffuseMaterial.h"
#include "Shading/GlossyMaterial.h"
#include "Shading/RefractiveMaterial.h"
#include "Shading/PhongMaterial.h"
//#include "Shading/BxDF.h"

#include "OpenImageDenoise/oidn.hpp"
#include "RandUtil.h"
#define paralellism
//#define Debug

#endif


//using namespace Eigen;
