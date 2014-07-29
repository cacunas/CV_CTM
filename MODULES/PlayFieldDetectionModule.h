#ifndef PLAY_FIELD_DETECTION_MODULE_H
#define PLAY_FIELD_DETECTION_MODULE_H

#include "Datapool.h"
#include "ModuleInterface.h"
#include <QDir>
#include "ThermalColor.h"

//#include <cmath>

class PlayFieldDetectionModule: public ModuleInterface
{
  public:
    PlayFieldDetectionModule(Datapool *i_data);
    ~PlayFieldDetectionModule();

    //Set module configuration parameters
    bool setParameters(QDomNode& config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool run();

    //update parameters at runtime.
    bool updateParameters();

    uchar calcThresh(cv::Mat histogram, uchar* ptr_peak);
    int getMaxLoc(cv::Mat histogram);
    void showHistogram( cv::Mat histogram, char windowName[]);
    void removeOutliersIQR(cv::Mat histogram, float IQRFactor);
    uchar inline normalizeBothSideScale(uchar value, uchar threshold);

    int height, width, histSize, numBins;
    float alpha, beta, threshold;
    bool firstTime, useMask, displayHistogram, displayFeatureMap, displayCriteriaMap;
    QString fileMaskName;
    ThermalColor thermalColor;
    uchar  redThreshold, redPeak, blueThreshold, bluePeak, greenThreshold, greenPeak, grayThreshold, grayPeak, minReliability,
      minFeature, reliability, minScale, maxScale;
    uchar *ptr_currGrayImg, *ptr_nonField, *ptr_blueChannel, *ptr_greenChannel, *ptr_redChannel;
    cv::Mat imgARGB32, currImgC3, currGrayImg, redHist, blueHist, greenHist, grayHist, fieldMask, structElement, nonField,
      reliabilityNormalized, featureMap, criteriaMap, reliabilityNormalizedThermal;
    /*
      implementacion del modelo de extraccion de zona de juego del paper "Automatic soccer
      players tracking in goal scenes by camera motion elimination"
    */
};
#endif // PLAY_FIELD_DETECTION_MODULE_H
