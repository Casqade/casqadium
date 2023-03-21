#include <cqde/types/CasqadiumEngine.hpp>

#include <cqde/types/graphics/ShaderManager.hpp>

#include <cqde/logger.hpp>


namespace cqde::types
{

static const char* GeometryVertexShader =
R"code(
#version 430

layout(location = 0) uniform mat4 uTransform;

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec2 aTexCoords;

out VertexOutput
{
  vec2 texCoords;

} vOutput;

out gl_PerVertex
{
  vec4 gl_Position;
};


void main()
{
  gl_Position = uTransform * aPos;
  vOutput.texCoords = aTexCoords;
}

)code";

static const char* GeometryFragmentShader =
R"code(
#version 430

uniform sampler2D uTexture;

layout(location = 16) uniform uint uObjectId;

in VertexOutput
{
  vec2 texCoords;

} vInput;

layout(location = 0 ) out vec4 fAlbedo;
layout(location = 1 ) out uint fObjectId;


void main()
{
  vec4 texel = texture(uTexture, vInput.texCoords);
  if ( texel.a > 0.0)
    fAlbedo = texel;
  else
    discard;

  fObjectId = uObjectId;
}

)code";

static const char* ShadowsVertexShader =
R"code(

)code";

static const char* ShadowsFragmentShader =
R"code(

)code";

static const char* UiElementsVertexShader =
R"code(
#version 430

layout(location = 0) uniform mat4 uTransform;

layout(location = 0) in vec4 aPos;

out gl_PerVertex
{
  vec4 gl_Position;
};


void main()
{
  gl_Position = uTransform * aPos;
}

)code";

static const char* UiElementsFragmentShader =
R"code(
#version 430

layout(location = 16) uniform uint uColor;

layout(location = 0 ) out vec4 fAlbedo;


void main()
{
  fAlbedo = unpackUnorm4x8(uColor);
}

)code";

bool
CasqadiumEngine::loadShaders()
{
  auto& shaderManager = mRegistry.ctx().emplace <ShaderManager> ();

  return
    shaderManager.load(
      ShaderType::Geometry,
      GeometryVertexShader,
      GeometryFragmentShader ) &&
//    shaderManager.load(
//      ShaderType::Shadows,
//      ShadowsVertexShader,
//      ShadowsFragmentShader ) &&
    shaderManager.load(
      ShaderType::UiElements,
      UiElementsVertexShader,
      UiElementsFragmentShader );

  return
    shaderManager.load(
      ShaderType::Geometry,
      "geometry.vert",
      "geometry.frag" ) &&
    shaderManager.load(
      ShaderType::Shadows,
      "shadows.vert",
      "shadows.frag" ) &&
    shaderManager.load(
      ShaderType::UiElements,
      "uiElements.vert",
      "uiElements.frag" );
}

} // namespace cqde::types