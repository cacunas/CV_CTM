#include "ReliabilitySegmentationModule.h"
#include <errno.h>
#include <iostream>
#include "image_display.h"
#include "src/MathFunctions.h"
#include <QMessageBox>
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <stdlib.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

ReliabilitySegmentationModule::ReliabilitySegmentationModule(Datapool *i_data):ModuleInterface(i_data)
{
	firstTime = true;
}

ReliabilitySegmentationModule::~ReliabilitySegmentationModule(){}

bool ReliabilitySegmentationModule::setParameters(QDomNode& config)
{
	QDomNode n;

	if(config.isNull())  //Parameter set for module not defined
	{
		AppendToLog("ReliabilitySegmentationModule: Warning: Missing config file, using fixed parameters\n");
		this->sigma = 40;
		this->beginningFrame = 2;
		this->dilatacion = 3;
		this->colorUmbral = 15;
	}
	else
	{
		if( !( n = XmlCommon::XmlCommon::getParameterNode("sigma", config) ).isNull() )
			this->sigma = XmlCommon::getParameterValue(n).toInt();
		else
		{
			this->sigma = 40;
			AppendToLog("ReliabilitySegmentationModule: Warning: 'sigma' not defined. Taking defaults: \n\t\tsigma = " + QString::number(this->sigma) + "\n");
		}
		if( !( n = XmlCommon::XmlCommon::getParameterNode("beginningFrame", config) ).isNull() )
			this->beginningFrame = XmlCommon::getParameterValue(n).toInt();
		else
		{
			this->beginningFrame = 2;
			AppendToLog("ReliabilitySegmentationModule: Warning: 'beginningFrame' not defined. Taking defaults: \n\t\tbeginningFrame = " + QString::number(this->beginningFrame) + "\n");
		}
		if( !( n = XmlCommon::XmlCommon::getParameterNode("dilatacion", config) ).isNull() )
			this->dilatacion = XmlCommon::getParameterValue(n).toInt();
		else
		{
			this->dilatacion = 3;
			AppendToLog("ReliabilitySegmentationModule: Warning: 'dilatacion' not defined. Taking defaults: \n\t\tdilatacion = " + QString::number(this->dilatacion) + "\n");
		}
	}

	//Setea la lista de parametros y la lista de tipos de parametros utilizadas en el ParameterDialog.
	addParameter("sigma", QString::number(this->sigma), "int");
	addParameter("beginningFrame", QString::number(this->beginningFrame), "int");
	addParameter("dilatacion", QString::number(this->dilatacion), "int");

	return true;
}

bool ReliabilitySegmentationModule::init()
{
	if(firstTime == false) {
			if(m_data->reliabilityMap != NULL)
			{
					delete m_data->reliabilityMap;
					m_data->reliabilityMap = NULL;
			}
			if(m_data->empiricalReliabilityMap != NULL)
			{
				delete m_data->empiricalReliabilityMap;
				m_data->empiricalReliabilityMap = NULL;
			}
			firstTime = true;
	}

	return true;
}

bool ReliabilitySegmentationModule::updateParameters()
{	
	parameter *sigma, *beginningFrame, *dilatacion;

	sigma = getParameter("sigma");
	beginningFrame = getParameter("beginningFrame");
	dilatacion = getParameter("dilatacion");

	this->sigma = sigma->value.toInt();
	this->beginningFrame = beginningFrame->value.toInt();
	this->dilatacion = dilatacion->value.toInt();

	return true;
}


//recive un mapa de confiabilidad con valores 0 (menor confiabilidad) al 255 (mayor confiabilidad)
//y retorna la conversion al espacio de color termal (BGR).
cv::Mat ReliabilitySegmentationModule::reliabilityToThermal(cv::Mat reliability)
{
	cv::Mat hue(currImgGray.rows, currImgGray.cols, CV_8UC1),
			S(currImgGray.rows, currImgGray.cols, CV_8UC1, cv::Scalar(255)),
			V(currImgGray.rows, currImgGray.cols, CV_8UC1, cv::Scalar(127)),
			lookUpTable(1,256,CV_8UC1),
			channels[] = {hue,S,V},
			thermalImageHsv (currImgGray.rows, currImgGray.cols, CV_8UC3),
			thermalImage;

	int from_to[] = { 0,0, 1,1, 2,2 };
	uchar *ptr_lookTableData = lookUpTable.data;

	for(int i = 0; i < 256; i++)
	{
		ptr_lookTableData[i] = i;
	}

	lookUpTable = 120*(lookUpTable/255.0); //transform to thermal space color
	cv::LUT(reliability, lookUpTable, hue);

	cv::mixChannels( channels, 3, &thermalImageHsv, 1, from_to, 3 );
	cv::cvtColor(thermalImageHsv, thermalImage, CV_HSV2BGR );

	return thermalImage;
}

