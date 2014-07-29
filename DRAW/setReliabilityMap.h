#ifndef SETREALIABILITYMAP_H
#define SETREALIABILITYMAP_H

#include "Datapool.h"
#include "drawInterface.h"

class setReliabilityMap: public drawInterface
{
public:
		setReliabilityMap(Datapool *i_data);

		~setReliabilityMap();

		//Set module configuration parameters
		bool setParameters(QDomNode &config);

		//Initialization after reading parameters
		bool init();

		//Function executed at each frame
		bool draw(QImage *image);

		bool paint(QImage *image);
};

#endif // SETREALIABILITYMAP_H
