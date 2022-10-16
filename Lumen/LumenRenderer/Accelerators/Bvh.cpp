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
        if (!m_Box.Hit(ray, t)) return false;

        record.m_T = t;
        record.m_Position = ray.At(t);

        HitRecords leftRecord{}, rightRecord{};
        leftRecord.m_T = rightRecord.m_T = t;
        bool hitLeft = m_Left->Hit(ray, leftRecord);
        bool hitRight = m_Right->Hit(ray, rightRecord);

        if (hitLeft && hitRight) {
            record = leftRecord.m_T < rightRecord.m_T ? leftRecord : rightRecord;
            return true;
        }

        if (hitLeft) {
            record = leftRecord;
            return true;
        }

        if (hitRight) {
            record = rightRecord;
            return true;
        }

        return false;
    }

    bool BVH::GetBounds(AABB &outbox) const {
        outbox = m_Box;
        return true;
    }

    BVH::BVH(const std::unordered_map<uint32_t, Object *> &Objects, size_t start, size_t end) {
        int axis = RandomInt(0, 2);
        auto comparator
                        = (axis == 0) ? box_x_compare
                        : (axis == 1) ? box_y_compare
                        : box_z_compare;

        size_t object_span = end - start;

        if (object_span == 1) {
            m_Left = m_Right = Objects.at(start);
        } else if (object_span == 2) {
            if (comparator(Objects.at(start), Objects.at(start + 1))) {
                m_Left = Objects.at(start);
                m_Right = Objects.at(start + 1);
            } else {
                m_Left = Objects.at(start + 1);
                m_Right = Objects.at(start);
            }
        } else {
            std::vector<Object *> objects;
            objects.reserve(object_span);
            for (size_t i = start; i < end; i++) {
                objects.push_back(Objects.at(i));
            }

            std::sort(std::execution::par, objects.begin(), objects.end(), comparator);

            size_t mid = start + object_span / 2;
            m_Left = new BVH(Objects, start, mid);
            m_Right = new BVH(Objects, mid, end);
        }

        AABB box_left, box_right;

        if (!m_Left->GetBounds(box_left) || !m_Right->GetBounds(box_right)) {
            std::cerr << "No bounding box in BVH constructor" << std::endl;
        }

        m_Box = AABB::SurroundingBox(box_left, box_right);
    }
} // LumenRender