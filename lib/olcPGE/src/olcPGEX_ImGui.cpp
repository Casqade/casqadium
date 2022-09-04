#include <olcPGE/olcPGEX_ImGui.hpp>


#ifdef OLC_GFX_OPENGL33
#define GLEW_STATIC
#include <GL/glew.h>
#undef GLEW_STATIC
#include "backends/imgui_impl_opengl3.h"
#else
#include "backends/imgui_impl_opengl2.h"
#endif

#include <ImGuizmo.h>


namespace olc
{

namespace imgui
{

PGE_ImGUI::PGE_ImGUI(bool _register_handler)
  : PGEX(true)
  , register_handler(_register_handler)
{

}

olc::rcode PGE_ImGUI::ImGui_ImplPGE_Init()
{
  ImGui::CreateContext();
  ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());

#ifdef OLC_GFX_OPENGL33
  GLenum err = glewInit();
  if ( err != GLEW_OK )
    return rcode::FAIL;

  if ( ImGui_ImplOpenGL3_Init() == false )
    return rcode::FAIL;
#else
  if ( ImGui_ImplOpenGL2_Init() == false )
    return rcode::FAIL;
#endif
  ImGuiIO& io = ImGui::GetIO();

  io.BackendPlatformName = "imgui_impl_pge";
  io.ConfigDockingWithShift = true;
  io.ConfigWindowsMoveFromTitleBarOnly = true;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  io.KeyMap[ImGuiKey_Tab] = olc::TAB;
  io.KeyMap[ImGuiKey_LeftArrow] = olc::LEFT;
  io.KeyMap[ImGuiKey_RightArrow] = olc::RIGHT;
  io.KeyMap[ImGuiKey_UpArrow] = olc::UP;
  io.KeyMap[ImGuiKey_DownArrow] = olc::DOWN;
  io.KeyMap[ImGuiKey_PageUp] = olc::PGUP;
  io.KeyMap[ImGuiKey_PageDown] = olc::PGDN;
  io.KeyMap[ImGuiKey_Home] = olc::HOME;
  io.KeyMap[ImGuiKey_End] = olc::END;
  io.KeyMap[ImGuiKey_Insert] = olc::INS;
  io.KeyMap[ImGuiKey_Delete] = olc::DEL;
  io.KeyMap[ImGuiKey_Backspace] = olc::BACK;
  io.KeyMap[ImGuiKey_Space] = olc::SPACE;
  io.KeyMap[ImGuiKey_Enter] = olc::ENTER;
  io.KeyMap[ImGuiKey_Escape] = olc::ESCAPE;
  io.KeyMap[ImGuiKey_KeypadEnter] = olc::RETURN;
  io.KeyMap[ImGuiKey_A] = olc::A;
  io.KeyMap[ImGuiKey_C] = olc::C;
  io.KeyMap[ImGuiKey_V] = olc::V;
  io.KeyMap[ImGuiKey_X] = olc::X;
  io.KeyMap[ImGuiKey_Y] = olc::Y;
  io.KeyMap[ImGuiKey_Z] = olc::Z;

  //Create a listing of all the control keys so we can iterate them later
  vControlInputKeys =
  {
    olc::TAB, olc::LEFT, olc::RIGHT, olc::UP, olc::DOWN,
    olc::PGUP, olc::PGDN, olc::HOME, olc::END, olc::INS,
    olc::DEL, olc::BACK, olc::SPACE, olc::ENTER, olc::ESCAPE,
    olc::RETURN, olc::A, olc::C, olc::V, olc::X, olc::Y, olc::Z
  };

  //Map keys which input values to input boxes.
  //This is not the ideal way to do this, but PGE does not expose
  //much in the way of keyboard or IME input.  Many special characters
  //are absent as well.
  vValueInputKeys =
  {
    {olc::A, 'a', 'A'},
    {olc::B, 'b', 'B'},
    {olc::C, 'c', 'C'},
    {olc::D, 'd', 'D'},
    {olc::E, 'e', 'E'},
    {olc::F, 'f', 'F'},
    {olc::G, 'g', 'G'},
    {olc::H, 'h', 'H'},
    {olc::I, 'i', 'I'},
    {olc::J, 'j', 'J'},
    {olc::K, 'k', 'K'},
    {olc::L, 'l', 'L'},
    {olc::M, 'm', 'M'},
    {olc::N, 'n', 'N'},
    {olc::O, 'o', 'O'},
    {olc::P, 'p', 'P'},
    {olc::Q, 'q', 'Q'},
    {olc::R, 'r', 'R'},
    {olc::S, 's', 'S'},
    {olc::T, 't', 'T'},
    {olc::U, 'u', 'U'},
    {olc::V, 'v', 'V'},
    {olc::W, 'w', 'W'},
    {olc::X, 'x', 'X'},
    {olc::Y, 'y', 'Y'},
    {olc::Z, 'z', 'Z'},
    {olc::K0, '0', ')'},
    {olc::K1, '1', '!'},
    {olc::K2, '2', '@'},
    {olc::K3, '3', '#'},
    {olc::K4, '4', '$'},
    {olc::K5, '5', '%'},
    {olc::K6, '6', '^'},
    {olc::K7, '7', '&'},
    {olc::K8, '8', '*'},
    {olc::K9, '9', '('},
    {olc::NP0, '0', '0'},
    {olc::NP1, '1', '1'},
    {olc::NP2, '2', '2'},
    {olc::NP3, '3', '3'},
    {olc::NP4, '4', '4'},
    {olc::NP5, '5', '5'},
    {olc::NP6, '6', '6'},
    {olc::NP7, '7', '7'},
    {olc::NP8, '8', '8'},
    {olc::NP9, '9', '9'},
    {olc::NP_MUL, '*', '*'},
    {olc::NP_DIV, '/', '/'},
    {olc::NP_ADD, '+', '+'},
    {olc::NP_SUB, '-', '-'},
    {olc::NP_DECIMAL, '.', '.'},
    {olc::PERIOD, '.', '>'},
    {olc::SPACE, ' ', ' '},
    {olc::OEM_1, ';', ':'},
    {olc::OEM_2, '/', '?'},
    {olc::OEM_3, '`', '~'},
    {olc::OEM_4, '[', '{'},
    {olc::OEM_5, '\\', '|'},
    {olc::OEM_6, ']', '}'},
    {olc::OEM_7, '\'', '"'},
    {olc::OEM_8, '-', '-'},
    {olc::EQUALS, '=', '+'},
    {olc::COMMA, ',', '<'},
    {olc::MINUS, '-', '_'}
  };

