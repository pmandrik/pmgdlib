// P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib

#ifndef TEST_DATA_HH
#define TEST_DATA_HH 1

#include "pmgdlib_core.h"
#include "pmgdlib_factory.h"

using namespace pmgd;

TEST(pmlib_data, io_load_dummy) {
  SysOptions bo;
  Backend bk = get_backend(bo);
  std::string txt_data = bk.io->ReadTxt("/any/path");
  EXPECT_EQ(txt_data, "dummy data");
}

TEST(pmlib_data, data_container) {
  DataContainer dc;
  std::string val = "123";
  std::shared_ptr<string> x1 = std::make_shared<std::string>(val);
  std::shared_ptr<int> x2 = std::make_shared<int>(atoi(val.c_str()));

  EXPECT_EQ(dc.Add("key 1", x1), PM_SUCCESS);
  EXPECT_EQ(dc.Add("key 2", x2), PM_SUCCESS);

  dc.verbose_lvl = verbose::SILENCE;
  EXPECT_EQ(dc.Add("key 2", x1), PM_ERROR_DUPLICATE);

  std::shared_ptr<string> y1 = dc.Get<string>("key 1");
  std::shared_ptr<int> y2 = dc.Get<int>("key 2");
  EXPECT_EQ(*y1, val);
  EXPECT_EQ(*y2, atoi(val.c_str()));
}

#ifdef USE_STB
TEST(pmlib_data, stb) {
  SysOptions bo;
  bo.img = "STB";
  Backend bk = get_backend(bo);

  std::shared_ptr<Image> img = get_test_image(image_type::UNSIGNED_CHAR);
  int ok = bk.io->WriteImage("img.png", img);
  EXPECT_EQ(ok, PM_SUCCESS);

  std::shared_ptr<Image> img2 = bk.io->ReadImage("img.png");
  EXPECT_EQ(img2->w, img->w);
  EXPECT_EQ(img2->h, img->h);
  for(int ww = 0; ww < img2->w; ww++){
    for(int hh = 0; hh < img2->h; hh++){
      for(int c = 0; c < 4; c++){
        EXPECT_EQ( ((unsigned char*)(img2->data))[ww + hh*img2->w + c], ((unsigned char*)(img->data))[ww + hh*img2->w + c]);
      }
    }
  }
}
#endif // USE_STB

#endif