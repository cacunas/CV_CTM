#include "TemporalVarianceBGModule.h"
#include <errno.h>
#include <iostream>
#include "image_display.h"
#include <QMessageBox>
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <cmath>
#include <cstdlib>
#include <cstring>

#define PI 3.14159265
#define BACKGROUND 0
#define FOREGROUND 255

TemporalVarianceBGModule::TemporalVarianceBGModule(Datapool *i_data):ModuleInterface(i_data)
{
}

TemporalVarianceBGModule::~TemporalVarianceBGModule()
{
	if(this->ptr_fgMean != NULL)
		free(ptr_fgMean);
	if(this->ptr_fgMean2 != NULL)
		free(ptr_fgMean2);
	if(this->ptr_fgVar != NULL)
		free(ptr_fgVar);
	if(this->ptr_bgMean != NULL)
		free(ptr_bgMean);
	if(this->ptr_bgMean2 != NULL)
		free(ptr_bgMean2);
	if(this->ptr_bgVar != NULL)
		free(ptr_bgVar);
	if(this->ptr_fgVarMask != NULL)
			free(ptr_fgVarMask);
	if(this->ptr_fgMask != NULL)
		free(ptr_fgMask);

	this->ptr_fgMean = NULL;
	this->ptr_fgMean2 = NULL;
	this->ptr_fgVar = NULL;
	this->ptr_bgMean = NULL;
	this->ptr_bgMean2 = NULL;
	this->ptr_bgVar = NULL;
	this->ptr_fgVarMask= NULL;
	this->ptr_fgMask = NULL;
}

