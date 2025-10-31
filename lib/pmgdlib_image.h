// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_IMAGE_HH
#define PMGDLIB_IMAGE_HH 1

#include "pmgdlib_defs.h"
#include "pmgdlib_core.h"

#ifdef USE_STB
  #define STBI_ONLY_PNG 1
  #define STB_IMAGE_IMPLEMENTATION
  #define STB_IMAGE_WRITE_IMPLEMENTATION
  #include "stb_image.h"
  #include "stb_image_write.h"
#endif

namespace pmgd {

  // image base class
  class Image {
    public:
      void *data;
      int w,h;
      int format, type;
      v2 size;

    public:
      Image(void *data, int w, int h, int format, int type){
        this->data = data;
        this->w = w;
        this->h = h;
        this->format = format;
        this->type = type;
        size = v2(w,h);

      }
  };

  std::shared_ptr<Image> get_test_image(int type = image_type::FLOAT){
    if(type == image_type::UNSIGNED_CHAR){
      unsigned char *test_image_data = new unsigned char[64] { 255, 0, 0, 255,   0, 255, 0, 255,   255, 0, 0, 255,   0, 0, 255, 255,
                                                               0, 255, 0, 255,   255, 0, 0, 255,   0, 255, 0, 255,   255, 0, 0, 255,
                                                               255, 0, 0, 255,   0, 255, 0, 255,   255, 0, 0, 255,   0, 0, 255, 255,
                                                               0, 255, 0, 255,   255, 0, 0, 255,   0, 255, 0, 255,   255, 0, 0, 255 };
      return std::make_shared<Image>(test_image_data, 4, 4, image_format::RGBA, image_type::UNSIGNED_CHAR);
    }

    // checker image, 4x4, GL_RGBA, GL_FLOAT data.
    float *test_image_data = new float[64] { 1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 1.0f,
                                             0.0f, 0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f, 1.0f,
                                             1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 1.0f,
                                             0.0f, 0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f, 1.0f };

    return std::make_shared<Image>(test_image_data, 4, 4, image_format::RGBA, image_type::FLOAT);
  };

  #ifdef USE_STB
    class IoImageStb : public IoImage {
      public:
      virtual std::shared_ptr<Image> Read(const std::string & path) {
        int w, h, n;
        unsigned char *data = stbi_load(path.c_str(), &w, &h, &n, 0);
        int format = image_format::UNDEFINED;
        if(n == 4) format = image_format::RGBA;
        return std::make_shared<Image>((void*)data, w, h, format, image_type::UNSIGNED_CHAR);
      };

      virtual int Write(const std::string & path, std::shared_ptr<Image> image) {
        int n = 0;
        if(image->format == image_format::RGBA) n = 4;
        return stbi_write_png(path.c_str(), image->w, image->h, n, image->data, 0) ? PM_SUCCESS : PM_ERROR_STB;
      }
    };
  #endif
};

#endif
