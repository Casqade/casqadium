#pragma once

#include <cstdint>
#include <unordered_map>


namespace cqde::types
{

class GlFramebuffer
{
  using GLuint = uint32_t;

  std::unordered_map <GLuint, GLuint> mAttachments {};

  GLuint mId {};

public:
  GlFramebuffer() = default;

  GlFramebuffer( GlFramebuffer&& );
  GlFramebuffer& operator = ( GlFramebuffer&& );

  GlFramebuffer( const GlFramebuffer& ) = delete;
  GlFramebuffer& operator = ( const GlFramebuffer& ) = delete;


  void create();

  void destroy();

  bool isValid() const;

  GLuint id() const;
};

} // namespace cqde::types
