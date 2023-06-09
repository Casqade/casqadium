#pragma once

#include <cstdint>
#include <cstddef>

#include <vector>


namespace cqde::types
{

template <typename Element>
size_t BufferSize(
  const std::vector <Element>& data )
{
  return sizeof(Element) * data.size();
}

class GlBuffer
{
  using GLuint = uint32_t;
  using GLbitfield = uint32_t;

  GLuint mId {};
  void* mData {};

public:
  GlBuffer() = default;

  GlBuffer( GlBuffer&& );
  GlBuffer& operator = ( GlBuffer&& );

  GlBuffer( const GlBuffer& ) = delete;
  GlBuffer& operator = ( const GlBuffer& ) = delete;

  void create(
    const size_t size,
    const GLbitfield storageFlags,
    const void* initialData = nullptr );

  void destroy();

  void* map(
    const size_t offset,
    const size_t size,
    const GLbitfield accessFlags );

  void unmap();

  void write(
    const size_t offset,
    const size_t size,
    const void* data ) const;

  void copy(
    const GlBuffer& target,
    const size_t size,
    const size_t readOffset = 0,
    const size_t writeOffset = 0 ) const;

  bool isValid() const;
  bool isMapped() const;

  GLuint id() const;
  void* data() const;

  size_t size() const;

  static GLbitfield MutableAccessFlags();

  static GLbitfield MutableStorageFlags();
  static GLbitfield ImmutableStorageFlags();
};

} // namespace cqde::types
