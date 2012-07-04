#ifndef CHEMCARDSPACKAGE_H
#define CHEMCARDSPACKAGE_H

#include "package.h"
#include "standard.h"
#include "maneuvering.h"

class ChemCardsPackage: public Package{
    Q_OBJECT

public:
    ChemCardsPackage();
};

class Acid: public BasicCard{
    Q_OBJECT

public:
    Q_INVOKABLE Acid(Card::Suit suit, int number);
    virtual QString getSubtype() const;
    virtual void use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const;
    virtual void onEffect(const CardEffectStruct &effect) const;

    virtual bool targetsFeasible(const QList<const Player *> &targets, const Player *Self) const;
    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
    virtual bool isAvailable(const Player *player) const;

    static bool IsAvailable(const Player *player);
};

class Base: public BasicCard{
    Q_OBJECT

public:
    Q_INVOKABLE Base(Card::Suit suit, int number);
    virtual QString getSubtype() const;
    virtual void use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const;
    virtual void onEffect(const CardEffectStruct &effect) const;

    virtual bool targetsFeasible(const QList<const Player *> &targets, const Player *Self) const;
    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
    virtual bool isAvailable(const Player *player) const;

    static bool IsAvailable(const Player *player);
};

class Safflower: public Peach{
    Q_OBJECT

public:
    Q_INVOKABLE Safflower(Card::Suit suit, int number);
};

class PotassiumDichromate: public Analeptic{
    Q_OBJECT

public:
    Q_INVOKABLE PotassiumDichromate(Card::Suit suit, int number);
};

class AquaRegia: public Dismantlement{
    Q_OBJECT

public:
    Q_INVOKABLE AquaRegia(Card::Suit suit, int number);
};

class EDTA:public Nullification{
    Q_OBJECT

public:
    Q_INVOKABLE EDTA(Card::Suit suit, int number);

};

class RefluxCondenser:public Snatch{
    Q_OBJECT

public:
    Q_INVOKABLE RefluxCondenser(Card::Suit suit, int number);

};

class Diatomite:public SingleTargetTrick{
    Q_OBJECT

public:
    Q_INVOKABLE Diatomite(Card::Suit suit, int number);
    virtual void onEffect(const CardEffectStruct &effect) const;
};

class Acids:public AOE{
    Q_OBJECT

public:
    Q_INVOKABLE Acids(Card::Suit suit, int number);
    virtual void onEffect(const CardEffectStruct &effect) const;
};

class Bases:public AOE{
    Q_OBJECT

public:
    Q_INVOKABLE Bases(Card::Suit suit, int number);
    virtual void onEffect(const CardEffectStruct &effect) const;
};

class NegativeCatalyst:public Indulgence{
    Q_OBJECT

public:
    Q_INVOKABLE NegativeCatalyst(Card::Suit suit, int number);

};

#endif // CHEMCARDSPACKAGE_H
