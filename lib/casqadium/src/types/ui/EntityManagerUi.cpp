#include <cqde/types/ui/EntityManagerUi.hpp>
#include <cqde/types/EntityManager.hpp>

#include <cqde/types/PackageManager.hpp>
#include <cqde/types/SystemManager.hpp>
#include <cqde/types/input/InputManager.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/EntityMetaInfo.hpp>

#include <cqde/common.hpp>
#include <cqde/ecs_helpers.hpp>

#include <entt/entity/registry.hpp>
#include <entt/meta/resolve.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>
#include <imgui_stdlib.h>


namespace cqde::ui
{

EntityManagerUi::EntityManagerUi(
  types::EntityManager* entityMgr )
  : mSelectedEntity{entt::null}
  , mSelectedComponent{entt::null}
  , mEntityMgr{entityMgr}
{}

void
EntityManagerUi::ui_show(
  entt::registry& registry )
{
  using compos::Tag;
  using compos::EntityMetaInfo;

  CQDE_ASSERT_DEBUG(mEntityMgr != nullptr, return);

  if ( ImGui::Begin("Registry view", nullptr,
                    ImGuiWindowFlags_HorizontalScrollbar ) == false )
  {
    ImGui::End(); // Registry view
    return;
  }

  if ( ImGui::CollapsingHeader("Filter") )
    mRegistryFilter.show(registry);

  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollY |
                          ImGuiTableFlags_RowBg;

  if ( ImGui::BeginTable( "Entities", 1, tableFlags,
                          {0.0f, ImGui::GetContentRegionAvail().y}) )
  {
    ImGui::TableNextColumn();

    bool entitiesFound {};

    registry.each(
    [ this, &registry, &entitiesFound] ( const entt::entity entity )
    {
      const auto packageSelected = mRegistryFilter.package();

      if (  packageSelected.str().empty() == false &&
            packageSelected != registry.get <EntityMetaInfo> (entity).packageId)
        return;

      const auto entityId = registry.get <Tag> (entity).id;

      if ( mRegistryFilter.query(entityId) == false )
        return;

      bool componentFound {};

      each_component(entity, registry,
      [this, &componentFound] ( const ComponentType component )
      {
        componentFound = mRegistryFilter.query(component);

        if ( componentFound == true )
          return false;

        return true;
      });

      if ( componentFound == false )
        return;

      entitiesFound = true;

      const auto nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow |
                             ImGuiTreeNodeFlags_OpenOnDoubleClick;

      if ( ImGui::TreeNodeEx(entityId.str().c_str(), nodeFlags) )
      {
        each_component(entity, registry,
        [this, &registry, entity, entityId] ( const ComponentType componentType )
        {
          auto flags = ImGuiTreeNodeFlags_Bullet |
                       ImGuiTreeNodeFlags_NoTreePushOnOpen;

          if ( mSelectedEntity == entity &&
               mSelectedComponent == componentType )
            flags |= ImGuiTreeNodeFlags_Selected;

          ImGui::TreeNodeEx(component_name(componentType).c_str(), flags);
          if ( ImGui::IsItemActivated() )
          {
            mSelectedEntity = entity;
            mSelectedComponent = componentType;
          }

          return true;
        });
        ImGui::TreePop();
      }
    });

    if ( entitiesFound == false )
      ImGui::Text("No results");

    ImGui::EndTable(); // Entities
  }

  ImGui::End(); // Registry view

  if ( ImGui::Begin("Component view") == false )
  {
    ImGui::End(); // Component view
    return;
  }

  if ( mSelectedEntity == entt::null ||
       mSelectedComponent == entt::null ||
       registry.valid(mSelectedEntity) == false )
  {
    ImGui::Text("No component selected");

    ImGui::End(); // Component view
    return;
  }

  const auto entityId = registry.get <Tag> (mSelectedEntity).id;

  auto component = entt::resolve(mSelectedComponent);

  if ( !component )
  {
    ImGui::Text("Selected component is invalid");

    ImGui::End(); // Component view
    return;
  }

  using fmt::format;
  using namespace entt::literals;

  const auto componentName = component_name(mSelectedComponent);

  ImGui::BulletText("%s", format("{} -> {}", entityId.str(), componentName).c_str());

  if ( component_is_empty(mSelectedComponent) == true )
  {
    ImGui::Text("No data for this component");
    ImGui::End(); // Component view
    return;
  }

  auto componentGet = component.func("get"_hs);
  auto componentInstance = componentGet.invoke({}, entt::forward_as_meta(registry), mSelectedEntity);

  if ( componentInstance )
  {
    auto componentUiShow = component.func("ui_edit_props"_hs);
    componentUiShow.invoke(componentInstance, mSelectedEntity, entt::forward_as_meta(registry));
  }

  ImGui::End(); // Component view
}

} // namespace cqde::types
