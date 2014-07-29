#ifndef SET2DCONTEXT_H
#define SET2DCONTEXT_H

#include "Datapool.h"
#include "drawInterface.h"

class set2DContext: public drawInterface
{
public:
    set2DContext(Datapool *i_data);

    ~set2DContext();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    bool paint(QImage *image);


};

#endif // SETHOMOGRAPHYCONTEXT_H
