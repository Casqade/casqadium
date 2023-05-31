#include <olcPGE/Decal.hpp>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cassert>


namespace olc
{

static uint32_t CreateTexture( const bool filtered, const bool clamp )
{
  assert(glfwGetCurrentContext() != nullptr);

  GLuint id;
  glCreateTextures(GL_TEXTURE_2D, 1, &id);

  if ( filtered == true )
  {
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  else
  {
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }

  if ( clamp == true )
  {
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
  else
  {
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

  return id;
}

static uint32_t DeleteTexture( const uint32_t id )
{
  glDeleteTextures(1, &id);
  return id;
}


Decal::Decal( olc::Sprite* spr, bool filter, bool clamp )
{
  id = 0;
  if (spr == nullptr)
    return;

  sprite = spr;
  id = CreateTexture(filter, clamp);
  Update();
}

Decal::Decal( const uint32_t nExistingTextureResource, olc::Sprite* spr )
{
  if (spr == nullptr)
    return;

  id = nExistingTextureResource;
}

void Decal::Update()
{
  if (sprite == nullptr)
    return;

  glDeleteTextures(1, &id);
  id = CreateTexture(false, false);

  glTextureStorage2D( id, 1,
    GL_RGBA8,
    sprite->width, sprite->height );

  glTextureSubImage2D( id, 0,
    0, 0,
    sprite->width, sprite->height,
    GL_RGBA, GL_UNSIGNED_BYTE,
    sprite->GetData() );

  glGenerateTextureMipmap(id);
}

Decal::~Decal()
{
  if ( id != 0 )
  {
    glDeleteTextures(1, &id);
    id = 0;
  }
}

} // namespace olc
