#ifndef TEMPORAL_VARIANCE_BG_MODULE_H
#define TEMPORAL_VARIANCE_BG_MODULE_H

#include "Datapool.h"
#include "ModuleInterface.h"
#include "ThermalColor.h"
#include <QDir>

#define TRUNCATE_VALUE 0.0000001 //valor utilizado para evitar problemas de aproximacion

class TemporalVarianceBGModule: public ModuleInterface
{
	public:
		TemporalVarianceBGModule(Datapool *i_data);
		~TemporalVarianceBGModule();

		//Set module configuration parameters
		bool setParameters(QDomNode& config);

		//Initialization after reading parameters
		bool init();

		//Function executed at each frame
		bool run();

		//update parameters at runtime.
		bool updateParameters();

		uchar inline normalizeScale(float value);
		uchar inline normalizeBothSideScale(float value, uchar threshold);
		float confidenceOfForeground(float diference, float stdDeviation);
		void showHistogram( cv::Mat histogram, int numBins);
		void percentilCut(cv::Mat &histogram, float IQRFactor);

		bool firstTime, displayFirstModel, displayFeatureMap;
		int width, height, bgSizeWindow, fgSizeWindow, factorSigmoid;
		float fgVarianceThreshold, detectionFactorThreshold, fgMaskThreshold,
			value, bgVarMean, numBgPixels, detectionThreshold;
		float  *ptr_bgMean, *ptr_bgMean2, *ptr_bgVar, *ptr_fgMean, *ptr_fgMean2, *ptr_fgVar;
		uchar *ptr_dataVarMask, *ptr_fgVarMask, *ptr_fgMask, *ptr_currGrayImg, *ptr_reliability,
			minScale, maxScale;
		cv::Mat fgVarianceMask, fgMask, currGrayImg, imgARGB32, currImgC3, reliability, reliabilityThermal,
			featureMap;
		ThermalColor thermalColor;
};
#endif // TEMPORAL_VARIANCE_BG_MODULE_H
