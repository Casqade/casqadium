#include <olcPGE/ImageLoader.hpp>
#include <olcPGE/Sprite.hpp>

#include <filesystem>
namespace _gfs = std::filesystem;


// O------------------------------------------------------------------------------O
// | START IMAGE LOADER: GDI+, Windows Only, always exists, a little slow         |
// O------------------------------------------------------------------------------O

#if defined(OLC_IMAGE_GDI)

#define _WINSOCKAPI_ // Thanks Cornchipss
#if !defined(VC_EXTRALEAN)
#define VC_EXTRALEAN
#endif

// In Code::Blocks
#if !defined(_WIN32_WINNT)
#ifdef HAVE_MSMF
#define _WIN32_WINNT 0x0600 // Windows Vista
#else
#define _WIN32_WINNT 0x0500 // Windows 2000
#endif
#endif

#include <objidl.h>
#include <gdiplus.h>
#if defined(__MINGW32__) // Thanks Gusgo & Dandistine, but c'mon mingw!! wtf?!
#include <gdiplus/gdiplusinit.h>
#else
#include <gdiplusinit.h>
#endif
#include <shlwapi.h>

#if !defined(__MINGW32__)
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Shlwapi.lib")
#endif

namespace olc
{
  // Thanks @MaGetzUb for this, which allows sprites to be defined
  // at construction, by initialising the GDI subsystem
  static class GDIPlusStartup
  {
  public:
    GDIPlusStartup()
    {
      Gdiplus::GdiplusStartupInput startupInput;
      GdiplusStartup(&token, &startupInput, NULL);
    }

    ULONG_PTR	token;

    ~GDIPlusStartup()
    {
      // Well, MarcusTU thought this was important :D
      Gdiplus::GdiplusShutdown(token);
    }
  } gdistartup;

  class ImageLoader_GDIPlus : public olc::ImageLoader
  {
  private:
    std::wstring ConvertS2W(std::string s)
    {
#ifdef __MINGW32__
      wchar_t* buffer = new wchar_t[s.length() + 1];
      mbstowcs(buffer, s.c_str(), s.length());
      buffer[s.length()] = L'\0';
#else
      int count = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
      wchar_t* buffer = new wchar_t[count];
      MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, buffer, count);
#endif
      std::wstring w(buffer);
      delete[] buffer;
      return w;
    }

  public:
    ImageLoader_GDIPlus() : ImageLoader()
    {}

    olc::rcode LoadImageResource(olc::Sprite* spr, const std::string& sImageFile) override
    {
      // clear out existing sprite
      spr->pColData.clear();

      // Open file
      Gdiplus::Bitmap* bmp = nullptr;

      // Check file exists
      if (!_gfs::exists(sImageFile)) return olc::rcode::NO_FILE;

      // Load sprite from file
      bmp = Gdiplus::Bitmap::FromFile(ConvertS2W(sImageFile).c_str());

      if (bmp->GetLastStatus() != Gdiplus::Ok) return olc::rcode::FAIL;
      spr->width = bmp->GetWidth();
      spr->height = bmp->GetHeight();

      spr->pColData.resize(spr->width * spr->height);

      for (int y = 0; y < spr->height; y++)
        for (int x = 0; x < spr->width; x++)
          {
            Gdiplus::Color c;
            bmp->GetPixel(x, y, &c);
            spr->SetPixel(x, y, olc::Pixel(c.GetRed(), c.GetGreen(), c.GetBlue(), c.GetAlpha()));
          }
      delete bmp;
      return olc::rcode::OK;
    }
  };
} // namespace olc

#endif

// O------------------------------------------------------------------------------O
// | END IMAGE LOADER: GDI+                                                       |
// O------------------------------------------------------------------------------O


// O------------------------------------------------------------------------------O
// | START IMAGE LOADER: libpng, default on linux, requires -lpng  (libpng-dev)   |
// O------------------------------------------------------------------------------O

#if defined(OLC_IMAGE_LIBPNG)

#include <png.h>

namespace olc
{
  void pngReadStream(png_structp pngPtr, png_bytep data, png_size_t length)
  {
    png_voidp a = png_get_io_ptr(pngPtr);
    ((std::istream*)a)->read((char*)data, length);
  }

  class ImageLoader_LibPNG : public olc::ImageLoader
  {
  public:
    ImageLoader_LibPNG() : ImageLoader()
    {}

