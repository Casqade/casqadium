#include <cqde/components/physics/RigidBody.hpp>

#include <cqde/conversion/rp3d_glm.hpp>
#include <cqde/conversion/json_glm_vec3.hpp>

#include <cqde/types/physics/ColliderFactory.hpp>
#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <reactphysics3d/body/RigidBody.h>
#include <reactphysics3d/components/RigidBodyComponents.h>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
RigidBody::ui_edit_props(
  const entt::entity entity,
  entt::registry& registry )
{
  using fmt::format;
  using types::ColliderFactory;
  using BodyType = rp3d::BodyType;

  const auto dragFloatFlags = ImGuiSliderFlags_NoRoundToFormat |
                              ImGuiSliderFlags_AlwaysClamp;

  struct
  {
    struct
    {
      glm::vec3 linear {};
      glm::vec3 angular{};

    } axisFactor {};

    struct
    {
      glm::vec3 linear {};
      glm::vec3 angular{};

    } velocity {};

    glm::vec3 force {};
    glm::vec3 torque {};
    glm::vec3 inertia {};
    glm::vec3 massCenter {};

  } state {}, statePrev {};

  if ( ImGui::CollapsingHeader("State", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    bool active = body->isActive();
    bool sleeping = body->isSleeping();
    bool sleepAllowed = body->isAllowedToSleep();

    if ( ImGui::Checkbox("Is active", &active) )
      body->setIsActive(active);

    if ( ImGui::Checkbox("Allow sleeping", &sleepAllowed) )
      body->setIsAllowedToSleep(sleepAllowed);

    ImGui::BeginDisabled(sleepAllowed == false);

    if ( ImGui::Checkbox("Is sleeping", &sleeping) )
      body->setIsSleeping(sleeping);

    ImGui::EndDisabled();
  }

  if ( ImGui::CollapsingHeader("Type", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    auto type = body->getType();

    if ( ImGui::BeginCombo("##bodyType", BodyTypeToString(type).c_str()) )
    {
      using BodyType = rp3d::BodyType;

      if ( ImGui::Selectable(BodyTypeToString(rp3d::BodyType::STATIC).c_str(),
                             type == rp3d::BodyType::STATIC) )
        body->setType(BodyType::STATIC);

      if ( ImGui::Selectable(BodyTypeToString(rp3d::BodyType::KINEMATIC).c_str(),
                             type == rp3d::BodyType::KINEMATIC) )
        body->setType(BodyType::KINEMATIC);

      if ( ImGui::Selectable(BodyTypeToString(rp3d::BodyType::DYNAMIC).c_str(),
                             type == rp3d::BodyType::DYNAMIC) )
        body->setType(BodyType::DYNAMIC);

      ImGui::EndCombo();
    }
  }

  if ( ImGui::CollapsingHeader("Mass", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( ImGui::Checkbox("Calc mass from colliders", &massPropsFromColliders) &&
         massPropsFromColliders == true )
      body->updateMassPropertiesFromColliders();

    state.massCenter = rp3dToGlm(body->getLocalCenterOfMass());

    statePrev = state;

    float mass = body->getMass();

    ImGui::Text("Mass");
    ImGui::SameLine();

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (  ImGui::DragFloat("##mass", &mass,
                            1.0f, 0.0f, 0.0f, "%.3f", dragFloatFlags) )
      body->setMass(mass);


    ImGui::Text("Center of mass");

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    ImGui::DragFloat("##massCenterX", &state.massCenter.x,
                     0.01f, 0.0f, 0.0f,
                     "X: %.2f", dragFloatFlags);

    ImGui::DragFloat("##massCenterY", &state.massCenter.y,
                     0.01f, 0.0f, 0.0f,
                     "Y: %.2f", dragFloatFlags);

    ImGui::DragFloat("##massCenterZ", &state.massCenter.z,
                     0.01f, 0.0f, 0.0f,
                     "Z: %.2f", dragFloatFlags);

    ImGui::PopItemWidth();

    if ( ImGui::Button("Reset##massCenterReset") )
      state.massCenter = glm::vec3{};

    if ( state.massCenter != statePrev.massCenter )
      body->setLocalCenterOfMass(glmToRp3d(state.massCenter));
  }

  if ( ImGui::CollapsingHeader("Damping", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    float dampLinear = body->getLinearDamping();
    float dampAngular = body->getAngularDamping();

    ImGui::Text("Linear damping");

    if (  ImGui::DragFloat("##dampLinear", &dampLinear,
                            0.01f, 0.0f, 0.0f, "%.2f", dragFloatFlags) )
      body->setLinearDamping(dampLinear);

    ImGui::Text("Angular damping");

    if (  ImGui::DragFloat("##dampAngular", &dampAngular,
                            0.01f, 0.0f, 0.0f, "%.2f", dragFloatFlags) )
      body->setAngularDamping(dampAngular);
  }

  if ( ImGui::CollapsingHeader("Lock axis factor", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    state.axisFactor.linear = rp3dToGlm(body->getLinearLockAxisFactor());
    state.axisFactor.angular = rp3dToGlm(body->getAngularLockAxisFactor());

    statePrev = state;

    ImGui::Text("Linear factor");

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    ImGui::DragFloat("##axisFactorLinearX", &state.axisFactor.linear.x,
                     0.1f, 0.0f, 0.0f,
                     "X: %.1f", dragFloatFlags);

    ImGui::DragFloat("##axisFactorLinearY", &state.axisFactor.linear.y,
                     0.1f, 0.0f, 0.0f,
                     "Y: %.1f", dragFloatFlags);

    ImGui::DragFloat("##axisFactorLinearZ", &state.axisFactor.linear.z,
                     0.1f, 0.0f, 0.0f,
                     "Z: %.1f", dragFloatFlags);

    ImGui::PopItemWidth();

    if ( ImGui::Button("Reset##axisFactorLinearReset") )
      state.axisFactor.linear = glm::vec3{1.0f};

    ImGui::Text("Angular factor");

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    ImGui::DragFloat("##axisFactorAngularX", &state.axisFactor.angular.x,
                     0.1f, 0.0f, 0.0f,
                     "X: %.1f", dragFloatFlags);

    ImGui::DragFloat("##axisFactorAngularY", &state.axisFactor.angular.y,
                     0.1f, 0.0f, 0.0f,
                     "Y: %.1f", dragFloatFlags);

    ImGui::DragFloat("##axisFactorAngularZ", &state.axisFactor.angular.z,
                     0.1f, 0.0f, 0.0f,
                     "Z: %.1f", dragFloatFlags);

    ImGui::PopItemWidth();

    if ( ImGui::Button("Reset##axisFactorAngularReset") )
      state.axisFactor.angular = glm::vec3{1.0f};

    if ( state.axisFactor.linear != statePrev.axisFactor.linear )
      body->setLinearLockAxisFactor(glmToRp3d(state.axisFactor.linear));

    if ( state.axisFactor.angular != statePrev.axisFactor.angular )
      body->setAngularLockAxisFactor(glmToRp3d(state.axisFactor.angular));
  }

  if ( ImGui::CollapsingHeader("Velocity", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    state.velocity.linear = rp3dToGlm(body->getLinearVelocity());
    state.velocity.angular = rp3dToGlm(body->getAngularVelocity());

    statePrev = state;

    ImGui::Text("Linear velocity");

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    ImGui::DragFloat("##velocityLinearX", &state.velocity.linear.x,
                     0.1f, 0.0f, 0.0f,
                     "X: %.1f", dragFloatFlags);

    ImGui::DragFloat("##velocityLinearY", &state.velocity.linear.y,
                     0.1f, 0.0f, 0.0f,
                     "Y: %.1f", dragFloatFlags);

    ImGui::DragFloat("##velocityLinearZ", &state.velocity.linear.z,
                     0.1f, 0.0f, 0.0f,
                     "Z: %.1f", dragFloatFlags);

    ImGui::PopItemWidth();

    if ( ImGui::Button("Reset##velocityLinearReset") )
      state.velocity.linear = glm::vec3{};

    ImGui::Text("Angular velocity");

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    ImGui::DragFloat("##velocityAngularX", &state.velocity.angular.x,
                     0.1f, 0.0f, 0.0f,
                     "X: %.1f", dragFloatFlags);

    ImGui::DragFloat("##velocityAngularY", &state.velocity.angular.y,
                     0.1f, 0.0f, 0.0f,
                     "Y: %.1f", dragFloatFlags);

    ImGui::DragFloat("##velocityAngularZ", &state.velocity.angular.z,
                     0.1f, 0.0f, 0.0f,
                     "Z: %.1f", dragFloatFlags);

    ImGui::PopItemWidth();

    if ( ImGui::Button("Reset##velocityAngularReset") )
      state.velocity.angular = glm::vec3{};

    if ( state.velocity.linear != statePrev.velocity.linear )
      body->setLinearVelocity(glmToRp3d(state.velocity.linear));

    if ( state.velocity.angular != statePrev.velocity.angular )
      body->setAngularVelocity(glmToRp3d(state.velocity.angular));
  }

  if ( ImGui::CollapsingHeader("Force", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    state.force = rp3dToGlm(body->getForce());

    statePrev = state;

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    ImGui::DragFloat("##forceX", &state.force.x,
                     0.01f, 0.0f, 0.0f,
                     "X: %.2f", dragFloatFlags);

    ImGui::DragFloat("##forceY", &state.force.y,
                     0.01f, 0.0f, 0.0f,
                     "Y: %.2f", dragFloatFlags);

    ImGui::DragFloat("##forceZ", &state.force.z,
                     0.01f, 0.0f, 0.0f,
                     "Z: %.2f", dragFloatFlags);

    ImGui::PopItemWidth();

    if ( ImGui::Button("Reset##forceReset") )
      state.force = glm::vec3{};

    if ( state.force != statePrev.force )
      body->applyWorldForceAtCenterOfMass(glmToRp3d(state.force));
  }

  if ( ImGui::CollapsingHeader("Torque", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    state.torque = rp3dToGlm(body->getForce());

    statePrev = state;

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    ImGui::DragFloat("##torqueX", &state.torque.x,
                     0.01f, 0.0f, 0.0f,
                     "X: %.2f", dragFloatFlags);

    ImGui::DragFloat("##torqueY", &state.torque.y,
                     0.01f, 0.0f, 0.0f,
                     "Y: %.2f", dragFloatFlags);

    ImGui::DragFloat("##torqueZ", &state.torque.z,
                     0.01f, 0.0f, 0.0f,
                     "Z: %.2f", dragFloatFlags);

    ImGui::PopItemWidth();

    if ( ImGui::Button("Reset##torqueReset") )
      state.torque = glm::vec3{};

    if ( state.torque != statePrev.torque )
      body->applyWorldTorque(glmToRp3d(state.torque));
  }

  if ( ImGui::CollapsingHeader("Inertia", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    state.inertia = rp3dToGlm(body->getForce());
    statePrev = state;

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    ImGui::DragFloat("##inertiaX", &state.inertia.x,
                     0.01f, 0.0f, 0.0f,
                     "X: %.2f", dragFloatFlags);

    ImGui::DragFloat("##inertiaY", &state.inertia.y,
                     0.01f, 0.0f, 0.0f,
                     "Y: %.2f", dragFloatFlags);

    ImGui::DragFloat("##inertiaZ", &state.inertia.z,
                     0.01f, 0.0f, 0.0f,
                     "Z: %.2f", dragFloatFlags);

    ImGui::PopItemWidth();

    if ( ImGui::Button("Reset##inertiaReset") )
      state.inertia = glm::vec3{};

    if ( state.inertia != statePrev.inertia )
      body->setLocalInertiaTensor(glmToRp3d(state.inertia));
  }

  if ( ImGui::CollapsingHeader("Colliders", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  static ui::StringFilter shapeFilter {"Shape ID"};

  static bool colliderWindowOpened {};
  static int32_t selectedCollider = {-1};

  if ( ImGui::SmallButton("+##colliderAdd") )
    ImGui::OpenPopup("##colliderAddPopup");

  if ( ImGui::BeginPopup("##colliderAddPopup") )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    shapeFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    const auto& colliderFactory = registry.ctx().get <ColliderFactory> ();

    for ( const auto& shapeId : colliderFactory.colliders() )
    {
      if ( shapeFilter.query(shapeId) == false )
        continue;

      if ( ImGui::Selectable(shapeId.c_str(), false) )
      {
        const auto collider = colliderFactory.create(shapeId);
        collider->init(registry, body);
        colliders.push_back(collider);

        ImGui::CloseCurrentPopup();
        break;
      }
    }

    ImGui::EndPopup(); // colliderAddPopup
  }

  ImGui::Separator();

  for ( auto iter = colliders.begin();
        iter < colliders.end();
        ++iter )
  {
    const auto index = std::distance(colliders.begin(), iter);
    ImGui::PushID(index);

    if ( ImGui::SmallButton("-##colliderRemove") )
    {
      if ( selectedCollider >= index )
        selectedCollider = -1;

      iter = colliders.erase(iter);

      if ( iter == colliders.end() )
      {
        ImGui::PopID(); // index
        break;
      }
    }

    const auto shapeId = (*iter)->type();

    const bool selected = index == selectedCollider;

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    ImGui::SameLine();
    if ( ImGui::Selectable(shapeId.c_str(),
                           selected, flags) )
    {
      selectedCollider = index;

      colliderWindowOpened = true;
      ImGui::SetWindowFocus("###colliderEditWindow");
    }

    static auto iter_dragged = colliders.end();

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
             iter_dragged >= colliders.begin() &&
             iter_dragged < colliders.end() )
          std::swap(*iter, *iter_dragged);

        iter_dragged = iter;
      }
    }

    if ( iter_dragged < colliders.end() &&
         ImGui::IsMouseReleased(ImGuiMouseButton_Left) == true )
      iter_dragged = colliders.end();

    ImGui::PopID(); // index
  }

  if ( colliderWindowOpened == false )
    return;

  if ( selectedCollider < 0 ||
       selectedCollider >= colliders.size() )
    return;

  const auto collider = colliders.at(selectedCollider);

  const auto windowTitle = format("Collider '{}'###colliderEditWindow",
                                  collider->type());

  if ( ImGui::Begin(windowTitle.c_str(),
                    &colliderWindowOpened, ImGuiWindowFlags_MenuBar) )
    collider->ui_show(registry);

  ImGui::End(); // windowTitle
}

} // namespace cqde
