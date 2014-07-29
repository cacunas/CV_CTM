#include "setLearning.h"
#include "src/MathFunctions.h"
#include "image_display.h"

setLearning::setLearning(Datapool *i_data): drawInterface(i_data){

    //Default of Learning Display Parameters
    m_activeLearningDisplay = false;
    m_display2D = true;
    m_showP = true;
    m_showEvents = false;
    m_showFlatP = false;
    m_showRecognised = true;
    m_highlightedAttribute = miles::LA_X;
    m_lDisplayType = LD_HighlightAttribute;
    m_PThreshold = 0.05;
}

setLearning::~setLearning(){}

bool setLearning::init(){
    if(!m_data->eventLearning.isNull() && m_data->eventLearning->validContexts.size() > 0){
        //Validate if the first learning context can be used with these parameters:
        m_activeLearningDisplay = false;
        bool x2D = false, y2D = false, x3D = false, y3D = false, param = false, xH = false, yH = false;
        std::map<miles::attribute_id, miles::SpLearningNumeric> & nparam = m_data->eventLearning->validContexts[0]->numericalAttributes;
        std::map<miles::attribute_id, miles::SpLearningSymbolic>& sparam = m_data->eventLearning->validContexts[0]->symbolicAttributes;

        if(nparam.size() > 0) {
            std::map<miles::attribute_id, miles::SpLearningNumeric>::iterator nparam_it, nparam_end = nparam.end();
            for(nparam_it = nparam.begin(); nparam_it != nparam_end; nparam_it++) {
                if(m_display2D && nparam_it->first == miles::LA_X)
                    x2D = true;
                if(m_display2D && nparam_it->first == miles::LA_Y)
                    y2D = true;
                if(m_displayH && nparam_it->first == miles::LA_HX)
                    xH = true;
                if(m_displayH && nparam_it->first == miles::LA_HY)
                    yH = true;
                if(!m_showP) {
                    if(nparam_it->first == m_highlightedAttribute)
                        param = true;
                }
            }
        }

        if(sparam.size() > 0 && !m_showP) {
            std::map<miles::attribute_id, miles::SpLearningSymbolic>::iterator sparam_it, sparam_end = sparam.end();
            for(sparam_it = sparam.begin(); sparam_it != sparam_end; sparam_it++) {
                if(sparam_it->first == m_highlightedAttribute)
                    param = true;
            }
        }

        if( (x2D && y2D) || (x3D && y3D) || (xH && yH)) { //If we have information from demanded position
            if(m_showP || param) //It will show P or have found the right parameter
                m_activeLearningDisplay = true;
        }
    }

    return true;
}

