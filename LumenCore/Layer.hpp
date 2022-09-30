//
// Created by enzoc on 30/09/2022.
//

#ifndef LUMEN_LAYER_HPP
#define LUMEN_LAYER_HPP

namespace Lumen {
    class Layer {
    public:
        virtual ~Layer() = default;

        virtual void OnAttach() {}

        virtual void OnDetach() {}

        virtual void OnUIRender() {}
    };
}

#endif //LUMEN_LAYER_HPP