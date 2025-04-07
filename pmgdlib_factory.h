// P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib
#ifndef PMGDLIB_FACTORY_HH
#define PMGDLIB_FACTORY_HH 1

#include "pmgdlib_core.h"

#ifdef USE_SDL
  #include "pmgdlib_sdl.h"
#endif

#ifdef USE_GL
  #include "pmgdlib_gl.h"
#endif

namespace pmgd {

  class Backend {
    public:
    std::shared_ptr<IO> io;
  };

  struct BackendOptions {
    std::string accelerator;
    std::string io;
  };

  Backend get_backend(BackendOptions options){
    std::shared_ptr<IO> io = std::make_shared<IO>();
    io->txt_imp = std::make_shared<IoTxt>();

    Backend back;
    back.io = io;

    if(options.accelerator == "GL"){
    }

    #ifdef USE_SDL
    if(options.io == "SDL"){
      io->txt_imp = std::make_shared<IoTxtSDL>();
    }
    #endif

    return back;
  }

};

#endif