bool setLearning::setParameters(QDomNode &config){
    QDomElement j = config.toElement();
    QString q_config_file("view_config.xml");
    if(config.isNull()){
        //parametros default
        AppendToLog("setLearning: Warning: Taking defaults...");
        m_display2D = true;
        m_display3D = false;
        m_displayH = false;
        m_showP = true;
        m_highlightedAttribute = miles::LA_X;
        m_lDisplayType = LD_HighlightAttribute;
        m_PThreshold = 0.15;
        m_PEventThreshold = 0.05;
        m_learningTransparencyLevel = 128;
        m_learningTransparentProbability = 0.1;
        m_showEvents = true;
        m_showFlatP = false;
        m_showRecognised = true;
    }
    //if(!m_data->eventLearning.isNull() && m_data->eventLearning->validContexts.size() > 0){
    else{
        if(!j.isNull()){
            //1. Process parameters
            QDomNode n, m, o;
            if(    ( n = XmlCommon::getParameterNode("LearningDisplay", j) ).isNull()
                || XmlCommon::getParameterValue(n) != "yes" ) {
                m_display2D = true;
                m_display3D = false;
                m_displayH = false;
                m_showP = true;
                m_highlightedAttribute = miles::LA_X;
                m_lDisplayType = LD_HighlightAttribute;
                m_PThreshold = 0.15;
                m_PEventThreshold = 0.05;
                m_learningTransparencyLevel = 128;
                m_learningTransparentProbability = 0.1;
                m_showEvents = true;
                m_showFlatP = false;
                m_showRecognised = true;
            }
            else {
                if( ( m = XmlCommon::getParameterNode("UsedPosition", n) ).isNull() ) {
                    m_display2D = true;
                    m_display3D = false;
                    m_displayH = false;
                }
                else {
                    m_display2D = (XmlCommon::getParameterValue(m) == "2D") ? true : false;
                    m_display3D = (XmlCommon::getParameterValue(m) == "3D") ? true : false;
                    m_displayH = (XmlCommon::getParameterValue(m) == "H") ? true : false;
                }
                if( ( m = XmlCommon::getParameterNode("Type", n) ).isNull() ) {
                    m_lDisplayType = LD_HighlightAttribute;
                }
                else {
                    QString aux = XmlCommon::getParameterValue(m);
                    if(aux == "HighlightAttribute")
                        m_lDisplayType = LD_HighlightAttribute;
                    else if(aux == "Events")
                        m_lDisplayType = LD_Events;
                    else
                        m_lDisplayType = LD_HighlightAttribute;
                }

                if( ( m = XmlCommon::getParameterNode("HighlightedAttribute", n) ).isNull() ) {
                    m_showP = true;
                    m_highlightedAttribute = miles::LA_X;
                }
                else {
                    QString aux = XmlCommon::getParameterValue(m);
                    if(aux == "P") {
                        m_showP = true;
                        m_highlightedAttribute = miles::LA_X;
                    }
                    else {
                        if(m_data->availableAttributes->idFromName.count(aux.toStdString()) > 0) {
                            m_showP = false;
                            m_highlightedAttribute = m_data->availableAttributes->idFromName[aux.toStdString()];
                        }
                        else {
                            m_showP = true;
                            m_highlightedAttribute = miles::LA_X;
                        }
                    }
                }
                if( ( m = XmlCommon::getParameterNode("PStateThreshold", n) ).isNull() ) {
                    m_PThreshold = 0.15;
                }
                else {
                    m_PThreshold = XmlCommon::getParameterValue(m).toDouble();
                }

                if( ( m = XmlCommon::getParameterNode("PEventThreshold", n) ).isNull() ) {
                    m_PEventThreshold = 0.05;
                }
                else {
                    m_PEventThreshold = XmlCommon::getParameterValue(m).toDouble();
                }

                if( ( m = XmlCommon::getParameterNode("TransparencyLevel", n) ).isNull() ) {
                    m_learningTransparencyLevel = 128;
                }
                else {
                    int aux = XmlCommon::getParameterValue(m).toInt();
                    if(aux < 0 || aux > 255) {
                        AppendToLog("Error setting parameter TransparencyLevel in configuration file '" + q_config_file + "'. Value must be in [0;255]. Taking default: 128.");
                        m_learningTransparencyLevel = 128;
                    }
                    else
                        m_learningTransparencyLevel = (uchar)aux;
                }

                if( ( m = XmlCommon::getParameterNode("TransparentProbability", n) ).isNull() ) {
                    m_learningTransparentProbability = 0.1;
                }
                else {
                    m_learningTransparentProbability = XmlCommon::getParameterValue(m).toDouble();
                    if(m_learningTransparentProbability < 0 || m_learningTransparentProbability > 1) {
                        AppendToLog("Error setting parameter TransparentProbability in configuration file '" + q_config_file + "'. Value must be in [0;1]. Taking default: 0.1.");
                        m_learningTransparentProbability = 0.1;
                    }
                }

                if( ( m = XmlCommon::getParameterNode("ShowEvents", n) ).isNull() ) {
                    m_showEvents = false;
                }
                else {
                    m_showEvents = (XmlCommon::getParameterValue(m) == "yes") ? true : false;
                }

                if( ( m = XmlCommon::getParameterNode("PFlat", n) ).isNull() ) {
                    m_showFlatP = false;
                }
                else {
                    m_showFlatP = (XmlCommon::getParameterValue(m) == "yes") ? true : false;
                }

                if( ( m = XmlCommon::getParameterNode("ShowRecognised", n) ).isNull() ) {
                    m_showRecognised = true;
                }
                else {
                    m_showRecognised = (XmlCommon::getParameterValue(m) == "yes") ? true : false;
                }
            }

            /*
            //Validate if the first learning context can be used with these parameters:
            m_activeLearningDisplay = false;
            bool x2D = false, y2D = false, x3D = false, y3D = false, param = false;
            std::map<miles::attribute_id, miles::SpLearningNumeric> & nparam = m_data->eventLearning->validContexts[0]->numericalAttributes;
            std::map<miles::attribute_id, miles::SpLearningSymbolic>& sparam = m_data->eventLearning->validContexts[0]->symbolicAttributes;

            if(nparam.size() > 0) {
                std::map<miles::attribute_id, miles::SpLearningNumeric>::iterator nparam_it, nparam_end = nparam.end();
                for(nparam_it = nparam.begin(); nparam_it != nparam_end; nparam_it++) {
                    if(m_display2D && nparam_it->first == miles::LA_X)
                        x2D = true;
                    if(m_display2D && nparam_it->first == miles::LA_Y)
                        y2D = true;
                    if(!m_showP) {
                        if(nparam_it->first == m_highlightedAttribute)
                            param = true;
                    }
                }
            }

            if(sparam.size() > 0 && !m_showP) {
                std::map<miles::attribute_id, miles::SpLearningSymbolic>::iterator sparam_it, sparam_end = sparam.end();
                for(sparam_it = sparam.begin(); sparam_it != sparam_end; sparam_it++) {
                    if(sparam_it->first == m_highlightedAttribute)
                        param = true;
                }
            }

            if( (x2D && y2D) || (x3D && y3D)) { //If we have information from demanded position
                if(m_showP || param) //It will show P or have found the right parameter
                    m_activeLearningDisplay = true;
            }

            //3. Clean xml variable
            delete xmlDisplayConfig;
            xmlDisplayConfig = NULL;
            */
        }
        else{
            AppendToLog("setLearning: Warning: Configuration '" + m_data->m_config_file + "' file does not define an entry BlobFilteringModule. Taking defaults...");
            m_display2D = true;
            m_display3D = false;
            m_displayH = false;
            m_showP = true;
            m_highlightedAttribute = miles::LA_X;
            m_lDisplayType = LD_HighlightAttribute;
            m_PThreshold = 0.15;
            m_PEventThreshold = 0.05;
            m_learningTransparencyLevel = 128;
            m_learningTransparentProbability = 0.1;
            m_showEvents = true;
            m_showFlatP = false;
            m_showRecognised = true;
        }
    }
    return true;
}

