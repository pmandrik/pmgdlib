// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_TIME_HH
#define PMGDLIB_TIME_HH 1

namespace pmgd {

  //! Ticker class around `itime` counter going [0, MAX_TIME]
  class Ticker {
    public:
      Ticker(){};
      Ticker(unsigned int max_time) : mtime(max_time) { dtime = 1./float(mtime); Reset(); }

      //! set `this->itime` to `time`
      inline void Set(unsigned int time){itime = std::min(time, mtime); ftime = itime * dtime;}
      inline void Reset(){ Set(0); }

      //! increment `itime` by `val` MAX_TIME; return `true` if itime == MAX_TIME
      bool Tick(unsigned int val=1){
        itime = std::min(itime + val, mtime);
        ftime = itime * dtime;
        return itime == mtime;
      }

      unsigned int mtime, itime;
      float dtime, ftime;
  };
};

#endif
