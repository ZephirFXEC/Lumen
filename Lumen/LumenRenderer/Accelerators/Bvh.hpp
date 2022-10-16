//
// Created by enzoc on 16/10/2022.
//

#ifndef LUMEN_BVH_HPP
#define LUMEN_BVH_HPP

#include "../Scene/Object.hpp"
#include "../Scene/Scene.hpp"

namespace LumenRender {

    class BVH : public Object {
    public:
        BVH() = default;

        explicit BVH(const Scene &scene) : BVH(scene.m_Objects, 0, scene.m_Objects.size()) {}
        BVH(const std::unordered_map<uint32_t, Object*> &Objects, size_t start, size_t end);

        bool Hit(const Ray &ray, HitRecords &record) const override;

        bool GetBounds(AABB &outbox) const override;


    public:
        Object* m_Left{};
        Object* m_Right{};
        AABB m_Box;
    };


} // LumenRender

#endif //LUMEN_BVH_HPP