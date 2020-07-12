#pragma once
#include "Rendering/ObjectTree.h"

namespace Renderer
{


    class FlatTree :
        public ObjectTree
    {
    public:

        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        FlatTree(std::vector<Object*>& objects) : Objects(objects) {}

        bool Intersect(Ray& ray, HitInfo& hit);
        bool Intersect(Ray& ray);

    private:
        std::vector<Object*> Objects;
    };


}