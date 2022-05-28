#pragma once

#include <json/forwards.h>

#include <string>


namespace cqde::types
{

struct JsonPackageStructure
{
  using Title = std::string;
  using Description = std::string;
  using Version = std::string;

  const std::string title {"title"};
  const std::string description {"description"};
  const std::string version {"version"};

  const std::string assets {"assets"};
  const std::string entities {"entities"};
  const std::string input {"input"};

  struct Assets //  object
  {
    const std::string audio {"audio"};
    const std::string fonts {"fonts"};
    const std::string text {"text"};
    const std::string textures {"textures"};

    struct Audio  //  object
    {};

    struct Fonts  //  object
    {};

    struct Text //  object
    {};

    struct Textures //  object
    {};
  };

  struct Entities //  array/object ?
  {};

  struct Input  //  object
  {
    using AxisId = std::string;

    struct Axis  //  object
    {
      using Alias = std::string;

      const std::string alias {"alias"};
      const std::string bindings {"bindings"};

      struct Bindings //  object
      {
        using Type = std::string;

        const std::string type {"type"};
      };
    };
  };
};

} // namespace cqde