bool ReliabilitySegmentationModule::run()
{
	if(firstTime)
	{
		firstTime = false;
		imgWidth = m_data->currentImage->width();
		imgHeight = m_data->currentImage->height();

		if(m_data->reliabilityMap == NULL || m_data->reliabilityMap->isNull())
		{
			m_data->reliabilityMap = new QImage(imgWidth,imgHeight, QImage::Format_ARGB32);
			reliabilityMap = cv::Mat(imgHeight, imgWidth, CV_8UC1 );
		}
		if(m_data->empiricalReliabilityMap == NULL || m_data->empiricalReliabilityMap->isNull())
		{
			m_data->empiricalReliabilityMap = new QImage(imgWidth,imgHeight, QImage::Format_ARGB32);
			empiricalReliabilityMap = cv::Mat(imgHeight, imgWidth, CV_8UC1 );

		}
	}

	uchar *ptr_imgData = m_data->currentImage->bits(),
			*ptr_fgData = m_data->fgImage->bits(),
			*ptr_bgData = m_data->bgImage->bits(),
			*ptr_mReliabilityMap, *ptr_mEmpiricalReliabilityMap;

	cv::Mat imgARGB32, bgARGB32, reliabilityMapARGB32, empiricalReliabilityMapARGB32;

	ptr_mReliabilityMap = m_data->reliabilityMap->bits();
	ptr_mEmpiricalReliabilityMap = m_data->empiricalReliabilityMap->bits();

	imgARGB32 = cv::Mat(imgHeight, imgWidth, CV_8UC4 );
	bgARGB32 = cv::Mat(imgHeight, imgWidth, CV_8UC4 );
	foreground = cv::Mat(imgHeight, imgWidth, CV_8UC1 );

	memcpy(imgARGB32.data, ptr_imgData, imgHeight*m_data->currentImage->bytesPerLine());
	memcpy(bgARGB32.data, ptr_bgData, imgHeight*m_data->bgImage->bytesPerLine());
	memcpy(foreground.data, ptr_fgData, imgHeight*m_data->fgImage->bytesPerLine());

	cv::cvtColor(imgARGB32, currImgColor, CV_RGBA2BGR);
	cv::cvtColor(bgARGB32, currBgColor, CV_RGBA2BGR);
	cv::cvtColor(currImgColor, currImgGray, CV_BGR2GRAY );
	cv::cvtColor(currBgColor, currBgGray, CV_BGR2GRAY );

//	cv::namedWindow("gris",1);
//	cv::imshow("gris",currImgGray);

	currImageReliability();

	//TODO: cambiar a futuro por actualizacion modelo de confiabilidad empirica
	if(m_data->processedFrames > beginningFrame)
	{
		empiricalReliability();
	}
	else
	{
		//copiar modelo de confiabilidad como modelo inicial de la confiabilidad empirica
		reliabilityMap.copyTo( empiricalReliabilityMap );
	}

	//copy reliabilityMap and empiricalReliabilityMap to m_data
	cv::cvtColor(reliabilityToThermal(reliabilityMap), reliabilityMapARGB32, CV_BGR2RGBA);
	cv::cvtColor(reliabilityToThermal( empiricalReliabilityMap), empiricalReliabilityMapARGB32, CV_BGR2RGBA);
	memcpy(ptr_mReliabilityMap, reliabilityMapARGB32.data, reliabilityMapARGB32.rows * reliabilityMapARGB32.step);
	memcpy(ptr_mEmpiricalReliabilityMap, empiricalReliabilityMapARGB32.data,
				 empiricalReliabilityMapARGB32.rows * empiricalReliabilityMapARGB32.step);

	m_data->reliabilityMap->save("ilum_" + QString::number( m_data->frameNumber ) + ".jpg");
	return true;
}

//Asign image reliability related to its pixel intensity
void ReliabilitySegmentationModule::currImageReliability()
{
	double mean = 127.0, reliabilityValue;
	uchar *ptr_tableData;
	cv::Mat lookUpTable(1,256,CV_8U);

	ptr_tableData = lookUpTable.data;

	for(int i = 0; i < 256; i++)
	{
		//pixelReliability best: 1;  worst:0
		reliabilityValue =  MathFunctions::gaussian(0,1,(mean-i)/this->sigma);
		ptr_tableData[i] = reliabilityValue*255;
	}

	cv::LUT(currImgGray, lookUpTable, this->reliabilityMap);
}

//Weber contrast compare the luminicence of background with the image or object
//high contrast 1; low contrast 0
uchar ReliabilitySegmentationModule::weberContrast(cv::Mat roiImg, cv::Mat roiBg, cv::Mat fgMask)
{
	cv::Mat result;
	std::vector<cv::Mat> channel(3);
	cv::Scalar channelMean;
	float contrast;

	cv::absdiff(roiImg,roiBg,result );
	cv::split(result,channel);

	cv::multiply(channel[0],fgMask,channel[0]);
	cv::multiply(channel[1],fgMask,channel[1]);
	cv::multiply(channel[2],fgMask,channel[2]);

	roiBg +=1; // para evitar divisiones por 0
	channelMean = cv::mean(result/roiBg);

	//find the maximun contrast in the channels
	if(channelMean[0] > channelMean[1])
		if(channelMean[0] > channelMean[2])
			contrast = channelMean[0];
		else
			contrast = channelMean[2];
	else
		if(channelMean[1] > channelMean[2])
			contrast = channelMean[1];
		else
			contrast = channelMean[2];


	if(contrast > 1.0)
		contrast = 1.0;
	return contrast*255;
}

