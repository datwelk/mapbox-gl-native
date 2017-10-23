#include <mbgl/text/placement.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>

namespace mbgl {

OpacityState::OpacityState(float targetOpacity_) : opacity(0), targetOpacity(targetOpacity_) {}

OpacityState::OpacityState(OpacityState& prevState, float increment, float targetOpacity) :
    opacity(std::fmax(0, std::fmin(1, prevState.opacity + prevState.targetOpacity == 1.0 ? increment : -increment))),
    targetOpacity(targetOpacity) {}

JointOpacityState::JointOpacityState(float iconOpacity_, float textOpacity_) :
    icon(OpacityState(iconOpacity_)),
    text(OpacityState(textOpacity_)) {}

JointOpacityState::JointOpacityState(JointOpacityState& prevOpacityState, float increment, float iconOpacity, float textOpacity) :
    icon(OpacityState(prevOpacityState.icon, increment, iconOpacity)),
    text(OpacityState(prevOpacityState.text, increment, textOpacity)) {}


Placement::Placement(const TransformState& transformState_) : collisionIndex(transformState_) {}

void Placement::placeLayer(RenderSymbolLayer&) {
    fprintf(stderr, "symbol layer\n");
}

void Placement::commit(TimePoint now) {
    commitTime = now;
}

JointOpacityState Placement::getOpacity(std::string& crossTileSymbolID) const {
    auto it = opacities.find(crossTileSymbolID);
    if (it != opacities.end()) {
        return it->second;
    } else {
        return JointOpacityState(0, 0);
    }

}

} // namespace mbgl
