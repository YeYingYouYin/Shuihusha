#ifndef ZCYNPACKAGE_H
#define ZCYNPACKAGE_H

#include "package.h"
#include "card.h"

class ZCYNPackage : public Package{
    Q_OBJECT

public:
    ZCYNPackage();
};

class SixiangCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE SixiangCard();

    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
    virtual bool targetsFeasible(const QList<const Player *> &targets, const Player *Self) const;
    virtual void onEffect(const CardEffectStruct &effect) const;
};

class JiemingCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE JiemingCard();

    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
    virtual void onEffect(const CardEffectStruct &effect) const;
};

#endif // ZCYNPACKAGE_H
