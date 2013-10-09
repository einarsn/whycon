#ifndef __WHYCON_VECTOR_H__
#define __WHYCON_VECTOR_H__

namespace cv {
  template<class T>
  class Vector {
    public:
      Vector(T x, T y, T z, T w);

      T x, y, z, w;
  };
}

#endif
