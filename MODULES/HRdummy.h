#ifndef HRDUMMY_H
#define HRDUMMY_H

#include <iostream>
#include <vector>

#include "Datapool.h"
#include "ModuleInterface.h"

using namespace std;

class HRdummy : public ModuleInterface
{
public:
    HRdummy(Datapool* i_data);
    ~HRdummy();

    //Set module configuration parameters
    bool setParameters(QDomNode& config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool run();

    //update parameters at runtime.
    bool updateParameters();

};

#endif // HRDUMMY_H