bool setLearning::draw(QImage *image){
    if(!paint(image))
        return false;
    return true;
}

bool setLearning::paint(QImage *image){
        QPainter painter;
        painter.begin(image);

        if(!m_data->eventLearning.isNull() && m_activeLearningDisplay) { //Learning module is executing
            if(m_lDisplayType == LD_HighlightAttribute) {
                if(m_showP)
                    displayShowLearningHighlightP(painter, m_data->currentImage->width(), m_data->currentImage->height(), m_data->eventLearning->validContexts[0]);
                else //Show attribute
                    displayShowLearningHighlightAttribute(painter, m_data->currentImage->width(), m_data->currentImage->height(), m_data->eventLearning->validContexts[0]);
                if(m_showRecognised)
                    displayShowLearningRecognised(painter, m_data->currentImage->width(), m_data->currentImage->height(), m_data->eventLearning->validContexts[0]);
            }
        }
        painter.end();
        return true;
}

void setLearning::displayShowLearningHighlightP(QPainter &painter, int W, int H, miles::SpLearningContext lc) {
    miles::SpHierarchyTree eventsHierarchy = lc->eventsHierarchy;
    std::map<long int, miles::SpLearningStateConcept> conceptsToShow;
    miles::attribute_id X, Y;
    if(m_display2D) {
        X = miles::LA_X;
        Y = miles::LA_Y;
    } else if(m_displayH) {
        X = miles::LA_HX;
        Y = miles::LA_HY;
    } else { //Change when adding 3D positions
        X = miles::LA_X;
        Y = miles::LA_Y;
    }
    //eventsHierarchy->getMostLikelySiblingStates(m_PThreshold, conceptsToShow);
    eventsHierarchy->getDeepestSiblingStates(m_PThreshold, conceptsToShow, X, Y);

    if(conceptsToShow.size() == 0)
        return;

    std::map<long int, miles::SpLearningEventConcept> eventsToShow;
    if(m_showEvents) { //Calculate events to show, and add involved states if not present
        getMostLikelyEventsFromStates(m_PEventThreshold, conceptsToShow, eventsToShow, lc);
    }

    //ALTERNATIVE SIZE WHEN ADDING 3D FEATURES
    //int w = image->width(), h = image->height(), wh = w*h;
    int w = W, h = H, wh = w*h;
    double *Ps = new double[wh];
    memset(Ps, 0, wh*sizeof(double));

    std::map<long int, miles::SpLearningStateConcept>::iterator it, it_end = conceptsToShow.end();
    double maxP = 0, P, Px, Py;
    //Get max P for normalization
    for(it = conceptsToShow.begin(); it != it_end; it++) {
        Px = MathFunctions::ONE_DIV_SQRT_2PI/(*it).second->numericalAttributeList[X]->getSigma();
        Py = MathFunctions::ONE_DIV_SQRT_2PI/(*it).second->numericalAttributeList[Y]->getSigma();
        P = (*it).second->getP()*Px*Py;
        if(P > maxP)
            maxP = P;
    }

    if(m_showFlatP)
        for(it = conceptsToShow.begin(); it != it_end; it++)
            (*it).second->setFlatConceptToArray(Ps, w, h, X, Y, maxP, m_learningTransparentProbability);
    else
        for(it = conceptsToShow.begin(); it != it_end; it++)
            (*it).second->setConceptToArray(Ps, w, h, X, Y, maxP, m_learningTransparentProbability);

    QImage im(w, h, QImage::Format_ARGB32);
    int i, j, bpl = im.bytesPerLine();
    uchar *pixels = im.bits(), *pix;
    memset(pixels, 0, h*bpl*sizeof(uchar));
    uchar val;
    int ival;
    QRgb *rgb;
    //Formato ARGB
    //b == bits[i]    g == bits[i+1]    r == bits[i+2]    alpha == bits[i+3]
    for(i=0; i<h; i++) {
        for(j=0; j<w; j++) {
            ival = round(Ps[i*w + j]);
            val = (uchar)(ival > 255 ? 255 : (ival < 0 ? 0 : ival));
            if(val > 0) {
                pix = pixels + i*bpl + 4*j;
                rgb = &(*MainWindow::thermalScaleTable)[(uchar)val];
                pix[0] = (uchar)qBlue(*rgb); //blue
                pix[2] = (uchar)qRed(*rgb); //red
                pix[3] = m_learningTransparencyLevel; //alpha
            }
        }
    }
    painter.drawImage(0,0,im);

    if(m_showEvents) //Paint events to show
        displayShowLearningEvents(painter, w, h, eventsToShow, eventsHierarchy->registeredStates);

    delete[] Ps;
}

