#pragma once

#include "Loader/AssimpLoader.h"
#include "Loader/SDLLoader.h"
#include "Light/Light.h"
#include "Light/PointLight.h"
#include "Camera/PinholeCamera.h"
#include "Rendering/Ray.h"
#include "Rendering/Scene.h"
#include "Rendering/FrameManager.h"
#include "Rendering/Utils/Spectrum.h"
#include "Rendering/NodeHierarchy.h"
#include "Rendering/FlatTree.h"
#include "Rendering/BVHTree.h"
#include "Object/Object.h"
#include "Object/GroundPlane.h"
#include "Object/Mesh.h"
#include "Shading/Texture.h"
#include "Shading/Material.h"
#include "Shading/DiffuseMaterial.h"
#include "Shading/GlossyMaterial.h"
#include "Shading/RefractiveMaterial.h"
//#include "Shading/BxDF.h"

#include "OpenImageDenoise/oidn.hpp"
#include "RandUtil.h"
//#define GammaCorrection
#define paralellism
//#define Debug


//using namespace Eigen;