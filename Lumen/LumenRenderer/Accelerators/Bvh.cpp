//
// Created by enzoc on 16/10/2022.
//

#include <execution>
#include "Bvh.hpp"
#include "../Utility/Utility.hpp"


#define BINS 8

namespace LumenRender {

    BVH::BVH(Mesh *tri_mesh) {

    }

    void BVH::Build() {

    }


    void BVH::Traversal() {


    }


    bool BVH::Hit(Ray &ray, float t_max) const {
        return false;
    }

    bool BVH::GetBounds(AABB &outbox) const {
        return false;
    }
} // LumenRender