#ifndef HRSEGMENTMODULE_H
#define HRSEGMENTMODULE_H

#include <QImage>
#include <QDir>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <cmath>
#include <QColor>
#include <QMessageBox>

#include "image_display.h"

#include "Datapool.h"
#include "ModuleInterface.h"

#include "asmOpenCV.h"

typedef std::vector<int> hist;

using namespace std;

class HRSegmentModule : public ModuleInterface
{
public:
    HRSegmentModule(Datapool* i_data);
    ~HRSegmentModule();

    //Set module configuration parameters
    bool setParameters(QDomNode& config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool run();

    //update parameters at runtime.
    bool updateParameters();

private:
    bool firstTime;
    float alpha, beta;
    float A_p[4];// histogram intensity peaks (R,G,B,Gray)
    float A_t[4];// intensity thresholds (R,G,B,Gray)

    // Convert ARGB QImage to Index8 QImage
    //QImage ARGB2RGB(const QImage argb);

    //Calculates Intensity Histograms per channel
    vector<hist> calculateHistograms(const QImage img);

    //Calculates first and second statistical moments of the intesity histograms
    vector<float> calculateMoments(vector<hist> channels);

    //Calculate channels peaks using paper proposed criterias
    void calculatePeaks(vector<hist> channels);

    //Calculate thresholds
    void calculateThresholds(vector<hist> ch);

    //Store grass area of the image on bgImage using a binary classifier
    QImage GrassClassifier(const QImage currIm);

    //Get the line on the grass selection usign Hough Transform
    QImage Line_detect(const QImage currIm);

    //Aplly a filter
    QImage ApplyFilter(const QImage f_in);

    //Discard Low Intensity and Low Gradient pixels
    QImage DLowIntGrad(const QImage src);

    // ROI dirty hack
    void HackForeGround();

    //Detect Players
    QImage ForeGround(const QImage curr, const QImage bg);

};

#endif // HRSEGMENTMODULE_H
