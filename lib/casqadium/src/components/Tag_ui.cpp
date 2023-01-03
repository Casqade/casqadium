#include <cqde/components/Tag.hpp>

#include <cqde/components/SceneNode.hpp>

#include <cqde/types/EntityManager.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>
#include <imgui_stdlib.h>

#include <json/value.h>


namespace cqde::compos
{

void
Tag::ui_edit_props(
  const entt::entity entity,
  entt::registry& registry )
{
  using fmt::format;
  using types::EntityManager;

  if ( ImGui::CollapsingHeader("ID", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  static EntityId idCurrent = id;
  static std::string idExisting {};
  static std::string idGenerated {};

  std::string idInputBuffer = id.str();
  static bool idInputBufferReset {};

  if ( id != idCurrent )
  {
    idCurrent = id;
    idGenerated.clear();
  }

  if ( idGenerated.empty() == false )
    idInputBuffer = idGenerated;

  if ( ImGui::BeginPopup("##tag_exists_popup") )
  {
    ImGui::Text("%s", format("Entity '{}' already exists", idExisting).c_str());
    ImGui::EndPopup(); // tag_exists_popup
  }

  auto& entityManager = registry.ctx().at <EntityManager> ();

  if ( ImGui::Button("%##entityIdGen") )
  {
    idInputBufferReset = true;
    idGenerated = entityManager.idGenerate(idInputBuffer).str();
  }

  if ( ImGui::IsItemHovered() )
    ImGui::SetTooltip("Generate unique entity ID");

  if ( idInputBufferReset == false )
  {
    ImGui::SameLine();
    if ( ImGui::InputText("##tag_id", &idInputBuffer) )
      idGenerated.clear();

    if ( ImGui::IsItemDeactivated() )
      idGenerated = idInputBuffer;
  }
  else
    idInputBufferReset = false;

  if ( ImGui::Button("Set") == false ||
       idInputBuffer == this->id.str() )
    return;

  if ( idInputBuffer.empty() == true )
  {
    idInputBufferReset = true;
    idGenerated.clear();
    return;
  }

  if ( entityManager.get_if_valid(idInputBuffer, registry) != entt::null )
  {
    idInputBufferReset = true;
    idExisting = idInputBuffer;
    idGenerated = id.str();

    ImGui::OpenPopup("##tag_exists_popup");
    return;
  }

  auto cNode = registry.try_get <SceneNode> (entity);

  entt::entity parent {};

  if ( cNode != nullptr )
  {
    parent = cNode->parent.get(registry);
    DetachChildNode(registry, parent, entity);
  }

  Json::Value jsonEntity {};

  id = idInputBuffer;
  entityManager.entitySerialize(registry, jsonEntity, entity);
  id = idCurrent;

  std::unordered_map <EntityId, EntityId, identifier_hash> idMap {};
  idMap[id] = idInputBuffer;

  const auto renamedEntity = entityManager.entityDeserialize(
    registry, idInputBuffer, jsonEntity[idInputBuffer], idMap );

  entityManager.removeLater(entity);

  if ( cNode != nullptr )
  {
    cNode = &registry.get <SceneNode> (renamedEntity);

    AttachChildNode(registry, parent, renamedEntity);

    for ( auto& child : cNode->children )
      AttachChildNode(registry, renamedEntity, child.get(registry));
  }
}

} // namespace cqde::compos
