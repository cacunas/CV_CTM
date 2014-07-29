#include "setHomographyContext.h"

setHomographyContext::setHomographyContext(Datapool *i_data):drawInterface(i_data){}

setHomographyContext::~setHomographyContext(){
    m_data = NULL;
}

bool setHomographyContext::setParameters(QDomNode &config){
    return true;
}
bool setHomographyContext::init(){ return true;}

bool setHomographyContext::draw(QImage *image){
    if(!paint(image))
        return false;
    return true;
}

bool setHomographyContext::paint(QImage *image){

    if(image == NULL || image->isNull())
        return false;

    if (!m_data->sceneModel || !m_data->sceneModel->hmatrix_filled)
        return false;

    QPen pen;
    pen.setCosmetic(true);
    pen.setWidth(0);
    pen.setCapStyle(Qt::FlatCap);

    QPainter painter;
    painter.begin(image);
    painter.setPen(pen);

    QColor color(255,255,0);

    std::vector< QSharedPointer<world::ZoneH> > &zones = m_data->sceneModel->ZonesH;
    if (zones.empty())
        return true;
    QTransform &t = m_data->sceneModel->h_trans;
    std::vector< QSharedPointer<world::ZoneH> >::iterator zone_it, zone_end = zones.end();
    for(zone_it = zones.begin(); zone_it != zone_end; zone_it++ ) {
        (*zone_it)->draw(painter, color, t);
    }
    return true;
}


