#include <fstream>
#include <iostream>

#include <Logger.hpp>

Logger::Logger()
  : Logger("log.log")
{}

Logger::Logger(const std::string& fileName)
  : std::ostream(this)
  , mLogToCmd(true)
  , mLogToFile(true)
  , mFileName(fileName)
{
  if (fileName.empty())
  {
      mLogToFile = false;
      return;
  }

  std::ofstream logFile( mFileName, std::ios::trunc );

  if ( logFile.is_open() )
    logFile.close();
  else
    mLogToFile = false;
}

int
Logger::overflow( int ch )
{
  if ( mLogToCmd )
    std::cout.put( ch );

  if ( mLogToFile )
  {
    std::ofstream logFile( mFileName, std::ios::app );
    if ( logFile.is_open() )
    {
      logFile.put( ch );
      logFile.close();
    }
  }

  return 0;
}

Logger&
Logger::operator << (const char* input)
{
  return operator << ( std::string(input) );
}

Logger&
Logger::operator << (const std::string& input)
{
  if ( mLogToCmd )
    std::cout << input + "\n";

  if ( mLogToFile )
  {
    std::ofstream logFile( mFileName, std::ios::app );
    if ( logFile.is_open() )
    {
      logFile << input + "\n";
      logFile.close();
    }
  }

  return *this;
}

std::string
Logger::outputFileName() const
{
  return mFileName;
}