bool TemporalVarianceBGModule::setParameters(QDomNode& config)
{
	QDomNode n;
	if(config.isNull())  //Parameter set for module not defined
	{
		this->bgSizeWindow = 11;
		this->fgSizeWindow = 5; //influye en la permanencia de los fantasmas.
		this->detectionFactorThreshold = 7.0;//valores altos, clasifican a mas pixeles como fondo
		this->fgVarianceThreshold = 187.0;//un pixel con varianza mayor a este umbral es considerado pixel en movimiento
		this->factorSigmoid = 3;//entre mas grande es este valor, mas pixeles son clasificados como fondo

		this->minScale = 0;
		this->maxScale = 80;
		this->displayFirstModel = true;
		this->displayFeatureMap = true;
	}
	else
	{
		if( !( n = XmlCommon::XmlCommon::getParameterNode("bgSizeWindow", config) ).isNull() )
				this->bgSizeWindow = XmlCommon::getParameterValue(n).toInt();
		else
		{
			this->bgSizeWindow = 9;
						AppendToLog("TemporalVarianceBGModule: Warning: 'bgSizeWindow' not defined. Taking defaults: \n\t\tbgSizeWindow = " + QString::number(this->bgSizeWindow) + "\n");
		}
		if( !( n = XmlCommon::XmlCommon::getParameterNode("fgSizeWindow", config) ).isNull() )
			this->fgSizeWindow = XmlCommon::getParameterValue(n).toInt();
		else
		{
			this->fgSizeWindow = 4;
						AppendToLog("TemporalVarianceBGModule: Warning: 'fgSizeWindow' not defined. Taking defaults: \n\t\tfgSizeWindow = " + QString::number(this->fgSizeWindow) + "\n");
		}
		if( !( n = XmlCommon::XmlCommon::getParameterNode("detectionFactorThreshold", config) ).isNull() )
			this->detectionFactorThreshold = XmlCommon::getParameterValue(n).toFloat();
		else
		{
			this->detectionFactorThreshold = 7;
						AppendToLog("TemporalVarianceBGModule: Warning: 'detectionFactorThreshold' not defined. Taking defaults: \n\t\tdetectionFactorThreshold = " + QString::number(this->detectionFactorThreshold) + "\n");
		}
		if( !( n = XmlCommon::XmlCommon::getParameterNode("fgVarianceThreshold", config) ).isNull() )
			this->fgVarianceThreshold = XmlCommon::getParameterValue(n).toFloat();
		else
		{
			this->fgVarianceThreshold = 187;
						AppendToLog("TemporalVarianceBGModule: Warning: 'fgVarianceThreshold' not defined. Taking defaults: \n\t\tfgVarianceThreshold = " + QString::number(this->fgVarianceThreshold) + "\n");
		}
		if( !( n = XmlCommon::XmlCommon::getParameterNode("factorSigmoid", config) ).isNull() )
			this->factorSigmoid = XmlCommon::getParameterValue(n).toInt();
		else
		{
			this->factorSigmoid = 3;
						AppendToLog("TemporalVarianceBGModule: Warning: 'factorSigmoid' not defined. Taking defaults: \n\t\tfactorSigmoid = " + QString::number(this->factorSigmoid) + "\n");
		}
		if( !( n = XmlCommon::XmlCommon::getParameterNode("minScale", config) ).isNull() )
			this->minScale = XmlCommon::getParameterValue(n).toInt();
		else
		{
			this->minScale = 0;
			AppendToLog("TemporalVarianceBGModule: Warning: 'minScale' not defined. Taking defaults: \n\t\tminScale = " + QString::number(this->minScale) + "\n");
		}
		if( !( n = XmlCommon::XmlCommon::getParameterNode("maxScale", config) ).isNull() )
			this->maxScale = XmlCommon::getParameterValue(n).toInt();
		else
		{
			this->maxScale = 255;
			AppendToLog("TemporalVarianceBGModule: Warning: 'maxScale' not defined. Taking defaults: \n\t\tmaxScale = " + QString::number(this->maxScale) + "\n");
		}
		if( !( n = XmlCommon::XmlCommon::getParameterNode("displayFeatureMap", config) ).isNull() )
			this->displayFeatureMap  = XmlCommon::getParameterValue(n) == "true" ? true : false;
		else
		{
			this->displayFeatureMap = false;
			AppendToLog("TemporalVarianceBGModule: Warning: 'displayFeatureMap' not defined. Taking defaults: \n\t\tdisplayFeatureMap = false \n");
		}
		if( !( n = XmlCommon::XmlCommon::getParameterNode("displayFirstModel", config) ).isNull() )
			this->displayFirstModel  = XmlCommon::getParameterValue(n) == "true" ? true : false;
		else
		{
			this->displayFirstModel = false;
			AppendToLog("TemporalVarianceBGModule: Warning: 'displayFirstModel' not defined. Taking defaults: \n\t\tdisplayFirstModel = false \n");
		}
	}

	//Setea la lista de parametros y la lista de tipos de parametros utilizadas en el ParameterDialog.
	addParameter("bgSizeWindow", QString::number(this->bgSizeWindow), "int");
	addParameter("fgSizeWindow", QString::number(this->fgSizeWindow), "int");
	addParameter("detectionFactorThreshold", QString::number(this->detectionFactorThreshold), "float");
	addParameter("fgVarianceThreshold", QString::number(this->fgVarianceThreshold), "float");
	addParameter("factorSigmoid", QString::number(this->factorSigmoid), "int");
	addParameter("minScale", QString::number(this->minScale), "uchar");
	addParameter("maxScale", QString::number(this->maxScale), "uchar");
	addParameter("displayFeatureMap", (this->displayFeatureMap== true)? "yes" : "no", "bool");
	addParameter("displayFirstModel", (this->displayFirstModel== true)? "yes" : "no", "bool");

	return true;
}

bool TemporalVarianceBGModule::init()
{
	firstTime = true;
	this->ptr_fgMean = NULL;
	this->ptr_fgMean2 = NULL;
	this->ptr_fgVar = NULL;
	this->ptr_bgMean = NULL;
	this->ptr_bgMean2 = NULL;
	this->ptr_bgVar = NULL;
	this->ptr_fgVarMask= NULL;
	this->ptr_fgMask = NULL;

	return true;
}

bool TemporalVarianceBGModule::updateParameters()
{
	parameter *bgSizeWindow, *fgSizeWindow, *detectionFactorThreshold, *fgVarianceThreshold, *factorSigmoid;

	bgSizeWindow = getParameter("bgSizeWindow");
	fgSizeWindow = getParameter("fgSizeWindow");
	detectionFactorThreshold = getParameter("detectionFactorThreshold");
	fgVarianceThreshold = getParameter("fgVarianceThreshold");
	factorSigmoid = getParameter("factorSigmoid");

	this->bgSizeWindow = bgSizeWindow->value.toInt();
	this->fgSizeWindow = fgSizeWindow->value.toInt();
	this->detectionFactorThreshold = detectionFactorThreshold->value.toDouble();
	this->fgVarianceThreshold = fgVarianceThreshold->value.toDouble();
	this->factorSigmoid = factorSigmoid->value.toInt();

	return true;
}

