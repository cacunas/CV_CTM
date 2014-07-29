#ifndef SETEMPIRICALRELIABILITYMAP_H
#define SETEMPIRICALRELIABILITYMAP_H

#include "Datapool.h"
#include "drawInterface.h"

class setEmpiricalReliabilityMap: public drawInterface
{
public:
		setEmpiricalReliabilityMap(Datapool *i_data);

		~setEmpiricalReliabilityMap();

		//Set module configuration parameters
		bool setParameters(QDomNode &config);

		//Initialization after reading parameters
		bool init();

		//Function executed at each frame
		bool draw(QImage *image);

		bool paint(QImage *image);
};
#endif // SETEMPIRICALRELIABILITYMAP_H
