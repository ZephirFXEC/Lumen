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


    TriangleMesh::TriangleMesh(const uint32_t nfaces,
                               const std::unique_ptr<uint32_t[]> &faceIndex,
                               const std::unique_ptr<uint32_t[]> &vertsIndex,
                               const std::unique_ptr<glm::vec3[]> &verts,
                               std::unique_ptr<glm::vec3[]> &normals,
                               std::unique_ptr<glm::vec2[]> &st): numTris(nfaces) {

        uint32_t k = 0, maxVertIndex = 0;
        // find out how many triangles we need to create for this mesh
        for (uint32_t i = 0; i < nfaces; ++i) {
            numTris += faceIndex[i] - 2;
            for (uint32_t j = 0; j < faceIndex[i]; ++j)
                if (vertsIndex[k + j] > maxVertIndex)
                    maxVertIndex = vertsIndex[k + j];
            k += faceIndex[i];
        }
        maxVertIndex += 1;

        // allocate memory to store the position of the mesh vertices
        P = std::unique_ptr<glm::vec3 []>(new glm::vec3[maxVertIndex]);
        for (uint32_t i = 0; i < maxVertIndex; ++i) {
            P[i] = verts[i];
        }

        // allocate memory to store triangle indices
        trisIndex = std::unique_ptr<uint32_t []>(new uint32_t [numTris * 3]);
        uint32_t l = 0;

        N = std::unique_ptr<glm::vec3 []>(new glm::vec3[numTris * 3]);
        texCoordinates = std::unique_ptr<glm::vec2 []>(new glm::vec2[numTris * 3]);
        for (uint32_t i = 0, k = 0; i < nfaces; ++i) {  //for each  face
            for (uint32_t j = 0; j < faceIndex[i] - 2; ++j) {  //for each triangle in the face
                trisIndex[l] = vertsIndex[k];
                trisIndex[l + 1] = vertsIndex[k + j + 1];
                trisIndex[l + 2] = vertsIndex[k + j + 2];
                N[l] = normals[k];
                N[l + 1] = normals[k + j + 1];
                N[l + 2] = normals[k + j + 2];
                texCoordinates[l] = st[k];
                texCoordinates[l + 1] = st[k + j + 1];
                texCoordinates[l + 2] = st[k + j + 2];
                l += 3;
            }
            k += faceIndex[i];
        }
        // you can use move if the input geometry is already triangulated
        //N = std::move(normals); // transfer ownership
        //sts = std::move(st); // transfer ownership
    }

    bool TriangleMesh::Intersect(const Ray& ray, HitRecords& hit) const {
        bool hitSomething = false;

        uint32_t j = 0;

        for (uint32_t i = 0; i < numTris; ++i) {

            HitRecords rec{};

            const glm::vec3 &v0 = P[trisIndex[j]];
            const glm::vec3 &v1 = P[trisIndex[j + 1]];
            const glm::vec3 &v2 = P[trisIndex[j + 2]];

            float u, v, t;

            if (TriangleIntersect(ray, v0, v1, v2, &u, &v, &t)) {
                rec.m_T = t;
                rec.m_Position = ray.At(rec.m_T);
                hitSomething = true;
                if (rec.m_T < hit.m_T) {
                    hit = rec;
                }
            }
        }

        return hitSomething;
    }



    TriangleMesh* generatePolySphere(float rad, uint32_t divs) {
        // generate points
        uint32_t numVertices = (divs - 1) * divs + 2;
        std::unique_ptr<glm::vec3 []> P(new glm::vec3[numVertices]);
        std::unique_ptr<glm::vec3 []> N(new glm::vec3[numVertices]);
        std::unique_ptr<glm::vec2 []> st(new glm::vec2[numVertices]);

        float u = -M_PI_2;
        float v = -M_PI;
        float du = M_PI / divs;
        float dv = 2 * M_PI / divs;

        P[0] = N[0] = glm::vec3(0, -rad, 0);
        uint32_t k = 1;
        for (uint32_t i = 0; i < divs - 1; i++) {
            u += du;
            v = -M_PI;
            for (uint32_t j = 0; j < divs; j++) {
                float x = rad * cos(u) * cos(v);
                float y = rad * sin(u);
                float z = rad * cos(u) * sin(v) ;
                P[k] = N[k] = glm::vec3(x, y, z);
                st[k].x = u / M_PI + 0.5;
                st[k].y = v * 0.5 / M_PI + 0.5;
                v += dv, k++;
            }
        }
        P[k] = N[k] = glm::vec3(0, rad, 0);

        uint32_t npolys = divs * divs;
        std::unique_ptr<uint32_t []> faceIndex(new uint32_t[npolys]);
        std::unique_ptr<uint32_t []> vertsIndex(new uint32_t[(6 + (divs - 1) * 4) * divs]);

        // create the connectivity lists
        uint32_t vid = 1, numV = 0, l = 0;
        k = 0;
        for (uint32_t i = 0; i < divs; i++) {
            for (uint32_t j = 0; j < divs; j++) {
                if (i == 0) {
                    faceIndex[k++] = 3;
                    vertsIndex[l] = 0;
                    vertsIndex[l + 1] = j + vid;
                    vertsIndex[l + 2] = (j == (divs - 1)) ? vid : j + vid + 1;
                    l += 3;
                }
                else if (i == (divs - 1)) {
                    faceIndex[k++] = 3;
                    vertsIndex[l] = j + vid + 1 - divs;
                    vertsIndex[l + 1] = vid + 1;
                    vertsIndex[l + 2] = (j == (divs - 1)) ? vid + 1 - divs : j + vid + 2 - divs;
                    l += 3;
                }
                else {
                    faceIndex[k++] = 4;
                    vertsIndex[l] = j + vid + 1 - divs;
                    vertsIndex[l + 1] = j + vid + 1;
                    vertsIndex[l + 2] = (j == (divs - 1)) ? vid + 1 : j + vid + 2;
                    vertsIndex[l + 3] = (j == (divs - 1)) ? vid + 1 - divs : j + vid + 2 - divs;
                    l += 4;
                }
                numV++;
            }
            vid = numV;
        }

        return new TriangleMesh(npolys, faceIndex, vertsIndex, P, N, st);
    }

    bool Triangle::Intersect(const Ray &ray, HitRecords &hit) const {
        float u, v, t;
        if (TriangleIntersect(ray, m_Vertices[0], m_Vertices[1], m_Vertices[2], &u, &v, &t)) {
            hit.m_T = t;
            hit.m_Position = ray.At(hit.m_T);
            return true;
        }
        return false;
    }
} // LumenRender