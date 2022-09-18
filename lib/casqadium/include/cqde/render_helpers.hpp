#pragma once

#include <olcPGE/olcPixelGameEngine.hpp>

#include <cqde/types/VertexBuffer.hpp>

#include <glm/fwd.hpp>


namespace olc
{
class Font;
}

namespace cqde
{

enum class LineRenderMode
{
  Strip,
  Loop,
};


types::VertexBuffer::WindingOrder
windingOrder(
  const std::vector <olc::vf2d>&,
  const bool yAxisUp = false );

types::VertexBuffer
vertexShader(
  const std::vector <glm::vec3>& vertices,
  const glm::mat4& modelViewMatrix,
  const glm::mat4& projectionMatrix,
  const glm::vec4& viewport );

void drawLines(
  const std::vector <olc::vf2d>&,
  const olc::Pixel& color,
  const LineRenderMode );

std::shared_ptr <olc::Renderable> textureFromText(
  const std::string& text,
  const olc::Pixel&,
  const bool monospaced );

std::shared_ptr <olc::Renderable> textureFromText(
  const std::string& text,
  olc::Font*,
  const olc::Pixel&,
  const bool antialiased );

} // namespace cqde
