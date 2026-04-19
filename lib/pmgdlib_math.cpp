// P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib

#include <vector>
#include <pmgdlib_math.h>

namespace pmgd {

  class v3;
  // ======= v2 =====================================================================================================
  v2 v2::Rotate(float angle){
    float c = cos(angle);
    float s = sin(angle);
    return  v2( x * c - y * s, x * s + y * c);
  }
  v2 operator + (v2 va, v2 vb){return v2(va.x + vb.x, va.y + vb.y);}
  v2 operator - (v2 va, v2 vb){return v2(va.x - vb.x, va.y - vb.y);}
  v2 operator * (float value, v2 v){return v2(v.x*value, v.y*value);}
  v2 operator * (v2 v, float value){return v2(v.x*value, v.y*value);}
  v2 operator * (v2 va, v2 vb){return v2(va.x * vb.x, va.y * vb.y);}
  v2 operator / (v2 v, float value){return v2(v.x/value, v.y/value);}
  v2 operator / (v2 va, v2 vb){return v2(va.x / vb.x, va.y / vb.y);}
  bool operator == (v2 va, v2 vb){return ((va.x == vb.x) and (va.y == vb.y));}
  bool operator != (v2 va, v2 vb){return ((va.x != vb.x) or (va.y != vb.y));}

  std::ostream & operator << (std::ostream & out, v2 v){return out << "v2(" << v.x << "," << v.y << ")";}
  // ======= v3 ====================================================================
  v3 v3::Rotate(float angle, int axis){
      if(axis==0){
        v2 yz = v2(y, z).Rotate(angle);
        return v3(x, yz.x, yz.y);
      }
      if(axis==1){
        v2 xz = v2(x, z).Rotate(angle);
        return v3(xz.x, y, xz.y);
      }
      if(axis==2){
        v2 xy = v2(x, y).Rotate(angle);
        return v3(xy.x, xy.y, z);
      }
      return *this;
    }

  v3 operator + (v3 va, v3 vb){return v3(va.x + vb.x, va.y + vb.y, va.z + vb.z);}
  v3 operator - (v3 va, v3 vb){return v3(va.x - vb.x, va.y - vb.y, va.z - vb.z);}
  v3 operator * (float value, v3 v){return v3(v.x*value, v.y*value, v.z*value);}
  v3 operator * (v3 v, float value){return v3(v.x*value, v.y*value, v.z*value);}
  v3 operator * (v3 va, v3 vb){return v3(va.x * vb.x, va.y * vb.y, va.z * vb.z);}
  v3 operator / (v3 v, float value){return v3(v.x/value, v.y/value, v.z/value);}
  v3 operator / (v3 va, v3 vb){return v3(va.x / vb.x, va.y / vb.y, va.z / vb.z);}

  bool operator == (v3 va, v3 vb){return ((va.x == vb.x) and (va.y == vb.y) and (va.z == vb.z));}
  bool operator != (v3 va, v3 vb){return ((va.x != vb.x) or (va.y != vb.y) or (va.z != vb.z));}

  std::ostream & operator << (std::ostream & out, v3 v){return out << "v3(" << v.x << "," << v.y << "," << v.z << ")";};

  v3 operator + (const v3 & va, const v2 & vb){return v3(va.x + vb.x, va.y + vb.y);}
  v2 operator + (const v2 & va, const v3 & vb){return v2(va.x + vb.x, va.y + vb.y);}
  v2 & v2::operator += (const v3 & v){x += v.x; y += v.y; return *this;}

  // ======= rgb ====================================================================
  rgb operator + (rgb va, rgb vb){return rgb(va.r + vb.r, va.g + vb.g, va.b + vb.b);}
  rgb operator - (rgb va, rgb vb){return rgb(va.r - vb.r, va.g - vb.g, va.b + vb.b);}
  rgb operator * (float value, rgb v){return rgb(v.r*value, v.g*value, v.b*value);}
  rgb operator * (rgb v, float value){return rgb(v.r*value, v.g*value, v.b*value);}
  rgb operator * (rgb va, rgb vb){return rgb(va.r * vb.r, va.g * vb.g, va.b * vb.b);}
  rgb operator / (rgb v, float value){return rgb(v.r/value, v.g/value, v.b/value);}
  rgb operator / (rgb va, rgb vb){return rgb(va.r / vb.r, va.g / vb.g, va.b / vb.b);}

  std::ostream & operator << (std::ostream & out, rgb v){return out << " rgb( " << v.r << ", " << v.g << ", " << v.b << ", " << v.a << " ) ";};

  // ======= random =====================================================================================================
  int rint(const int start, const int end){
    if(end < start) return 0;
    return rand() % (end + 1 - start) + start;
  }

  std::vector<int> rsample(const unsigned int population_size, const unsigned int sample_size){
    std::vector<int> pool(population_size);
    std::iota(pool.begin(), pool.end(), 0);
    for(unsigned int i = 0; i < sample_size; ++i){
      int r = rint(i, population_size-1);
      std::swap(pool[i], pool[r]);
    }
    pool.resize(sample_size);
    return pool;
  }
};
