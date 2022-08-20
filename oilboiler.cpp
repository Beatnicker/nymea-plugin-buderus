#include "oilboiler.h"

OilBoiler::OilBoiler(QObject *parent)
    : QObject{parent}
{

}
/**
 * @brief OilBoiler::ActualPower
 * "id": "/heatSources/hs1/actualPower"
 * @return
 */
float OilBoiler::actualPower() const
{
    return m_actualPower;
}
/**
 * @brief OilBoiler::FlameStatus
 * "id": "/heatSources/hs1/flameStatus"
 * @return
 */
bool OilBoiler::flameStatus() const
{
    return m_flameStatus;
}
/**
 * @brief OilBoiler::NumberOfStarts
 * "id": "/heatSources/numberOfStarts"
 * @return
 */
float OilBoiler::numberOfStarts() const
{
    return m_numberOfStarts;
}

void OilBoiler::readValues() {



}
