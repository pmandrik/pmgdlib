// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_SDL_HH
#define PMGDLIB_SDL_HH 1

#include <SDL.h>
#include <SDL_opengl.h>

#include "pmgdlib_core.h"

namespace pmgd {

  const int SDL_KSTART = 4;
  const int SDL_KEND   = 231;
  enum key {
    A  =     SDL_SCANCODE_A ,
    B  =     SDL_SCANCODE_B ,
    C  =     SDL_SCANCODE_C ,
    D  =     SDL_SCANCODE_D ,
    E  =     SDL_SCANCODE_E ,
    F  =     SDL_SCANCODE_F ,
    G  =     SDL_SCANCODE_G ,
    H  =     SDL_SCANCODE_H ,
    I  =     SDL_SCANCODE_I ,
    J  =     SDL_SCANCODE_J ,
    K  =     SDL_SCANCODE_K ,
    L  =     SDL_SCANCODE_L ,
    M  =     SDL_SCANCODE_M ,
    N  =     SDL_SCANCODE_N ,
    O  =     SDL_SCANCODE_O ,
    P  =     SDL_SCANCODE_P ,
    Q  =     SDL_SCANCODE_Q ,
    R  =     SDL_SCANCODE_R ,
    S  =     SDL_SCANCODE_S ,
    T  =     SDL_SCANCODE_T ,
    U  =     SDL_SCANCODE_U ,
    V  =     SDL_SCANCODE_V ,
    W  =     SDL_SCANCODE_W ,
    X  =     SDL_SCANCODE_X ,
    Y  =     SDL_SCANCODE_Y ,
    Z  =     SDL_SCANCODE_Z ,
    K_1  =     SDL_SCANCODE_1 ,
    K_2  =     SDL_SCANCODE_2 ,
    K_3  =     SDL_SCANCODE_3 ,
    K_4  =     SDL_SCANCODE_4 ,
    K_5  =     SDL_SCANCODE_5 ,
    K_6  =     SDL_SCANCODE_6 ,
    K_7  =     SDL_SCANCODE_7 ,
    K_8  =     SDL_SCANCODE_8 ,
    K_9  =     SDL_SCANCODE_9 ,
    K_0  =     SDL_SCANCODE_0 ,
    RETURN  =     SDL_SCANCODE_RETURN ,
    ESCAPE  =     SDL_SCANCODE_ESCAPE ,
    BACKSPACE  =     SDL_SCANCODE_BACKSPACE ,
    TAB  =     SDL_SCANCODE_TAB ,
    SPACE  =     SDL_SCANCODE_SPACE ,
    MINUS  =     SDL_SCANCODE_MINUS ,
    EQUALS  =     SDL_SCANCODE_EQUALS ,
    LEFTBRACKET  =     SDL_SCANCODE_LEFTBRACKET ,
    RIGHTBRACKET  =     SDL_SCANCODE_RIGHTBRACKET ,
    BACKSLASH  =     SDL_SCANCODE_BACKSLASH ,
    NONUSHASH  =     SDL_SCANCODE_NONUSHASH ,
    SEMICOLON  =     SDL_SCANCODE_SEMICOLON ,
    APOSTROPHE  =     SDL_SCANCODE_APOSTROPHE ,
    GRAVE  =     SDL_SCANCODE_GRAVE ,
    COMMA  =     SDL_SCANCODE_COMMA ,
    PERIOD  =     SDL_SCANCODE_PERIOD ,
    SLASH  =     SDL_SCANCODE_SLASH ,
    CAPSLOCK  =     SDL_SCANCODE_CAPSLOCK ,
    F1  =     SDL_SCANCODE_F1 ,
    F2  =     SDL_SCANCODE_F2 ,
    F3  =     SDL_SCANCODE_F3 ,
    F4  =     SDL_SCANCODE_F4 ,
    F5  =     SDL_SCANCODE_F5 ,
    F6  =     SDL_SCANCODE_F6 ,
    F7  =     SDL_SCANCODE_F7 ,
    F8  =     SDL_SCANCODE_F8 ,
    F9  =     SDL_SCANCODE_F9 ,
    F10  =     SDL_SCANCODE_F10 ,
    F11  =     SDL_SCANCODE_F11 ,
    F12  =     SDL_SCANCODE_F12 ,
    PRINTSCREEN  =     SDL_SCANCODE_PRINTSCREEN ,
    SCROLLLOCK  =     SDL_SCANCODE_SCROLLLOCK ,
    PAUSE  =     SDL_SCANCODE_PAUSE ,
    INSERT  =     SDL_SCANCODE_INSERT ,
    HOME  =     SDL_SCANCODE_HOME ,
    PAGEUP  =     SDL_SCANCODE_PAGEUP ,
    DELETE  =     SDL_SCANCODE_DELETE ,
    END  =     SDL_SCANCODE_END ,
    PAGEDOWN  =     SDL_SCANCODE_PAGEDOWN ,
    RIGHT  =     SDL_SCANCODE_RIGHT ,
    LEFT  =     SDL_SCANCODE_LEFT ,
    DOWN  =     SDL_SCANCODE_DOWN ,
    UP  =     SDL_SCANCODE_UP ,
    NUMLOCKCLEAR  =     SDL_SCANCODE_NUMLOCKCLEAR ,
    KP_DIVIDE  =     SDL_SCANCODE_KP_DIVIDE ,
    KP_MULTIPLY  =     SDL_SCANCODE_KP_MULTIPLY ,
    KP_MINUS  =     SDL_SCANCODE_KP_MINUS ,
    KP_PLUS  =     SDL_SCANCODE_KP_PLUS ,
    KP_ENTER  =     SDL_SCANCODE_KP_ENTER ,
    KP_1  =     SDL_SCANCODE_KP_1 ,
    KP_2  =     SDL_SCANCODE_KP_2 ,
    KP_3  =     SDL_SCANCODE_KP_3 ,
    KP_4  =     SDL_SCANCODE_KP_4 ,
    KP_5  =     SDL_SCANCODE_KP_5 ,
    KP_6  =     SDL_SCANCODE_KP_6 ,
    KP_7  =     SDL_SCANCODE_KP_7 ,
    KP_8  =     SDL_SCANCODE_KP_8 ,
    KP_9  =     SDL_SCANCODE_KP_9 ,
    KP_0  =     SDL_SCANCODE_KP_0 ,
    KP_PERIOD  =     SDL_SCANCODE_KP_PERIOD ,
    NONUSBACKSLASH  =     SDL_SCANCODE_NONUSBACKSLASH ,
    APPLICATION  =     SDL_SCANCODE_APPLICATION ,
    POWER  =     SDL_SCANCODE_POWER ,
    KP_EQUALS  =     SDL_SCANCODE_KP_EQUALS ,
    F13  =     SDL_SCANCODE_F13 ,
    F14  =     SDL_SCANCODE_F14 ,
    F15  =     SDL_SCANCODE_F15 ,
    F16  =     SDL_SCANCODE_F16 ,
    F17  =     SDL_SCANCODE_F17 ,
    F18  =     SDL_SCANCODE_F18 ,
    F19  =     SDL_SCANCODE_F19 ,
    F20  =     SDL_SCANCODE_F20 ,
    F21  =     SDL_SCANCODE_F21 ,
    F22  =     SDL_SCANCODE_F22 ,
    F23  =     SDL_SCANCODE_F23 ,
    F24  =     SDL_SCANCODE_F24 ,
    EXECUTE  =     SDL_SCANCODE_EXECUTE ,
    HELP  =     SDL_SCANCODE_HELP ,
    MENU  =     SDL_SCANCODE_MENU ,
    SELECT  =     SDL_SCANCODE_SELECT ,
    STOP  =     SDL_SCANCODE_STOP ,
    AGAIN  =     SDL_SCANCODE_AGAIN ,
    UNDO  =     SDL_SCANCODE_UNDO ,
    CUT  =     SDL_SCANCODE_CUT ,
    COPY  =     SDL_SCANCODE_COPY ,
    PASTE  =     SDL_SCANCODE_PASTE ,
    FIND  =     SDL_SCANCODE_FIND ,
    MUTE  =     SDL_SCANCODE_MUTE ,
    VOLUMEUP  =     SDL_SCANCODE_VOLUMEUP ,
    VOLUMEDOWN  =     SDL_SCANCODE_VOLUMEDOWN ,
    KP_COMMA  =     SDL_SCANCODE_KP_COMMA ,
    KP_EQUALSAS400  =     SDL_SCANCODE_KP_EQUALSAS400 ,
    INTERNATIONAL1  =     SDL_SCANCODE_INTERNATIONAL1 ,
    INTERNATIONAL2  =     SDL_SCANCODE_INTERNATIONAL2 ,
    INTERNATIONAL3  =     SDL_SCANCODE_INTERNATIONAL3 ,
    INTERNATIONAL4  =     SDL_SCANCODE_INTERNATIONAL4 ,
    INTERNATIONAL5  =     SDL_SCANCODE_INTERNATIONAL5 ,
    INTERNATIONAL6  =     SDL_SCANCODE_INTERNATIONAL6 ,
    INTERNATIONAL7  =     SDL_SCANCODE_INTERNATIONAL7 ,
    INTERNATIONAL8  =     SDL_SCANCODE_INTERNATIONAL8 ,
    INTERNATIONAL9  =     SDL_SCANCODE_INTERNATIONAL9 ,
    LANG1  =     SDL_SCANCODE_LANG1 ,
    LANG2  =     SDL_SCANCODE_LANG2 ,
    LANG3  =     SDL_SCANCODE_LANG3 ,
    LANG4  =     SDL_SCANCODE_LANG4 ,
    LANG5  =     SDL_SCANCODE_LANG5 ,
    LANG6  =     SDL_SCANCODE_LANG6 ,
    LANG7  =     SDL_SCANCODE_LANG7 ,
    LANG8  =     SDL_SCANCODE_LANG8 ,
    LANG9  =     SDL_SCANCODE_LANG9 ,
    ALTERASE  =     SDL_SCANCODE_ALTERASE ,
    SYSREQ  =     SDL_SCANCODE_SYSREQ ,
    CANCEL  =     SDL_SCANCODE_CANCEL ,
    CLEAR  =     SDL_SCANCODE_CLEAR ,
    PRIOR  =     SDL_SCANCODE_PRIOR ,
    RETURN2  =     SDL_SCANCODE_RETURN2 ,
    SEPARATOR  =     SDL_SCANCODE_SEPARATOR ,
    OUT  =     SDL_SCANCODE_OUT ,
    OPER  =     SDL_SCANCODE_OPER ,
    CLEARAGAIN  =     SDL_SCANCODE_CLEARAGAIN ,
    CRSEL  =     SDL_SCANCODE_CRSEL ,
    EXSEL  =     SDL_SCANCODE_EXSEL ,
    KP_00  =     SDL_SCANCODE_KP_00 ,
    KP_000  =     SDL_SCANCODE_KP_000 ,
    THOUSANDSSEPARATOR  =     SDL_SCANCODE_THOUSANDSSEPARATOR ,
    DECIMALSEPARATOR  =     SDL_SCANCODE_DECIMALSEPARATOR ,
    CURRENCYUNIT  =     SDL_SCANCODE_CURRENCYUNIT ,
    CURRENCYSUBUNIT  =     SDL_SCANCODE_CURRENCYSUBUNIT ,
    KP_LEFTPAREN  =     SDL_SCANCODE_KP_LEFTPAREN ,
    KP_RIGHTPAREN  =     SDL_SCANCODE_KP_RIGHTPAREN ,
    KP_LEFTBRACE  =     SDL_SCANCODE_KP_LEFTBRACE ,
    KP_RIGHTBRACE  =     SDL_SCANCODE_KP_RIGHTBRACE ,
    KP_TAB  =     SDL_SCANCODE_KP_TAB ,
    KP_BACKSPACE  =     SDL_SCANCODE_KP_BACKSPACE ,
    KP_A  =     SDL_SCANCODE_KP_A ,
    KP_B  =     SDL_SCANCODE_KP_B ,
    KP_C  =     SDL_SCANCODE_KP_C ,
    KP_D  =     SDL_SCANCODE_KP_D ,
    KP_E  =     SDL_SCANCODE_KP_E ,
    KP_F  =     SDL_SCANCODE_KP_F ,
    KP_XOR  =     SDL_SCANCODE_KP_XOR ,
    KP_POWER  =     SDL_SCANCODE_KP_POWER ,
    KP_PERCENT  =     SDL_SCANCODE_KP_PERCENT ,
    KP_LESS  =     SDL_SCANCODE_KP_LESS ,
    KP_GREATER  =     SDL_SCANCODE_KP_GREATER ,
    KP_AMPERSAND  =     SDL_SCANCODE_KP_AMPERSAND ,
    KP_DBLAMPERSAND  =     SDL_SCANCODE_KP_DBLAMPERSAND ,
    KP_VERTICALBAR  =     SDL_SCANCODE_KP_VERTICALBAR ,
    KP_DBLVERTICALBAR  =     SDL_SCANCODE_KP_DBLVERTICALBAR ,
    KP_COLON  =     SDL_SCANCODE_KP_COLON ,
    KP_HASH  =     SDL_SCANCODE_KP_HASH ,
    KP_SPACE  =     SDL_SCANCODE_KP_SPACE ,
    KP_AT  =     SDL_SCANCODE_KP_AT ,
    KP_EXCLAM  =     SDL_SCANCODE_KP_EXCLAM ,
    KP_MEMSTORE  =     SDL_SCANCODE_KP_MEMSTORE ,
    KP_MEMRECALL  =     SDL_SCANCODE_KP_MEMRECALL ,
    KP_MEMCLEAR  =     SDL_SCANCODE_KP_MEMCLEAR ,
    KP_MEMADD  =     SDL_SCANCODE_KP_MEMADD ,
    KP_MEMSUBTRACT  =     SDL_SCANCODE_KP_MEMSUBTRACT ,
    KP_MEMMULTIPLY  =     SDL_SCANCODE_KP_MEMMULTIPLY ,
    KP_MEMDIVIDE  =     SDL_SCANCODE_KP_MEMDIVIDE ,
    KP_PLUSMINUS  =     SDL_SCANCODE_KP_PLUSMINUS ,
    KP_CLEAR  =     SDL_SCANCODE_KP_CLEAR ,
    KP_CLEARENTRY  =     SDL_SCANCODE_KP_CLEARENTRY ,
    KP_BINARY  =     SDL_SCANCODE_KP_BINARY ,
    KP_OCTAL  =     SDL_SCANCODE_KP_OCTAL ,
    KP_DECIMAL  =     SDL_SCANCODE_KP_DECIMAL ,
    KP_HEXADECIMAL  =     SDL_SCANCODE_KP_HEXADECIMAL ,
    LCTRL  =     SDL_SCANCODE_LCTRL ,
    LSHIFT  =     SDL_SCANCODE_LSHIFT ,
    LALT  =     SDL_SCANCODE_LALT ,
    LGUI  =     SDL_SCANCODE_LGUI ,
    RCTRL  =     SDL_SCANCODE_RCTRL ,
    RSHIFT  =     SDL_SCANCODE_RSHIFT ,
    RALT  =     SDL_SCANCODE_RALT ,
    RGUI  =     SDL_SCANCODE_RGUI ,
  };

