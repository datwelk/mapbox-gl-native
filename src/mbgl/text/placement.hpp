#pragma once

#include <string>
#include <unordered_map>
#include <mbgl/util/chrono.hpp>
#include <mbgl/text/collision_index.hpp>

namespace mbgl {

    class RenderSymbolLayer;

    class OpacityState {
        public:
            OpacityState(float targetOpacity);
            OpacityState(OpacityState& prevOpacityState, float increment, float targetOpacity);
            float opacity;
            float targetOpacity;
    };

    class JointOpacityState {
        public:
            JointOpacityState(float iconOpacity, float textOpacity);
            JointOpacityState(JointOpacityState& prevOpacityState, float increment, float iconOpacity, float textOpacity);
            OpacityState icon;
            OpacityState text;
    };

    class Placement {
        public:
            Placement(const TransformState&);

            void placeLayer(RenderSymbolLayer&);

            void commit(TimePoint);

            JointOpacityState getOpacity(std::string& crossTileSymbolID) const;

        private:
            CollisionIndex collisionIndex;
            TimePoint commitTime;
            std::unordered_map<std::string,JointOpacityState> opacities;
    };
} // namespace mbgl
