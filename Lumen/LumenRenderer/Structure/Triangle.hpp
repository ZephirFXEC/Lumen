// Copyright (c) 2023.
// Enzo Crema
// All rights reserved

//
// Created by Enzo on 06/04/2023.
//

#ifndef LUMEN_TRIANGLE_HPP
#define LUMEN_TRIANGLE_HPP

#include <embree4/rtcore.h>
#include <glm/glm.hpp>


namespace LumenRender {

    struct Triangle {

        Triangle() = default;

        explicit Triangle(Triangle *triangle);

        ~Triangle();

        RTCScene g_scene;
        glm::vec3 *face_colors;
        glm::vec3 *vertex_colors;
    };


} // LumenRender

#endif //LUMEN_TRIANGLE_HPP
