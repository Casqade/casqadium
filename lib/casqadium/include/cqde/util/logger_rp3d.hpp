#pragma once

#include <reactphysics3d/utils/Logger.h>


namespace cqde
{

class logger_rp3d : public reactphysics3d::Logger
{
public:
  ~logger_rp3d() override = default;

  void log( Level,
            const std::string& physicsWorldName,
            Category,
            const std::string& message,
            const char* filename,
            int lineNumber ) override;
};

} // namespace cqde