    olc::rcode LoadImageResource(olc::Sprite* spr, const std::string& sImageFile) override
    {
      // clear out existing sprite
      spr->pColData.clear();

      ////////////////////////////////////////////////////////////////////////////
      // Use libpng, Thanks to Guillaume Cottenceau
      // https://gist.github.com/niw/5963798
      // Also reading png from streams
      // http://www.piko3d.net/tutorials/libpng-tutorial-loading-png-files-from-streams/
      png_structp png;
      png_infop info;

      auto loadPNG = [&]()
      {
          png_read_info(png, info);
          png_byte color_type;
          png_byte bit_depth;
          png_bytep* row_pointers;
          spr->width = png_get_image_width(png, info);
          spr->height = png_get_image_height(png, info);
          color_type = png_get_color_type(png, info);
          bit_depth = png_get_bit_depth(png, info);
          if (bit_depth == 16) png_set_strip_16(png);
          if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
          if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)	png_set_expand_gray_1_2_4_to_8(png);
          if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
          if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)
            png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
          if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
            png_set_gray_to_rgb(png);
          png_read_update_info(png, info);
          row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * spr->height);
          for (int y = 0; y < spr->height; y++) {
              row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png, info));
            }
          png_read_image(png, row_pointers);
          ////////////////////////////////////////////////////////////////////////////
          // Create sprite array
          spr->pColData.resize(spr->width * spr->height);
          // Iterate through image rows, converting into sprite format
          for (int y = 0; y < spr->height; y++)
            {
              png_bytep row = row_pointers[y];
              for (int x = 0; x < spr->width; x++)
                {
                  png_bytep px = &(row[x * 4]);
                  spr->SetPixel(x, y, Pixel(px[0], px[1], px[2], px[3]));
                }
            }

          for (int y = 0; y < spr->height; y++) // Thanks maksym33
            free(row_pointers[y]);
          free(row_pointers);
          png_destroy_read_struct(&png, &info, nullptr);
        };

      png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
      if (!png) goto fail_load;

      info = png_create_info_struct(png);
      if (!info) goto fail_load;

      if (setjmp(png_jmpbuf(png))) goto fail_load;

      {
        FILE* f = fopen(sImageFile.c_str(), "rb");
        if (!f) return olc::rcode::NO_FILE;
        png_init_io(png, f);
        loadPNG();
        fclose(f);
      }

      return olc::rcode::OK;

fail_load:
      spr->width = 0;
      spr->height = 0;
      spr->pColData.clear();
      return olc::rcode::FAIL;
    }
  };
} // namespace olc

#endif

// O------------------------------------------------------------------------------O
// | END IMAGE LOADER: libpng                                                     |
// O------------------------------------------------------------------------------O

// O------------------------------------------------------------------------------O
// | START IMAGE LOADER: stb_image.h, all systems, very fast                      |
// O------------------------------------------------------------------------------O
// Thanks to Sean Barrett - https://github.com/nothings/stb/blob/master/stb_image.h
// MIT License - Copyright(c) 2017 Sean Barrett

#if defined(OLC_IMAGE_STB)

#define STB_IMAGE_IMPLEMENTATION
#include <olcPGE/thirdparty/stb_image.h>

#include <cstring>


namespace olc
{
  class ImageLoader_STB : public olc::ImageLoader
  {
  public:
    ImageLoader_STB() : ImageLoader()
    {}

    olc::rcode LoadImageResource(olc::Sprite* spr, const std::string& sImageFile) override
    {
      // clear out existing sprite
      spr->pColData.clear();

      // Open file
      stbi_uc* bytes = nullptr;
      int w = 0, h = 0, cmp = 0;

      // Check file exists
      if (!_gfs::exists(sImageFile)) return olc::rcode::NO_FILE;
      bytes = stbi_load(sImageFile.c_str(), &w, &h, &cmp, 4);

      if (!bytes) return olc::rcode::FAIL;
      spr->width = w; spr->height = h;
      spr->pColData.resize(spr->width * spr->height);
      std::memcpy(spr->pColData.data(), bytes, spr->width * spr->height * 4);
      delete[] bytes;
      return olc::rcode::OK;
    }
  };
}

#endif

// O------------------------------------------------------------------------------O
// | END IMAGE LOADER: stb_image.h                                              |
// O------------------------------------------------------------------------------O


std::unique_ptr <olc::ImageLoader> olc::Sprite::loader =

#if defined(OLC_IMAGE_GDI)
    std::make_unique <olc::ImageLoader_GDIPlus> ();

#elif defined(OLC_IMAGE_LIBPNG)
    std::make_unique <olc::ImageLoader_LibPNG> ();

#elif defined(OLC_IMAGE_STB)
    std::make_unique <olc::ImageLoader_STB> ();
#endif