void setLearning::displayShowLearningHighlightAttribute(QPainter &painter, int W, int H, miles::SpLearningContext lc) {
    miles::SpHierarchyTree eventsHierarchy = lc->eventsHierarchy;
    std::map<long int, miles::SpLearningStateConcept> conceptsToShow;
    miles::attribute_id X, Y;
    if(m_display2D) {
        X = miles::LA_X;
        Y = miles::LA_Y;
    } else if(m_displayH) {
        X = miles::LA_HX;
        Y = miles::LA_HY;
    }  else { //Change when adding 3D positions
        X = miles::LA_X;
        Y = miles::LA_Y;
    }

    //eventsHierarchy->getMostLikelySiblingStates(m_PThreshold, conceptsToShow);
    eventsHierarchy->getDeepestSiblingStates(m_PThreshold, conceptsToShow, X, Y);
    //eventsHierarchy->getMostLikelySiblingStates(m_PThreshold, conceptsToShow);

    if(conceptsToShow.size() == 0)
        return;

    std::map<long int, miles::SpLearningEventConcept> eventsToShow;
    if(m_showEvents) { //Calculate events to show, and add involved states if not present
        getMostLikelyEventsFromStates(m_PEventThreshold, conceptsToShow, eventsToShow, lc);
    }

    //ALTERNATIVE SIZE WHEN ADDING 3D FEATURES
    //int w = image->width(), h = image->height(), wh = w*h;
    int w = W, h = H, wh=w*h;

    double *As = new double[wh];
    memset(As, 0, wh*sizeof(double));

    std::map<long int, miles::SpLearningStateConcept>::iterator it, it_end = conceptsToShow.end();
    double maxA = 0, A;
    //Get max Attribute for normalization
    for(it = conceptsToShow.begin(); it != it_end; it++) {
        A = (*it).second->numericalAttributeList[m_highlightedAttribute]->getMean();
        if(A > maxA)
            maxA = A;
    }

    for(it = conceptsToShow.begin(); it != it_end; it++)
        (*it).second->setAttributeConceptToArray(As, w, h, X, Y, m_highlightedAttribute, maxA, m_learningTransparentProbability);

    QImage im(w, h, QImage::Format_ARGB32);
    int i, j, bpl = im.bytesPerLine();
    uchar *pixels = im.bits(), *pix;
    memset(pixels, 0, h*bpl*sizeof(uchar));
    uchar val;
    int ival, r, g;
    //Formato ARGB
    //b == bits[i]    g == bits[i+1]    r == bits[i+2]    alpha == bits[i+3]
    for(i=0; i<h; i++) {
        for(j=0; j<w; j++) {
            ival = round(As[i*w + j]);
            val = (uchar)(ival > 255 ? 255 : (ival < 0 ? 0 : ival));
            if(val > 0) {
                pix = pixels + i*bpl + 4*j;
                int r = (val<=128)? 0   :  (int) (255*(val - 128)/127.0);
                int g = (val>=128)? 255 :  (int) ((255*val)/128.0);

                pix[1] = (uchar)g; //green
                pix[2] = (uchar)r; //red
                pix[3] = m_learningTransparencyLevel; //alpha
            }
        }
    }
    painter.drawImage(0,0,im);

    if(m_showEvents) //Paint events to show
        displayShowLearningEvents(painter, w, h, eventsToShow, eventsHierarchy->registeredStates);

    delete[] As;

}

