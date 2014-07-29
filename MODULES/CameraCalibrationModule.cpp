#include "CameraCalibrationModule.h"
#include "image_display.h"
#include <fstream>
#include <string>
#include <iostream>

CameraCalibrationModule::CameraCalibrationModule(Datapool *i_data):ModuleInterface(i_data)
{
}
CameraCalibrationModule::~CameraCalibrationModule()
{
}

bool CameraCalibrationModule::setParameters(QDomNode& config)
{
	QDomNode n;
	if(config.isNull())  //Parameter set for module not defined
	{
		this->flag = 0;
	}
	else
	{
		this->flag = 0;
		if( !( n = XmlCommon::XmlCommon::getParameterNode("fixK1", config) ).isNull() )
			this->flag  |= XmlCommon::getParameterValue(n) == "true" ? CV_CALIB_FIX_K1 : 0;
		if( !( n = XmlCommon::XmlCommon::getParameterNode("fixK2", config) ).isNull() )
			this->flag  |= XmlCommon::getParameterValue(n) == "true" ? CV_CALIB_FIX_K2 : 0;
		if( !( n = XmlCommon::XmlCommon::getParameterNode("fixK3", config) ).isNull() )
			this->flag  |= XmlCommon::getParameterValue(n) == "true" ? CV_CALIB_FIX_K3 : 0;
		if( !( n = XmlCommon::XmlCommon::getParameterNode("fixK4", config) ).isNull() )
			this->flag  |= XmlCommon::getParameterValue(n) == "true" ? CV_CALIB_FIX_K4 : 0;
		if( !( n = XmlCommon::XmlCommon::getParameterNode("fixK5", config) ).isNull() )
			this->flag  |= XmlCommon::getParameterValue(n) == "true" ? CV_CALIB_FIX_K5 : 0;
		if( !( n = XmlCommon::XmlCommon::getParameterNode("fixK6", config) ).isNull() )
			this->flag  |= XmlCommon::getParameterValue(n) == "true" ? CV_CALIB_FIX_K6 : 0;
		if( !( n = XmlCommon::XmlCommon::getParameterNode("useIntrinsicGuess", config) ).isNull() )
			this->flag  |= XmlCommon::getParameterValue(n) == "true" ? CV_CALIB_USE_INTRINSIC_GUESS : 0;
		if( !( n = XmlCommon::XmlCommon::getParameterNode("fixPrincipalPoint", config) ).isNull() )
			this->flag  |= XmlCommon::getParameterValue(n) == "true" ? CV_CALIB_FIX_PRINCIPAL_POINT : 0;
		if( !( n = XmlCommon::XmlCommon::getParameterNode("fixAspecRatio", config) ).isNull() )
			this->flag  |= XmlCommon::getParameterValue(n) == "true" ? CV_CALIB_FIX_ASPECT_RATIO : 0;
		if( !( n = XmlCommon::XmlCommon::getParameterNode("zeroTangentDist", config) ).isNull() )
			this->flag  |= XmlCommon::getParameterValue(n) == "true" ? CV_CALIB_ZERO_TANGENT_DIST : 0;
		if( !( n = XmlCommon::XmlCommon::getParameterNode("rationalModel", config) ).isNull() )
			this->flag  |= XmlCommon::getParameterValue(n) == "true" ? CV_CALIB_RATIONAL_MODEL : 0;
	}

	return true;
}

bool CameraCalibrationModule::init()
{
	firstTime = true;
	return true;
}

bool CameraCalibrationModule::updateParameters()
{
	return true;
}

bool CameraCalibrationModule::run()
{

	if(firstTime)
	{
		double error = calibrate();
		if(error < 0)
			return false;

		std::cout	<< "Calibration error: " << error << std::endl;
		firstTime = false;
	}
	return true;
	/*
	memcpy(imgARGB32.data, m_data->currentImage->bits(),
			m_data->currentImage->height()*m_data->currentImage->bytesPerLine());

	cv::cvtColor(imgARGB32, currImgC3, CV_RGBA2RGB);

  cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(),
                              cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, cv::Size(width,height), 1,
                                                            cv::Size(width,height), 0),
                              cv::Size(width,height), CV_16SC2, map1, map2);

        cv::remap(currImgC3, undistorted, map1, map2, cv::INTER_LINEAR);

	cv::cvtColor(undistorted, imgARGB32, CV_RGB2RGBA);
	memcpy(m_data->currentImage->bits(), undistorted.data,
				 m_data->currentImage->height()*m_data->currentImage->bytesPerLine());

	cv::Point3f jugador;
	jugador.x = 26.5369159347864;
	jugador.y = 	29.1097040041772;
	jugador.z = 0;

	std::vector<cv::Point3f> obj;
	std::vector<cv::Point2f> out;
	obj.push_back(jugador);

	jugador.x = 22.5290586168584;
	jugador.y = 	16.1480290689568;
	jugador.z = 0;
	obj.push_back(jugador);
	jugador.x = 44.7662047876363;
	jugador.y = 15.7307606432662;
	jugador.z = 0;
	obj.push_back(jugador);
	jugador.x = 41.8132332170463;
	jugador.y = 38.0322958439948;
	jugador.z = 0;
	obj.push_back(jugador);

	cv::projectPoints( obj , rvecs[0], tvecs[0], cameraMatrix, distCoeffs, out);
	cv::circle( currImgC3, out[0], 3, cv::Scalar(0,0,255), -1, 8, 0 );
	cv::circle( currImgC3, out[1], 3, cv::Scalar(0,0,255), -1, 8, 0 );
	cv::circle( currImgC3, out[2], 3, cv::Scalar(0,0,255), -1, 8, 0 );
	cv::circle( currImgC3, out[3], 3, cv::Scalar(0,0,255), -1, 8, 0 );
	cv::namedWindow("undistort",1);
	cv::imshow("undistort", currImgC3);
//	cv::imwrite("undistort.jpg",undistorted);

	return true;
	*/
}

double CameraCalibrationModule::calibrate()
{
	std::vector<std::vector<cv::Point2f> > imageList;
	std::vector<std::vector<cv::Point3f> > objetcList;
	std::vector<cv::Point2f> imagePoints;
	std::vector<cv::Point3f> objectPoints;
	int width = m_data->currentImage->width();
	int height= m_data->currentImage->height();
	float imgX, imgY, objX, objY;

	m_data->cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
	m_data->distCoeffs = cv::Mat::zeros(8, 1, CV_64F);

	std::ifstream file;
	file.open("calibrate.txt");

	while(file >> imgX >> imgY >> objX >> objY)
	{
		objectPoints.push_back( cv::Point3f(objX, objY, 0.0));
		imagePoints.push_back( cv::Point2f(imgX, imgY));
	}
	file.close();

	if(objectPoints.size() < 4)
	{
		AppendToLog("CameraCalibrationModule: Warning: Too few data (there are " + QString::number(objectPoints.size()) + ")\n");
		return -1.0;
	}

	imageList.push_back(imagePoints);
	objetcList.push_back(objectPoints);

	double rms = cv::calibrateCamera(objetcList, imageList, cv::Size(width,height), m_data->cameraMatrix,

					 m_data->distCoeffs, m_data->rvecs, m_data->tvecs, this->flag);

	return rms;
}
