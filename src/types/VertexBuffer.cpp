#include <cqde/types/VertexBuffer.hpp>

#include <glad/gl.h>


namespace cqde::types
{

VertexBuffer::VertexBuffer(
  VertexBuffer&& other )
{
  *this = std::move(other);
}

VertexBuffer&
VertexBuffer::operator = (
  VertexBuffer&& other )
{
  std::swap(vertices, other.vertices);
  std::swap(indices, other.indices);

  std::swap(vao, other.vao);
  std::swap(vbo, other.vbo);
  std::swap(ebo, other.ebo);

  return *this;
}

void
VertexBuffer::create()
{
  vbo.create(
    sizeof(decltype(vertices)::value_type) * vertices.size(),
    GlBuffer::ImmutableStorageFlags(),
    vertices.data() );

  ebo.create(
    sizeof(decltype(indices)::value_type) * indices.size(),
    GlBuffer::ImmutableStorageFlags(),
    indices.data() );

  vao.create();

  vao.attachBuffer( vbo, 0, 0,
    sizeof(decltype(vertices)::value_type) );

  vao.enableAttribute(0);
  vao.setAttributeFormat( 0,
    decltype(vertices)::value_type::length(),
    0, GL_FLOAT );

  vao.attachElementBuffer(ebo);
}

void
VertexBuffer::destroy()
{
  if ( vao.isValid() == true )
    vao.destroy();

  if ( vbo.isValid() == true )
    vbo.destroy();

  if ( ebo.isValid() == true )
    ebo.destroy();
}

} // namespace cqde::types
