#ifndef SETHISTOSTAT_H
#define SETHISTOSTAT_H

#include "Datapool.h"
#include "drawInterface.h"
#include "QPoint"


class setHistoStat: public drawInterface
{
public:
    setHistoStat(Datapool *i_data);

    ~setHistoStat();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    int Hues[36];

    QPoint *loc;

    float ratio;
    float colorAccuracyRatio;


    void displayShowHull(QPainter &painter, SpHullModel hull);
    void displayShowAxis(QPainter &painter, SpHullModel hull);
    void displayShowNormals(QPainter &painter, SpHullModel hull);
    void displayBackBone(QPainter &painter, SpHullModel hull);

};


#endif // SETHISTOSTAT_H
