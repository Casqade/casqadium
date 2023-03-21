#pragma once

#include <olcPGE/olcPGE_export.hpp>
#include <olcPGE/fwd.hpp>

#include <string>


namespace olc
{

enum rcode { FAIL = 0, OK = 1, NO_FILE = -1 };

class ImageLoader
{
public:
  ImageLoader() = default;
  virtual ~ImageLoader() = default;
  virtual olc::rcode LoadImageResource(olc::Sprite* spr, const std::string& sImageFile) = 0;
};

} // namespace olc
