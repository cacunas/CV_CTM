#include "MorphologyModule.h"
#include "image_display.h"

MorphologyModule::MorphologyModule(Datapool *i_data):
	ModuleInterface(i_data)
{
}

MorphologyModule::~MorphologyModule()
{
}

bool MorphologyModule::setParameters(QDomNode& config){

	QDomNode n;
	if(config.isNull())  //Parameter set for module not defined
	{
		this->size = 1;
		this->morphOption = 2;
		this->morphIterations = 1;
		this->typeElement = 0;
	}
	else
	{
		if( !( n = XmlCommon::XmlCommon::getParameterNode("size", config) ).isNull() )
			this->size = XmlCommon::getParameterValue(n).toInt();
		else
		{
			this->size = 1;
			AppendToLog("MorphologyModule: Warning: 'size' not defined. Taking defaults: \n\t\tsize = " + QString::number(this->size) + "\n");
		}
		if( !( n = XmlCommon::XmlCommon::getParameterNode("morphOption", config) ).isNull() )
			this->morphOption = XmlCommon::getParameterValue(n).toInt();
		else
		{
			this->morphOption = 2;
			AppendToLog("MorphologyModule: Warning: 'morphOption' not defined. Taking defaults: \n\t\tmorphOption = " + QString::number(this->morphOption) + "\n");
		}
		if( !( n = XmlCommon::XmlCommon::getParameterNode("morphIterations", config) ).isNull() )
			this->morphIterations = XmlCommon::getParameterValue(n).toInt();
		else
		{
			this->morphIterations = 1;
			AppendToLog("MorphologyModule: Warning: 'morphIterations' not defined. Taking defaults: \n\t\tmorphIterations = " + QString::number(this->morphIterations) + "\n");
		}
		if( !( n = XmlCommon::XmlCommon::getParameterNode("typeElement", config) ).isNull() )
			this->typeElement = XmlCommon::getParameterValue(n).toInt();
		else
		{
			this->typeElement = 0;
			AppendToLog("MorphologyModule: Warning: 'typeElement' not defined. Taking defaults: \n\t\ttypeElement = " + QString::number(this->typeElement) + "\n");
		}
	}

	addParameter("size", QString::number(this->size), "int");
	addParameter("morphOption", QString::number(this->morphOption), "int");
	addParameter("morphIterations", QString::number(this->morphIterations), "int");
	addParameter("typeElement", QString::number(this->typeElement), "int");

	return true;
}

bool MorphologyModule::updateParameters(){
	parameter *size, *morphOption, *morphIterations;

	size = getParameter("size");
	morphOption = getParameter("morphOption");
	morphIterations = getParameter("morphIterations");

	this->size = size->value.toInt();
	this->morphOption = morphOption->value.toInt();
	this->morphIterations = morphIterations->value.toInt();

	return true;
}

bool MorphologyModule::init(){
	this->firstTime = true;

	return true;
}

bool MorphologyModule::run()
{
	if(firstTime)
	{
		this->height = m_data->fgImage->height();
		this->width = m_data->fgImage->width();
		fgMat = cv::Mat(height, width, CV_8UC1);
		element = cv::getStructuringElement( this->typeElement,
														 cv::Size( 2*size+1, 2*size+1),
														 cv::Point( size, size ) );

		firstTime = false;
	}

	memcpy(fgMat.data, m_data->fgImage->bits(), height * m_data->fgImage->bytesPerLine());

	if(this->morphOption < 2)
	{
		if	(this->morphOption == 0)
		{
			cv::erode(fgMat, fgMat, element, cv::Point(-1,-1), this->morphIterations,
								cv::BORDER_CONSTANT, cv::morphologyDefaultBorderValue() );
		}
		if (this->morphOption == 1)
		{
			cv::dilate(fgMat, fgMat, element, cv::Point(-1,-1), this->morphIterations,
								cv::BORDER_CONSTANT, cv::morphologyDefaultBorderValue() );
		}
	}
	else
	{
		cv::morphologyEx( fgMat, fgMat, morphOption, element, cv::Point(-1,-1), this->morphIterations,
											cv::BORDER_CONSTANT, cv::morphologyDefaultBorderValue() );
	}

	memcpy(m_data->fgImage->bits(), fgMat.data, height * m_data->fgImage->bytesPerLine());

	return true;
}
