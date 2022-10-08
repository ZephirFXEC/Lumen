//
// Created by enzoc on 06/10/2022.
//

#include "Object.hpp"

#include <glm/gtx/intersect.hpp>

#define M_PI 3.14159265358979323846
#define M_PI_2 6.28318530717958647692

#define EPSILON 0.000001
#define CROSS(dest,v1,v2) \
          dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
          dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
          dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
#define SUB(dest,v1,v2) \
          dest[0]=v1[0]-v2[0]; \
          dest[1]=v1[1]-v2[1]; \
          dest[2]=v1[2]-v2[2];


namespace LumenRender {


    bool TriangleIntersect(const Ray &ray, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float *u, float *v, float *t) {
        double edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
        double det,inv_det;

        /* find vectors for two edges sharing vert0 */
        SUB(edge1, v1, v0);
        SUB(edge2, v2, v0);

        /* begin calculating determinant - also used to calculate U parameter */
        CROSS(pvec, ray.Direction, edge2);

        /* if determinant is near zero, ray lies in plane of triangle */
        det = DOT(edge1, pvec);

        /* calculate distance from vert0 to ray origin */
        SUB(tvec, ray.Origin, v0);
        inv_det = 1.0 / det;

        CROSS(qvec, tvec, edge1);

        if (det > EPSILON)
        {
            *u = DOT(tvec, pvec);
            if (*u < 0.0 || *u > det)
                return false;

            /* calculate V parameter and test bounds */
            *v = DOT(ray.Direction, qvec);
            if (*v < 0.0 || *u + *v > det)
                return false;

        }
        else if(det < -EPSILON)
        {
            /* calculate U parameter and test bounds */
            *u = DOT(tvec, pvec);
            if (*u > 0.0 || *u < det)
                return 0;

            /* calculate V parameter and test bounds */
            *v = DOT(ray.Direction, qvec) ;
            if (*v > 0.0 || *u + *v < det)
                return false;
        }
        else return false;  /* ray is parallell to the plane of the triangle */

        *t = DOT(edge2, qvec) * inv_det;
        (*u) *= inv_det;
        (*v) *= inv_det;

        return true;
    }


} // LumenRender