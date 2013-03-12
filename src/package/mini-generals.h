#ifndef MINIGENERALS_H
#define MINIGENERALS_H

#include "carditem.h"
#include "engine.h"
#include "client.h"

class FangdiaoCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE FangdiaoCard();
    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
    virtual void use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &list) const;
};

class YinlangCard:public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE YinlangCard();

    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
    virtual void use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const;
};

class BeishuiCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE BeishuiCard();

    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
    virtual void onUse(Room *room, const CardUseStruct &card_use) const;
};

class PushouCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE PushouCard();

    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
    virtual void onEffect(const CardEffectStruct &effect) const;
};

class Qi6ingCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE Qi6ingCard();

    virtual void onUse(Room *room, const CardUseStruct &card_use) const;
};

class XianhaiCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE XianhaiCard();
    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
    virtual void onUse(Room *room, const CardUseStruct &card_use) const;
};

#endif // MINIGENERALS_H
