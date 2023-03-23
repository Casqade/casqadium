#include <cqde/types/graphics/GlVertexArray.hpp>

#include <cqde/common.hpp>

#include <glad/gl.h>


namespace cqde::types
{

GlVertexArray::GlVertexArray(
  GlVertexArray&& other )
{
  *this = std::move(other);
}

GlVertexArray&
GlVertexArray::operator = (
  GlVertexArray&& other )
{
  CQDE_ASSERT_DEBUG(isValid() == false, destroy());

  std::swap(mId, other.mId);

  return *this;
}

void
GlVertexArray::create()
{
  CQDE_ASSERT_DEBUG(isValid() == false, destroy());
  glCreateVertexArrays(1, &mId);
}

void
GlVertexArray::destroy()
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);

  glDeleteVertexArrays(1, &mId);
  mId = 0;
}

void
GlVertexArray::bind() const
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);
  glBindVertexArray(mId);
}

void
GlVertexArray::unbind() const
{
  glBindVertexArray(0);
}

void
GlVertexArray::attachElementBuffer(
  const GlBuffer& buffer )
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);
  CQDE_ASSERT_DEBUG(buffer.isValid() == true, return);

  glVertexArrayElementBuffer(mId, buffer.id());
}

void
GlVertexArray::detachElementBuffer()
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);

  glVertexArrayElementBuffer(mId, 0);
}

void
GlVertexArray::attachBuffer(
  const GlBuffer& buffer,
  const size_t attribIndex,
  const size_t offset,
  const size_t stride )
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);
  CQDE_ASSERT_DEBUG(buffer.isValid() == true, return);

  auto bindingIndex = std::find(
    mBindings.begin(),
    mBindings.end(),
    buffer.id() );

  bindingIndex = mBindings.emplace(bindingIndex, buffer.id());

  glVertexArrayAttribBinding( mId,
    attribIndex,
    std::distance(mBindings.begin(), bindingIndex) );

  glVertexArrayVertexBuffer( mId,
    std::distance(mBindings.begin(), bindingIndex),
    buffer.id(), offset, stride );
}

void
GlVertexArray::detachBuffer(
  const GlBuffer& buffer )
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);
  CQDE_ASSERT_DEBUG(buffer.isValid() == true, return);

  auto bindingIndex = std::find(
    mBindings.begin(),
    mBindings.end(),
    buffer.id() );

  CQDE_ASSERT_DEBUG(bindingIndex != mBindings.end(), return);

  glVertexArrayVertexBuffer( mId,
    std::distance(mBindings.begin(), bindingIndex),
    0, 0, 0 );

  mBindings.erase(bindingIndex);
}

void
GlVertexArray::setAttributeFormat(
  const size_t attribIndex,
  const size_t componentsPerVertex,
  const size_t offset,
  const GLenum dataType,
  const AttributeType attributeType,
  const bool normalize ) const
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);

  switch (attributeType)
  {
    case AttributeType::Float:
    {
      glVertexArrayAttribFormat( mId,
        attribIndex, componentsPerVertex,
        dataType, normalize, offset );

      return;
    }

    case AttributeType::Double:
    {
      CQDE_ASSERT_DEBUG(dataType == GL_DOUBLE, return);

      glVertexArrayAttribLFormat( mId,
        attribIndex, componentsPerVertex,
        dataType, offset );

      return;
    }

    case AttributeType::Integer:
    {
      glVertexArrayAttribIFormat( mId,
        attribIndex, componentsPerVertex,
        dataType, offset );

      return;
    }

    default:
      CQDE_ASSERT_DEBUG(false, return);
  }
}

void
GlVertexArray::setBindingDivisor(
  const GlBuffer& buffer,
  const size_t divisor ) const
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);

  const auto bindingIndex = std::find(
    mBindings.begin(),
    mBindings.end(),
    buffer.id() );

  CQDE_ASSERT_DEBUG(bindingIndex != mBindings.end(), return);

  glVertexArrayBindingDivisor( mId,
    std::distance(mBindings.begin(), bindingIndex),
    divisor );
}

void
GlVertexArray::enableAttribute(
  const size_t index ) const
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);

  glEnableVertexArrayAttrib(mId, index);
}

void
GlVertexArray::disableAttribute(
  const size_t index ) const
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);

  glDisableVertexArrayAttrib(mId, index);
}

bool
GlVertexArray::isValid() const
{
  return mId != 0;
}

GLuint
GlVertexArray::id() const
{
  return mId;
}

} // namespace cqde::types
