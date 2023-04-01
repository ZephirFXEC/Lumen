#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include <glm/glm.hpp>
#include "Ray.hpp"


namespace LumenRender
{

    struct alignas(16) Vertex {
        float x, y, z;
    }

    struct alignas(16) Tri {
        Vertex v0, v1, v2;
    };

    class Triangle {
    public:
        Triangle() = default;
        Triangle(Tri tri) : m_Tri(tri) {}
        ~Triangle() = default;

        bool Intersect(const Ray& ray, HitRecords& hit) const;


    private:
        Tri m_Tri;
    };
    
} // namespace LumenRender




#endif