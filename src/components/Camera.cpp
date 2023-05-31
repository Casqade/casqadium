#include <cqde/components/Camera.hpp>
#include <cqde/components/Transform.hpp>

#include <cqde/assert.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/conversion/json_glm_vec4.hpp>

#include <entt/entity/registry.hpp>

#include <glad/gl.h>

#include <glm/gtc/type_ptr.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>


namespace cqde::compos
{

Camera::Camera(
  const glm::vec4& _viewport,
  const Projection _projectionType )
  : viewport(_viewport)
  , projectionType(_projectionType)
{}

glm::vec4
Camera::viewportScaled(
  const glm::vec2& targetSize ) const
{
  if ( aspectRatio > 0.0f )
  {
    switch (aspectRatioBehaviour)
    {
      case AspectRatioBehaviour::Keep:
      {
        const auto ratio = std::min(
          targetSize.x / aspectRatio, targetSize.y );

        const auto width = viewport.z * aspectRatio * ratio;
        const auto height = viewport.w * ratio;

        const auto offsetX = (targetSize.x - aspectRatio * ratio) * 0.5f;
        const auto offsetY = (targetSize.y - ratio) * 0.5f;

        return
        {
          offsetX + viewport.x * aspectRatio * ratio,
          offsetY + viewport.y * ratio,
          width,
          height,
        };
      }

      default:
        break;
    }
  }

  return viewport * glm::vec4
  {
    targetSize.x, targetSize.y,
    targetSize.x, targetSize.y,
  };
}

glm::mat4
Camera::projMatrix(
  const glm::vec2& targetSize ) const
{
  const auto viewport = viewportScaled(targetSize);

  auto ratio = viewport.z / viewport.w;

  if ( aspectRatioBehaviour == AspectRatioBehaviour::Stretch )
    ratio = aspectRatio * this->viewport.z / this->viewport.w;

  if ( projectionType == Projection::Perspective )
    return glm::perspectiveFov(
      fov, ratio, 1.0f,
      zRange.first, zRange.second );

  return glm::ortho(
    -ratio * scale, ratio * scale,
    -scale, scale,
    zRange.first, zRange.second );
}

glm::mat4
Camera::viewMatrix(
  const entt::registry& registry,
  const entt::entity    entity,
  const Transform&      cTransform ) const
{
  return glm::inverse(GetWorldMatrix(registry, entity, cTransform));
}


std::string
Camera::AspectRatioBehaviourToString(
  const Camera::AspectRatioBehaviour type )
{
  using ProjectionType = Camera::Projection;

  switch (type)
  {
    case AspectRatioBehaviour::Keep:
      return "keep";

    case AspectRatioBehaviour::Stretch:
      return "stretch";

    case AspectRatioBehaviour::Native:
      return "native";
  }

  CQDE_ASSERT_DEBUG(false, return "");
}

Camera::AspectRatioBehaviour
Camera::AspectRatioBehaviourFromString(
  const std::string& type )
{
  using fmt::format;
  using ProjectionType = Camera::Projection;

  if ( type == AspectRatioBehaviourToString(AspectRatioBehaviour::Keep) )
    return AspectRatioBehaviour::Keep;

  if ( type == AspectRatioBehaviourToString(AspectRatioBehaviour::Stretch) )
    return AspectRatioBehaviour::Stretch;

  if ( type == AspectRatioBehaviourToString(AspectRatioBehaviour::Native) )
    return AspectRatioBehaviour::Native;

  throw std::runtime_error(
    format("'{}' is not a valid camera aspect ratio behaviour", type));
}

std::string
Camera::ProjectionTypeToString(
  const Camera::Projection type )
{
  using ProjectionType = Camera::Projection;

  switch (type)
  {
    case ProjectionType::Orthographic:
      return "orthographic";

    case ProjectionType::Perspective:
      return "perspective";
  }

  CQDE_ASSERT_DEBUG(false, return "");
}

Camera::Projection
Camera::ProjectionTypeFromString(
  const std::string& type )
{
  using fmt::format;
  using ProjectionType = Camera::Projection;

  if ( type == ProjectionTypeToString(ProjectionType::Orthographic) )
    return ProjectionType::Orthographic;

  if ( type == ProjectionTypeToString(ProjectionType::Perspective) )
    return ProjectionType::Perspective;

  throw std::runtime_error(
    format("'{}' is not a valid camera projection type", type));
}

std::string
Camera::RenderModeToString(
  const Camera::RenderMode mode )
{
  using RenderMode = Camera::RenderMode;

  switch (mode)
  {
    case RenderMode::Solid:
      return "solid";

    case RenderMode::Wireframe:
      return "wireframe";
  }

  CQDE_ASSERT_DEBUG(false, return "");
}

Camera::RenderMode
Camera::RenderModeFromString(
  const std::string& mode )
{
  using fmt::format;
  using RenderMode = Camera::RenderMode;

  if ( mode == RenderModeToString(RenderMode::Solid) )
    return RenderMode::Solid;

  if ( mode == RenderModeToString(RenderMode::Wireframe) )
    return RenderMode::Wireframe;

  throw std::runtime_error(
    format("'{}' is not a valid camera render mode", mode));
}

std::string
Camera::TextureModeToString(
  const Camera::TextureMode mode )
{
  using RenderMode = Camera::RenderMode;

  switch (mode)
  {
    case TextureMode::Textured:
      return "textured";

    case TextureMode::NoTexture:
      return "noTexture";
  }

  CQDE_ASSERT_DEBUG(false, return "");
}

Camera::TextureMode
Camera::TextureModeFromString(
  const std::string& mode )
{
  using fmt::format;
  using RenderMode = Camera::RenderMode;

  if ( mode == TextureModeToString(TextureMode::Textured) )
    return TextureMode::Textured;

  if ( mode == TextureModeToString(TextureMode::NoTexture) )
    return TextureMode::NoTexture;

  throw std::runtime_error(
    format("'{}' is not a valid camera texture mode", mode));
}

std::string
Camera::LightingModeToString(
  const Camera::LightingMode type )
{
  using RenderMode = Camera::RenderMode;

  switch (type)
  {
    case LightingMode::Diffuse:
      return "diffuse";

    case LightingMode::FullBright:
      return "fullBright";
  }

  CQDE_ASSERT_DEBUG(false, return "");
}

Camera::LightingMode
Camera::LightingModeFromString(
  const std::string& mode )
{
  using fmt::format;
  using RenderMode = Camera::RenderMode;

  if ( mode == LightingModeToString(LightingMode::Diffuse) )
    return LightingMode::Diffuse;

  if ( mode == LightingModeToString(LightingMode::FullBright) )
    return LightingMode::FullBright;

  throw std::runtime_error(
    format("'{}' is not a valid camera render mode", mode));
}

const static Json::Value cameraJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  root["viewport"] = ValueType::arrayValue;
  root["viewport"].setComment("// 'viewport' must be a JSON array"s,
                              Json::CommentPlacement::commentBefore);

