#include "setMultiModelObjects.h"

setMultiModelObjects::setMultiModelObjects(Datapool *i_data): drawInterface(i_data){
    //image = NULL;
}


setMultiModelObjects::~setMultiModelObjects(){}

//Set module configuration parameters
bool setMultiModelObjects::setParameters(QDomNode &config){
    /*
    QDomNode n;
    if(config.isNull()) { //Parameter set for module not defined
        afterModule = '-';
    }
    else {
        if( ( n = getParameterNode("afterModule", config) ).isNull() )
            afterModule = '-';
        else
            afterModule = getParameterValue(n);
    }
    */
    return true;
}

//Initialization after reading parameters
bool setMultiModelObjects::init(){
    return true;
}

//Function executed at each frame
bool setMultiModelObjects::draw(QImage *image){
    if(!paint(image))
        return false;
    return true;
}

bool setMultiModelObjects::paint(QImage *image){

    QPainter painter;
    painter.begin(image);
    //std::cerr << "Err: Frame number: " << m_data->frameNumber << std::endl;

    if(!m_data->RMMobjects.empty()) {
        std::deque<SpRMMMobileObject>::iterator end_it = m_data->RMMobjects.end(), iter;
        SpRMMMobileObject o;
        for(iter=m_data->RMMobjects.begin();iter != end_it; iter++) {
            //Draw current 2D info
            o = *iter;
            displayShowRMMMobile(painter, o);
        }
    }
    painter.end();
    return true;
}


void setMultiModelObjects::displayShowRMMMobile(QPainter &painter, std::deque<SpReliabilitySingleModelInterface> &models) {
    if(models.empty())
        return;
    int i, s = models.size();
    SpReliabilitySingleModelInterface current;
    for(i=s-1; i>=0; i--) {
        current = models[i];
        current->draw(painter, 255, 255, 255);
    }


}

void setMultiModelObjects::displayShowRMMMobile(QPainter &painter, SpRMMMobileObject mobileObject) {
    //std::cerr << "Err: Mobile ID: " << mobileObject->mobile_id << std::endl;
    displayShowRMMMobile(painter, mobileObject->multiModel.multiModelDAG);
}

