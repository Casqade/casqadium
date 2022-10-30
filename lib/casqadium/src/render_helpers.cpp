#include <cqde/render_helpers.hpp>
#include <cqde/math_helpers.hpp>

#include <olcPGE/olcPGEX_TTF.hpp>

#include <glm/gtx/intersect.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/ext/matrix_projection.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <unordered_set>


namespace cqde
{

types::VertexBuffer::WindingOrder
windingOrder(
  const std::vector <olc::vf2d>& vertices,
  const bool yAxisUp )
{
  using WindingOrder = types::VertexBuffer::WindingOrder;

  float area = {};

  for ( size_t i = 0, iNext = 1;
        i < vertices.size();
        ++i, iNext = (i + 1) % vertices.size() )
    area += (vertices[iNext].x - vertices[i].x)
          * (vertices[iNext].y + vertices[i].y);

  return WindingOrder{yAxisUp ? area > 0.0f : area < 0.0f};
}

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

/// Approximates clipped points
/// based on their non-clipped neighbours
types::VertexBuffer
vertexShader(
  const std::vector <glm::vec3>& vertices,
  const glm::mat4& modelView,
  const glm::mat4& projection,
  const glm::vec4& viewport,
  const float zNear )
{
  using types::VertexBuffer;

  if ( vertices.size() == 0 )
    return {{}, -1.0f};

  std::vector <glm::vec3> verticesProjected (4);
  std::unordered_set <size_t> clippedIndices {};

  for ( size_t i = 0; i < vertices.size(); ++i )
  {
    auto& vertex = verticesProjected[i];
    vertex = glm::project(vertices[i], modelView,
                          projection, viewport);

//    valid vertex depth
    if ( vertex.z >= 0.0f &&
         vertex.z <= 1.0f )
    {
      if ( i == 0 )
        continue;

      if (  vertexClip( vertices, verticesProjected,
                        clippedIndices, i, i - 1,
                        modelView, projection,
                        viewport, zNear ) == false )
        return {{}, -1.0f};

      if ( i + 1 != vertices.size() )
        continue;

      if (  vertexClip( vertices, verticesProjected,
                        clippedIndices, i, 0,
                        modelView, projection,
                        viewport, zNear ) == false )
        return {{}, -1.0f};

      continue;
    }

    vertex.z = -1.0f * vertices.size();

//    invalid vertex depth
    if ( i == 0 )
      continue;

    if (  vertexClip( vertices, verticesProjected,
                      clippedIndices, i - 1, i,
                      modelView,
                      projection,
                      viewport, zNear ) == true )
      continue;

    if ( i + 1 != vertices.size() )
      continue;

    vertexClip( vertices, verticesProjected,
                clippedIndices, 0, i,
                modelView, projection,
                viewport, zNear );
  }

  VertexBuffer vb {};
  vb.vertices.reserve(verticesProjected.size());

  for ( const auto& vertex : verticesProjected )
  {
    vb.depth += vertex.z;
    vb.vertices.push_back({ vertex.x, // converting y axis to the top-left origin
                            viewport.y * 2 + viewport.w - vertex.y });
  }

  vb.depth /= vb.vertices.size();

  const Rect bb = boundingBox(vb.vertices);

  const Rect vp
  {
    .left = viewport.x,
    .right = viewport.x + viewport.z,
    .top = viewport.y,
    .bottom = viewport.y + viewport.w
  };

  if ( rectsIntersect(bb, vp) == false )
    return {{}, -1.0f};

  vb.windingOrder = windingOrder(vb.vertices);

  return vb;
}

/// Discards the buffer completely
/// if any vertex gets clipped
types::VertexBuffer
vertexShader(
  const std::vector <glm::vec3>& vertices,
  const glm::mat4& modelViewMatrix,
  const glm::mat4& projectionMatrix,
  const glm::vec4& viewport )
{
  using types::VertexBuffer;

  if ( vertices.size() == 0 )
    return {{}, -1.0f, {}};

  VertexBuffer vb{};
  vb.vertices.reserve(vertices.size());

  for ( const glm::vec3& srcVert : vertices )
  {
    const glm::vec3 vertex =
      glm::project( srcVert,
                    modelViewMatrix,
                    projectionMatrix,
                    viewport );

    if ( vertex.z < 0.0f ||
         vertex.z > 1.0f )
      return {{}, -1.0f, {}};

    vb.vertices.push_back({ vertex.x, // converting y axis to top-left origin
                            viewport.y * 2 + viewport.w - vertex.y });
    vb.depth += vertex.z;
  }

  const Rect bb = boundingBox(vb.vertices);

  const Rect vp
  {
    .left = viewport.x,
    .right = viewport.x + viewport.z,
    .top = viewport.y,
    .bottom = viewport.y + viewport.w
  };

  if ( rectsIntersect(bb, vp) == false )
    return {{}, -1.0f};

  vb.depth /= vb.vertices.size();

  vb.windingOrder = windingOrder(vb.vertices);

  return vb;
}

void drawLines(
  const std::vector <olc::vf2d>& vertices,
  const olc::Pixel& color,
  const LineRenderMode mode )
{
  auto pge = olc::renderer->ptrPGE;

  switch (mode)
  {
    case LineRenderMode::Strip:
    {
      for ( size_t i = 0, iNext = 1;
            iNext < vertices.size();
            ++i, ++iNext )
        pge->DrawLineDecal( vertices[i],
                            vertices[iNext],
                            color );
      return;
    }

    case LineRenderMode::Loop:
    {
      for ( size_t i = 0, iNext = 1;
            i < vertices.size();
            ++i, iNext = (i + 1) % vertices.size() )
        pge->DrawLineDecal( vertices[i],
                            vertices[iNext],
                            color );
      return;
    }
  }
}

std::shared_ptr <olc::Renderable>
textureFromText(
  const std::string& text,
  const olc::Pixel& color,
  const bool monospaced )
{
  olc::PixelGameEngine* pge = olc::renderer->ptrPGE;

  const auto textSize = monospaced ?
                          pge->GetTextSize(text)
                        : pge->GetTextSizeProp(text);

  auto texture = std::make_shared <olc::Renderable> ();
  texture->Create(textSize.x, textSize.y);

  const auto drawTargetPrev = pge->GetDrawTarget();

  pge->SetDrawTarget(texture->Sprite());

  monospaced ?
      pge->DrawString({0, 0}, text, color)
    : pge->DrawStringProp({0, 0}, text, color);

  pge->SetDrawTarget(drawTargetPrev);

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
  const std::u32string textU32 {text.begin(), text.end()};
  auto renderable = font->RenderStringToRenderable(textU32, color, antialiased);

  return std::make_shared <olc::Renderable> (std::move(renderable));
}

} // namespace cqde
