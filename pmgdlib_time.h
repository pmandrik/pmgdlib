// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_TIME_HH
#define PMGDLIB_TIME_HH 1

namespace pmgd {

  class Ticker {
    public:
    Ticker(){};
    Ticker(unsigned int max_time) : mtime(max_time) { dtime = 1./float(mtime); Reset(); }
    inline void Set(unsigned int time){itime = time; ftime = itime * dtime;}
    inline void Reset(){ Set(0); }

    bool Tick(unsigned int val=1){
      itime = std::min(itime + val, mtime);
      ftime = itime * dtime;
      return itime != mtime;
    }

    unsigned int mtime, itime;
    float dtime, ftime;
  };
};

#endif
