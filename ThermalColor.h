#ifndef THERMAL_COLOR_H
#define THERMAL_COLOR_H
#include <opencv2/opencv.hpp>
#include <vector>

class ThermalColor
{
public:

  cv::Mat reliabilityToThermal(cv::Mat reliability);
  cv::Mat thermalToReliability(cv::Mat thermal);
  void displayScale(int width, int height);


  cv::Mat scale;
};

#endif // THERMAL_COLOR_H
