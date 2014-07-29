#include "PlayFieldDetectionModule.h"
#include <errno.h>
#include <iostream>
#include "image_display.h"
#include <QMessageBox>
#include <QPainter>
#include <QFile>
#include <QTextStream>

PlayFieldDetectionModule::PlayFieldDetectionModule(Datapool *i_data):ModuleInterface(i_data)
{
}

PlayFieldDetectionModule::~PlayFieldDetectionModule()
{
}

bool PlayFieldDetectionModule::setParameters(QDomNode& config)
{
    QDomNode n;
    if(config.isNull())  //Parameter set for module not defined
    {
        this->alpha = 0.1; //vecindad del peak en el histograma usado para general el umbral
        this->beta = 0.75; //factor que pondera la desviacion estandar en la imagen de escala de grises
        this->minScale = 0;
        this->maxScale = 255;
        this->fileMaskName = "fieldMask.jpg";
        this->displayCriteriaMap = false;
        this->displayFeatureMap = false;
        this->displayHistogram = false;
    }
    else
    {
        if( !( n = XmlCommon::XmlCommon::getParameterNode("alpha", config) ).isNull() )
            this->alpha = XmlCommon::getParameterValue(n).toFloat();
        else
        {
            this->alpha = 0.1;
            AppendToLog("PlayFieldDetectionModule: Warning: 'alpha' not defined. Taking defaults: \n\t\talpha = " + QString::number(this->alpha) + "\n");
        }
        if( !( n = XmlCommon::XmlCommon::getParameterNode("beta", config) ).isNull() )
            this->beta = XmlCommon::getParameterValue(n).toFloat();
        else
        {
            this->beta = 0.75;
            AppendToLog("PlayFieldDetectionModule: Warning: 'beta' not defined. Taking defaults: \n\t\tbeta = " + QString::number(this->beta) + "\n");
        }
        if( !( n = XmlCommon::XmlCommon::getParameterNode("minScale", config) ).isNull() )
          this->minScale = XmlCommon::getParameterValue(n).toFloat();
        else
        {
          this->minScale = 0;
          AppendToLog("PlayFieldDetectionModule: Warning: 'minScale' not defined. Taking defaults: \n\t\tminScale = " + QString::number(this->minScale) + "\n");
        }
        if( !( n = XmlCommon::XmlCommon::getParameterNode("maxScale", config) ).isNull() )
          this->maxScale = XmlCommon::getParameterValue(n).toFloat();
        else
        {
          this->maxScale = 255;
          AppendToLog("PlayFieldDetectionModule: Warning: 'maxScale' not defined. Taking defaults: \n\t\tmaxScale = " + QString::number(this->maxScale) + "\n");
        }
        if( !( n = XmlCommon::XmlCommon::getParameterNode("fileMaskName", config) ).isNull() )
            this->fileMaskName = XmlCommon::getParameterValue(n);
        else
        {
            this->fileMaskName = "fieldMask.jpg";
            AppendToLog("PlayFieldDetectionModule: Warning: 'fileMaskName' not defined. Taking defaults: \n\t\tfileMaskName = " + this->fileMaskName + "\n");
        }
        if( !( n = XmlCommon::XmlCommon::getParameterNode("displayCriteriaMap", config) ).isNull() )
            this->displayCriteriaMap  = XmlCommon::getParameterValue(n) == "true" ? true : false;
        else
        {
            this->displayCriteriaMap = false;
            AppendToLog("PlayFieldDetectionModule: Warning: 'displayCriteriaMap' not defined. Taking defaults: \n\t\tdisplayCriteriaMap = false \n");
        }
        if( !( n = XmlCommon::XmlCommon::getParameterNode("displayFeatureMap", config) ).isNull() )
          this->displayFeatureMap  = XmlCommon::getParameterValue(n) == "true" ? true : false;
        else
        {
          this->displayFeatureMap = false;
          AppendToLog("PlayFieldDetectionModule: Warning: 'displayFeatureMap' not defined. Taking defaults: \n\t\tdisplayFeatureMap = false \n");
        }
        if( !( n = XmlCommon::XmlCommon::getParameterNode("displayHistogram", config) ).isNull() )
          this->displayHistogram  = XmlCommon::getParameterValue(n) == "true" ? true : false;
        else
        {
          this->displayHistogram = false;
          AppendToLog("PlayFieldDetectionModule: Warning: 'displayHistogram' not defined. Taking defaults: \n\t\tdisplayHistogram = false \n");
        }
    }

    //Setea la lista de parametros y la lista de tipos de parametros utilizadas en el ParameterDialog.
    addParameter("alpha", QString::number(this->alpha), "float");
    addParameter("beta", QString::number(this->beta), "float");
    addParameter("minScale", QString::number(this->minScale), "uchar");
    addParameter("maxScale", QString::number(this->maxScale), "uchar");
    addParameter("fileMaskName", this->fileMaskName, "QString");
    addParameter("displayCriteriaMap", (this->displayCriteriaMap == true)? "yes" : "no", "bool");
    addParameter("displayFeatureMap", (this->displayFeatureMap == true)? "yes" : "no", "bool");
    addParameter("displayHistogram", (this->displayHistogram == true)? "yes" : "no", "bool");

    return true;
}

