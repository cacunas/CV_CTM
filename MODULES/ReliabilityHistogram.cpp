#include "ReliabilityHistogram.h"

ReliabilityHistogram::ReliabilityHistogram(Datapool *i_data):ModuleInterface(i_data)
{
}

ReliabilityHistogram::~ReliabilityHistogram()
{
}

bool ReliabilityHistogram::setParameters(QDomNode& config)
{
	QDomNode n;

	this->numBins = 256;
	this->showHistogram = true;
    this->fileMaskName = "maskAlfheim.jpg";
//    this->fileMaskName = "maskValpo.jpg";
	this->showDataAtFrame = 1000;
//	if(config.isNull())  //Parameter set for module not defined

	return true;
}

bool ReliabilityHistogram::init()
{
	firstTime = true;

	return true;
}

bool ReliabilityHistogram::updateParameters()
{
	return true;
}

bool ReliabilityHistogram::run()
{
	if(firstTime)
	{
		if(thermalARGB32.empty() || thermalARGB32.data == NULL)
		{
			this->height = m_data->reliabilityMap->height();
			this->width = m_data->reliabilityMap->width();
			this->thermalARGB32 = cv::Mat(height, width, CV_8UC4 );
		}

		QByteArray string8bit = this->fileMaskName.toLocal8Bit();
//		this->fieldMask = cv::imread(string8bit.data(), CV_LOAD_IMAGE_GRAYSCALE );
        this->fieldMask = cv::Mat();
		this->firstTime = false;
	}

	if(m_data->reliabilityMap->bits() == NULL || m_data->reliabilityMap->isNull() )
	{
		AppendToLog("ReliabilityHistogram: Warning: 'reliability map' not found.\n");
		return true;
	}

	memcpy(thermalARGB32.data, m_data->reliabilityMap->bits(), height*m_data->reliabilityMap->bytesPerLine());
	cv::cvtColor(thermalARGB32, currThermal, CV_RGBA2BGR);
	reliability = thermal.thermalToReliability( currThermal );

  float range[] = { 0, 255 } ;
  const float* histRange = { range };
  cv::calcHist( &reliability, 1, 0, this->fieldMask, reliabHisto, 1, &numBins, &histRange, true, true );

	if(m_data->frameNumber == this->showDataAtFrame)
	{
		cv::normalize(this->reliabHisto, histNorm, 0, 100, cv::NORM_MINMAX, -1, cv::Mat() );
		for(int i = 0; i < 256; i++)
		{
			std::cout << histNorm.at<float>(i) << " ";
		}
		std::cout<< std::endl;
	}

	if(showHistogram)
		displayHistogram(this->reliabHisto);

	return true;
}

void ReliabilityHistogram::displayHistogram( cv::Mat histogram)
{
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound( (double) hist_w/numBins );

    cv::Mat histNorm, histImage( hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0) );


    if(histNorm.data == NULL)
      cv::normalize(histogram, histNorm, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );


    for( int i = 1; i < this->numBins; i++ )
    {
        cv::line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(histNorm.at<float>(i-1)) ) ,
                         cv::Point( bin_w*(i), hist_h - cvRound(histNorm.at<float>(i)) ),
                         cv::Scalar( 0, 255, 0), 2, 8, 0  );
    }

    cv::namedWindow("Reliability Histogram", CV_WINDOW_AUTOSIZE );
    cv::imshow("Reliability Histogram", histImage );
}