void setLearning::displayShowLearningRecognised(QPainter &painter, int W, int H, miles::SpLearningContext lc) {
    miles::SpHierarchyTree eventsHierarchy = lc->eventsHierarchy;
    std::map<long int, miles::SpLearningStateConcept> conceptsToShow;

    getLikelyRecognisedStates(m_PThreshold, conceptsToShow, lc);

    if(conceptsToShow.size() == 0)
        return;

    std::map<long int, miles::SpLearningEventConcept> eventsToShow;

    //getLikelyRecognisedEvents(m_PEventThreshold, conceptsToShow, eventsToShow, lc);

    //ALTERNATIVE SIZE WHEN ADDING 3D FEATURES
    //int w = image->width(), h = image->height(), wh = w*h;
    int w = W, h = H, wh = w*h;

    double *Ps = new double[wh];
    memset(Ps, 0, wh*sizeof(double));

    std::map<long int, miles::SpLearningStateConcept>::iterator it, it_end = conceptsToShow.end();
    double maxP = 0, P;
    miles::attribute_id X, Y;
    if(m_display2D) {
        X = miles::LA_X;
        Y = miles::LA_Y;
    } else if(m_displayH) {
        X = miles::LA_HX;
        Y = miles::LA_HY;
    }  else { //Change when adding 3D positions
        X = miles::LA_X;
        Y = miles::LA_Y;
    }
    //Get max P for normalization
    for(it = conceptsToShow.begin(); it != it_end; it++) {
        P = (*it).second->getPositionP(X, Y);
        if(P > maxP)
            maxP = P;
    }

    for(it = conceptsToShow.begin(); it != it_end; it++)
        (*it).second->setConceptBorderToArray(Ps, w, h, X, Y, maxP, m_learningTransparentProbability);

    QImage im(w, h, QImage::Format_ARGB32);
    int i, j, bpl = im.bytesPerLine();
    uchar *pixels = im.bits(), *pix;
    memset(pixels, 0, h*bpl*sizeof(uchar));
    uchar val;
    int ival;
    double m =127 / 255.0;
    int gs;
    uchar ugs;
    //Formato ARGB
    //b == bits[i]    g == bits[i+1]    r == bits[i+2]    alpha == bits[i+3]
    for(i=0; i<h; i++) {
        for(j=0; j<w; j++) {
            ival = round(Ps[i*w + j]);
            val = (uchar)(ival > 255 ? 255 : (ival < 0 ? 0 : ival));
            if(val > 0) {
                pix = pixels + i*bpl + 4*j;
                gs = round(ival*m + 128);
                if (gs<0) gs = 0;
                else if(gs>255) gs= 255;
                ugs = (uchar)gs;
                pix[0] = pix[1] = pix[2] = ugs;
                pix[3] = m_learningTransparencyLevel; //alpha
            }
        }
    }
    painter.drawImage(0,0,im);

    //displayShowRecognisedEvents(painter, w, h, eventsToShow, eventsHierarchy->registeredStates);

    delete[] Ps;
}

