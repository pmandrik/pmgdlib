// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_CORE_HH
#define PMGDLIB_CORE_HH 1

#include "pmgdlib_defs.h"

namespace pmgd {

  // mouse base class
  class Mouse {
    public:
      void Tick(){
        left_up   = false;
        right_up  = false;
        middle_up = false;
        left_down   = false;
        right_down  = false;
        middle_down = false;
        wheel = 0;
      }

      int x, y, wheel;
      bool left_hold, right_hold, middle_hold;
      bool left_down, right_down, middle_down;
      bool left_up,   right_up,   middle_up;

      void SetWheel(float w){
        this->wheel = w;
      }

      void SetXY(float x, float y){
        this->x = x;
        this->y = y;
      }
  };

  // keyboard base class
  class Keyboard {
    public:
      Keyboard(){
        hor = 0; ver = 0;
        actz = false; actx = false; exit = false; screenshoot = false; hor_pressed = false; ver_pressed = false;
        for(int key = 0; key < 232; key++){
          keyboard_pressed[key] = 0;
          keyboard_holded[key]  = 0;
          keyboard_raised[key]  = 0;
        }
      }

      void Update(int key, bool pressed){
        /// Unsafe
        if(pressed){
          if(not keyboard_holded[key]){
            keyboard_pressed[key] = true;
            keyboard_holded[key] = true;
          }
          else keyboard_pressed[key] = false;
        } else {
          if(keyboard_holded[key]){
            keyboard_holded[key] = false;
            keyboard_raised[key] = true;
            keyboard_pressed[key] = false;
          }
          else keyboard_raised[key] = false;
        }
      }

      void Tick(){
        //! TODO move to logical keyboard from physical
        //! update keyboard in the implementation
        //! call this to update some regular keyboard events
        hor = check_sign(Holded(key::D) + Holded(key::RIGHT) - Holded(key::A) - Holded(key::LEFT));
        ver = check_sign(Holded(key::W) + Holded(key::UP) - Holded(key::S) - Holded(key::DOWN));
        actz = Pressed(key::Z) + Pressed(key::K) + Pressed(key::RETURN) + Pressed(key::SPACE);
        actx = Pressed(key::X) + Pressed(key::L);
        hor_pressed = check_sign(Pressed(key::D) + Pressed(key::RIGHT) - Pressed(key::A) - Pressed(key::LEFT));
        ver_pressed = check_sign(Pressed(key::W) + Pressed(key::UP) - Pressed(key::S) - Pressed(key::DOWN));
        exit = Pressed(key::ESCAPE);
        screenshoot = Holded(key::KP_0);
      }

      inline bool Pressed(int key){ return keyboard_pressed[key]; }
      inline bool Holded(int key) { return keyboard_holded[key];  }
      inline bool Raised(int key) { return keyboard_raised[key];  }

      int hor, ver;
      bool actz, actx, exit, screenshoot, hor_pressed, ver_pressed;
      bool keyboard_pressed[232], keyboard_holded[232], keyboard_raised[232];
  };

  class KeyboardLogical {
  };

};

#endif
