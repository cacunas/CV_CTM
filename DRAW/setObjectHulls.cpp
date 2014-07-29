#include "setObjectHulls.h"

setObjectHulls::setObjectHulls(Datapool *i_data): drawInterface(i_data){
}


setObjectHulls::~setObjectHulls(){}

//Set module configuration parameters
bool setObjectHulls::setParameters(QDomNode &config){
    /*
    QDomNode n;
    if(config.isNull()) { //Parameter set for module not defined
    } else {
        if( ( n = getParameterNode("afterModule", config) ).isNull() )
            afterModule = '-';
        else
            afterModule = getParameterValue(n);
    }
    */
    return true;
}

//Initialization after reading parameters
bool setObjectHulls::init(){
    return true;
}

//Function executed at each frame
bool setObjectHulls::draw(QImage *image){
    if(!paint(image))
        return false;
    return true;
}

bool setObjectHulls::paint(QImage *image){

    QPainter painter;
    painter.begin(image);

    if(!m_data->hulls.empty()) {
        std::deque<SpHullModel>::iterator end_it = m_data->hulls.end(), iter;
        SpHullModel h;
        for(iter=m_data->hulls.begin();iter != end_it; iter++) {
            //Draw current 2D info
            h = *iter;
            displayShowHull(painter, h);
            displayShowAxis(painter, h);
            displayShowNormals(painter, h);
        }
    }
    painter.end();
    return true;
}

void setObjectHulls::displayShowHull(QPainter &painter, SpHullModel hull) {
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

void setObjectHulls::displayShowAxis(QPainter &painter, SpHullModel hull) {
    cv::Point p1 = hull->axis1,
              p2 = hull->axis2;

    if(p1 == p2)
        return;

    painter.setPen(QColor(255, 0, 0, 255));//red

    painter.drawLine(hull->off_x + p1.x, hull->off_y + p1.y,
                     hull->off_x + p2.x, hull->off_y + p2.y);

}


void setObjectHulls::displayShowNormals(QPainter &painter, SpHullModel hull) {
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

