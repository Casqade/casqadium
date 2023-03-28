#include <cqde/components/TextureBuffer.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <glad/gl.h>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value textureBufferJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& textures = root["texture"];
  textures = ValueType::stringValue;
  textures.setComment("// 'texture' must be a JSON string"s,
                      Json::CommentPlacement::commentBefore);

  return root;
}();

TextureBuffer::TextureBuffer()
{
  create();
}

TextureBuffer::TextureBuffer(
  const TextureBuffer& other )
{
  *this = other;
}

TextureBuffer::~TextureBuffer()
{
  --refCounter;

  if ( refCounter == 0 )
    destroy();
}

TextureBuffer&
TextureBuffer::operator = (
  const TextureBuffer& other )
{
  texture = other.texture;
  uvBuffer = other.uvBuffer;
  uv = other.uv;
  refCounter = ++other.refCounter;

  return *this;
}

void
TextureBuffer::create()
{
  glGenBuffers(1, &uv);
}

void
TextureBuffer::update()
{
  if ( uv == 0 )
    create();

  glBindBuffer(GL_ARRAY_BUFFER, uv);

  glBufferData(
    GL_ARRAY_BUFFER,
    uvBuffer.size() * sizeof(decltype(uvBuffer)::value_type),
    uvBuffer.data(), GL_STATIC_DRAW );
}

void
TextureBuffer::destroy()
{
  if ( uv == 0 )
    return;

  glDeleteBuffers(1, &uv);
  uv = 0;
}

Json::Value
TextureBuffer::serialize() const
{
  Json::Value json {};

  json["texture"] = texture.str();

  return json;
}

void
TextureBuffer::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
//  jsonValidateObject(json, textureBufferJsonReference);

  auto& comp = registry.emplace_or_replace <TextureBuffer> (entity);

  if ( json.isMember("texture") == true )
    comp.texture = json["texture"].asString();
  else if ( json["textures"].empty() == false )
    comp.texture = json["textures"].begin()->asString();

  comp.update();
}

} // namespace cqde::compos
