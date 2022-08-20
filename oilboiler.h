#ifndef OILBOILER_H
#define OILBOILER_H

#include <QObject>

class OilBoiler : public QObject
{
    Q_OBJECT
public:
    explicit OilBoiler(QObject *parent = nullptr);

    int ActualPower() const;

    bool FlameStatus() const;

    float numberOfStarts() const;

private:
    float m_ActualPower;
    bool m_FlameStatus;
    float m_NumberOfStarts;


signals:

};

#endif // OILBOILER_H
