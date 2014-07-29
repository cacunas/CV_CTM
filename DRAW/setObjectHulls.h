#ifndef SETOBJECTHULLS_H
#define SETOBJECTHULLS_H

#include <QPainter>

#include "drawInterface.h"

class setObjectHulls: public drawInterface
{
public:
    setObjectHulls(Datapool *i_data);


    ~setObjectHulls();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    bool paint(QImage *image);
    void displayShowHull(QPainter &painter, SpHullModel hull);
    void displayShowAxis(QPainter &painter, SpHullModel hull);
    void displayShowNormals(QPainter &painter, SpHullModel hull);

};

#endif // SETOBJECTHULLS_H
