#include <cqde/types/graphics/Mesh.hpp>

#include <glad/gl.h>


namespace cqde::types
{

void
Mesh::create()
{
  glCreateBuffers(1, &ebo);
  glNamedBufferStorage(ebo,
    sizeof(decltype(indices)::value_type) * indices.size(),
    indices.data(),
    GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT );

  glCreateBuffers(1, &vboVertices);
  glNamedBufferStorage(vboVertices,
    sizeof(decltype(vertices)::value_type) * vertices.size(),
    vertices.data(),
    GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT );

  glCreateBuffers(1, &vboNormals);
  glNamedBufferStorage(vboNormals,
    sizeof(decltype(normals)::value_type) * normals.size(),
    normals.data(),
    GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT );

  glCreateBuffers(1, &vboTexCoords);
  glNamedBufferStorage(vboTexCoords,
    sizeof(decltype(texCoords)::value_type) * texCoords.size(),
    texCoords.data(),
    GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT );

  glCreateVertexArrays(1, &vao);

  if ( GLAD_GL_ARB_multi_bind != 0 )
  {
    const GLuint buffers []
    {
      vboVertices,
      vboNormals,
      vboTexCoords,
    };

    const GLintptr offsets []
    {
      0, 0, 0,
    };

    const GLint strides []
    {
      0, 0, 0,
    };

    glVertexArrayVertexBuffers(
      vao, 0, sizeof(buffers) / sizeof(buffers[0]),
      buffers, offsets,
      strides );
  }
  else
  {
    glVertexArrayVertexBuffer(vao, 0, vboVertices, 0, 0);
    glVertexArrayVertexBuffer(vao, 1, vboNormals, 0, 0);
    glVertexArrayVertexBuffer(vao, 2, vboTexCoords, 0, 0);
  }

  glVertexArrayElementBuffer(vao, ebo);

  glEnableVertexArrayAttrib(vao, 0);
  glEnableVertexArrayAttrib(vao, 1);
  glEnableVertexArrayAttrib(vao, 2);

  glVertexArrayAttribFormat(vao,
    0, sizeof(decltype(vertices)::value_type),
    GL_FLOAT, GL_FALSE, 0 );

  glVertexArrayAttribFormat(vao,
    1, sizeof(decltype(normals)::value_type),
    GL_FLOAT, GL_FALSE, 0 );

  glVertexArrayAttribFormat(vao,
    2, sizeof(decltype(texCoords)::value_type),
    GL_FLOAT, GL_FALSE, 0 );

  glVertexArrayAttribBinding(vao, 0, 0);
  glVertexArrayAttribBinding(vao, 1, 1);
  glVertexArrayAttribBinding(vao, 2, 2);
}

} // namespace cqde::types
