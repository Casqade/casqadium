#include <cqde/types/graphics/MeshStorage.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <glad/gl.h>

#include <cstring>


namespace cqde::types
{

MeshStorage::MeshStorage()
{
  vao.create();

  vao.enableAttribute(0);
  vao.setAttributeFormat(
    0, glm::vec3::length(),
    0, GL_FLOAT );

  vao.enableAttribute(1);
  vao.setAttributeFormat(
    1, glm::vec2::length(),
    0, GL_FLOAT );

  resize(4, 6);
}

MeshStorage::~MeshStorage()
{
  elementBuffer.destroy();
  texCoordsBuffer.destroy();
  verticesBuffer.destroy();

  vao.destroy();
}

void
MeshStorage::resize(
  const size_t newVertexCount,
  const size_t newElementCount )
{
  if (  newElementCount <= elementCount &&
        newVertexCount <= vertexCount )
    return;

  vertexCount = newVertexCount;
  elementCount = newElementCount;

  const auto verticesSize = vertexCount * sizeof(glm::vec3);
  const auto texCoordsSize = vertexCount * sizeof(glm::vec2);
  const auto indicesSize = elementCount * sizeof(uint32_t);
  const auto verticesStride = sizeof(glm::vec3);
  const auto texCoordsStride = sizeof(glm::vec2);

  if ( texCoordsBuffer.isValid() == true )
    vao.detachBuffer(texCoordsBuffer);

  if ( verticesBuffer.isValid() == true )
    vao.detachBuffer(verticesBuffer);

  resizeBuffer( verticesBuffer,
    0, 0, verticesSize, verticesStride );

  resizeBuffer( texCoordsBuffer,
    1, 1, texCoordsSize, texCoordsStride );

  GlBuffer newElementBuffer {};

  newElementBuffer.create(
    indicesSize,
    GlBuffer::MutableAccessFlags() );

  newElementBuffer.map(
    0, indicesSize,
    GlBuffer::MutableAccessFlags() );

  if ( elementBuffer.isValid() == true )
  {
    std::memcpy(
      newElementBuffer.data(),
      elementBuffer.data(),
      elementBuffer.size() );

    vao.detachElementBuffer();
    elementBuffer.destroy();
  }

  elementBuffer = std::move(newElementBuffer);
  vao.attachElementBuffer(elementBuffer);
}

void
MeshStorage::resizeBuffer(
  GlBuffer& buffer,
  const size_t attribIndex,
  const size_t bindingIndex,
  const size_t bufferSize,
  const size_t stride )
{
  GlBuffer newBuffer {};

  newBuffer.create(
    bufferSize,
    GlBuffer::MutableAccessFlags() );

  newBuffer.map(
    0, bufferSize,
    GlBuffer::MutableAccessFlags() );

  if ( buffer.isValid() == true )
  {
    std::memcpy(
      newBuffer.data(),
      buffer.data(),
      buffer.size() );

    buffer.destroy();
  }

  buffer = std::move(newBuffer);

  vao.attachBuffer( buffer,
    attribIndex, bindingIndex, 0, stride );
}

} // namespace cqde::types