void setLearning::getMostLikelyEventsFromStates(double threshold,
                                                  std::map<long int, miles::SpLearningStateConcept>& conceptsToShow,
                                                  std::map<long int, miles::SpLearningEventConcept>& eventsToShow,
                                                  miles::SpLearningContext lc) {
    if(conceptsToShow.empty()) {
        return;
    }

    miles::SpHierarchyTree eventsHierarchy = lc->eventsHierarchy;

    std::map<long int, miles::SpLearningStateConcept> conceptsToAdd;
    std::map<long int, miles::SpLearningStateConcept>::iterator it, it_end = conceptsToShow.end();

    //Extract likely events and states not already included
    for(it=conceptsToShow.begin(); it != it_end; it++)
        recurseToLikelyEvents(true, (*it).second, threshold, conceptsToAdd, eventsToShow, (*it).second->getP(), lc);

    //Add new concepts from events
    it_end = conceptsToAdd.end();
    for(it=conceptsToAdd.begin(); it != it_end; it++) {
        if(conceptsToShow.count((*it).first) == 0)
            conceptsToShow[(*it).first] = (*it).second;
    }

}

void setLearning::displayShowLearningEvents(QPainter &painter, int W, int H,
                                              std::map<long int, miles::SpLearningEventConcept>& eventsToShow,
                                              std::map<long int, miles::SpLearningStateConcept>& registeredStates) {

    if(eventsToShow.size() == 0)
        return;

    std::map<long int, miles::SpLearningEventConcept>::iterator it, it_end = eventsToShow.end();
    double maxP = 0, P;

    miles::attribute_id X, Y;
    if(m_display2D) {
        X = miles::LA_X;
        Y = miles::LA_Y;
    } else if(m_displayH) {
        X = miles::LA_HX;
        Y = miles::LA_HY;
    }  else { //Change when adding 3D positions
        X = miles::LA_X;
        Y = miles::LA_Y;
    }

    QImage im(W, H, QImage::Format_ARGB32);
    int i, j, bpl = im.bytesPerLine();
    uchar *pixels = im.bits(), *pix;
    memset(pixels, 0, H*bpl*sizeof(uchar));
    uchar val;
    int ival;
    QRgb *rgb;

    getEventsImage(im, eventsToShow, registeredStates, X, Y);

    painter.drawImage(0,0,im);

}

