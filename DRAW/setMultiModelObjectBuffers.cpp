#include "setMultiModelObjectBuffers.h"

setMultiModelObjectBuffers::setMultiModelObjectBuffers(Datapool *i_data): drawInterface(i_data){
    //image = NULL;
}


setMultiModelObjectBuffers::~setMultiModelObjectBuffers(){}

//Set module configuration parameters
bool setMultiModelObjectBuffers::setParameters(QDomNode &config){
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
bool setMultiModelObjectBuffers::init(){
    return true;
}

//Function executed at each frame
bool setMultiModelObjectBuffers::draw(QImage *image){
    if(!paint(image))
        return false;
    return true;
}

bool setMultiModelObjectBuffers::paint(QImage *image){

    QPainter painter;
    painter.begin(image);
    if(!m_data->RMMobjects.empty()) {
        std::deque<SpRMMMobileObject>::iterator end_it = m_data->RMMobjects.end(), iter;
        SpRMMMobileObject o;
        for(iter=m_data->RMMobjects.begin();iter != end_it; iter++) {
            //Draw current 2D info
            o = *iter;
            displayShowRMMMobileBuffer(painter, o);
        }
    }
    painter.end();
    return true;
}


void setMultiModelObjectBuffers::displayShowRMMMobileBuffer(QPainter &painter, std::deque<SpReliabilitySingleModelInterface> &models) {
    if(models.empty())
        return;
    int i, s = models.size();
    SpReliabilitySingleModelInterface current;
    for(i=0; i<s; i++) {
        current = models[i];
//TODO put some other draw methods to models
//        current->instances.draw(painter);

    }


}

void setMultiModelObjectBuffers::displayShowRMMMobileBuffer(QPainter &painter, SpRMMMobileObject mobileObject) {
    displayShowRMMMobileBuffer(painter, mobileObject->multiModel.multiModelDAG);
}

