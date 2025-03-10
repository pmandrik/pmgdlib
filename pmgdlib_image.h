// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_IMAGE_HH
#define PMGDLIB_IMAGE_HH 1

#include "pmgdlib_defs.h"

namespace pmgd {

  // image base class
  class Image {
    public:
      void *data;
      int w,h;
      int format,type;
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

  std::shared_ptr<Image> get_test_image(){
    // checker image, 4x4, GL_RGBA, GL_FLOAT data.
    float *test_image_data = new float[64] { 1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 1.0f,
                                             0.0f, 0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f, 1.0f,
                                             1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 1.0f,
                                             0.0f, 0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f, 1.0f };

    return std::make_shared<Image>(test_image_data, 4, 4, image_format::RGBA, image_type::FLOAT);
  };

};

#endif