bool PlayFieldDetectionModule::init()
{
  this->numBins = 256;
  this->firstTime = true;

  return true;
}

bool PlayFieldDetectionModule::updateParameters()
{
  return true;
}

bool PlayFieldDetectionModule::run()
{
  if(firstTime)
  {
    if(imgARGB32.empty() || imgARGB32.data == NULL)
    {
      this->height = m_data->currentImage->height();
      this->width = m_data->currentImage->width();
      this->imgARGB32 = cv::Mat(height, width, CV_8UC4 );
    }

    if(m_data->fgImage == NULL)
    {
      m_data->fgImage = new QImage(width,height, QImage::Format_Indexed8);
      m_data->fgImage->setColorTable(*(m_data->grayScaleTable));
      memset(m_data->fgImage->bits(), 0, height*m_data->fgImage->bytesPerLine());
    }
    if(m_data->reliabilityMap == NULL || m_data->reliabilityMap->isNull())
    {
      m_data->reliabilityMap = new QImage(width,height, QImage::Format_ARGB32);
    }

    this->reliabilityNormalized = cv::Mat(height,width, CV_8UC1);

    this->nonField = cv::Mat(height, width, CV_8UC1 );
    ptr_nonField = nonField.data;

    QByteArray string8bit = this->fileMaskName.toLocal8Bit();
    this->fieldMask = cv::imread(string8bit.data(), CV_LOAD_IMAGE_GRAYSCALE );

//    if(fieldMask.empty() && !fileMaskName.compare("none"))
    if(fieldMask.empty() )
    {
//      AppendToLog("PlayFieldDetectionModule: Warning: Mask Image "+ this->fileMaskName + " not found.\n");
      this->useMask = false;
    }
    else
    {
      if(fieldMask.rows == height && fieldMask.cols == width)
        this->useMask = true;
      else
      {
        this->useMask = false;
        this->fieldMask = cv::Mat();
        AppendToLog("PlayFieldDetectionModule: Warning: 'Mask Image' doesnt have the needed dimension. Mask Image won't be use.\n");
      }
    }

    criteriaMap = cv::Mat(height, width, CV_8UC3, cv::Scalar(0,255,255));//yellow first criteria

    firstTime = false;
  }

  uchar *ptr_imgData = m_data->currentImage->bits();

  memcpy(imgARGB32.data, ptr_imgData, m_data->currentImage->height()*m_data->currentImage->bytesPerLine());
  cv::cvtColor(imgARGB32, currImgC3, CV_RGBA2BGR);
  cv::cvtColor(currImgC3, currGrayImg, CV_BGR2GRAY );
  ptr_currGrayImg = currGrayImg.data;

  std::vector<cv::Mat> bgr_planes;
  cv::split( currImgC3, bgr_planes );

  float range[] = { 0, 255} ;
  const float* histRange = { range };

  cv::calcHist( &bgr_planes[0], 1, 0, this->fieldMask, blueHist, 1, &numBins, &histRange, true, false );
  cv::calcHist( &bgr_planes[1], 1, 0, this->fieldMask, greenHist, 1, &numBins, &histRange, true, false );
  cv::calcHist( &bgr_planes[2], 1, 0, this->fieldMask, redHist, 1, &numBins, &histRange, true, false );
  cv::calcHist( &currGrayImg, 1, 0, this->fieldMask, grayHist, 1, &numBins, &histRange, true, false );

//  percentilCut(blueHist, 1.5);
//  percentilCut(greenHist, 1.5);
//  percentilCut(redHist, 1.5);
//  percentilCut(grayHist, 1.5);

  if(this->displayHistogram)
  {
    showHistogram(blueHist, "blueHist");
    showHistogram(greenHist, "greenHist");
    showHistogram(redHist, "redHist");
    showHistogram(grayHist, "grayHist");
  }

  this->blueThreshold = calcThresh(blueHist,&this->bluePeak);
  this->greenThreshold = calcThresh(greenHist,&this->greenPeak);
  this->redThreshold = calcThresh(redHist,&this->redPeak);

  this->grayPeak = getMaxLoc( grayHist);

  float stdGray = 0, grayMean;

  grayMean = cv::mean(currGrayImg)[0];
  for( int i = 0; i < currGrayImg.rows*currGrayImg.cols; i++)
  {
    stdGray += (ptr_currGrayImg[i] - grayMean ) * (ptr_currGrayImg[i] - grayMean );
  }
  stdGray /= currGrayImg.rows*currGrayImg.cols;
  stdGray = sqrt(stdGray);

//  this->grayThreshold = this->grayPeak + this->beta * stdGray;
  this->grayThreshold = this->beta * stdGray;

//hand sets values
//  this->bluePeak = 30;
//  this->greenPeak = 80;
//  this->redPeak = 0;
//  this->grayPeak = 74;

//  this->blueThreshold = 95;
//  this->greenThreshold = 200;
//  this->redThreshold = 90;
//  this->grayThreshold = grayPeak +  this->beta * stdGray;

//  std::cout<<"blue: " <<(int)blueThreshold <<  " "<< (int)bluePeak <<std::endl;
//  std::cout<<"green "<< (int)greenThreshold << " "<< (int)greenPeak<<std::endl;
//  std::cout<<"red " <<(int)redThreshold << " "<< (int)redPeak<<std::endl;
//  std::cout<< "gray" <<(int)grayThreshold << " "<< (int)grayPeak<<std::endl;

  //grass clasification: 0 -> grass; 255-> non-grass
  ptr_blueChannel = bgr_planes[0].data,
      ptr_greenChannel = bgr_planes[1].data,
      ptr_redChannel = bgr_planes[2].data;

  ptr_nonField = nonField.data;
  memset(ptr_nonField, 255, nonField.cols * nonField.rows);

  //foreground classification
  featureMap = bgr_planes[2].clone();
  for(int i = 0; i < width*height; i++)
  {
    if(this->useMask && fieldMask.data[i]  == 0)
    {
      ptr_nonField[i] = 0;
      featureMap.data[i] = 0;
      continue;
    }

     minFeature = ptr_redChannel[i];
     minReliability = normalizeBothSideScale(minFeature, ptr_greenChannel[i]);
    if( ptr_redChannel[i] < ptr_greenChannel[i] ) //criteria
    {
      reliability = normalizeBothSideScale(ptr_blueChannel[i], ptr_greenChannel[i]);
      if(minReliability >  reliability)
      {
        minFeature = ptr_blueChannel[i];
        minReliability = reliability;
      }

      //blue -  green = cian
      criteriaMap.data[3*i] = 255;
      criteriaMap.data[3*i + 1] = 255;
      criteriaMap.data[3*i + 2] = 0;
      if(  ptr_blueChannel[i] < ptr_greenChannel[i]) //criteria
      {
        threshold = greenPeak + greenThreshold;
        reliability = normalizeBothSideScale(ptr_greenChannel[i], threshold > 255? 255: threshold);
        if(minReliability >  reliability)
        {
            minFeature = ptr_greenChannel[i];
            minReliability = reliability;
        }

        //green
        criteriaMap.data[3*i] = 0;
        criteriaMap.data[3*i + 1] = 255;
        criteriaMap.data[3*i + 2] = 0;
        if( abs( ptr_greenChannel[i] - greenPeak ) <= greenThreshold) //criteria
        {
          threshold = bluePeak + blueThreshold;
          reliability = normalizeBothSideScale(ptr_blueChannel[i], threshold > 255? 255: threshold);
          if(minReliability >  reliability)
          {
            minFeature = ptr_blueChannel[i];
            minReliability = reliability;
          }

          //blue
          criteriaMap.data[3*i] = 255;
          criteriaMap.data[3*i + 1] = 0;
          criteriaMap.data[3*i + 2] = 0;
          if( abs( ptr_blueChannel[i] - bluePeak ) <= blueThreshold) //criteria
          {

            threshold = redPeak + redThreshold;
            reliability = normalizeBothSideScale(ptr_redChannel[i], threshold > 255? 255: threshold);
            if(minReliability >  reliability)
            {
              minFeature = ptr_redChannel[i];
              minReliability = reliability;
            }

            //red
            criteriaMap.data[3*i] = 0;
            criteriaMap.data[3*i + 1] = 0;
            criteriaMap.data[3*i + 2] = 255;
            if( abs( ptr_redChannel[i] - redPeak ) <= redThreshold) //criteria
            {

              threshold = grayPeak + grayThreshold;
              reliability = normalizeBothSideScale(ptr_currGrayImg[i], threshold > 255? 255: threshold);
              if(minReliability >  reliability)
              {
                minFeature = ptr_currGrayImg[i];
                minReliability = reliability;
              }

              //gray
              criteriaMap.data[3*i] = 127;
              criteriaMap.data[3*i + 1] = 127;
              criteriaMap.data[3*i + 2] = 127;
              if( abs(ptr_currGrayImg[i] - grayPeak) <= grayThreshold) //criteria
              {
                //gray
                criteriaMap.data[3*i] = 255;
                criteriaMap.data[3*i + 1] = 255;
                criteriaMap.data[3*i + 2] = 255;

                ptr_nonField[i] = 0;
              }
            }
          }
        }
      }
    }

    //linear scale
    this->featureMap.data[i] = this->minFeature;
    this->reliabilityNormalized.data[i] = this->minReliability;

  }

  //adaptative threshold, only for experimentation
//  cv::Mat binary;
//  cv::adaptiveThreshold(reliabilityNormalized, binary, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 21, 13);
//  cv::threshold(reliabilityNormalized, binary, 125, 255, cv::THRESH_OTSU);
//  binary = binary == 0;
//  binary = binary & fieldMask;
//  cv::namedWindow("segmentation");
//  cv::imshow("segmentation",binary);

  if(displayFeatureMap)
  {
    cv::Mat featureMapThermal = this->thermalColor.reliabilityToThermal(featureMap);
    cv::cvtColor(featureMapThermal,featureMapThermal, CV_BGR2RGB);
//    cv::namedWindow("featureMap");
//    cv::imshow("featureMap", featureMap); //grayscale
    cv::namedWindow("featureMapThermal");
    cv::imshow("featureMapThermal", featureMapThermal);

  }
  if(displayCriteriaMap)
  {
    cv::namedWindow("Criteria");
    cv::imshow("Criteria",criteriaMap);
  }

  //copy back foreground
  memcpy(m_data->fgImage->bits(), nonField.data, height*m_data->fgImage->bytesPerLine());

  //copy back reliability
//  cv::namedWindow("reliability");
//  cv::imshow("reliability",reliabilityNormalized); //grayScale
  reliabilityNormalizedThermal = thermalColor.reliabilityToThermal(reliabilityNormalized);
  cv::cvtColor(reliabilityNormalizedThermal, imgARGB32, CV_BGR2RGBA);
  memcpy(m_data->reliabilityMap->bits(), imgARGB32.data, height * m_data->reliabilityMap->bytesPerLine());

  return true;
}

