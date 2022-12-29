#include <cqde/types/physics/colliders/Collider.hpp>

#include <cqde/common.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/conversion/rp3d_glm.hpp>
#include <cqde/conversion/json_glm_vec4.hpp>

#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/PhysicsManager.hpp>
#include <cqde/types/physics/ColliderFactory.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <reactphysics3d/collision/Collider.h>

#include <entt/entity/registry.hpp>

#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>

#include <bitset>


namespace cqde::types
{

void
Collider::ui_show(
  const entt::registry& registry )
{
  using fmt::format;
  using ui::ToggleButton;

  const auto collisionGroupsShow =
  [] ( std::array <ToggleButton, 16>& buttons, std::bitset <16>& groups )
  {
    for ( size_t i = 0; i < buttons.size(); ++i )
    {
      if ( groups[i] != buttons[i].value() )
        buttons[i].setValue(groups[i]);

      groups[i] = buttons[i].isDown();

      if ( i < buttons.size() - 1 )
        ImGui::SameLine();
    }
  };

  const auto callbacksShow =
  [&registry] ( std::vector <CallbackId>& callbacks )
  {
    if ( ImGui::SmallButton("+##callbackAdd") )
      ImGui::OpenPopup("##callbackAddPopup");

    if ( ImGui::BeginPopup("##callbackAddPopup") )
    {
      static ui::StringFilter callbackFilter {"Callback ID"};

      if ( ImGui::IsWindowAppearing() )
        ImGui::SetKeyboardFocusHere(2);

      callbackFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

      auto& callbackManager = registry.ctx().at <CallbackManager> ();

      for ( const auto& callbackId : callbackManager.callbacksSorted() )
      {
        if ( callbackFilter.query(callbackId.str()) == false )
          continue;

        if ( ImGui::Selectable(callbackId.str().c_str(), false) )
        {
          callbacks.push_back(callbackId);
          ImGui::CloseCurrentPopup();
          break;
        }
      }

      ImGui::EndPopup(); // callbackAddPopup
    }

    ImGui::Separator();

    for ( auto iter = callbacks.begin();
          iter < callbacks.end();
          ++iter )
    {
      ImGui::PushID(std::distance(callbacks.begin(), iter));

      if ( ImGui::SmallButton("-##callbackDel") )
        iter = callbacks.erase(iter);

      if ( iter == callbacks.end() )
      {
        ImGui::PopID(); // iter
        break;
      }

      const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                          ImGuiSelectableFlags_AllowItemOverlap;

      ImGui::SameLine();
      ImGui::Selectable(format("{}###", iter->str()).c_str(),
                        false, flags);

      static auto iter_dragged = callbacks.end();

      if ( ImGui::IsItemHovered() == true )
        ImGui::SetTooltip("Drag to reorder");

      if ( ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) == true &&
           ImGui::IsMouseDown(ImGuiMouseButton_Left) == true )
      {
        auto mouseClickPos = ImGui::GetIO().MouseClickedPos[ImGuiMouseButton_Left];

        if (  mouseClickPos.x > ImGui::GetItemRectMin().x &&
              mouseClickPos.x < ImGui::GetItemRectMax().x )
        {
          if ( iter_dragged != iter &&
               iter_dragged >= callbacks.begin() &&
               iter_dragged < callbacks.end() )
            std::swap(*iter, *iter_dragged);

          iter_dragged = iter;
        }
      }

      if ( iter_dragged != callbacks.end() &&
           ImGui::IsMouseReleased(ImGuiMouseButton_Left) == true )
        iter_dragged = callbacks.end();

      ImGui::PopID(); // iter
    }
  };

  struct
  {
    glm::mat4 transform {1.0f};

    struct
    {
      std::bitset <16> group {};
      std::bitset <16> mask {};

    } collision {};

  } state {}, statePrev {};

  const auto flags = ImGuiSliderFlags_NoRoundToFormat |
                     ImGuiSliderFlags_AlwaysClamp;

