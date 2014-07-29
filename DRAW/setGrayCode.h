#ifndef SETGRAYCODE_H
#define SETGRAYCODE_H

#include "Datapool.h"
#include "drawInterface.h"


class setGrayCode: public drawInterface
{
public:
    setGrayCode(Datapool *i_data);

    ~setGrayCode();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    bool paint(QImage *image);
};

#endif // SETGRAYCODE_H