bool TemporalVarianceBGModule::run()
{
	uchar *ptr_imgData = m_data->currentImage->bits();

	if(imgARGB32.empty() || imgARGB32.data == NULL)
	{
		this->height = m_data->currentImage->height();
		this->width = m_data->currentImage->width();
		this->imgARGB32 = cv::Mat(height, width, CV_8UC4 );
	}
	memcpy(imgARGB32.data, ptr_imgData, m_data->currentImage->height()*m_data->currentImage->bytesPerLine());
	cv::cvtColor(imgARGB32, currImgC3, CV_RGBA2BGR);
	cv::cvtColor(currImgC3, currGrayImg, CV_BGR2GRAY );
	ptr_currGrayImg = currGrayImg.data;

	if(firstTime)
	{
		this->height = m_data->currentImage->height();
		this->width = m_data->currentImage->width();

		ptr_fgMean = (float *) malloc(sizeof(float) * width* height );
		ptr_fgMean2 = (float *) malloc(sizeof(float) * width*height );
		ptr_fgVar = (float *) malloc(sizeof(float) * width*height );
		memset(ptr_fgVar, 0, sizeof(float) * width*height);

		ptr_bgMean = (float *) malloc(sizeof(float) * width*height );
		memset(ptr_bgMean, 0, sizeof(float) * width*height);
		ptr_bgMean2 = (float *) malloc(sizeof(float) * width*height );
		memset(ptr_bgMean2, 0, sizeof(float) * width*height);
		ptr_bgVar = (float *) malloc(sizeof(float) * width*height );
		memset(ptr_bgVar, 0, sizeof(float) * width*height);

		ptr_fgVarMask = (uchar *) malloc(sizeof(uchar)* width*height );
		ptr_fgMask= (uchar *) malloc(sizeof(uchar)* width*height );

		if(m_data->fgImage == NULL)
		{
			m_data->fgImage = new QImage(width,height, QImage::Format_Indexed8);
			m_data->fgImage->setColorTable(*(m_data->grayScaleTable));
			memset(m_data->fgImage->bits(), 0, height*m_data->fgImage->bytesPerLine());
		}
		if(m_data->temporalVariance == NULL)
		{
			m_data->temporalVariance = new QImage(width,height, QImage::Format_Indexed8);
			m_data->temporalVariance->setColorTable(*(m_data->grayScaleTable));
			memset(m_data->temporalVariance->bits(), 0, height*m_data->temporalVariance->bytesPerLine());
		}
		if(m_data->reliabilityMap == NULL || m_data->reliabilityMap->isNull())
		{
			m_data->reliabilityMap = new QImage(width,height, QImage::Format_ARGB32);
		}

		for(int i = 0; i < height*width; i++)
		{
			ptr_fgMean[i] = ptr_currGrayImg[i];
			ptr_fgMean2[i] = ptr_currGrayImg[i]*ptr_currGrayImg[i];
			ptr_bgMean[i] = ptr_currGrayImg[i];
			ptr_bgMean2[i] = ptr_currGrayImg[i]*ptr_currGrayImg[i];
		}

		reliability = cv::Mat(height, width, CV_8UC1);
		ptr_reliability = reliability.data;

		firstTime = false;
		return true;
	}

	for(int i = 0; i < width* height; i++)
	{
		//fg model
		ptr_fgMean[i] =( (fgSizeWindow - 1)*ptr_fgMean[i] + ptr_currGrayImg[i])/fgSizeWindow;
		ptr_fgMean2[i] =( (fgSizeWindow - 1)*ptr_fgMean2[i] + ptr_currGrayImg[i]*ptr_currGrayImg[i])/fgSizeWindow;
		ptr_fgVar[i] = ptr_fgMean2[i] - ptr_fgMean[i]*ptr_fgMean[i];
		if(ptr_fgVar[i] < TRUNCATE_VALUE)
			ptr_fgVar[i] = 0.0;

		ptr_fgVarMask[i] = (ptr_fgVar[i] > this->fgVarianceThreshold)? FOREGROUND: BACKGROUND;

		//bg model
		if(ptr_fgVarMask[i] == BACKGROUND)
		{
			ptr_bgMean[i] = ( (bgSizeWindow - 1)*ptr_bgMean[i] + ptr_currGrayImg[i])/bgSizeWindow;
			ptr_bgMean2[i] = ( (bgSizeWindow - 1)*ptr_bgMean2[i] + ptr_currGrayImg[i]*ptr_currGrayImg[i])/bgSizeWindow;
			ptr_bgVar[i] = ptr_bgMean2[i] - ptr_bgMean[i]*ptr_bgMean[i];

			if(ptr_bgVar[i] < TRUNCATE_VALUE)
				ptr_bgVar[i] = 0.0;
		}
	}

	ptr_dataVarMask = m_data->temporalVariance->bits();
	memcpy(ptr_dataVarMask, ptr_fgVarMask,  width* height);

	//background update
	bgVarMean = 0.0, numBgPixels = 0.0;

	for(int i = 0; i < width* height; i++)
	{
		if( ptr_bgVar[i] != 0.0)
		{
			bgVarMean += sqrt(ptr_bgVar[i]);
			numBgPixels++;
		}
	}
	bgVarMean /= numBgPixels;

  detectionThreshold = this->detectionFactorThreshold * bgVarMean;
  featureMap = cv::Mat(height,width, CV_8UC1);

	for(int i = 0; i < width* height; i++)
	{
		value = sqrt(ptr_fgVar[i])* confidenceOfForeground(fabs(ptr_bgMean[i] - ptr_currGrayImg[i] ), sqrt(ptr_bgVar[i])); //value in [0,255]
		featureMap.data[i] = normalizeScale(value);
		ptr_reliability[i] = normalizeBothSideScale(value, detectionThreshold);
		ptr_fgMask[i] = (value > detectionThreshold)? FOREGROUND: BACKGROUND;
	}

  uchar *ptr_dataFgImg= m_data->fgImage->bits();
  memcpy(ptr_dataFgImg, ptr_fgMask, height*m_data->fgImage->bytesPerLine());

  reliabilityThermal = thermalColor.reliabilityToThermal(reliability);
  cv::cvtColor(reliabilityThermal, imgARGB32, CV_BGR2RGBA);
  memcpy(m_data->reliabilityMap->bits(), imgARGB32.data, height * m_data->reliabilityMap->bytesPerLine());

  if(displayFeatureMap)
  {
    cv::namedWindow("featureMap");
    cv::imshow("featureMap",featureMap);
  }

  if(displayFirstModel)
  {
    cv::Mat firstModel (height, width, CV_8UC1, this->ptr_fgVarMask);
    cv::namedWindow("firstModelFg");
    cv::imshow("firstModelFg",firstModel);
  }

  /*
  if(displaySecondModel) // second model is calculated on-line
  {
    cv::namedWindow("secondModelFg");
    cv::imshow("secondModelFg",featureMap);
  }
  */

  //save results,
  //TODO: make it an option avaible from parameters.
//  m_data->fgImage->save("TV_fg_" +  QString::number(m_data->frameNumber) + ".jpg");
//  m_data->reliabilityMap->save("TV_feature_" +  QString::number(m_data->frameNumber) + ".jpg");

	return true;
}