  if ( ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    state.transform  = rp3dToGlm(mCollider->getLocalToBodyTransform());

    statePrev.transform = state.transform;

    if ( ImGui::BeginTabBar("Transform") )
    {
      if ( ImGui::BeginTabItem("Translation") )
      {
        auto translation = state.transform[3];

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

        ImGui::DragFloat("##translationX", &translation.x,
                         0.01f, 0.0f, 0.0f,
                         "X: %.3f", flags);

        ImGui::DragFloat("##translationY", &translation.y,
                         0.01f, 0.0f, 0.0f,
                         "Y: %.3f", flags);

        ImGui::DragFloat("##translationZ", &translation.z,
                         0.01f, 0.0f, 0.0f,
                         "Z: %.3f", flags);

        ImGui::PopItemWidth();

        if ( ImGui::Button("Reset##translationReset") )
          translation = {glm::vec3{}, 1.0f};

        if ( translation != state.transform[3] )
          state.transform[3] = translation;

        ImGui::EndTabItem(); // Translation
      }

      if ( ImGui::BeginTabItem("Orientation") )
      {
        glm::quat orientation {state.transform};

        auto orientationEuler = glm::degrees(glm::eulerAngles(orientation));

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

        ImGui::DragFloat( "##orientationX", &orientationEuler.x,
                          0.1f, -180.0f, 180.0f,
                          "X: %.1f°", flags );

        ImGui::DragFloat( "##orientationY", &orientationEuler.y,
                          0.1f, -180.0f, 180.0f,
                          "Y: %.1f°", flags );

        ImGui::DragFloat( "##orientationZ", &orientationEuler.z,
                          0.1f, -180.0f, 180.0f,
                          "Z: %.1f°", flags );

        ImGui::PopItemWidth();

        orientation = glm::radians(orientationEuler);

        if ( ImGui::Button("Reset##orientationReset") )
          orientation = glm::vec3{};

        if ( glm::all(glm::equal( orientation, glm::quat{state.transform},
                                  glm::epsilon <float> ())) == false )
        {
          const auto translation = state.transform[3];
          state.transform = glm::mat4{orientation};
          state.transform[3] = translation;
        }

        ImGui::EndTabItem(); // Orientation
      }

      if ( state.transform != statePrev.transform )
        mCollider->setLocalToBodyTransform(glmToRp3d(state.transform));

      ImGui::EndTabBar(); // Transform
    }
  }

  if ( ImGui::CollapsingHeader("Trigger", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    bool isTrigger = mCollider->getIsTrigger();

    if ( ImGui::Checkbox("Is trigger", &isTrigger) )
      mCollider->setIsTrigger(isTrigger);
  }

  if ( ImGui::CollapsingHeader("Callbacks", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( ImGui::BeginTabBar("Callbacks") )
    {
      if ( ImGui::BeginTabItem("onEnter") )
      {
        callbacksShow(mCallbacks.onEnter);
        ImGui::EndTabItem(); // onEnter
      }

      if ( ImGui::BeginTabItem("onStay") )
      {
        callbacksShow(mCallbacks.onStay);
        ImGui::EndTabItem(); // onStay
      }

      if ( ImGui::BeginTabItem("onLeave") )
      {
        callbacksShow(mCallbacks.onLeave);
        ImGui::EndTabItem(); // onLeave
      }

      ImGui::EndTabBar(); // Callbacks
    }
  }

  static std::array <ToggleButton, 16> buttonsCategory
  { // todo: category names from PhysicsManager ?
    ToggleButton{"0"},
    ToggleButton{"1"},
    ToggleButton{"2"},
    ToggleButton{"3"},
    ToggleButton{"4"},
    ToggleButton{"5"},
    ToggleButton{"6"},
    ToggleButton{"7"},
    ToggleButton{"8"},
    ToggleButton{"9"},
    ToggleButton{"10"},
    ToggleButton{"11"},
    ToggleButton{"12"},
    ToggleButton{"13"},
    ToggleButton{"14"},
    ToggleButton{"15"},
  };
  static std::array <ToggleButton, 16> buttonsMask {buttonsCategory};

  if ( ImGui::CollapsingHeader("Collision", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( ImGui::BeginTabBar("Collision") )
    {
      state.collision.group = mCollider->getCollisionCategoryBits();
      state.collision.mask = mCollider->getCollideWithMaskBits();

      statePrev.collision = state.collision;

      if ( ImGui::BeginTabItem("Collision group") )
      {
        collisionGroupsShow(buttonsCategory, state.collision.group);
        ImGui::EndTabItem(); // Collision group
      }

      if ( ImGui::BeginTabItem("Collides with groups") )
      {
        collisionGroupsShow(buttonsMask, state.collision.mask);
        ImGui::EndTabItem(); // Collides with groups
      }

      if ( state.collision.group != statePrev.collision.group )
        mCollider->setCollisionCategoryBits(state.collision.group.to_ulong());

      if ( state.collision.mask != statePrev.collision.mask )
        mCollider->setCollideWithMaskBits(state.collision.mask.to_ulong());

      ImGui::EndTabBar(); // Collision
    }
  }

  if ( ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    auto& colliderMaterial = mCollider->getMaterial();

    float bounciness = colliderMaterial.getBounciness();
    float density = colliderMaterial.getMassDensity();
    float friction = colliderMaterial.getFrictionCoefficient();

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    if (  ImGui::DragFloat( "##bounciness", &bounciness,
                            0.01f, 0.0f, 1.0f,
                            "Bounciness: %.2f") )
      colliderMaterial.setBounciness(bounciness);

    if (  ImGui::DragFloat("##density", &density,
                            0.05f, 0.0f,
                           std::numeric_limits <float>::max(),
                           "Mass density: %.2f") )
      colliderMaterial.setMassDensity(density);

    if (  ImGui::DragFloat("##friction", &friction,
                            0.01f, 0.0f,
                           std::numeric_limits <float>::max(),
                           "Friction: %.2f") )
      colliderMaterial.setFrictionCoefficient(friction);

    ImGui::PopItemWidth();
  }
}

} // namespace cqde::types
