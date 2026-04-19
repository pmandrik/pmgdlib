// P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib

#include "pmgdlib_factory.h"
#include "pmgdlib_defs.h"

namespace pmgd {

  // ======= factory ====================================================================

  // ======= functions to use outside ====================================================================
  std::shared_ptr<Backend> get_backend(const SysOptions & options){
    int verbose_lvl = msg_verbose_lvl();

    msg_debug("Factory backend start ...");
    std::shared_ptr<Backend> back = std::make_shared<Backend>();

    #ifdef USE_GL
    if(options.accelerator == "GL"){
      msg_debug("setup GL accelerator backend ...");
      back->accel_imp = std::make_shared<AccelFactoryGL>();
      msg_debug("setup GL accelerator backend ... ok");
    }
    #endif

    if(options.io == "SDL"){
      #ifdef USE_SDL
        msg_debug("setup SDL IO backend ...");
        back->txt_imp = std::make_shared<IoTxtSDL>();
        msg_debug("setup SDL IO backend ... ok");
      #else
        msg_warning("to use SDL IO recompile sources with -DUSE_SDL");
      #endif
    }

    if(options.multimedia_library == "SDL"){
      #ifdef USE_SDL
        msg_debug("setup SDL multimedia backend ...");
        back->sys_imp = std::make_shared<SysFactorySDL>();
        msg_debug("setup SDL multimedia backend ... ok");
      #else
        msg_warning("to use SDL multimedia_library recompile sources with -DUSE_SDL");
      #endif
    }

    #ifdef USE_STB
    if(options.img == "STB"){
      msg_debug("setup STB image IO backend ...");
      back->img_imp = std::make_shared<IoImageStb>();
      msg_debug("setup STB image IO backend ... ok");
    }
    #endif

    msg_debug("Factory backend ok ...");
    return back;
  };
};