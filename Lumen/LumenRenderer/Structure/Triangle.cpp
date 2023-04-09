// Copyright (c) 2023.
// Enzo Crema
// All rights reserved

//
// Created by Enzo on 06/04/2023.
//

#include "Triangle.hpp"

namespace LumenRender {
    Triangle::Triangle(Triangle *triangle) {
        triangle->g_scene = nullptr;
        triangle->face_colors = nullptr;
        triangle->vertex_colors = nullptr;
    }

    Triangle::~Triangle() {
        rtcReleaseScene(g_scene);
        delete[] face_colors;
        delete[] vertex_colors;
    }


} // LumenRender