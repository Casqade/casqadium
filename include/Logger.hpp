#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <fstream>


inline std::string operator ""_s ( const char* str,
                                   std::size_t )
{
  return std::string(str);
}

class Logger :  private std::streambuf,
                public std::ostream
{
  bool mLogToCmd;
  bool mLogToFile;
  std::string mFileName;

public:
  Logger();
  Logger( const std::string& fileName );

  int overflow( int ch ) override;

  Logger& operator << ( const char* input );
  Logger& operator << ( const std::string& input );

  template <typename Input>
  inline Logger& operator << ( const Input& input )
  {
    return operator << ( std::to_string(input) );
  }

  std::string outputFileName() const;
};


#endif
