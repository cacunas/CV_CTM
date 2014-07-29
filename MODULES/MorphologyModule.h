#ifndef MORPHOLOGYMODULE_H
#define MORPHOLOGYMODULE_H

#include "ModuleInterface.h"


class MorphologyModule: public ModuleInterface {
public:
	MorphologyModule(Datapool *i_data);
	~MorphologyModule();

	//Set module configuration parameters
	bool setParameters(QDomNode& config);

	//Initialization after reading parameters
	bool init();

	//Function executed at each frame
	bool run();

	//update parameters at runtime.
	bool updateParameters();

	bool firstTime;
	int size, width, height, morphOption, morphIterations, typeElement;
	cv::Mat element, fgMat;
};
//morphOption: 0 Erode, 1 Dilate, 2 Opening, 3 Closing, 4 Gradient, 5 Top Hat, 6 Black Hat
//typeElement: 0 Rect, 1 Cross, 2 Ellipse

#endif // MORPHOLOGYMODULE_H
