#include <cqde/types/LaunchParams.hpp>

#include <cqde/assert.hpp>

#include <algorithm>


namespace cqde::types
{

LaunchParams::LaunchParams(
  const int argc,
  char* argv[] )
{
  CQDE_ASSERT_DEBUG(argc >= 1, return);

  args.clear();

  for ( int i = 0; i < argc; ++i )
    args.push_back(argv[i]);
}

std::string
LaunchParams::executableName() const
{
  CQDE_ASSERT_DEBUG(args.size() > 0, return {});

  return args.front();
}

bool
LaunchParams::startEditor() const
{
  return std::find(
    args.begin(),
    args.end(),
    "--editor" ) != args.end();
}

std::string
LaunchParams::entryPoint() const
{
  const std::string entryKey {"--entry="};

  const auto iter = std::find_if(
    args.begin(),
    args.end(),
    [entryKey] ( const std::string& arg )
    {
      return arg.find(entryKey) != std::string::npos;
    });

  if ( iter == args.end() )
    return {};

  const auto entryPos = iter->find(entryKey) + entryKey.size();

  return iter->substr(entryPos);
}

} // namespace cqde::types
