#include <cqde/types/CasqadiumEngine.hpp>

#include <cqde/logger.hpp>

#include <cqde/types/EntityManager.hpp>

#include <entt/entity/registry.hpp>
#include <entt/locator/locator.hpp>
#include <entt/meta/context.hpp>

#include <GLFW/glfw3.h>

#include <cstdlib>


int
main( int argc, char* argv[] )
{
  using cqde::types::CasqadiumEngine;
  using WindowHints = CasqadiumEngine::WindowHints;
  using EntityManager = cqde::types::EntityManager;

  bool status {};

  {
    const WindowHints windowHints
    {
      {GLFW_CONTEXT_VERSION_MAJOR, 4},
      {GLFW_CONTEXT_VERSION_MINOR, 3},
      {GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE},
      {GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE},
      {GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE},
    };

    CasqadiumEngine engine {{argc, argv}};

    const auto config = engine.config();

    config->log.pattern = "[%T.%e] [%^%l%$] [%s:%#] [thread %t] %v";
    config->engine.tickRate = 120;

    engine.setConfig(config);

    engine.readConfig();

    const auto logger = engine.logger();

    if ( spdlog::get(logger->name()) == nullptr )
      spdlog::register_logger(logger);

    spdlog::set_default_logger(logger);

    if ( engine.init(windowHints) == false )
    {
      spdlog::shutdown();
      return EXIT_FAILURE;
    }

//    ImGui::SetCurrentContext(engine.ImguiContext());

    auto& registry = engine.registry();

    using MetaCtxLocator = entt::locator <entt::meta_ctx>;
    using MetaCtxHandle = MetaCtxLocator::node_type;

    MetaCtxLocator::reset(registry.ctx().get <MetaCtxHandle> ());

    auto& entityManager = registry.ctx().get <EntityManager> ();

//    register custom components

    try
    {
      engine.run();
    }
    catch ( const std::exception& e )
    {
      LOG_ERROR("{}", e.what());
    }

    engine.shutdown();
  }

  spdlog::shutdown();

  return status == true
      ? EXIT_SUCCESS
      : EXIT_FAILURE;
}
