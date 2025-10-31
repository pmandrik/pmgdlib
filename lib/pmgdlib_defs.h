// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_DEFS_HH
#define PMGDLIB_DEFS_HH 1

namespace pmgd {

  //! functions exit codes
  enum {
    PM_SUCCESS = 0,
    PM_ERROR = 1,
    PM_ERROR_404 = 404,
    PM_ERROR_500 = 500,
    PM_ERROR_DUPLICATE = 500001,
    PM_ERROR_INCORRECT_ARGUMENTS = 500003,
    PM_ERROR_CLASS_ATTRIBUTES = 500004,
    PM_ERROR_SCHEMA = 500005,
    PM_ERROR_IO = 500006,
    PM_ERROR_GL = 500007,
    PM_ERROR_STB = 500008,
  };

  namespace image_format {
    enum  {
      UNDEFINED = -1,
      RGBA,
    };
  };

  namespace sys {
    static const float PERSPECTIVE_EDGE = -1000000;
  };

  namespace image_type {
    enum  {
      UNDEFINED = -1,
      UNSIGNED_CHAR,
      UNSIGNED_INT,
      FLOAT,
    };
  };

};

#endif
