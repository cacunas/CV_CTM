#ifndef RELIABILITY_HISTOGRAM_H
#define RELIABILITY_HISTOGRAM_H

#include "Datapool.h"
#include "ModuleInterface.h"
#include <QDir>
#include "ThermalColor.h"
#include "image_display.h"

class ReliabilityHistogram: public ModuleInterface
{
	public:
		ReliabilityHistogram(Datapool *i_data);
		~ReliabilityHistogram();

		//Set module configuration parameters
		bool setParameters(QDomNode& config);

		//Initialization after reading parameters
		bool init();

		//Function executed at each frame
		bool run();

		//update parameters at runtime.
		bool updateParameters();

		void displayHistogram( cv::Mat histogram);

		bool firstTime, showHistogram;
		int width, height, numBins, showDataAtFrame;
		cv::Mat  currGrayImg, thermalARGB32, currThermal, reliabHisto, histNorm, reliability, fieldMask;
		std::vector<cv::Mat> channels;
		ThermalColor thermal;
		QString fileMaskName;
};

#endif // RELIABILITY_HISTOGRAM_H
