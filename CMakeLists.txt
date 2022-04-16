# CMakeList.txt: projeto de CMake para PTracer, incluir origem e definir
# lógica específica a um projeto aqui.
#
cmake_minimum_required (VERSION 3.8)

project ("PTracer")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(PTracer_HEADER_FILES 
# ------------ Header Files ------------
# Threading header files
#"src/include/Threading/ThreadingManager.h"
"src/include/Threading/Barrier.h" 
"src/include/Threading/ThreadPool.h" 
"src/include/Threading/Threading.h"
"src/include/Threading/Work.h"
# Objects header files
"src/include/Object/Object.h"
"src/include/Object/Mesh.h"
"src/include/Object/GroundPlane.h"
# Camera header files
"src/include/Camera/PinholeCamera.h"
# Rendering header files
"src/include/Rendering/NodeHierarchy.h"
"src/include/Rendering/FlatTree.h"
"src/include/Rendering/BVHTree.h"
"src/include/Rendering/Ray.h"
"src/include/Rendering/Scene.h"
"src/include/Rendering/FrameManager.h"
"src/include/Rendering/Integrator/Integrator.h"
#"src/include/Rendering/Integrator/SamplerIntegrator.h"
# Lights header files
"src/include/Light/Light.h"
"src/include/Light/DirectLight.h"
"src/include/Light/PointLight.h"
"src/include/Light/PrimitiveLight.h"
# Shading header files
"src/include/Shading/Material.h"
"src/include/Shading/DiffuseMaterial.h"
"src/include/Shading/GlossyMaterial.h"
"src/include/Shading/RefractiveMaterial.h"
"src/include/Shading/PhongMaterial.h"
"src/include/Shading/Textures/RGBTexture.h"
"src/include/Shading/Textures/TextureManager.h"
# Loader header files
"src/include/Loader/AssimpUtils.h"
"src/include/Loader/AssimpLoader.h"
"src/include/Loader/SDLLoader.h"
# Renderheaders
"src/include/RenderHeaders.h"
# PCH
"src/include/pch.h"
)

set(PTracer_SOURCE_FILES
# ------------ Source Files ------------
# Threading source files
#"src/source/Threading/ThreadingManager.cpp"
"src/source/Threading/ThreadPool.cpp"
# Objects source files
"src/source/Object/Object.cpp"
"src/source/Object/Mesh.cpp"
"src/source/Object/GroundPlane.cpp"
# Camera Source files
"src/source/Camera/PinholeCamera.cpp"
# Rendering source files
"src/source/Rendering/FlatTree.cpp"
"src/source/Rendering/BVHTree.cpp"
"src/source/Rendering/Ray.cpp"
"src/source/Rendering/Scene.cpp"
"src/source/Rendering/FrameManager.cpp"
"src/source/Rendering/Integrator/Integrator.cpp"
#"src/source/Rendering/Integrator/SamplerIntegrator.cpp"
# Lights source files
"src/source/Light/Light.cpp"
"src/source/Light/DirectLight.cpp"
"src/source/Light/PointLight.cpp"
"src/source/Light/PrimitiveLight.cpp"
# Shading source files
"src/source/Shading/Material.cpp"
"src/source/Shading/DiffuseMaterial.cpp"
"src/source/Shading/GlossyMaterial.cpp"
"src/source/Shading/RefractiveMaterial.cpp"
"src/source/Shading/PhongMaterial.cpp"
"src/source/Shading/Textures/RGBTexture.cpp"
"src/source/Shading/Textures/TextureManager.cpp"
# Loader source files
"src/source/Loader/AssimpUtils.cpp"
"src/source/Loader/AssimpLoader.cpp"
"src/source/Loader/SDLLoader.cpp"
# PCH
"src/source/pch.cpp"
)

source_group(Threading REGULAR_EXPRESSION "src/(source|include)/Threading/.*")
source_group(Objects REGULAR_EXPRESSION "src/(source|include)/Object/.*")
source_group(Cameras REGULAR_EXPRESSION "src/(source|include)/Camera/.*")
source_group(Lights REGULAR_EXPRESSION "src/(source|include)/Light/.*")
source_group(Shading REGULAR_EXPRESSION "src/(source|include)/Shading/.*")
source_group(Rendering REGULAR_EXPRESSION "src/(source|include)/Rendering/.*")
source_group(Loaders REGULAR_EXPRESSION "src/(source|include)/Loader/.*")

# Adicione a origem ao executável deste projeto.
add_executable (PTracer 
"src/PTracer.h"
"src/PTracer.cpp"
${PTracer_HEADER_FILES}
${PTracer_SOURCE_FILES}
)

# Definition used by the StructLayout VS 2019 extension.
target_compile_definitions(PTracer PRIVATE CMAKE_EXPORT_COMPILE_COMMANDS)

target_include_directories(PTracer PUBLIC 
	src/include
	thirdparty/Eigen
	thirdparty/assimp/include
	thirdparty/OpenImageDenoiser/include
)

get_target_property(PTracer_INCLUDE_DIRS PTracer INCLUDE_DIRECTORIES)
message("Include Dirs: ")
foreach(dir IN LISTS PTracer_INCLUDE_DIRS)
	message(${dir})
endforeach()

target_precompile_headers(PTracer PRIVATE 
	src/include/pch.h
)

get_target_property(PTracer_PRECOMPILE_HEADERS PTracer PRECOMPILE_HEADERS)
message("Precompiled headers: " ${PTracer_PRECOMPILE_HEADERS})

find_library(ASSIMP_LIB 
NAMES assimp-vc140-mt 
HINTS thirdparty/assimp/lib/x64
REQUIRED)

find_library(OIDN_LIB
NAMES OpenImageDenoise
HINTS thirdparty/OpenImageDenoiser/lib
REQUIRED
)

find_library(TBB_LIB
NAMES tbb
HINTS thirdparty/OpenImageDenoiser/lib
REQUIRED
)

find_library(TBBMALLOC_LIB
NAMES tbbmalloc
HINTS thirdparty/OpenImageDenoiser/lib
REQUIRED
)

target_link_libraries(PTracer ${ASSIMP_LIB} ${OIDN_LIB} ${TBB_LIB} ${TBBMALLOC_LIB})
get_target_property(PTracer_LINK_LIBRARIES PTracer LINK_LIBRARIES)
message("Linked libraries: " ${PTracer_LINK_LIBRARIES})

# Currently, I have only set the compilation flags for MSVC compiler. 
# Will focus on bringing other compiling systems after restructuring part of the multithreading system 
if (MSVC)
	target_compile_options(PTracer PUBLIC /Ox /Oi /W3 /arch:AVX2 /openmp)
endif()
# TODO: Adicione testes e instale destinos, se necessário.

