#include <iostream>
#include "many_circle_detector.h"
using namespace std;

cv::ManyCircleDetector::ManyCircleDetector(int _number_of_circles, int _width, int _height, float _diameter_ratio) : 
  context(_width, _height), width(_width), height(_height), number_of_circles(_number_of_circles)
{
  circles.resize(number_of_circles);
  //detectors.resize(number_of_circles, CircleDetector(width, height, &context, _diameter_ratio));
}

cv::ManyCircleDetector::~ManyCircleDetector(void) {
}


bool cv::ManyCircleDetector::detect(const cv::Mat& image, bool reset, int max_attempts, int refine_max_step) {
  cv::Mat image_gray;
  cv::cvtColor(image, image_gray, CV_RGB2GRAY);
  std::vector<cv::Vec3f> hough_circles;

  cv::SimpleBlobDetector::Params parameters;
  parameters.minThreshold = 96;
  parameters.maxThreshold = 192;
  parameters.thresholdStep = 8;
  
  parameters.blobColor = 255;
  parameters.filterByCircularity = true;
  parameters.minDistBetweenBlobs = 1;

  parameters.filterByCircularity = false;
  parameters.minCircularity = 0.7;

  parameters.filterByInertia = false;
  parameters.minInertiaRatio = 0.5;

  parameters.minConvexity = 0.85;
  
  cv::SimpleBlobDetector detector(parameters);
  std::vector<cv::KeyPoint> keypoints;
  detector.detect(image_gray, keypoints);

  cout << "blobs: " << keypoints.size() << endl;
  int valid_count = 0;
  uint i, j;
  for (i = 0, j = 0; i < keypoints.size() && j < circles.size(); i++) {
    float radius = keypoints[i].size;

    if (radius < 8) continue;
    

    CircleDetector::Circle& c = circles[j];
    c.x = keypoints[i].pt.x;
    c.y = keypoints[i].pt.y;
    c.minx = std::max(0.f, c.x - radius);
    c.maxx = std::min((float)image.cols - 1, c.x + radius);
    c.miny = std::max(0.f, c.y - radius);
    c.maxy = std::min((float)image.rows - 1, c.y + radius);
    c.v0 = 1;
    c.v1 = 0;
    c.m0 = c.m1 = radius * 0.5;
    c.roundness = 1;
    c.valid = true;
    valid_count++;
    j++;
  }
  cout << "valid: " << valid_count << endl;
  return (valid_count == circles.size());
}

#if 0
bool cv::ManyCircleDetector::detect(const cv::Mat& image, bool reset, int max_attempts, int refine_max_step) {
  bool all_detected = true;

  cv::Mat input;
  if (reset) image.copyTo(input); // image will be modified on reset
  else input = image;
  
  for (int i = 0; i < number_of_circles && all_detected; i++) {    
    cout << "detecting circle " << i << endl;
    
    for (int j = 0; j < max_attempts; j++) {
      cout << "attempt " << j << endl;

      for (int refine_counter = 0; refine_counter < refine_max_step; refine_counter++)
      {
        if (refine_counter > 0) cout << "refining step " << refine_counter << "/" << refine_max_step << endl;
        int prev_threshold = detectors[i].get_threshold();

        int64_t ticks = cv::getTickCount();
        
        if (refine_counter == 0 && reset)
          circles[i] = detectors[i].detect(input, (i == 0 ? CircleDetector::Circle() : circles[i-1]));
        else
          circles[i] = detectors[i].detect(input, circles[i]);
          
        double delta = (double)(cv::getTickCount() - ticks) / cv::getTickFrequency();
        cout << "t: " << delta << " " << " fps: " << 1/delta << endl;

        if (!circles[i].valid) break;

        // if the threshold changed, keep refining this circle
        if (detectors[i].get_threshold() == prev_threshold) break;
      }

      if (circles[i].valid) {
        cout << "detection of circle " << i << " ok" << endl;
        if (reset) detectors[i].cover_last_detected(input);
        break; // detection was successful, dont keep trying
      }
    }

    // detection was not possible for this circle, abort search
    if (!circles[i].valid) { all_detected = false; break; }
  }
  
  return all_detected;
}
#endif
