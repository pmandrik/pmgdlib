// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_DEFS_HH
#define PMGDLIB_DEFS_HH 1

namespace pmgd {

  //! functions exit codes
  enum {
    PM_SUCCESS = 0,
    PM_ERROR_DUPLICATE = 1,
    PM_ERROR_404 = 404,
    PM_ERROR_500 = 500,
    PM_ERROR_INCORRECT_ARGUMENTS,
    PM_ERROR_CLASS_ATTRIBUTES,
    PM_ERROR_GL
  };

  namespace image_format {
    enum  {
      UNDEFINED = -1,
      RGBA,
    };
  };

  namespace image_type {
    enum  {
      UNDEFINED = -1,
      UNSIGNED_INT,
      FLOAT,
    };
  };

};

#endif
