#pragma once

#include <cqde/types/graphics/GlBuffer.hpp>

#include <vector>


namespace cqde::types
{

template <typename Element>
size_t BufferSize(
  const std::vector <Element>& data )
{
  return sizeof(Element) * data.size();
}

class GlVertexArray
{
  using GLuint = uint32_t;
  using GLenum = uint32_t;

  std::vector <GLuint> mBindings {};

  GLuint mId {};

public:
  enum class AttributeType
  {
    Float,
    Double,
    Integer,
  };

  GlVertexArray() = default;

  GlVertexArray( GlVertexArray&& );
  GlVertexArray& operator = ( GlVertexArray&& );

  GlVertexArray( const GlVertexArray& ) = delete;
  GlVertexArray& operator = ( const GlVertexArray& ) = delete;

  void create();
  void destroy();

  void bind() const;
  void unbind() const;

  void attachElementBuffer(
    const GlBuffer& );

  void detachElementBuffer();

  void attachBuffer(
    const GlBuffer&,
    const size_t attribIndex,
    const size_t offset,
    const size_t stride );

  void detachBuffer(
    const GlBuffer& );

  void setAttributeFormat(
    const size_t attribIndex,
    const size_t componentsPerVertex,
    const size_t offset,
    const GLenum dataType,
    const AttributeType = AttributeType::Float,
    const bool normalize = false ) const;

  void setBindingDivisor(
    const GlBuffer&,
    const size_t divisor ) const;

  void enableAttribute( const size_t index ) const;
  void disableAttribute( const size_t index ) const;

  bool isValid() const;

  GLuint id() const;
};

} // namespace cqde::types
