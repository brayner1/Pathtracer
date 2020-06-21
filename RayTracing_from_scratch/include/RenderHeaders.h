#pragma once

#include "Loader/SceneLoader.h"
#include "Rendering/Ray.h"
#include "Rendering/PinholeCamera.h"
#include "Rendering/Scene.h"
#include "Rendering/Light.h"
#include "Rendering/RenderManager.h"
#include "Rendering/Utils/Spectrum.h"
#include "Object/Object.h"
#include "Object/GroundPlane.h"
#include "Object/Triangle.h"
#include "Object/Mesh.h"
#include "Shading/Texture.h"
#include "Shading/Material.h"
#include "Shading/DiffuseMaterial.h"
#include "Shading/GlossyMaterial.h"
#include "Shading/RefractiveMaterial.h"
#include "Shading/BxDF.h"

#include "OpenImageDenoise/oidn.hpp"
#include "RandUtil.h"
//#define GammaCorrection
#define paralellism
//#define Debug


//using namespace Eigen;