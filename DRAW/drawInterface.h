#ifndef DRAWINTERFACE_H
#define DRAWINTERFACE_H

#include <map>
#include <string>
#include <QImage>
#include <QPainter>
#include <QDir>
#include "string.h"
#include "Datapool.h"

#include "QtXml/QDomDocument"


typedef std::map<std::string, std::map<std::string, std::string> >* TParameterList;

/*
enum DrawType {
    SETBLOBS,
    OTHER
};
*/
class drawInterface
{
public:
    drawInterface(Datapool *i_data);
  virtual ~drawInterface() = 0;

  //Set module configuration parameters
  virtual bool setParameters(QDomNode &config) = 0;

  //Initialization after reading parameters
  virtual bool init() = 0;

  //Function executed at each frame
  virtual bool draw(QImage *image) = 0;

  static QDomNode getParameterNode(QString pname, QDomElement& elem);
  static QDomNode getParameterNode(QString pname, QDomNode& node);
  static QString getParameterValue(QDomNode& node);
  static QString getParameterValue(QDomElement& elem);
  static void copyBytes(QImage *im1, QImage *im2);
  static void showPolygon3D(QPainter& painter, SceneModel *i_pSmNode, QSharedPointer < polygon3D<double> > poly, const QColor& color);
  static void showImagePolygon(QPainter& painter, QSharedPointer < polygon2D<int> > poly, const QColor& color);
  static void paintImageLine(QPainter& painter, point2D<int> *pointBegin, point2D<int> *pointEnd, const QColor& color);
  static int get2Dfrom3D(SceneModel *i_pSmNode, point3D<double> *point3D, point2D<double> *point2D);
  static void paintLine(QPainter& painter, point2D<double> *pointBegin, point2D<double> *pointEnd);

  std::string name;
protected:
  //Reference to external datapool:
  Datapool *m_data;
};

#endif // DRAWINTERFACE_H
