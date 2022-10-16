//
// Created by enzoc on 16/10/2022.
//

#ifndef LUMEN_UTILITY_HPP
#define LUMEN_UTILITY_HPP

#include <glm/glm.hpp>
#include <cmath>
#include <random>

inline int RandomInt(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(min, max);
    return dis(gen);
}

inline float RandomFloat(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float>dis(min, max);
    return dis(gen);
}

inline bool box_compare(const LumenRender::Object* a, const LumenRender::Object* b, int axis) {
    LumenRender::AABB box_a;
    LumenRender::AABB box_b;

    if (!a->GetBounds(box_a) || !b->GetBounds(box_b))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    return box_a.pMin[axis] < box_b.pMin[axis];
}


bool box_x_compare (const LumenRender::Object* a, const LumenRender::Object* b) {
    return box_compare(a, b, 0);
}

bool box_y_compare (const LumenRender::Object* a, const LumenRender::Object* b) {
    return box_compare(a, b, 1);
}

bool box_z_compare (const LumenRender::Object* a, const LumenRender::Object* b) {
    return box_compare(a, b, 2);
}


#endif //LUMEN_UTILITY_HPP