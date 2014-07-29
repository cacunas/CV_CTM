#include "drawInterface.h"

drawInterface::drawInterface(Datapool *i_data):m_data(i_data){}
drawInterface::~drawInterface() { }

QDomNode drawInterface::getParameterNode(QString pname, QDomElement& elem) {
    QDomNodeList n = elem.elementsByTagName(pname);
    return n.item(0);
}

QDomNode drawInterface::getParameterNode(QString pname, QDomNode& node) {
    if(node.isNull())
        return node;
    QDomNodeList n = node.toElement().elementsByTagName(pname);
    return n.item(0);
}

QString drawInterface::getParameterValue(QDomNode& node) {
    return node.toElement().attribute("value");
}

QString drawInterface::getParameterValue(QDomElement& elem) {
    return elem.attribute("value");
}

void drawInterface::copyBytes(QImage *im1, QImage *im2) {
    memcpy(im1->bits(),im2->bits(), im1->height()*im1->bytesPerLine());
}

void drawInterface::showPolygon3D(QPainter& painter, SceneModel *i_pSmNode, QSharedPointer < polygon3D<double> > poly, const QColor& color) {

    unsigned int i;
    point3D<double> *begin, *end;
    point2D<double> pointBegin, pointEnd;

    if(poly == NULL) return;

    for(i=0; i < POLYGON_N_POINTS(poly)-1; i++) {
        begin = POLYGON_NTH_POINT(poly,i);
        end = POLYGON_NTH_POINT(poly, i+1);

        //Get 2D coordinate from 3D coordinate
        get2Dfrom3D(i_pSmNode, begin, &pointBegin);
        get2Dfrom3D(i_pSmNode, end, &pointEnd);
        painter.setPen(color);
        paintLine(painter, &pointBegin, &pointEnd);
    }

    // the last line
    begin = POLYGON_NTH_POINT(poly,POLYGON_N_POINTS(poly)-1);
    end = POLYGON_NTH_POINT(poly,0);

    //Get 2D coordinate from 3D coordinate
    get2Dfrom3D(i_pSmNode, begin, &pointBegin);
    get2Dfrom3D(i_pSmNode, end, &pointEnd);
    painter.setPen(color);
    paintLine(painter, &pointBegin, &pointEnd);
}


void drawInterface::showImagePolygon(QPainter& painter, QSharedPointer < polygon2D<int> > poly, const QColor& color) {
  unsigned int i;
  point2D<int> *begin, *end;

  if(poly.isNull()) return;

  for(i=0; i < POLYGON_N_POINTS(poly)-1; i++) {
    begin = POLYGON_NTH_POINT(poly,i);
    end = POLYGON_NTH_POINT(poly, i+1);

    paintImageLine(painter, begin, end, color);
  }

  // the last line
  begin = POLYGON_NTH_POINT(poly,POLYGON_N_POINTS(poly)-1);
  end = POLYGON_NTH_POINT(poly,0);

  paintImageLine(painter, begin, end, color);

}

void drawInterface::paintImageLine(QPainter& painter, point2D<int> *pointBegin, point2D<int> *pointEnd, const QColor& color) {
    painter.setPen(color);
    painter.drawLine(POINT_2D_X(pointBegin), POINT_2D_Y(pointBegin),
                   POINT_2D_X(pointEnd), POINT_2D_Y(pointEnd));
}

int drawInterface::get2Dfrom3D(SceneModel *i_pSmNode, point3D<double> *point3D, point2D<double> *point2D) {
    return i_pSmNode->worldToImgCoords(SM_CALIB_MATRIX(i_pSmNode),
                                     POINT_3D_X(point3D), POINT_3D_Y(point3D), POINT_3D_Z(point3D),
                                     &POINT_2D_X(point2D), &POINT_2D_Y(point2D));
}

void drawInterface::paintLine(QPainter& painter, point2D<double> *pointBegin, point2D<double> *pointEnd) {
  painter.drawLine((int)nearbyint(POINT_2D_X(pointBegin)), (int)nearbyint(POINT_2D_Y(pointBegin)),
                   (int)nearbyint(POINT_2D_X(pointEnd)), (int)nearbyint(POINT_2D_Y(pointEnd)));

}