void setLearning::getLikelyRecognisedStates(double P, std::map<long int, miles::SpLearningStateConcept> &states,
                                              miles::SpLearningContext lc) {
    std::map<unsigned long, std::map<int, miles::SpLevelData> > &levelData = m_data->levelData;

    if(levelData.empty())
        return;

    std::map<unsigned long, std::map<int, miles::SpLevelData> >::iterator obj_it, obj_it_end = levelData.end();
    std::map<int, miles::SpLevelData>::iterator it_lev, it_lev_end;
    miles::SpLearningStateConcept current;
    std::map<long int, miles::SpLearningStateConcept> &regStates = lc->eventsHierarchy->registeredStates;
    miles::SpLearningContextualisedObject currentObject;

    for(obj_it = levelData.begin(); obj_it != obj_it_end; obj_it++) {
        if(m_data->eventLearning->followedObjects.count((*obj_it).first)==0)
            continue;
        currentObject = m_data->eventLearning->followedObjects[(*obj_it).first];
        if(currentObject->lastUpdatedFrame != m_data->frameNumber)
            continue;
        std::map<int, miles::SpLevelData> &levels = (*obj_it).second;
        if(levels.size() > 1) {
            it_lev = levels.begin();
            it_lev++;
            if(regStates.count(it_lev->second->currentState) == 0)
                continue;
            current = regStates[it_lev->second->currentState];
            if(current->P < P) {
                states[current->id] = current;
            } else {
                miles::SpLearningStateConcept next;
                long int id;
                it_lev_end = levels.end();
                for(it_lev++; it_lev != it_lev_end; it_lev++) {
                    id = it_lev->second->currentState;
                    next = regStates[id];
                    if(next->P >= P)
                        current = next;
                    else
                        break;
                }
                states[current->id] = current;
            }
        } else if(levels.size() == 1) {
            it_lev = levels.begin();
            current = regStates[it_lev->second->currentState];
            states[current->id] = current;
        }
    }
}

void setLearning::recurseToLikelyEvents(bool isLikely, miles::SpLearningStateConcept state, double threshold,
                                          std::map<long int, miles::SpLearningStateConcept>& conceptsToAdd,
                                          std::map<long int, miles::SpLearningEventConcept>& eventsToShow,
                                          double P, miles::SpLearningContext lc) {
    //Add to states and events if not included
    if(conceptsToAdd.count(state->id) == 0) {

        conceptsToAdd[state->id] = state;

        //If there is, at least one event
        int n;
        if( (n = state->outgoingEvents.size()) > 0) {
            int i, j, num_added = 0;
            long int parent;
            bool eventsUnuseful[n];
            memset(eventsUnuseful, false, n*sizeof(bool));
            miles::SpLearningEventConcept most;
            std::map<long int, miles::SpLearningStateConcept>& States = lc->eventsHierarchy->registeredStates;
            double most_P = -1;
            std::map <long int, miles::SpLearningEventConcept>::iterator it, it2, it_end = state->outgoingEvents.end();
            miles::SpLearningEventConcept event;
            miles::SpLearningStateConcept pState;
            for(i=0, it = state->outgoingEvents.begin(); it != it_end; i++, it++) {
                if(eventsUnuseful[i] == false) {
                    event = (*it).second;
                    if(P * event->P > threshold) { //Still significative according to threshold
                        //We accept an event, then arriving states to higher levels are discarded
                        for(pState = States[event->arrivingState],parent=pState->parent; parent != -1;) {
                            for(j=0, it2 = state->outgoingEvents.begin(); it2 != it_end; j++, it2++) {
                                    if((*it2).second->arrivingState == parent) {
                                        eventsUnuseful[j] = true;
                                        break;
                                    }
                            }
                            pState = States[parent];
                            parent = pState->parent;
                        }
                    } else
                        eventsUnuseful[i] = true;

                }
            }

            for(i=0, it = state->outgoingEvents.begin(); it != it_end; i++, it++) {
                if(eventsUnuseful[i] == false) {
                    event = (*it).second;
                    if(eventsToShow.count(event->id) == 0) {
                        num_added++;
                        eventsToShow[event->id] = event;
                        recurseToLikelyEvents(false, States[event->arrivingState],
                                              threshold, conceptsToAdd, eventsToShow, P*event->P, lc);
                    }
                } else if(isLikely && num_added == 0 && event->P > most_P) {
                    most_P = event->P;
                    most = event;
                }
            }

            //For likely events
            if(isLikely && num_added == 0) {
                eventsToShow[most->id] = most;
                if(conceptsToAdd.count(most->arrivingState) == 0)
                    conceptsToAdd[most->arrivingState] = States[most->arrivingState];
            }
        }
    }
}

