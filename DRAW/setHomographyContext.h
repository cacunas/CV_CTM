#ifndef SETHOMOGRAPHYCONTEXT_H
#define SETHOMOGRAPHYCONTEXT_H

#include "Datapool.h"
#include "drawInterface.h"

class setHomographyContext: public drawInterface
{
public:
    setHomographyContext(Datapool *i_data);

    ~setHomographyContext();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    bool paint(QImage *image);


};

#endif // SETHOMOGRAPHYCONTEXT_H
