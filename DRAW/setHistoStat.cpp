#include "setHistoStat.h"
#include "paintView.h"
#include <QPainter>
#include "MODELS/DeguModelDynamics.h"

setHistoStat::setHistoStat(Datapool *i_data):drawInterface(i_data){}

setHistoStat::~setHistoStat(){
    m_data = NULL;
}

bool setHistoStat::setParameters(QDomNode &config){
    return true;
}

bool setHistoStat::init(){
    std::fill_n(Hues, 36, 0);
    colorAccuracyRatio=0;
    loc=new QPoint(30,150);
    return true;}

bool setHistoStat::draw(QImage *image){



    std::map<QString,DeguModelDynamics>::iterator modeloMain;
    modeloMain= m_data->modelsToFollow.find("main");

    if(modeloMain!=m_data->modelsToFollow.end()){
        for (int i=0; i<36; i++){
            Hues[i]=modeloMain->second.Hues[i];

        }
        int Ps=modeloMain->second.pSize;
        if(Ps!=0)
            ratio=modeloMain->second.pDiscard/Ps;
        else
                ratio=-1;
        colorAccuracyRatio=modeloMain->second.colorAccuracy;
    }




    int max=1;
    int suma=0;
    for (int i=0; i<36; i++){
        if(Hues[i]>max)
            max=Hues[i];
        suma+=Hues[i];
    }


    float ratio=100/((float)max);


    QPainter painter;
    painter.begin(image);

    painter.setPen(QColor(0, 255, 0, 255));

    for (int i=0; i<36; i++){

        if(i<6){
           painter.setPen(QColor(255, 255*i/6, 0, 255));
        }else if(i>=6 && i <12){
            painter.setPen(QColor(255*(12-i)/6, 255, 0, 255));
        }else if(i>=12 && i <18){
            painter.setPen(QColor(0, 255, 255*(i-12)/6, 255));
        }else if(i>=18 && i <24){
            painter.setPen(QColor(0, 255*(24-i)/6, 255, 255));
        }else if(i>=24 && i <30){
            painter.setPen(QColor(255*(i-24)/6, 0, 255, 255));
        }else {
            painter.setPen(QColor(255, 0, 255*(36-i)/6, 255));
        }

        painter.drawRect ( 20+i*10, 120-ratio*Hues[i], 10, ratio*Hues[i] );

    }

    QString valor1="0" ;
    painter.drawText(QPoint(18,135), valor1);

    valor1="60" ;
    painter.drawText(QPoint(78,135), valor1);

    valor1="120" ;
    painter.drawText(QPoint(138,135), valor1);

    valor1="180" ;
    painter.drawText(QPoint(198,135), valor1);

    valor1="240" ;
    painter.drawText(QPoint(258,135), valor1);

    valor1="300" ;
    painter.drawText(QPoint(318,135), valor1);

    valor1="360" ;
    painter.drawText(QPoint(378,135), valor1);

    if(suma!=0){
        valor1=QString::number(((float)max)/((float)suma)*100)+"%" ;
        painter.drawText(QPoint(8,15), valor1);
    }

    QString valor="Lost pixels ratio "+ QString::number(ratio*100)+"%" ;
    painter.drawText(*loc, valor);

    QString valor2="Color accertive "+ QString::number(colorAccuracyRatio)+"%" ;
    painter.drawText(QPoint(30,180), valor2);


    //---- hulls
    int numeroDeHulls = m_data->hulls.size();

    QString valor3="Numero de Hulls "+ QString::number(numeroDeHulls);
    painter.drawText(QPoint(30,200), valor3);


    if(!m_data->hulls.empty()) {
        std::deque<SpHullModel>::iterator end_it = m_data->hulls.end(), iter;
        SpHullModel hl;
        for(iter=m_data->hulls.begin();iter != end_it; iter++) {


            //Draw current 2D info
            hl = *iter;
            displayShowHull(painter, hl);
            displayShowAxis(painter, hl);
            displayShowNormals(painter, hl);
            displayBackBone(painter,hl);
        }
    }



    painter.end();



    return true;
}



void setHistoStat::displayShowHull(QPainter &painter, SpHullModel hull) {
    std::vector<cv::Point> &h = hull->local_hull;
    int i, n = h.size();
    if(n == 0)
        return;
    int X   = hull->off_x,
        Y   = hull->off_y;
    cv::Point p1, p2;

    painter.setPen(QColor(0, 255, 0, 255));//green

    p2 = h[n-1];

    for(i = 0; i < n; i++) {
        p1 = p2;
        p2 = h[i];
        painter.drawLine(X  + p1.x, Y + p1.y, X  + p2.x, Y + p2.y);
    }

}

void setHistoStat::displayShowAxis(QPainter &painter, SpHullModel hull) {
    cv::Point p1 = hull->axis1,
              p2 = hull->axis2;

    if(p1 == p2)
        return;

    painter.setPen(QColor(255, 0, 0, 255));//red

    painter.drawLine(hull->off_x + p1.x, hull->off_y + p1.y,
                     hull->off_x + p2.x, hull->off_y + p2.y);

}


void setHistoStat::displayShowNormals(QPainter &painter, SpHullModel hull) {
    std::vector< segment2D<float> > &segs = hull->segs;
    int i, n = segs.size();
    if(n == 0)
        return;
    int X   = hull->off_x,
        Y   = hull->off_y;

    painter.setPen(QColor(0, 0, 255, 255));//blue

    for(i = 0; i < n; i++) {
        segment2D<float> &s = segs[i];
        painter.drawLine(X  + s.first.x, Y + s.first.y, X  + s.last.x, Y + s.last.y);
    }

}

void setHistoStat::displayBackBone(QPainter &painter, SpHullModel hull) {
    std::vector< segment2D<float> > &segs = hull->segs;
    int i, n = segs.size();
    if(n == 0)
        return;
    int X   = hull->off_x,
        Y   = hull->off_y;

    painter.setPen(QColor(255, 0, 255, 255));//

    for(i = 0; i < n-1; i++) {
        segment2D<float> &s = segs[i];
        segment2D<float> &s2 = segs[i+1];
        double middleX1=(s.first.x+s.last.x)/2;
        double middleY1=(s.first.y+s.last.y)/2;
        double middleX2=(s2.first.x+s2.last.x)/2;
        double middleY2=(s2.first.y+s2.last.y)/2;

        painter.drawLine(X  + middleX1, Y + middleY1, X  + middleX2, Y + middleY2);
    }

}
