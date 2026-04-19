// P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef PMGDLIB_MATH_HH
#define PMGDLIB_MATH_HH 1

#include <math.h>
#include <numeric>
#include <ostream>

namespace pmgd {

  const float PI = 3.14159265358979323846;
  const float OSML = 0.00000000000001;
  const float PI_180 = PI/180.;
  const float IP_081 = 180./PI;

  class v3;
  // ======= v2 =====================================================================================================
  class v2 {
    public:
    float x, y, z;
    v2(float xx = 0., float yy = 0., float zz = 0.):x(xx),y(yy),z(zz){};
    v2 Rotate(float angle);
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
  };

  v2 operator + (v2 va, v2 vb);
  v2 operator - (v2 va, v2 vb);
  v2 operator * (float value, v2 v);
  v2 operator * (v2 v, float value);
  v2 operator * (v2 va, v2 vb);
  v2 operator / (v2 v, float value);
  v2 operator / (v2 va, v2 vb);
  bool operator == (v2 va, v2 vb);
  bool operator != (v2 va, v2 vb);
  std::ostream & operator << (std::ostream & out, v2 v);

  // ======= v3 =====================================================================================================
  class v3 {
    public:
    float x, y, z;
    v3(float xx = 0., float yy = 0., float zz = 0.):x(xx),y(yy),z(zz){};
    v3(v2 v):x(v.x),y(v.y),z(v.z){};
    v3 Rotate(float angle, int axis);
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

    v3 & operator *= (float value){x *= value; y *= value; z *= value; return *this;}
    v3 & operator /= (float value){x /= value; y /= value; z /= value; return *this;}
    v3 & operator += (const v3 & v){x += v.x; y += v.y; z += v.z; return *this;}
    v3 & operator += (const v2 & v){x += v.x; y += v.y; return *this;}
    v3 & operator -= (const v3 & v){x -= v.x; y -= v.y; z -= v.z; return *this;}
    v3 & operator *= (const v3 & v){x *= v.x; y *= v.y; z *= v.z; return *this;}
    v3 & operator /= (const v3 & v){x /= v.x; y /= v.y; z /= v.z; return *this;}
    v3 operator - () const {return v3(-x, -y, -z);}
  };

  v3 operator + (v3 va, v3 vb);
  v3 operator - (v3 va, v3 vb);
  v3 operator * (float value, v3 v);
  v3 operator * (v3 v, float value);
  v3 operator * (v3 va, v3 vb);
  v3 operator / (v3 v, float value);
  v3 operator / (v3 va, v3 vb);
  bool operator == (v3 va, v3 vb);
  bool operator != (v3 va, v3 vb);
  std::ostream & operator << (std::ostream & out, v3 v);
  v3 operator + (const v3 & va, const v2 & vb);
  v2 operator + (const v2 & va, const v3 & vb);

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

  rgb operator + (rgb va, rgb vb);
  rgb operator - (rgb va, rgb vb);
  rgb operator * (float value, rgb v);
  rgb operator * (rgb v, float value);
  rgb operator * (rgb va, rgb vb);
  rgb operator / (rgb v, float value);
  rgb operator / (rgb va, rgb vb);
  std::ostream & operator << (std::ostream & out, rgb v);

  // ======= random =====================================================================================================
  int rint(const int start, const int end);
  std::vector<int> rsample(const unsigned int population_size, const unsigned int sample_size);
};

#endif