float ReliabilitySegmentationModule::pixelColorGrade(uchar L, uchar a, uchar b)
{
	float gradeColor = abs(128-a) + abs(128-b) + 128 - abs(127-L); //cilindrical non-color volume
	return gradeColor/(3*128);
}

/*
determinar el grado de color del objeto en movimiento //hay q ver definir el criterio cuando el color es similar al del fondo
*/
uchar ReliabilitySegmentationModule::maskColorGrade(cv::Mat roiImg, cv::Mat roiBg, cv::Mat fgMask)
{
	//transformar img to cielab
	//usar como medida de color = |a-128|+|b-128| + (127 - |128-L|)
	//promediar para cada pixel en movimiento el grado de color
	//transformar al rango 0-255 el promedio

	cv::Mat roiImgLab;
	cv::cvtColor(roiImg, roiImgLab, CV_BGR2Lab);
	float meanColorGrade=0.0;
	for(int i=0; i < roiImg.rows; i++)
	{
		for(int j = 0; j < roiImg.cols; j++)
		{
			meanColorGrade += fgMask.at<uchar>(i,j) * pixelColorGrade(roiImgLab.at<cv::Vec3b>(i,j)[0],roiImgLab.at<cv::Vec3b>(i,j)[1],roiImgLab.at<cv::Vec3b>(i,j)[2]);
		}
	}
	meanColorGrade /= roiImg.rows* roiImg.cols;
	return meanColorGrade*255;
}

//actualiza la confiabilidad del modelo (modelo de confiabilidad empirica) usando reliabilityMesaure del blob
void ReliabilitySegmentationModule::applyReliabilityAura(cv::Mat patchAura, cv::Mat fgMask, uchar reliabilityMesaure) //reliabilityMeasure [0,255]
{
reliabilityMesaure *=4;
//	int minSide = patchAura.cols > patchAura.rows? patchAura.rows: patchAura.cols;
	cv::Mat mask, negMask, result;

//	minSide *= this->auraDiameter;
	patchAura.adjustROI(dilatacion,dilatacion,dilatacion,dilatacion);//grow in every direction dilatacion pixels

	mask = (patchAura < reliabilityMesaure);
	mask /= 255;
	cv::multiply(mask,fgMask,mask); //pixeles en movimiento y menores a la confiabilidad
	negMask = 1-mask;

	multiply(patchAura,negMask,result);//assign 0 to pixel less than contrast value
	result += reliabilityMesaure*mask;
	result.copyTo(patchAura);
}

/*TODO: crear capa q permita cambiar distintas medidas de dificultad de zonas
//a futuro se agregaran mas medida.
float ReliabilitySegmentationModule::getReliabilityMeasure(Blob &ptr_blob, int option)
{
	float reliabilityValue;



	return reliabilityValue ;
}
*/

void ReliabilitySegmentationModule::empiricalReliability()
{
	if(m_data->blobs.empty())
	{
		return;
	}

	cv::Mat roiCurrImg, roiCurrBg, roiEmpiricalReliabilityMap, fgRoi, fgMask,
			element = cv::getStructuringElement( cv::MORPH_ELLIPSE,
																					 cv::Size( 2*dilatacion+1, 2*dilatacion+1 ),
																					 cv::Point( dilatacion, dilatacion ) );
	uchar reliabilityAuraValue;

	std::vector<Blob>::iterator end_blobVector = m_data->blobs.end(), blobIter;
	Blob *ptr_blob;

	int left, right, top, bottom;

	for(blobIter = m_data->blobs.begin(); blobIter != end_blobVector; blobIter++)
	{
		ptr_blob = &*blobIter;

		top = BLOB_YTOP(ptr_blob);
		bottom = BLOB_YBOTTOM(ptr_blob);
		left = BLOB_XLEFT(ptr_blob);
		right = BLOB_XRIGHT(ptr_blob);

		roiCurrImg = currImgColor(cv::Range(top,bottom),cv::Range(left,right));
		roiCurrBg = currBgColor(cv::Range(top,bottom),cv::Range(left,right));
		roiEmpiricalReliabilityMap = this->empiricalReliabilityMap(cv::Range(top,bottom),cv::Range(left,right));
		fgRoi = this->foreground(cv::Range(top,bottom),cv::Range(left,right));

		cv::dilate( fgRoi, fgMask, element); //obtener el aura
		fgMask /= 255;
		//reliabilityAuraValue = weberContrast(roiCurrImg, roiCurrBg, fgMask);//cambiar
		reliabilityAuraValue = maskColorGrade(roiCurrImg, roiCurrBg, fgMask);
		std::cout<<"umbral value:" << (int)reliabilityAuraValue<< std::endl;

		fgRoi.adjustROI(dilatacion,dilatacion,dilatacion,dilatacion);
		cv::dilate( fgRoi, fgMask, element);
		fgMask /= 255;
		applyReliabilityAura(roiEmpiricalReliabilityMap, fgMask, reliabilityAuraValue);
	}
}

