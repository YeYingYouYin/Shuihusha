#ifndef PLOUGHPACKAGE_H
#define PLOUGHPACKAGE_H

#include "package.h"
#include "standard.h"

class Drivolt:public SingleTargetTrick{
    Q_OBJECT

public:
    Q_INVOKABLE Drivolt(Card::Suit suit, int number);

    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
    virtual void onEffect(const CardEffectStruct &effect) const;
};

class Wiretap: public SingleTargetTrick{
    Q_OBJECT

public:
    Q_INVOKABLE Wiretap(Card::Suit suit, int number);

    virtual bool targetFilter(const QList<const ClientPlayer *> &targets, const ClientPlayer *to_select) const;
    virtual void onEffect(const CardEffectStruct &effect) const;
};

class Assassinate: public SingleTargetTrick{
    Q_OBJECT

public:
    Q_INVOKABLE Assassinate(Card::Suit suit, int number);

    virtual bool targetFilter(const QList<const ClientPlayer *> &targets, const ClientPlayer *to_select) const;
    virtual void onEffect(const CardEffectStruct &effect) const;
};

class Provistore:public DelayedTrick{
    Q_OBJECT

public:
    Q_INVOKABLE Provistore(Card::Suit suit, int number);

    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
    virtual void takeEffect(ServerPlayer *target) const;
};

class Treasury: public Disaster{
    Q_OBJECT

public:
    Q_INVOKABLE Treasury(Card::Suit suit, int number);
    virtual void takeEffect(ServerPlayer *target) const;
};

class Tsunami: public Disaster{
    Q_OBJECT

public:
    Q_INVOKABLE Tsunami(Card::Suit suit, int number);
    virtual void takeEffect(ServerPlayer *target) const;
};

class DoubleWhip:public Weapon{
    Q_OBJECT

public:
    Q_INVOKABLE DoubleWhip(Card::Suit suit, int number);
};
/*
class XunzhiCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE XunzhiCard();

    virtual void use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const;
};

class YisheCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE YisheCard();

    virtual void use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const;
};

class YisheAskCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE YisheAskCard();

    virtual void use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const;
};

class TaichenCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE TaichenCard();

    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
    virtual void onEffect(const CardEffectStruct &effect) const;
};
*/
class PloughPackage: public Package{
    Q_OBJECT

public:
    PloughPackage();
};

#endif // PLOUGHPACKAGE_H