  return rcode::OK;
}

//This currently does nothing, but is defined in the event that it needs to eventually do something
void PGE_ImGUI::ImGui_ImplPGE_Shutdown(void)
{
  return;
}

void PGE_ImGUI::ImGui_ImplPGE_UpdateMouse(void)
{
  if ( olc::platform->ptrPGE->GetKeepMouseCentered() == true )
  {
    ImGuiIO& io = ImGui::GetIO();

    io.MouseDown[0] = false;
    io.MouseDown[1] = false;
    io.MouseDown[2] = false;
    io.MouseDown[3] = false;
    io.MouseDown[4] = false;

    io.MousePos = {-100.0f, -100.0f};
    io.MouseWheel = 0.0f;

    return;
  }

  ImGuiIO& io = ImGui::GetIO();
  olc::vi2d windowMouse = pge->GetWindowMouse();

  //Update Mouse Buttons
  io.MouseDown[0] = pge->GetMouse(0).bHeld;
  io.MouseDown[1] = pge->GetMouse(1).bHeld;
  io.MouseDown[2] = pge->GetMouse(2).bHeld;
  io.MouseDown[3] = pge->GetMouse(3).bHeld;
  io.MouseDown[4] = pge->GetMouse(4).bHeld;

  //Update the Mouse Position
  io.MousePos = ImVec2((float)windowMouse.x, (float)windowMouse.y);

  //Update Mouse Wheel
  io.MouseWheel = static_cast<float>(pge->GetMouseWheel()) / fScrollSensitivity;
}

void PGE_ImGUI::ImGui_ImplPGE_UpdateKeys(void)
{
  ImGuiIO& io = ImGui::GetIO();

  io.KeyCtrl = pge->GetKey(olc::CTRL).bHeld;
  io.KeyAlt = false; // pge->GetKey(olc::ALT).bHeld;
  io.KeyShift = pge->GetKey(olc::SHIFT).bHeld;
  io.KeySuper = false;

  //Update the status of the control keys.
  for (auto const& x : vControlInputKeys) {
      io.KeysDown[x] = pge->GetKey(x).bHeld;
    }

  //Get the status of the SHIFT key to so we can change the inserted character
  bool isShift = pge->GetKey(SHIFT).bHeld;

  //Get characters that can be entered into inputs
  for (auto& m : vValueInputKeys) {
      if (pge->GetKey(m.key).bPressed) {
          io.AddInputCharacter(isShift ? m.upper : m.lower);
        }
    }
}

void PGE_ImGUI::ImGui_ImplPGE_NewFrame(void)
{
  if ( pge->GetElapsedTime() <= 0.0f )
    return;

#ifdef OLC_GFX_OPENGL33
  ImGui_ImplOpenGL3_NewFrame();
#else
  ImGui_ImplOpenGL2_NewFrame();
#endif

  ImGuiIO& io = ImGui::GetIO();
  olc::vi2d windowSize = pge->GetWindowSize();
  IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL2_NewFrame().");

  io.DisplaySize = ImVec2((float)windowSize.x, (float)windowSize.y);

  io.DeltaTime = pge->GetElapsedTime();

  ImGui_ImplPGE_UpdateKeys();

  ImGui_ImplPGE_UpdateMouse();
}

void PGE_ImGUI::ImGui_ImplPGE_SetScrollSensitivity(float val)
{
  this->fScrollSensitivity = val;
}

void PGE_ImGUI::ImGui_ImplPGE_Render(void)
{
  //This finishes the Dear ImGui and renders it to the screen
  ImGui::Render();
#ifdef OLC_GFX_OPENGL33
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#else
  ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
#endif
}

//Before the OnUserCreate function runs, we will register the UI drawing function
//to the layer 0, if we were told to do so during construction
//We can't register this during construction because the pge-vLayers is not configured until after construction
void PGE_ImGUI::OnBeforeUserCreate()
{
  if (register_handler) {
      pge->SetLayerCustomRenderFunction(0, std::bind(&PGE_ImGUI::ImGui_ImplPGE_Render, this));
    }
}

//After the OnUserCreate function runs, we will run all of the Init code to setup
//the ImGui.  This will happen automatically in PGE 2.10+
void PGE_ImGUI::OnAfterUserCreate()
{
  ImGui_ImplPGE_Init();
}

//Before the OnUserUpdate runs, do the pre-frame ImGui intialization
void PGE_ImGUI::OnBeforeUserUpdate(float& fElapsedTime)
{
  ImGui_ImplPGE_NewFrame();
  ImGui::NewFrame();
  ImGuizmo::BeginFrame();
}

//There is currently no "after update" logic to run for ImGui
void PGE_ImGUI::OnAfterUserUpdate(float fElapsedTime)
{}

}

}
