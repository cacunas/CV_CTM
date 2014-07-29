#ifndef SETLEARNING_H
#define SETLEARNING_H

#include "Datapool.h"
#include "drawInterface.h"
#include "src/blob.h"
#include "src/LearningContext.h"
#include "src/HierarchyTree.h"
#include "src/LearningConcept.h"
#include "src/LearningContextualisedObject.h"
#include <QPainter>

class setLearning: public drawInterface
{
public:
    setLearning(Datapool *i_data);
    ~setLearning();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    bool paint(QImage *image);

    void displayShowLearningHighlightP(QPainter &painter, int W, int H, miles::SpLearningContext lc);
    void displayShowLearningHighlightAttribute(QPainter &painter, int W, int H, miles::SpLearningContext lc);
    void displayShowLearningRecognised(QPainter &painter, int W, int H, miles::SpLearningContext lc);

    void getMostLikelyEventsFromStates(double threshold, std::map<long int, miles::SpLearningStateConcept>& conceptsToShow,
                                       std::map<long int, miles::SpLearningEventConcept>& eventsToShow,
                                       miles::SpLearningContext lc);
    void displayShowLearningEvents(QPainter &painter, int W, int H,
                                   std::map<long int, miles::SpLearningEventConcept>& eventsToShow,
                                   std::map<long int, miles::SpLearningStateConcept>& registeredStates);
    void getLikelyRecognisedStates(double P, std::map<long int, miles::SpLearningStateConcept> &states,
                                   miles::SpLearningContext lc);
    void recurseToLikelyEvents(bool isLikely, miles::SpLearningStateConcept state, double threshold,
                               std::map<long int, miles::SpLearningStateConcept>& conceptsToAdd,
                               std::map<long int, miles::SpLearningEventConcept>& eventsToShow,
                               double P, miles::SpLearningContext lc);
    void getEventsImage(QImage &im, std::map<long int, miles::SpLearningEventConcept>& eventsToShow,
                        std::map<long int, miles::SpLearningStateConcept>& registeredStates,
                        miles::attribute_id X, miles::attribute_id Y);
    void paintArrow(QImage &im, int x1, int y1, int x2, int y2, double P, double maxP);

    bool m_activeLearningDisplay;
    bool m_showP;
    bool m_showRecognised;
    bool m_display2D;
    bool m_display3D;
    bool m_displayH;
    bool m_showEvents;
    bool m_showFlatP;
    double m_PThreshold;
    double m_PEventThreshold;
    uchar m_learningTransparencyLevel;
    double m_learningTransparentProbability;
    miles::attribute_id m_highlightedAttribute;


    enum learningDisplayType {
          LD_HighlightAttribute,
          LD_Events,
    };
    enum learningDisplayType m_lDisplayType;
};

#endif // SETLEARNING_H
