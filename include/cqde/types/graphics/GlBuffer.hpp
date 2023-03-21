#pragma once

#include <cstdint>
#include <cstddef>


namespace cqde::types
{

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

  bool isValid() const;
  bool isMapped() const;

  GLuint id() const;
  void* data() const;

  static GLbitfield MutableAccessFlags();

  static GLbitfield MutableStorageFlags();
  static GLbitfield ImmutableStorageFlags();
};

} // namespace cqde::types
