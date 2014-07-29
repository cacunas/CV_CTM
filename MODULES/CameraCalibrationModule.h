#ifndef CAMERA_CALIBRATION_MODULE_H
#define CAMERA_CALIBRATION_MODULE_H

#include "Datapool.h"
#include "ModuleInterface.h"
#include <QDir>

class CameraCalibrationModule: public ModuleInterface
{
	public:
	CameraCalibrationModule(Datapool *i_data);
	~CameraCalibrationModule();

	//Set module configuration parameters
	bool setParameters(QDomNode& config);

	//Initialization after reading parameters
	bool init();

	//update parameters at runtime.
	bool updateParameters();

	//Function executed at each frame
	bool run();

	double calibrate();

	bool firstTime;
	int width, height, flag;
	cv::Mat  imgARGB32, currImgC3, undistorted, map1, map2;
};
//formato archivo calibracion "calibrate.txt":
//X_objeto Y_objeto X_imagen Y_imagen
#endif // CAMERA_CALIBRATION_MODULE_H
