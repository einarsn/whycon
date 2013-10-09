#ifndef __WHYCON_MAT_H__
#define __WHYCON_MAT_H__

#include <vector>

namespace cv {
  template<class T>
  class Mat {
    public:
      Mat(int width, int height);

    private:
      vector<T> data;
  };
}

#endif
