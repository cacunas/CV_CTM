#ifndef RELIABILITYSEGMENTATIONMODULE_H
#define RELIABILITYSEGMENTATIONMODULE_H

#include <map>
#include <string>
#include <QImage>
#include "Datapool.h"
#include "ModuleInterface.h"
#include "src/blob.h"
#include <QDir>

class ReliabilitySegmentationModule:public ModuleInterface
{
public:
	ReliabilitySegmentationModule(Datapool *i_data);
	~ReliabilitySegmentationModule();

	//Set module configuration parameters
	bool setParameters(QDomNode& config);

	//Initialization after reading parameters
	bool init();

	//Function executed at each frame
	bool run();

	//update parameters at runtime.
	bool updateParameters();

	void currImageReliability();
	void empiricalReliability();
	void contrastOfBlob();
	uchar weberContrast(cv::Mat roiImg, cv::Mat roiBg, cv::Mat fgMask);
	float pixelColorGrade(uchar L, uchar a, uchar b);
	uchar maskColorGrade(cv::Mat roiImg, cv::Mat roiBg, cv::Mat fgMask);
	void applyReliabilityAura(cv::Mat patchAura, cv::Mat fgMask, uchar reliabilityMesaure);
	cv::Mat reliabilityToThermal(cv::Mat reliability);


	//parameters
	int sigma, beginningFrame, dilatacion, colorUmbral;

private:
	bool firstTime;
	int imgWidth, imgHeight;
	cv::Mat currImgGray, currImgColor, currBgGray, currBgColor, reliabilityMap, empiricalReliabilityMap, foreground;
};

#endif // RELIABILITYSEGMENTATIONMODULE_H