//iguala la distancia de minScale a threshold y de threshold a maxScale, de tal
//forma que valores cercanos al umbral esten cercanos a 0, y valores cercanos
//a minScale o maxScale esten cercanos a 255.
uchar inline PlayFieldDetectionModule::normalizeBothSideScale(uchar value, uchar threshold)
{
  uchar leftScale, rightScale;
  float thresholded;

  leftScale = threshold - minScale;
  rightScale = maxScale - threshold;
  thresholded = value - threshold;

  if( thresholded < 0) //|---featureVal-----Threshold ----|
  {
    if(value <= minScale)
      return 0;
    else
      return -((value - threshold ) *255 / ( leftScale ));
  }

  else
  {
    if(value >= maxScale)
      return 255;
    else
    {
      if(thresholded > 0) //|-----Threshold ----featureVal----|
        return  ((value - threshold )*255.0 / ( rightScale ));

      else //|-----Threshold = featureVal ------|
        return  leftScale > rightScale? value/leftScale : value/rightScale;
    }
  }
}

//Remove outlier using the interquartile range (IQR) TODO: move this function to histogramTool class
void PlayFieldDetectionModule::removeOutliersIQR(cv::Mat histogram, float IQRFactor)
{
  float cumulative = 0, total, IQR;
  int numBins = histogram.cols > histogram.rows? histogram.cols : histogram.rows;
  int median = -1, quartil1 = -1, quartil3 = -1, lowerBound, upperBound;

  total = cv::sum( histogram )[0];
  histogram = histogram / total;
  for(int i = 0; i < numBins; i++ )
  {
    cumulative += histogram.at<float>(i);
    if( cumulative > 0.25 &&  quartil1 == -1)
    {
      quartil1 = i;
      continue;
    }
    if( cumulative > 0.5 && median == -1)
    {
      median = i;
      continue;
    }
    if( cumulative > 0.75 &&  quartil3 == -1)
    {
      quartil3 = i;
      break;
    }
  }

  IQR = quartil3 - quartil1;

  lowerBound = quartil1 - IQRFactor * IQR;
  lowerBound  = lowerBound >= 0? lowerBound : 0;

  upperBound = quartil3 + IQRFactor * IQR;
  upperBound  = upperBound <= 255? upperBound : 255;

  for(int i = 0; i < lowerBound; i++) //cut left tail
  {
    histogram.at<float>(i) = 0;
  }

  for(int i = upperBound + 1; i < numBins; i++) //cut right tail
  {
    histogram.at<float>(i) = 0;
  }
}

