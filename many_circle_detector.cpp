#include <iostream>
#include "many_circle_detector.h"
using namespace std;

cv::ManyCircleDetector::ManyCircleDetector(int _number_of_circles, int _width, int _height, float _diameter_ratio) : 
  context(_width, _height), width(_width), height(_height), number_of_circles(_number_of_circles)
{
  circles.resize(number_of_circles);
  detectors.resize(number_of_circles, CircleDetector(width, height, &context, _diameter_ratio));
}

cv::ManyCircleDetector::~ManyCircleDetector(void) {
}

bool cv::ManyCircleDetector::initialize(const cv::Mat& image) {
  cv::Mat marked_image;
  image.copyTo(marked_image);

  int attempts = 100;
  for (int i = 0; i < number_of_circles; i++) {
    detectors[i].draw = true;
    for (int j = 0; j < attempts; j++) {
      int64_t ticks = cv::getTickCount();
      cout << "detecting circle " << i << " attempt " << j << endl;
      circles[i] = detectors[i].detect(marked_image, (i != 0 ? circles[i - 1] : CircleDetector::Circle()));
      double delta = (double)(cv::getTickCount() - ticks) / cv::getTickFrequency();
      cout << "t: " << delta << " " << " fps: " << 1/delta << endl;
      if (circles[i].valid) break;
    }
    detectors[i].draw = false;
    
    if (!circles[i].valid) return false;    
  }
  return true;
}

bool cv::ManyCircleDetector::detect(const cv::Mat& image) {
  bool all_detected = true;
  cv::Mat marked_image;
  image.copyTo(marked_image);

  for (int i = 0; i < number_of_circles; i++) {
    int64_t ticks = cv::getTickCount();
    cout << "detecting circle " << i << endl;
    circles[i] = detectors[i].detect(marked_image, circles[i]); // TODO: modify current
    if (!circles[i].valid) { all_detected = false; break; }
    double delta = (double)(cv::getTickCount() - ticks) / cv::getTickFrequency();
    cout << "t: " << delta << " " << " fps: " << 1/delta << endl;
  }
  
  return all_detected;
}

