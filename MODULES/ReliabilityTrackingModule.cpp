#include "ReliabilityTrackingModule.h"
#include "image_display.h"

#include <errno.h>
#include <iostream>
#include <QMessageBox>
#include <QPainter>
#include <deque>
#include "src/object.h"
#include <QPointer>

ReliabilityTrackingModule::ReliabilityTrackingModule(Datapool *i_data): ModuleInterface(i_data) {
    m_ReliabilityTracker = new ReliabilityTracker(i_data);
}

ReliabilityTrackingModule::~ReliabilityTrackingModule() {
    delete m_ReliabilityTracker;
    //QSharedPointer
}

//Set module configuration parameters
bool ReliabilityTrackingModule::setParameters(QDomNode& config) {
  return m_ReliabilityTracker->setParameters(config);
}

bool ReliabilityTrackingModule::updateParameters(){
    return true;
}

//Function executed at each frame
bool ReliabilityTrackingModule::run() {
    if(m_ReliabilityTracker->initialPreparation)
        if(!m_ReliabilityTracker->initialPrepareRun())
            return false;

    if(!m_ReliabilityTracker->prepareRun())
        return false;
    m_ReliabilityTracker->run(m_data->blobs);

    //Set most likely mobiles
    m_ReliabilityTracker->getMostLikelyMobileObjects(m_data->objects);

  //  Blob b;
  //  Object o;
  //  int X1,Y1,X2,Y2,id;
  //  double X,Y,x,y,Vx,Vy;
  //  ObjectType type;
  //  std::map<int, Object>::iterator it, end_it;

  //                      b.Xu = Y1 < Y2 ? Y1 : Y2;
                        //Y upper is leftmost horizontal value
  //                      b.Yl = X1 < X2 ? X1 : X2;
                        //Height (in number of pixels):
  //                      b.H = (Y1 > Y2 ? Y1 : Y2) - b.Xu + 1;
                        //Height (in number of pixels):
  //                      b.W = (X1 > X2 ? X1 : X2) - b.Yl + 1;
  //                      Scene::getXY(m_pos,X,Y,b);
  //                      Scene::img_to_world_coords_given_height(m_data->sceneModel->p_matrix, X, Y, 0.0, &x, &y);

    return true;
}

bool ReliabilityTrackingModule::init() {
    return m_ReliabilityTracker->init();
}