//normalize values between minScale and maxScale using linear aproximation.
//values smaller or equals than minScale are define as 0, and values greater
//or equals than maxScale are define as 255.
uchar inline TemporalVarianceBGModule::normalizeScale(float value)
{
  if(value <= minScale)
    return 0;
  if(value >= maxScale)
    return 255;

  return ((value - minScale)*255)/(maxScale - minScale);
}

//iguala la distancia de minScale a threshold y de threshold a maxScale, de tal
//forma que valores cercanos al umbral esten cercanos a 0, y valores cercanos
//a minScale o maxScale esten cercanos a 255.
uchar inline TemporalVarianceBGModule::normalizeBothSideScale(float value, uchar threshold)
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

void TemporalVarianceBGModule::showHistogram( cv::Mat histogram, int numBins)
{
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound( (double) hist_w/numBins );

    cv::Mat histNorm, histImage( hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0) );


    if(histNorm.data == NULL)
      cv::normalize(histogram, histNorm, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );


    for( int i = 1; i < numBins; i++ )
    {
        cv::line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(histNorm.at<float>(i-1)) ) ,
                         cv::Point( bin_w*(i), hist_h - cvRound(histNorm.at<float>(i)) ),
                         cv::Scalar( 0, 255, 0), 2, 8, 0  );
    }

    cv::namedWindow("Histogram", CV_WINDOW_AUTOSIZE );
    cv::imshow("Histogram", histImage );
}

float TemporalVarianceBGModule::confidenceOfForeground(float diference, float stdDeviation)
{
	float value;
	if(stdDeviation == 0)
	{
		return 0;
	}
	if(diference < stdDeviation*this->factorSigmoid)
	{
		value = 0.5*(1.0 - cos((PI*diference)/(factorSigmoid*stdDeviation)) );
		return value;
	}
	return 1;
}
