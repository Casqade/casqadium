#include <cqde/types/graphics/PrimaryRenderTarget.hpp>


namespace cqde::types
{

PrimaryRenderTarget::PrimaryRenderTarget()
{
  const std::vector <glm::vec2> verticesData
  {
    {-1.0f, -1.0f},
    {1.0f, -1.0f},
    {1.0f, 1.0f},
    {-1.0f, 1.0f},
  };

  const std::vector <glm::vec2> texCoordsData
  {
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f},
  };

  const auto verticesSize =
    sizeof(decltype(verticesData)::value_type) * verticesData.size();

  const auto texCoordsSize =
    sizeof(decltype(texCoordsData)::value_type) * texCoordsData.size();

  vertices.create( verticesSize,
    GlBuffer::ImmutableStorageFlags(),
    verticesData.data() );

  texCoords.create( texCoordsSize,
    GlBuffer::ImmutableStorageFlags(),
    texCoordsData.data() );

  const auto stride = sizeof(glm::vec2);

  vao.create();

  vao.enableAttribute(0);
  vao.setAttributeFormat(
    0, 2, 0, GL_FLOAT );

  vao.enableAttribute(1);
  vao.setAttributeFormat(
    1, 2, 0, GL_FLOAT );

  vao.attachBuffer(
    vertices, 0,
    0, stride );

  vao.attachBuffer(
    texCoords, 1,
    0, stride );
}

PrimaryRenderTarget::~PrimaryRenderTarget()
{
  vao.destroy();
  vertices.destroy();
  texCoords.destroy();
}

} // namespace cqde::types
