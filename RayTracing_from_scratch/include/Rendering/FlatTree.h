#pragma once
#include "Rendering/NodeHierarchy.h"

namespace Renderer
{


    class FlatTree :
        public NodeHierarchy
    {
    public:

        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        FlatTree(std::vector<Object*>& objects);

        bool Intersect(const Ray& ray, HitInfo& hit_info);
        float Intersect(const Ray& ray);

    private:
        std::vector<NodePrimitive> primitives;
        //std::vector<Object*> Objects;
    };


}