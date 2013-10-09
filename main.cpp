#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "localization_system.h"
using namespace std;

bool stop = false;
void interrupt(int s) {
  stop = true;
}

int main(int argc, char** argv)
{
  signal(SIGINT, interrupt);

  /* process command line */
  if (argc < 6 || string(argv[1]) == "-h")
  {
    cout << "localization-system: [number of circles to detect] [calibration file] [axis definition PNG file] [PNG images directory] [output directory]" << endl;
    return 1;
  }

  int circles = atoi(argv[1]);
  string calibration_file(argv[2]);
  string axis_file(argv[3]);
  string input_directory(argv[4]);
  string output_directory(argv[5]);

  /* create output directory */
  if (mkdir(output_directory.c_str(), 0755) == -1)
    throw std::runtime_error(string("could not create output directory: ") + strerror(errno));

  /* load calibration */
  cv::Mat K, dist_coeff; // TODO: reemplazar
  // TODO: esto se puede reemplaar por una funcion propia que lea el archvo y llene las matrices
  cv::LocalizationSystem::load_matlab_calibration(calibration_file, K, dist_coeff);

  /* initialize system */
  int width = 0; // TODO: leer desde imagen
  int height = 0; 
  cv::LocalizationSystem system(circles, width, height, K, dist_coeff);

  /* create output data file */
  ofstream output_log((output_directory + "/positions.txt").c_str(), ios_base::out | ios_base::trunc);
  if (!output_log) throw std::runtime_error("error creating output data file");

  /* read axis definition file */
  cv::Mat frame = cv::imread(axis_definition_file.c_str()); // TODO: reemplazar
  system.set_axis(frame);
  system.draw_axis(frame);
  cv::imwrite(output_directory + "/axis_detected.png", frame); // TODO: reemplazar (o sacar funcionalidad)

  /* reads input image names */
  list<string> image_names;
  glob_t g;
  glob(input_directory + "/*.png", 0, NULL, &g);
  int image_count = g->gl_pathc;
  if (image_count == 0) throw std::runtime_error("no input images found in directory");
  
  for (char** ptr = gl->gl_pathv, ptr, ptr++) image_names.push_back(*ptr);
  globfree(&g);

  int frame_count = 0;
  bool initialized = false;
  while (!stop && !image_names.empty()) {
    string image_name = image_names.front();
    image_names.pop_front();
    
    // TODO: leer imagen actual aca y ponerla en frame
    
    if (!initialized)
      initialized = system.initialize(frame); // find circles in image
    else {
      bool localized_ok = system.localize(frame, 50);
      
      if (localized_ok) {
        for (int i = 0; i < circles; i++)
        {
          const cv::CircleDetector::Circle& circle = system.get_circle(i);
          cv::Vec3f coord = system.get_pose(circle).pos;
          cv::Vec3f coord_trans = system.get_transformed_pose(circle).pos;
          ostringstream ostr;
          ostr << fixed << setprecision(2) << "[" << coord_trans(0) << "," << coord_trans(1) << "]";
          ostr << i;
          output_file << setprecision(15) << "frame " << frame_count << " circle " << i
            << " transformed: " << coord_trans(0) << " " << coord_trans(1) << " " << coord_trans(2)
            << " original: " << coord(0) << " " << coord(1) << " " << coord(2) << endl;
          // TODO: draw output of localization here? -> save into file in output directory
        }
      }
    }
    frame_count++;
  }

  return 0;
}

