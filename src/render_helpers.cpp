#include <cqde/render_helpers.hpp>
#include <cqde/string_helpers.hpp>
#include <cqde/math_helpers.hpp>

#include <olcPGE/Font.hpp>

#include <glm/gtx/intersect.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/ext/matrix_projection.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <unordered_set>


namespace cqde
{

static bool
vertexClip(
  const std::vector <glm::vec3>& vertices,
  std::vector <glm::vec3>& verticesProjected,
  std::unordered_set <size_t>& clippedIndices,
  const size_t vertexSourceIndex,
  const size_t vertexToClipIndex,
  const glm::mat4& modelView,
  const glm::mat4& projection,
  const glm::vec4& viewport,
  const float zNear )
{
  if ( clippedIndices.count(vertexSourceIndex) != 0 )
    return false;

  if ( clippedIndices.count(vertexToClipIndex) != 0 )
    return true;

  if (  verticesProjected[vertexToClipIndex].z >= 0.0f &&
        verticesProjected[vertexToClipIndex].z <= 1.0f )
    return true;

  if ( verticesProjected[vertexSourceIndex].z < 0.0f ||
       verticesProjected[vertexSourceIndex].z > 1.0f )
    return false;

  const auto& vertexSource = vertices[vertexSourceIndex];
  const auto& vertexToClip = vertices[vertexToClipIndex];

  const glm::vec3 p0 = glm::translate(modelView, vertexSource)[3];
  const glm::vec3 p1 = glm::translate(modelView, vertexToClip)[3];

  const auto line = p1 - p0;
  const auto direction = glm::normalize(line);

  float distance {};

  const float zNearEpsilon = zNear + glm::epsilon <float> ();

  const bool intersects =
    glm::intersectRayPlane(p0, direction,
                           glm::vec3{glm::vec2{}, -zNearEpsilon},
                           glm::vec3{glm::vec2{}, -1.0f},
                           distance);

  if ( intersects == false )
    return false;

  if ( distance <= 0.0f )
    return false;

  const auto vertexClipped = p0 + (direction * distance);

  const auto vertexProjected =
    glm::project(vertexClipped, glm::mat4{1.0f},
                 projection, viewport);

  if (  vertexProjected.z < 0.0f ||
        vertexProjected.z > 1.0f )
    return false;

  const glm::vec2 lineProjected = vertexProjected - verticesProjected[vertexSourceIndex];

  const auto lengthFull = glm::length(line);
  const auto lengthClipped = glm::distance(p0, vertexClipped);

  const auto lengthCoef = lengthFull / lengthClipped;

  verticesProjected[vertexToClipIndex] = {lineProjected * lengthCoef, vertexProjected.z};

  clippedIndices.insert(vertexToClipIndex);

  return true;
}

std::shared_ptr <olc::Renderable>
textureFromText(
  const std::string& text,
  const olc::Pixel& colorFg,
  const olc::Pixel& colorBg,
  const bool monospaced )
{
  const auto textSize
    = monospaced
    ? olc::Sprite::GetTextSize(text)
    : olc::Sprite::GetTextSizeProp(text);

  auto texture = std::make_shared <olc::Renderable> ();
  texture->Create(textSize.x, textSize.y);

  auto sprite = texture->Sprite();

  sprite->Clear(colorBg);

  monospaced ?
      sprite->DrawString({0, 0}, text, colorFg)
    : sprite->DrawString({0, 0}, text, colorFg);

  texture->Decal()->Update();

  return texture;
}

std::shared_ptr <olc::Renderable>
textureFromText(
  const std::string& text,
  olc::Font* font,
  const olc::Pixel& color,
  const bool antialiased )
{
  auto renderable = font->RenderStringToRenderable(stringFromUTF8(text), color, antialiased);

  return std::make_shared <olc::Renderable> (std::move(renderable));
}

olc::Sprite
spriteFromText(
  const std::string& text,
  const olc::Pixel& colorFg,
  const olc::Pixel& colorBg,
  const bool monospaced )
{
  const auto textSize
    = monospaced
    ? olc::Sprite::GetTextSize(text)
    : olc::Sprite::GetTextSizeProp(text);

  olc::Sprite sprite {textSize.x, textSize.y};

  sprite.Clear(colorBg);

  monospaced ?
      sprite.DrawString({0, 0}, text, colorFg)
    : sprite.DrawStringProp({0, 0}, text, colorFg);

  return sprite;
}

olc::Sprite
spriteFromText(
  const std::string& text,
  olc::Font* font,
  const olc::Pixel& color,
  const bool antialiased )
{
  auto sprite = font->RenderStringToSprite(stringFromUTF8(text), color, antialiased);

  olc::Sprite result = std::move(*sprite);

  delete sprite;

  return result;
}

} // namespace cqde