  //! Clocker is used to sleep between frames to get given FPS
  class ClockerSDL {
    public:
    ClockerSDL(){}
    ClockerSDL(int fps){ delay = 1000/fps; ntime = SDL_GetTicks(); };
    float Tick(void){
      if(delay > SDL_GetTicks() - ntime) SDL_Delay( delay - SDL_GetTicks() + ntime );
      answer = SDL_GetTicks() - ntime;
      ntime = SDL_GetTicks();
      return 1./answer;
    }
    Uint32 delay, ntime, dtime, answer;
  };

  //! Core is used for following:
  //! - update Mouse & Keyboard items states
  //! - call ClockerSDL to sleep between frames
  class CoreSDL : public Core {
    SDL_Event game_event;
    Mouse * mouse;
    Keyboard * keyboard;
    const Uint8 * kstate;
    bool mouse_locker = false;
    ClockerSDL clocker_sdl;

    public:
    SDL_Window * window;
    SDL_GLContext glcontext;

    CoreSDL(Mouse * mouse, Keyboard * keyboard, int fps) {
      this->mouse = mouse;
      this->keyboard = keyboard;
      clocker_sdl = ClockerSDL(fps);
    }

    void CastMouseClick(const SDL_Event & game_event){
      if(mouse_locker) return;
      if( game_event.type == SDL_MOUSEBUTTONDOWN ){
        if(game_event.button.button == SDL_BUTTON_LEFT  ) {mouse->left_down   = true; mouse->left_hold = true; }
        if(game_event.button.button == SDL_BUTTON_MIDDLE) {mouse->middle_down = true; mouse->middle_hold = true; }
        if(game_event.button.button == SDL_BUTTON_RIGHT ) {mouse->right_down  = true; mouse->right_hold = true; }
      }
      if( game_event.type == SDL_MOUSEBUTTONUP ){
        if(game_event.button.button == SDL_BUTTON_LEFT  ) {mouse->left_up   = true; mouse->left_hold = false; }
        if(game_event.button.button == SDL_BUTTON_MIDDLE) {mouse->middle_up = true; mouse->middle_hold = false; }
        if(game_event.button.button == SDL_BUTTON_RIGHT ) {mouse->right_up  = true; mouse->right_hold = false; }
      }
      mouse_locker = true; // lock mouse for any other mouse events in this tick
    }