///* //neighborhood based in frequency, usefull to define the stdDev
uchar PlayFieldDetectionModule::calcThresh(cv::Mat histogram, uchar* ptr_peak)
{
  float acumSum = 0, numData = 0, mean, stdDev = 0;
  double peakFreq;

  cv::minMaxLoc(histogram, 0, &peakFreq, 0, 0);
//  (* ptr_peak) = getMaxLoc(histogram);

  for(int i = 0; i < this->numBins; i++)
  {
    if( histogram.at<float>(i) >= this->alpha * peakFreq )
    {
      acumSum += i * histogram.at<float>(i);
      numData += histogram.at<float>(i);
    }
  }
  mean = acumSum / numData;
  (* ptr_peak) = mean;

  for(int i = 0; i < this->numBins; i++)
  {
    if(histogram.at<float>(i) >= this->alpha * peakFreq )
    {
     stdDev += (histogram.at<float>(i) - mean) * (histogram.at<float>(i) - mean);
    }
  }

  stdDev /= numData;
  stdDev = sqrt(stdDev);

  return stdDev;
}
//*/

/* //neighborhood based in frequency and locality, usefull to define the peak
uchar PlayFieldDetectionModule::calcThresh(cv::Mat histogram, uchar* ptr_peak)
{
    int leftBound, rightBound;
    float acumSum = 0, numData = 0, mean, stdDev = 0;
    double peakFreq, minFreq;
    cv::Point minLoc, peakLoc;

  //  cv::minMaxLoc(histogram, &minFreq, &peakFreq, &minPos, &peakLoc.y);
    cv::minMaxLoc(histogram, &minFreq, &peakFreq, &minLoc, &peakLoc);

    leftBound = peakLoc.y;
    for (int i = peakLoc.y; i >= 0; i--)
    {
        if(histogram.at<float>(i) < this->alpha * peakFreq || i == 0)
        {
            leftBound = i;
            break;
        }
    }

    rightBound = peakLoc.y;
    for (int i = peakLoc.y; i < this->numBins; i++)
    {
        if(histogram.at<float>(i) < this->alpha * peakFreq || i == 255)
        {
            rightBound = i;
            break;
        }
    }

    if(leftBound == rightBound)
        return 0;

    for(int i = leftBound; i <= rightBound; i++)
    {
      acumSum += i * histogram.at<float>(i);
      numData += histogram.at<float>(i);
    }
    mean = acumSum / numData;
    (*ptr_peak) = mean;

    for(int i = leftBound; i <= rightBound; i++)
    {
      stdDev += histogram.at<float>(i) *( i - mean)*(i - mean);
    }

    stdDev /= numData;
    stdDev = sqrt(stdDev);

    return stdDev;
}
*/

int PlayFieldDetectionModule::getMaxLoc(cv::Mat histogram)
{
  float currMax = 0;
  int maxIdx = -1;

  for (int i = 0; i < this->numBins; i++ )
  {
    if(histogram.at<float>(i) > currMax)
    {
      currMax = histogram.at<float>(i);
      maxIdx = i;
    }
  }
  return maxIdx;
}

void PlayFieldDetectionModule::showHistogram( cv::Mat histogram, char windowName[])
{
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound( (double) hist_w/numBins );

    cv::Mat histNorm, histImage( hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0) );

    if(histNorm.data == NULL)
      cv::normalize(histogram, histNorm, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );


    for( int i = 1; i < histogram.rows; i++ )
    {
        cv::line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(histNorm.at<float>(i-1)) ) ,
                         cv::Point( bin_w*(i), hist_h - cvRound(histNorm.at<float>(i)) ),
                         cv::Scalar( 0, 255, 0), 2, 8, 0  );
    }

    cv::namedWindow(windowName, CV_WINDOW_AUTOSIZE );
    cv::imshow(windowName, histImage );
}