void setLearning::getEventsImage(QImage &im,
                    std::map<long int, miles::SpLearningEventConcept>& eventsToShow,
                    std::map<long int, miles::SpLearningStateConcept>& registeredStates,
                    miles::attribute_id X, miles::attribute_id Y) {
    //Clean memory
    miles::SpLearningStateConcept s1, s2;
    miles::SpLearningEventConcept e;
    int x1, y1, x2, y2;
    double P, maxP = -1;
    std::map<long int, miles::SpLearningEventConcept>::iterator it, it_end = eventsToShow.end();

    //Get max P for normalization
    for(it = eventsToShow.begin(); it != it_end; it++) {
        P = (*it).second->P;
        if(P > maxP)
            maxP = P;
    }

    for(it = eventsToShow.begin(); it != it_end; it++) {
        e = (*it).second;
        s1 = registeredStates[e->startingState];
        x1 = (int)round(s1->numericalAttributeList[X]->getMean());
        y1 = (int)round(s1->numericalAttributeList[Y]->getMean());
        s2 = registeredStates[e->arrivingState];
        x2 = (int)round(s2->numericalAttributeList[X]->getMean());
        y2 = (int)round(s2->numericalAttributeList[Y]->getMean());
        paintArrow(im, x1, y1, x2, y2, e->P, maxP);
    }

}

void setLearning::paintArrow(QImage &im, int x1, int y1, int x2, int y2, double P, double maxP) {
    double p = P/maxP;
    int alevel = (int)(5*p);

    QPainter painter;
    painter.begin(&im);

    int r = (p<=0.5)? 0   :  (int) (510*p - 255);
    int g = (p>=0.5)? 255 :  (int) (510*p);
    QColor color(r,g,0,m_learningTransparencyLevel);
    painter.setPen(color);
    painter.setBrush(color);
    int K = alevel;
    QPolygon pol;
    if(y1 == y2)
        pol << QPoint(x1,y1) << QPoint(x2,y2+K) << QPoint(x2,y2-K);
    else if(x1 == x2)
        pol << QPoint(x1,y1) << QPoint(x2+K,y2) << QPoint(x2-K,y2);
    else {
        double m = (y2-y1)/((double)(x2-x1)), J = m*K/sqrt(m*m + 1), b = y2 + x2/m;
        pol << QPoint(x1,y1) << QPoint(x2+J,b - (x2+J)/m) << QPoint(x2-J,b - (x2-J)/m);
    }
    painter.drawConvexPolygon(pol);
    //painter.drawLine(x1,y1,x2,y2);

    painter.end();
/*    switch(alevel) {
        case 0:


        break;
        default:
    }*/

}


