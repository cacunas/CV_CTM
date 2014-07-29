#include "set2DContext.h"

set2DContext::set2DContext(Datapool *i_data):drawInterface(i_data){}

set2DContext::~set2DContext(){
    m_data = NULL;
}

bool set2DContext::setParameters(QDomNode &config){
    return true;
}
bool set2DContext::init(){ return true;}

bool set2DContext::draw(QImage *image){
    if(!paint(image))
        return false;
    return true;
}

bool set2DContext::paint(QImage *image){

    if(image == NULL || image->isNull())
        return false;

    if (!m_data->sceneModel)
        return false;

    QPen pen;
    pen.setCosmetic(true);
    pen.setWidth(0);
    pen.setCapStyle(Qt::FlatCap);

    QPainter painter;
    painter.begin(image);
    painter.setPen(pen);

    QColor color(255,0,255);

    std::vector< QSharedPointer<world::Zone2D> > &zones = m_data->sceneModel->Zones2D;
    if (zones.empty())
        return true;
    std::vector< QSharedPointer<world::Zone2D> >::iterator zone_it, zone_end = zones.end();
    for(zone_it = zones.begin(); zone_it != zone_end; zone_it++ ) {
        (*zone_it)->draw(painter, color);
    }
    return true;
}
