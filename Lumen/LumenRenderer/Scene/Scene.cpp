//
// Created by enzoc on 02/10/2022.
//

#include "Scene.hpp"

namespace LumenRender {


    auto Scene::Hit(Ray &ray, float t_max) const -> bool {
        Ray temp = ray;
        bool hit_anything = false;
        float closest_so_far = t_max;

        for (const auto &[index, object]: m_Objects) {
          /*
                      auto hit = std::visit([&](auto&& arg) -> bool {
                          return arg->Hit(temp, closest_so_far) && temp.m_Record.m_T < closest_so_far;
                      }, object);
          */
          bool hit = object->Hit(temp, closest_so_far) && temp.m_Record.m_T < closest_so_far;
          if (hit) {
            hit_anything = true;
            closest_so_far = temp.m_Record.m_T;
            ray = temp;
          }
        }

        return hit_anything;
    }

    auto Scene::GetBounds(AABB &outbox) const -> AABB
    {
      AABB temp_box;
      bool first_box = true;

      if (m_Objects.empty()) return outbox;

      for (const auto &[index, object] : m_Objects) {
        outbox = first_box ? temp_box : AABB::Union(outbox, temp_box);
        first_box = false;
      }

      /*
      for (const auto &[index, object]: m_Objects) {
          auto box = std::visit([&](auto&& arg) -> AABB {
              return arg->GetBounds(temp_box);
          }, object);

          outbox = first_box ? box : AABB::Union(outbox, box);
          first_box = false;
      } */

      return outbox;
    }

    auto Scene::DeepCopy() const -> std::shared_ptr<IHittable> {
        auto scene = std::make_shared<Scene>();
        /*
                for (const auto &[index, object]: m_Objects) {
                    scene->AddObject(object);
                }
        */
        return scene;
    }


} // LumenRender