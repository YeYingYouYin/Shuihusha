#ifndef JOYPACKAGE_H
#define JOYPACKAGE_H

#include "package.h"
#include "standard.h"
#include "engine.h"

class KusoPackage: public CardPackage{
    Q_OBJECT

public:
    KusoPackage();
};

class JoyPackage: public CardPackage{
    Q_OBJECT

public:
    JoyPackage();
};

class Stink: public BasicCard{
    Q_OBJECT

public:
    Q_INVOKABLE Stink(Card::Suit suit, int number);
    virtual QString getSubtype() const;
    virtual QString getEffectPath(bool is_male) const;
    virtual void use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const;
};

class Saru: public OffensiveHorse{
    Q_OBJECT

public:
    Q_INVOKABLE Saru(Card::Suit suit, int number);

    virtual void onInstall(ServerPlayer *player) const;
    virtual void onUninstall(ServerPlayer *player) const;
    virtual QString getEffectPath(bool is_male) const;

private:
    TriggerSkill *grab_peach;
};

class GaleShell:public Armor{
    Q_OBJECT

public:
    Q_INVOKABLE GaleShell(Card::Suit suit, int number);

    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
    virtual void onUse(Room *room, const CardUseStruct &card_use) const;
};

class Poison: public BasicCard{
    Q_OBJECT

public:
    Q_INVOKABLE Poison(Card::Suit suit, int number);
    virtual QString getSubtype() const;
    virtual QString getEffectPath(bool is_male) const;
    virtual void onEffect(const CardEffectStruct &effect) const;
    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
};

class JoyGeneralPackage: public GeneralPackage{
    Q_OBJECT

public:
    JoyGeneralPackage();
};

class ZhuangcheCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE ZhuangcheCard();

    virtual void use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const;
};

class ChuiniuCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE ChuiniuCard();

    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
    virtual void use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const;
};

#endif // JOYPACKAGE_H
