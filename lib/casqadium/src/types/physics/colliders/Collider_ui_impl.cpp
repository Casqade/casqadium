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

      for ( const auto& callbackId : callbackManager.callbacks() )
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

    bool isTrigger {};

    decltype(callbacks) callbacks {};

    struct
    {
      std::bitset <16> group {};
      std::bitset <16> mask {};

    } collision {};

    struct
    {
      float bounciness {};
      float density {};
      float friction {};

    } material {};

  } state {}, statePrev {};

  const auto flags = ImGuiSliderFlags_NoRoundToFormat |
                     ImGuiSliderFlags_AlwaysClamp;

  if ( ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( mCollider == nullptr )
    {
      auto& jsonTransform = mState["transform"];

      state.transform[0] << jsonTransform[0];
      state.transform[1] << jsonTransform[1];
      state.transform[2] << jsonTransform[2];
      state.transform[3] << jsonTransform[3];
    }
    else
      state.transform = rp3dToGlm(mCollider->getLocalToBodyTransform());

    statePrev.transform = state.transform;

    if ( ImGui::BeginTabBar("Transform") )
    {
      if ( ImGui::BeginTabItem("Translation") )
      {
        auto translation = state.transform[3];

        ImGui::AlignTextToFramePadding();
        ImGui::Text("X");

        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragFloat("##translationX", &translation.x,
                         0.01f, 0.0f, 0.0f, "%.3f", flags);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Y");

        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragFloat("##translationY", &translation.y,
                         0.01f, 0.0f, 0.0f, "%.3f", flags);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Z");

        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragFloat("##translationZ", &translation.z,
                         0.01f, 0.0f, 0.0f, "%.3f", flags);

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

        ImGui::AlignTextToFramePadding();
        ImGui::Text("X");

        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragFloat( "##orientationX", &orientationEuler.x,
                          0.1f, -180.0f, 180.0f, "%.1f°", flags );

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Y");

        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragFloat( "##orientationY", &orientationEuler.y,
                          0.1f, -180.0f, 180.0f, "%.1f°", flags );

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Z");

        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::DragFloat( "##orientationZ", &orientationEuler.z,
                          0.1f, -180.0f, 180.0f, "%.1f°", flags );

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
      {
        if ( mCollider == nullptr )
        {
          auto& jsonTransform = mState["transform"];

          jsonTransform.clear();
          jsonTransform[0] << state.transform[0];
          jsonTransform[1] << state.transform[1];
          jsonTransform[2] << state.transform[2];
          jsonTransform[3] << state.transform[3];

        }
        else
          mCollider->setLocalToBodyTransform(glmToRp3d(state.transform));
      }

      ImGui::EndTabBar(); // Transform
    }
  }

  if ( ImGui::CollapsingHeader("Trigger", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( mCollider == nullptr )
      state.isTrigger = mState["isTrigger"].asBool();
    else
      state.isTrigger = mCollider->getIsTrigger();

    statePrev.isTrigger = state.isTrigger;

    ImGui::Checkbox("Is trigger", &state.isTrigger);

    if ( state.isTrigger != statePrev.isTrigger )
    {
      if ( mCollider == nullptr )
        mState["isTrigger"] = state.isTrigger;
      else
        mCollider->setIsTrigger(state.isTrigger);
    }
  }

  if ( ImGui::CollapsingHeader("Callbacks", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( ImGui::BeginTabBar("Callbacks") )
    {
      if ( mCollider == nullptr )
      {
        auto& jsonCallbacks = mState["callbacks"];

        for ( const auto& callback : jsonCallbacks["onEnter"] )
          state.callbacks.onEnter.push_back(callback.asString());

        for ( const auto& callback : jsonCallbacks["onStay"] )
          state.callbacks.onStay.push_back(callback.asString());

        for ( const auto& callback : jsonCallbacks["onLeave"] )
          state.callbacks.onLeave.push_back(callback.asString());
      }
      else
        state.callbacks = callbacks;

      statePrev.callbacks = state.callbacks;

      if ( ImGui::BeginTabItem("onEnter") )
      {
        callbacksShow(state.callbacks.onEnter);
        ImGui::EndTabItem(); // onEnter
      }

      if ( ImGui::BeginTabItem("onStay") )
      {
        callbacksShow(state.callbacks.onStay);
        ImGui::EndTabItem(); // onStay
      }

      if ( ImGui::BeginTabItem("onLeave") )
      {
        callbacksShow(state.callbacks.onLeave);
        ImGui::EndTabItem(); // onLeave
      }

      if ( mCollider == nullptr )
      {
        auto& jsonCallbacks = mState["callbacks"];

        if ( state.callbacks.onEnter != statePrev.callbacks.onEnter )
        {
          auto& jsonCallbacksOnEnter = jsonCallbacks["onEnter"];
          jsonCallbacksOnEnter = Json::arrayValue;

          for ( const auto& callback : state.callbacks.onEnter )
            jsonCallbacksOnEnter.append(callback.str());
        }

        if ( state.callbacks.onStay != statePrev.callbacks.onStay )
        {
          auto& jsonCallbacksOnStay = jsonCallbacks["onStay"];
          jsonCallbacksOnStay = Json::arrayValue;

          for ( const auto& callback : state.callbacks.onStay )
            jsonCallbacksOnStay.append(callback.str());
        }

        if ( state.callbacks.onLeave != statePrev.callbacks.onLeave )
        {
          auto& jsonCallbacksOnLeave = jsonCallbacks["onLeave"];
          jsonCallbacksOnLeave = Json::arrayValue;

          for ( const auto& callback : state.callbacks.onLeave )
            jsonCallbacksOnLeave.append(callback.str());
        }
      }
      else
      {
        if ( state.callbacks.onEnter != statePrev.callbacks.onEnter )
          callbacks.onEnter = state.callbacks.onEnter;

        if ( state.callbacks.onStay != statePrev.callbacks.onStay )
          callbacks.onStay = state.callbacks.onStay;

        if ( state.callbacks.onLeave != statePrev.callbacks.onLeave )
          callbacks.onLeave = state.callbacks.onLeave;
      }

      ImGui::EndTabBar(); // Callbacks
    }
  }

  static std::array <ToggleButton, 16> buttonsCategory
  { // todo: names categories from PhysicsManager ?
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
      if ( mCollider == nullptr )
      {
        auto& jsonCollision = mState["collision"];

        state.collision.group = jsonCollision["group"].asUInt();
        state.collision.mask = jsonCollision["mask"].asUInt();
      }
      else
      {
        state.collision.group = mCollider->getCollisionCategoryBits();
        state.collision.mask = mCollider->getCollideWithMaskBits();
      }

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
      {
        if ( mCollider == nullptr )
        {
          auto& jsonGroup = mState["collision"]["group"];
          jsonGroup = static_cast <uint32_t> (state.collision.group.to_ulong());
        }
        else
          mCollider->setCollisionCategoryBits(state.collision.group.to_ulong());
      }

      if ( state.collision.mask != statePrev.collision.mask )
      {
        if ( mCollider == nullptr )
        {
          auto& jsonMask = mState["collision"]["mask"];
          jsonMask = static_cast <uint32_t> (state.collision.mask.to_ulong());
        }
        else
          mCollider->setCollisionCategoryBits(state.collision.mask.to_ulong());
      }

      ImGui::EndTabBar(); // Collision
    }
  }

  if ( ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( mCollider == nullptr )
    {
      auto& jsonMaterial = mState["material"];

      state.material.bounciness = jsonMaterial["bounciness"].asFloat();
      state.material.density = jsonMaterial["density"].asFloat();
      state.material.friction = jsonMaterial["friction"].asFloat();
    }
    else
    {
      auto& colliderMaterial = mCollider->getMaterial();

      state.material.bounciness = colliderMaterial.getBounciness();
      state.material.density = colliderMaterial.getMassDensity();
      state.material.friction = colliderMaterial.getFrictionCoefficient();
    }

    statePrev.material = state.material;

    auto& jsonMaterial = mState["material"];

    ImGui::Text("Bounciness");
    ImGui::SameLine();

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##bounciness", &state.material.bounciness,
                      0.01f, 0.0f, 1.0f);

    ImGui::Text("Mass density");
    ImGui::SameLine();

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##density", &state.material.density,
                     0.05f, 0.0f, std::numeric_limits <float>::max());

    ImGui::Text("Friction");
    ImGui::SameLine();

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::DragFloat("##friction", &state.material.friction,
                     0.01f, 0.0f, 1.0f);

    if ( mCollider == nullptr )
    {
      auto& jsonMaterial = mState["material"];

      if ( state.material.bounciness != statePrev.material.bounciness )
        jsonMaterial["bounciness"] = state.material.bounciness;

      if ( state.material.density != statePrev.material.density )
        jsonMaterial["density"] = state.material.density;

      if ( state.material.friction != statePrev.material.friction )
        jsonMaterial["friction"] = state.material.friction;
    }
    else
    {
      auto& colliderMaterial = mCollider->getMaterial();

      if ( state.material.bounciness != statePrev.material.bounciness )
        colliderMaterial.setBounciness(state.material.bounciness);

      if ( state.material.density != statePrev.material.density )
        colliderMaterial.setMassDensity(state.material.density);

      if ( state.material.friction != statePrev.material.friction )
        colliderMaterial.setFrictionCoefficient(state.material.friction);
    }
  }
}

} // namespace cqde::types
