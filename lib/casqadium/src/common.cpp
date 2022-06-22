#include <cqde/common.hpp>
#include <cqde/components/InputController.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/Tag.hpp>
#include <cqde/components/Transform.hpp>

#include <cqde/types/input/InputBinding.hpp>
#include <cqde/types/EntityTagManager.hpp>
#include <cqde/types/InputCallbackStorage.hpp>

#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>

#include <entt/entity/registry.hpp>

#include <olcPGE/olcMouseInputId.hpp>

#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>

#include <thirdparty/ctpl/ctpl_stl.h>


namespace cqde
{

Json::CharReaderBuilder
jsonReader()
{
  static Json::CharReaderBuilder reader =
  []
  {
    Json::CharReaderBuilder reader {};

    reader["collectComments"] = true;
    reader["allowComments"] = true;
    reader["allowTrailingCommas"] = true;
    reader["strictRoot"] = true;
    reader["allowDroppedNullPlaceholders"] = false;
    reader["allowNumericKeys"] = false;
    reader["allowSingleQuotes"] = false;
    reader["stackLimit"] = 1000;
    reader["failIfExtra"] = false;
    reader["rejectDupKeys"] = true;
    reader["allowSpecialFloats"] = true;
    reader["skipBom"] = true;

    return reader;
  }();

  return reader;
}

Json::StreamWriterBuilder
jsonWriter()
{
  static Json::StreamWriterBuilder writer =
  []
  {
    Json::StreamWriterBuilder writer {};

    writer["indentation"] = "  ";
    writer["commentStyle"] = "All";
    writer["precisionType"] = "  ";
    writer["enableYAMLCompatibility"] = false;
    writer["dropNullPlaceholders"] = false;
    writer["useSpecialFloats"] = true;
    writer["emitUTF8"] = true;
    writer["precision"] = 17;
    writer["precisionType"] = "significant";

    return writer;
  }();

  return writer;
}

static std::string
openmodeToString( const std::ios::openmode flags )
{
  std::string result {};

  if ( (flags & std::ios::binary) == std::ios::binary )
    result += "binary,";

  if ( (flags & std::ios::in) == std::ios::in )
    result += "in,";

  if ( (flags & std::ios::out) == std::ios::out )
    result += "out,";

  if ( (flags & std::ios::app) == std::ios::app )
    result += "app,";

  if ( (flags & std::ios::ate) == std::ios::ate )
    result += "ate,";

  if ( (flags & std::ios::trunc) == std::ios::trunc )
    result += "trunc,";

  if ( result.empty() == false )
    result.pop_back();  // get rid of trailing comma

  return result;
}

std::fstream
fileOpen(
  const std::filesystem::path& path,
  const std::ios::openmode flags )
{
  std::filesystem::file_status fileStatus {};
  std::filesystem::path targetPath = path;

  try
  {
    if ( std::filesystem::is_symlink(path) == true )
      targetPath = std::filesystem::read_symlink(path);

    fileStatus = std::filesystem::status(targetPath);
  }
  catch ( const std::filesystem::filesystem_error& e )
  {
    throw std::runtime_error(cqde::format("Failed to open '{}': {}",
                                          path.string(), e.code().message()));
  }

  if ( (flags & std::ios::in) == std::ios::in &&
       fileStatus.type() == std::filesystem::file_type::not_found )
    throw std::runtime_error(cqde::format("Failed to open '{}': {}",
                                          path.string(), std::strerror(ENOENT)));

  if ( fileStatus.type() != std::filesystem::file_type::not_found &&
       fileStatus.type() != std::filesystem::file_type::regular )
    throw std::runtime_error(cqde::format("Failed to open '{}': Is not a regular file", path.string()));

  std::fstream file {};
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try
  {
    file.open(targetPath, flags);
  }
  catch ( const std::system_error& e )
  {
    throw std::runtime_error(cqde::format("Failed to open '{}' (mode={}): {}",
                                          path.string(), openmodeToString(flags),
                                          std::strerror(errno)));
  }

  return file;
}

Json::Value
jsonParse( std::istream& stream )
{
  Json::Value result {};
  Json::String parseErrors {};

  if ( Json::parseFromStream( jsonReader(), stream,
                              &result, &parseErrors ) == false )
  {
    throw std::runtime_error(parseErrors);
  }

  return result;
}

Json::Value
fileParse( const std::filesystem::path& path )
{
  try
  {
    std::fstream file = fileOpen( path, std::ios::in );
    return jsonParse(file);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(cqde::format("Failed to parse JSON '{}': {}",
                                          path.string(), e.what()));
  }
}

void
initHwControls( types::HwControlMap& controlMap )
{
  using olc::MouseInputId;

  controlMap =
  {
    {olc::Key::NONE, cqde::InputHwId("Key_Unknown")},
    {olc::Key::A, cqde::InputHwId("Key_A")},
    {olc::Key::D, cqde::InputHwId("Key_D")},
    {olc::Key::S, cqde::InputHwId("Key_S")},
    {olc::Key::W, cqde::InputHwId("Key_W")},
    {olc::Key::SHIFT, cqde::InputHwId("Key_Shift")},
    {olc::Key::SPACE, cqde::InputHwId("Key_Space")},
    {olc::Key::ENTER, cqde::InputHwId("Key_Enter")},

    {int32_t(MouseInputId::ButtonLeft), cqde::InputHwId("MouseButton_Left")},
    {int32_t(MouseInputId::ButtonRight), cqde::InputHwId("MouseButton_Right")},
    {int32_t(MouseInputId::ButtonMiddle), cqde::InputHwId("MouseButton_Middle")},
    {int32_t(MouseInputId::ButtonX1), cqde::InputHwId("MouseButton_X1")},
    {int32_t(MouseInputId::ButtonX2), cqde::InputHwId("MouseButton_X2")},

    {int32_t(MouseInputId::Wheel), cqde::InputHwId("MouseWheel_Y")},

    {int32_t(MouseInputId::MoveX), cqde::InputHwId("MouseMove_X")},
    {int32_t(MouseInputId::MoveY), cqde::InputHwId("MouseMove_Y")},
    {int32_t(MouseInputId::PosX), cqde::InputHwId("MousePos_X")},
    {int32_t(MouseInputId::PosY), cqde::InputHwId("MousePos_Y")},

    {int32_t(MouseInputId::ENUM_END) + 1, cqde::InputHwId("Undefined")},
  };
}

void
engineInit( entt::registry& registry )
{
  using namespace cqde::compos;
  using namespace cqde::types;

  InputController::Register();
  SceneNode::Register();
  Tag::Register();
  Transform::Register();

  registry.ctx().emplace <EntityTagManager> ();
  registry.ctx().emplace <InputCallbackStorage> ();

  initHwControls( registry.ctx().emplace <HwControlMap> () );
  registry.ctx().emplace <InputBindings> ();

  auto& tp = registry.ctx().emplace <ctpl::thread_pool> (std::thread::hardware_concurrency() | 1);

  registry.ctx().emplace <FontAssetManager> (tp);
  registry.ctx().emplace <TextureAssetManager> (tp);
  registry.ctx().emplace <TextStringAssetManager> (tp);
}

} // namespace cqde