  root["viewport"].append(ValueType::realValue);
  root["viewport"].begin()->setComment("// 'viewport' element must be a JSON float"s,
                                        Json::CommentPlacement::commentBefore);

  root["z-range"] = ValueType::arrayValue;
  root["z-range"].setComment("// 'z-range' must be a JSON array"s,
                              Json::CommentPlacement::commentBefore);

  root["z-range"].append(ValueType::realValue);
  root["z-range"].begin()->setComment("// 'z-range' element must be a JSON float"s,
                                      Json::CommentPlacement::commentBefore);

  root["fov"] = ValueType::realValue;
  root["fov"].setComment("// 'fov' must be a JSON float"s,
                          Json::CommentPlacement::commentBefore);

  root["layer"] = ValueType::intValue;
  root["layer"].setComment("// 'layer' must be a JSON integer"s,
                          Json::CommentPlacement::commentBefore);

  root["projection"] = ValueType::stringValue;
  root["projection"].setComment("// 'projection' must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  root["renderMode"] = ValueType::stringValue;
  root["renderMode"].setComment("// 'renderMode' must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  root["textureMode"] = ValueType::stringValue;
  root["textureMode"].setComment("// 'textureMode' must be a JSON string"s,
                                  Json::CommentPlacement::commentBefore);

  root["lightingMode"] = ValueType::stringValue;
  root["lightingMode"].setComment("// 'lightingMode' must be a JSON string"s,
                                  Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
Camera::serialize() const
{
  Json::Value json {};

  json["viewport"] << viewport;
  json["z-range"].append(zRange.first);
  json["z-range"].append(zRange.second);

  if ( projectionType == Projection::Perspective )
    json["fov"] = glm::degrees(fov);
  else
    json["fov"] = fov;

  json["aspectRatio"] = aspectRatio;
  json["layer"] = layer;

  json["aspectRatioBehaviour"] = AspectRatioBehaviourToString(aspectRatioBehaviour);
  json["projection"] = ProjectionTypeToString(projectionType);
  json["renderMode"] = RenderModeToString(renderMode);
  json["textureMode"] = TextureModeToString(textureMode);
  json["lightingMode"] = LightingModeToString(lightingMode);

  return json;
}

void
Camera::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  using fmt::format;

  jsonValidateObject(json, cameraJsonReference);

  if ( json["z-range"].size() != 2 )
    throw std::runtime_error(
      "'z-range' must contain 2 elements");

  auto& comp = registry.emplace_or_replace <Camera> (entity);

  comp.aspectRatioBehaviour = AspectRatioBehaviourFromString( json["aspectRatioBehaviour"].asString() );
  comp.projectionType = ProjectionTypeFromString( json["projection"].asString() );
  comp.renderMode = RenderModeFromString( json["renderMode"].asString() );
  comp.textureMode = TextureModeFromString( json["textureMode"].asString() );
  comp.lightingMode = LightingModeFromString( json["lightingMode"].asString() );

  if ( comp.projectionType == Projection::Perspective )
    comp.fov = glm::radians(json["fov"].asFloat());
  else
    comp.fov = json["fov"].asFloat();

  comp.aspectRatio = json["aspectRatio"].asFloat();
  comp.layer = json["layer"].asInt();

  comp.zRange.first = json["z-range"][0].asFloat();
  comp.zRange.second = json["z-range"][1].asFloat();

  try
  {
    comp.viewport << json["viewport"];
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("'viewport' parse error: {}",
              e.what()));
  }

//  todo: render, texture & lighting modes
}

} // namespace cqde::compos