    void UpdateMouse(){
      mouse_locker = false;
      while(SDL_PollEvent(&game_event)){
        if(game_event.type == SDL_MOUSEMOTION) mouse->SetXY( game_event.motion.x, game_event.motion.y );
        else if(game_event.type == SDL_MOUSEBUTTONDOWN or game_event.type == SDL_MOUSEBUTTONUP) CastMouseClick( game_event );
        else if(game_event.type == SDL_MOUSEWHEEL) mouse->SetWheel( game_event.wheel.y );
      }
    }

    void UpdateKeyboard(){
      kstate = SDL_GetKeyboardState(NULL);
      for(int key = SDL_KSTART; key < SDL_KEND; key++){
        keyboard->Update(key, kstate[key]);
      }
    }

    void Tick(){
      /// sleep a litle bit between frames
      clocker_sdl.Tick();

      /// check SDL events & update mouse state
      UpdateMouse();

      /// update keyboard state
      UpdateKeyboard();
    }
  };

  // ======= Functions for reading text files ====================================================================
  std::string read_txt_file_sdl(const std::string & path){
    SDL_RWops *file = SDL_RWFromFile(path.c_str(), "r");
    if( file == NULL ) {
      msg_err("read_text_files(): cant open file", path, "return empty string");
      return "";
    }

    Sint64 res_size = SDL_RWsize(file);
    char* res = (char*)malloc(res_size + 1);

    Sint64 nb_read_total = 0, nb_read = 1;
    char* buf = res;
    while (nb_read_total < res_size && nb_read != 0) {
      nb_read = SDL_RWread(file, buf, 1, (res_size - nb_read_total));
      nb_read_total += nb_read;
      buf += nb_read;
    }
    SDL_RWclose(file);
    if (nb_read_total != res_size) {
      free(res);
      msg_err("read_text_files(): error during reading the file", path);
    }

    res[nb_read_total] = '\0';
    std::string str = std::string(res);
    free(res);
    return str;
  }

  int write_txt_file_sdl(const std::string & path, const std::string & data){
    return PM_SUCCESS;
  }

  class IoTxtSDL : public IoTxt {
    public:
    virtual std::string Read(const std::string & path){ return read_txt_file_sdl(path); };
    virtual int Write(const std::string & path, const std::string & data){ return write_txt_file_sdl(path, data); };
  };

  class WindowSDL : public Window {
    public:
    SDL_Window * window;
    SDL_GLContext gl_context;
  };
  
  class SysFactorySDL: public SysFactory {
    public:
    virtual std::shared_ptr<Window> CreateWindow(const SysOptions & opts) {
      std::shared_ptr<WindowSDL> window = std::make_shared<WindowSDL>(); 

      int window_flag = 0;
      if(opts.gl) 
        window_flag |= SDL_WINDOW_OPENGL;

      window->window = SDL_CreateWindow("SDL2", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        opts.screen_width, opts.screen_height, window_flag);
      
      if(opts.gl)
        window->gl_context = SDL_GL_CreateContext(window->window);

      return window;
    }
  };

};

#endif
