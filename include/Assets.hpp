#ifndef ASSETS_HPP
#define ASSETS_HPP

#include <map>
#include <string>
#include <memory>


template <class T>
class Singleton
{
protected:
  static T instance;

  inline Singleton() = default;
  virtual inline ~Singleton() = default;

public:
  Singleton( Singleton& ) = delete;
  void operator=( const Singleton & ) = delete;

  static inline T* get()
  {
    return &instance;
  }
};

template <class Asset, typename AssetId>
class Assets
{
protected:
  static std::string AssetsPath;
  static std::string AssetType;

  static std::map <AssetId, std::shared_ptr <Asset>> mAssets;

public:
  static bool Load( const AssetId, const std::string& );
  static Asset& Get( const AssetId );
};


#endif
