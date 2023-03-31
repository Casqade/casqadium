#include <cqde/types/CasqadiumEngine.hpp>

#include <cqde/types/graphics/ShaderManager.hpp>


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

layout(location = 0) out vec4 fAlbedo;
layout(location = 1) out uint fObjectId;


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

static const char* WireframeVertexShader =
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

static const char* WireframeFragmentShader =
R"code(
#version 430

layout(location = 16) uniform uint uObjectId;

in VertexOutput
{
  vec2 texCoords;

} vInput;

layout(location = 0) out vec4 fAlbedo;
layout(location = 1) out uint fObjectId;


void main()
{
  float alpha =
    float(vInput.texCoords.x <= 0.01) + float(vInput.texCoords.x >= 0.99)
  + float(vInput.texCoords.y <= 0.01) + float(vInput.texCoords.y >= 0.99);

  fAlbedo = vec4(0.75, 0.75, 0.75, alpha);
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

layout(location = 0) out vec4 fAlbedo;


void main()
{
  fAlbedo = unpackUnorm4x8(uColor);
}

)code";

static const char* FullscreenQuadVertexShader =
R"code(
#version 430

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
  gl_Position = aPos;
  vOutput.texCoords = aTexCoords;
}

)code";

static const char* FullscreenQuadFragmentShader =
R"code(
#version 430

uniform sampler2D uTexture;

in VertexOutput
{
  vec2 texCoords;

} vInput;

layout(location = 0) out vec4 fAlbedo;


void main()
{
  fAlbedo = texture(uTexture, vInput.texCoords);
}

)code";

static const char* DebugDrawVertexShader =
R"code(
#version 430

layout(location = 0) uniform mat4 uTransform;

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec4 aColor;

out VertexOutput
{
  vec4 color;

} vOutput;

out gl_PerVertex
{
  vec4 gl_Position;
};


void main()
{
  gl_Position = uTransform * aPos;
  vOutput.color = aColor;
}

)code";

static const char* DebugDrawFragmentShader =
R"code(
#version 430

in VertexOutput
{
  vec4 color;

} vInput;

layout(location = 0) out vec4 fAlbedo;


void main()
{
  fAlbedo = vInput.color;
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
    shaderManager.load(
      ShaderType::Wireframe,
      WireframeVertexShader,
      WireframeFragmentShader ) &&
//    shaderManager.load(
//      ShaderType::Shadows,
//      ShadowsVertexShader,
//      ShadowsFragmentShader ) &&
    shaderManager.load(
      ShaderType::UiElements,
      UiElementsVertexShader,
      UiElementsFragmentShader ) &&
    shaderManager.load(
      ShaderType::FullscreenQuad,
      FullscreenQuadVertexShader,
      FullscreenQuadFragmentShader ) &&
    shaderManager.load(
      ShaderType::DebugDraw,
      DebugDrawVertexShader,
      DebugDrawFragmentShader );

  return
    shaderManager.load(
      ShaderType::Geometry,
      "shaders/geometry.vert",
      "shaders/geometry.frag" ) &&
    shaderManager.load(
      ShaderType::Wireframe,
      "shaders/wireframe.vert",
      "shaders/wireframe.frag" ) &&
    shaderManager.load(
      ShaderType::Shadows,
      "shaders/shadows.vert",
      "shaders/shadows.frag" ) &&
    shaderManager.load(
      ShaderType::UiElements,
      "shaders/uiElements.vert",
      "shaders/uiElements.frag" ) &&
    shaderManager.load(
      ShaderType::FullscreenQuad,
      "shaders/fullscreenQuad.vert",
      "shaders/fullscreenQuad.frag" ) &&
    shaderManager.load(
      ShaderType::DebugDraw,
      "shaders/debugDraw.vert",
      "shaders/debugDraw.frag" );
}

} // namespace cqde::types
