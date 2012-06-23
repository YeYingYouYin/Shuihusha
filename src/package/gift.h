#ifndef GIFTPACKAGE_H
#define GIFTPACKAGE_H

#include "package.h"
#include "card.h"

class GiftPackage : public Package{
    Q_OBJECT

public:
    GiftPackage();
};

class XunlieCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE XunlieCard();
    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
    virtual void onEffect(const CardEffectStruct &effect) const;
};

class LianzhuCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE LianzhuCard();
    virtual void onUse(Room *room, const CardUseStruct &card_use) const;
};

class HuazhuCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE HuazhuCard();

    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
    virtual void use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const;
};

#endif // GIFTPACKAGE_H
