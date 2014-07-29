#ifndef SET_TEMPORAL_VARIANCE_H
#define SET_TEMPORAL_VARIANCE_H

#include "Datapool.h"
#include "drawInterface.h"

class setTemporalVariance: public drawInterface
{
public:
        setTemporalVariance(Datapool *i_data);

        ~setTemporalVariance();

		//Set module configuration parameters
		bool setParameters(QDomNode &config);

		//Initialization after reading parameters
		bool init();

		//Function executed at each frame
		bool draw(QImage *image);

		bool paint(QImage *image);
};

#endif // SET_TEMPORAL_VARIANCE_H
