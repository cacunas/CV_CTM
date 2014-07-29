#include "ThermalColor.h"

//thermal -> cv_8UC3
cv::Mat ThermalColor::thermalToReliability(cv::Mat thermal)
{
  cv::Mat hsv, reliability;
  std::vector<cv::Mat> channels;

  cv::cvtColor(thermal, hsv, CV_BGR2HSV);
  cv::split( hsv, channels );
  cv::normalize(channels[0], reliability, 0, 255, cv::NORM_MINMAX, -1, cv::Mat() );

	return reliability;
}

cv::Mat ThermalColor::reliabilityToThermal(cv::Mat reliability)
{
	cv::Mat hue(reliability.rows, reliability.cols, CV_8UC1),
			S(reliability.rows, reliability.cols, CV_8UC1, cv::Scalar(255)),
			V(reliability.rows, reliability.cols, CV_8UC1, cv::Scalar(127)),
			lookUpTable(1,256,CV_8UC1),
			channels[] = {hue,S,V},
			thermalImageHsv (reliability.rows, reliability.cols, CV_8UC3),
			thermalImage;

	int from_to[] = { 0,0, 1,1, 2,2 };
	uchar *ptr_lookTableData = lookUpTable.data;

	for(int i = 0; i < 256; i++)
	{
		ptr_lookTableData[i] = i;
	}

	lookUpTable = 120*(lookUpTable/255.0); //transform to thermal space color (use 120/180 color range)
	cv::LUT(reliability, lookUpTable, hue);

	cv::mixChannels( channels, 3, &thermalImageHsv, 1, from_to, 3 );
	cv::cvtColor(thermalImageHsv, thermalImage, CV_HSV2BGR );

	return thermalImage;
}

void ThermalColor::displayScale(int width, int height)
{
	if(scale.empty())
	{
		scale = cv::Mat(256,1,CV_8UC1);
		for(int i = 0; i < 255; i++)
			scale.data[i] = i;

		cv::resize(scale,scale, cv::Size(width, height));
		scale = reliabilityToThermal(scale);
	}

	cv::namedWindow("Escala");
	cv::imshow("Escala",scale);
}
