//
// Created by enzoc on 16/10/2022.
//

#include <iostream>
#include <execution>
#include "Bvh.hpp"
#include "../Utility/Utility.hpp"

namespace LumenRender {
    bool BVH::Hit(const Ray &ray, HitRecords &record) const {
        float t;
        float closest = std::numeric_limits<float>::max();
        if (!m_Box.Hit(ray, t)) return false;

        record.m_T = t;
        bool hitLeft = m_Left->Hit(ray, record) && record.m_T < closest;
        bool hitRight = m_Right->Hit(ray, record) && record.m_T < closest;

        return hitLeft || hitRight;
    }

    bool BVH::GetBounds(AABB &outbox) const {
        outbox = m_Box;
        return true;
    }

    BVH::BVH(const std::unordered_map<uint32_t, Object *> &Objects, size_t start, size_t end) {

        int axis = RandomInt(0,2);
        auto comparator = (axis == 0) ? box_x_compare : (axis == 1) ? box_y_compare : box_z_compare;

        size_t object_span = end - start;

        if (object_span == 1) {
            m_Left = m_Right = Objects.at(start);
        } else if (object_span == 2) {
            if (comparator(Objects.at(start), Objects.at(start+1))) {
                m_Left = Objects.at(start);
                m_Right = Objects.at(start+1);
            } else {
                m_Left = Objects.at(start+1);
                m_Right = Objects.at(start);
            }
        } else {
            std::vector<Object*> objects_vector;
            for (auto &[index, object]: Objects) {
                objects_vector.push_back(object);
            }

            std::sort(objects_vector.begin() + start, objects_vector.begin() + end, comparator);

            std::unordered_map<uint32_t, Object*> sorted_objects;
            sorted_objects.reserve(Objects.size());

            for (size_t i = 0; i < objects_vector.size(); i++) {
                sorted_objects.insert({i, objects_vector[i]});
            }

            auto mid = start + object_span/2;
            m_Left = new BVH(sorted_objects, start, mid);
            m_Right = new BVH(sorted_objects, mid, end);
        }

        AABB box_left, box_right;

        if ( !m_Left->GetBounds( box_left) || !m_Right->GetBounds(box_right))
            std::cerr << "No bounding box in bvh_node constructor.\n";

        m_Box = AABB::SurroundingBox(box_left, box_right);
    }
} // LumenRender