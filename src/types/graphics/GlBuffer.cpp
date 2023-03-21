#include <cqde/types/graphics/GlBuffer.hpp>

#include <cqde/common.hpp>

#include <glad/gl.h>


namespace cqde::types
{

GlBuffer::GlBuffer(
  GlBuffer&& other )
{
  *this = std::move(other);
}

GlBuffer&
GlBuffer::operator = (
  GlBuffer&& other )
{
  CQDE_ASSERT_DEBUG(isMapped() == false, unmap());
  CQDE_ASSERT_DEBUG(isValid() == false, destroy());

  std::swap(mId, other.mId);
  std::swap(mData, other.mData);

  return *this;
}

void
GlBuffer::create(
  const size_t size,
  const GLbitfield storageFlags,
  const void* initialData )
{
  CQDE_ASSERT_DEBUG(isMapped() == false, unmap());
  CQDE_ASSERT_DEBUG(isValid() == false, destroy());

  glCreateBuffers(1, &mId);

  glNamedBufferStorage( mId,
    size, initialData,
    storageFlags );
}

void
GlBuffer::destroy()
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);

  if ( isMapped() == true )
    unmap();

  glDeleteBuffers(1, &mId);
  mId = 0;
}

void*
GlBuffer::map(
  const size_t offset,
  const size_t size,
  const GLbitfield accessFlags )
{
  CQDE_ASSERT_DEBUG(isValid() == true, return nullptr);
  CQDE_ASSERT_DEBUG(isMapped() == false, unmap());

  mData = glMapNamedBufferRange( mId,
    offset, size,
    accessFlags );

  return mData;
}

void
GlBuffer::unmap()
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);
  CQDE_ASSERT_DEBUG(isMapped() == true, return);

  glUnmapNamedBuffer(mId);
  mData = nullptr;
}

void
GlBuffer::write(
  const size_t offset,
  const size_t size,
  const void* data ) const
{
  CQDE_ASSERT_DEBUG(isValid() == true, return);

  glNamedBufferSubData( mId,
    offset, size, data );
}

bool
GlBuffer::isValid() const
{
  return mId != 0;
}

bool
GlBuffer::isMapped() const
{
  return mData != nullptr;
}

GLuint
GlBuffer::id() const
{
  return mId;
}

void*
GlBuffer::data() const
{
  return mData;
}

GLbitfield
GlBuffer::MutableAccessFlags()
{
  return
    GL_MAP_WRITE_BIT |
    GL_MAP_PERSISTENT_BIT |
    GL_MAP_COHERENT_BIT;
}

GLbitfield
GlBuffer::MutableStorageFlags()
{
  return GL_DYNAMIC_STORAGE_BIT;
}

GLbitfield
GlBuffer::ImmutableStorageFlags()
{
  return
    GL_MAP_WRITE_BIT |
    GL_MAP_PERSISTENT_BIT;
}

} // namespace cqde::types
