// P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_MATH_HH
#define PMGDLIB_MATH_HH 1

#include <math.h>
#include <numeric>

namespace pmgd {

  const float PI = 3.14159265358979323846;
  const float OSML = 0.00000000000001;
  const float PI_180 = PI/180.;
  const float IP_081 = 180./PI;

  class v3;
  // ======= v2 =====================================================================================================
  class v2 {
    public:
    v2(float xx = 0., float yy = 0., float zz = 0.):x(xx),y(yy),z(zz){};
    v2 Rotate(float angle){
      float c = cos(angle);
      float s = sin(angle);
      return  v2( x * c - y * s, x * s + y * c);
    }
    inline v2 Rotated(float angle){return Rotate(angle * PI_180);}
    inline float L(){return sqrt(x*x + y*y);}
    inline float L2(){return x*x + y*y;}
    inline float Angle(){return (y > 0 ? acos(x / (L()+OSML) ) : PI - acos(x / (L()+OSML) ));}
    inline float Angled(){return IP_081 * (y > 0 ? acos(x / (L()+OSML) ) : 2*PI - acos(x / (L()+OSML) ));}
    inline v2 Set(const v2 & v){x = v.x; y = v.y; return *this;}

    v2 & operator *= (float value){x *= value; y *= value; return *this;}
    v2 & operator /= (float value){x /= value; y /= value; return *this;}
    v2 & operator += (const v2 & v){x += v.x; y += v.y; return *this;}
    v2 & operator += (const v3 & v);
    v2 & operator -= (const v2 & v){x -= v.x; y -= v.y; return *this;}
    v2 & operator *= (const v2 & v){x *= v.x; y *= v.y; return *this;}
    v2 & operator /= (const v2 & v){x /= v.x; y /= v.y; return *this;}
    v2 operator - () const {return v2(-x, -y);}

    float x, y, z;
  };

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

  // ======= v3 =====================================================================================================
  class v3 {
    public:
    v3(float xx = 0., float yy = 0., float zz = 0.):x(xx),y(yy),z(zz){};
    v3(v2 v):x(v.x),y(v.y),z(v.z){};
    inline v3 Rotate(float angle, int axis){
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
    inline v3 Rotated(const float & angle, int axis){return Rotate(angle * PI_180, axis);}
    inline v3 Rotated(const float & angle_x, const float & angle_y, const float & angle_z){
      return Rotate(angle_x * PI_180, 0).Rotate(angle_y * PI_180, 1).Rotate(angle_z * PI_180, 2);
    }

    inline v3 PerspectiveScale(const v3 & start_pos, const float & end_plane_x, const float & end_plane_y){
      return v3(start_pos.x - (z - end_plane_x) / (start_pos.z - end_plane_x) * (start_pos.x - x),
                start_pos.y - (z - end_plane_y) / (start_pos.z - end_plane_y) * (start_pos.y - y),
                z);
    }

    inline float L(){return sqrt(x*x + y*y + z*z);}
    inline float L2(){return x*x + y*y + z*z;}

    float x, y, z;

    v3 & operator *= (float value){x *= value; y *= value; z *= value; return *this;}
    v3 & operator /= (float value){x /= value; y /= value; z /= value; return *this;}
    v3 & operator += (const v3 & v){x += v.x; y += v.y; z += v.z; return *this;}
    v3 & operator += (const v2 & v){x += v.x; y += v.y; return *this;}
    v3 & operator -= (const v3 & v){x -= v.x; y -= v.y; z -= v.z; return *this;}
    v3 & operator *= (const v3 & v){x *= v.x; y *= v.y; z *= v.z; return *this;}
    v3 & operator /= (const v3 & v){x /= v.x; y /= v.y; z /= v.z; return *this;}
    v3 operator - () const {return v3(-x, -y, -z);}
  };

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
  class rgb {
    public:
    rgb(int xx = 0, int yy = 0, int zz = 0, int aa = 255):r(xx/255.f),g(yy/255.f),b(zz/255.f),a(aa/255.f){};
    rgb(float xx = 0.f, float yy = 0.f, float zz = 0.f, float aa = 1.f):r(xx),g(yy),b(zz),a(aa){};
    float r, g, b, a;

    rgb & operator *= (float value){r *= value; g *= value; b *= value; return *this;}
    rgb & operator /= (float value){r /= value; g /= value; b /= value; return *this;}
    rgb & operator += (const rgb & v){r += v.r; g += v.g; b += v.b; return *this;}
    rgb & operator -= (const rgb & v){r -= v.r; g -= v.g; b -= v.b; return *this;}
    rgb & operator *= (const rgb & v){r *= v.r; g *= v.g; b *= v.b; return *this;}
    rgb & operator /= (const rgb & v){r /= v.r; g /= v.g; b /= v.b; return *this;}
    rgb operator - () const {return rgb(-r, -g, -b);}

    rgb Norm_255i(){return rgb(int(r*255), int(g*255), int(b*255), int(a*255));}
    rgb Norm_255f(){return rgb(r*255, g*255, b*255, a*255);}
    rgb Norm_1f()  {return rgb(r/255.f, g/255.f, b/255.f, a/255.f);}

    bool Equal_i(const int & rr, const int & gg, const int & bb) const {
      return (int(r)==rr) and (int(g)==gg) and (int(b)==bb);
    }
  };

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

#endif
