#include "HRdummy.h"

/**
 * @brief HRdummy::HRdummy
 */
HRdummy::HRdummy(Datapool *i_data) : ModuleInterface(i_data)
{
}

HRdummy::~HRdummy()
{
}

bool HRdummy::setParameters(QDomNode &config)
{
    return true;
}

bool HRdummy::init()
{
    return true;
}

bool HRdummy::run()
{
    return true;
}

bool HRdummy::updateParameters()
{
    return true;
}
