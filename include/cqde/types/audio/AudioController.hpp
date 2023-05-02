#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <soloud.h>

#include <json/forwards.h>

#include <unordered_map>


namespace cqde::types
{

Json::Value&
operator << ( Json::Value&, const SoLoud::Fader& );

SoLoud::Fader&
operator << ( SoLoud::Fader&, const Json::Value& );

Json::Value&
operator << ( Json::Value&, const SoLoud::FilterInstance& );

SoLoud::FilterInstance&
operator << ( SoLoud::FilterInstance&, const Json::Value& );

} // namespace cqde::types
