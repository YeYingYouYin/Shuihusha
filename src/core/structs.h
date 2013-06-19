#ifndef STRUCTS_H
#define STRUCTS_H

class Room;
class TriggerSkill;
class ServerPlayer;
class Card;
class Slash;
class GameRule;

#include "player.h"

#include <QVariant>
#include <json/json.h>

struct DamageStruct{
    DamageStruct();

    enum Nature{
        Normal, // normal slash, duel and most damage caused by skill
        Fire,  // fire slash, fire attack and few damage skill (Yeyan, etc)
        Thunder // lightning, thunder slash, and few damage skill (Leiji, etc)
    };

    DamageStruct(const Card *card, ServerPlayer *from, ServerPlayer *to, int damage = 1, Nature nature = Normal);
    DamageStruct(const QString &reason, ServerPlayer *from, ServerPlayer *to, int damage = 1, Nature nature = Normal);

    ServerPlayer *from;
    ServerPlayer *to;
    const Card *card;
    int damage;
    Nature nature;
    bool chain;
    QString reason;

    QString getReason() const;
};

struct CardEffectStruct{
    CardEffectStruct();

    const Card *card;

    ServerPlayer *from;
    ServerPlayer *to;

    bool multiple;
};

struct SlashEffectStruct{
    SlashEffectStruct();

    const Slash *slash;
    const Card *jink;

    ServerPlayer *from;
    ServerPlayer *to;

    bool drank;

    DamageStruct::Nature nature;
};

struct CardUseStruct{
    CardUseStruct();
    bool isValid() const;
    void parse(const QString &str, Room *room);
    bool tryParse(const Json::Value&, Room *room);

    const Card *card;
    ServerPlayer *from;
    QList<ServerPlayer *> to;
    bool mute;
};

struct CardMoveStruct{
    int card_id;
    Player::Place from_place, to_place;
    ServerPlayer *from, *to;
    bool open;

    QString toString() const;
};

struct DyingStruct{
    DyingStruct();

    ServerPlayer *who; // who is ask for help
    DamageStruct *damage; // if it is NULL that means the dying is caused by losing hp
    QList<ServerPlayer *> savers; // savers are the available players who can use peach for the dying player
};

struct RecoverStruct{
    RecoverStruct();
    RecoverStruct(const Card *card, ServerPlayer *who = NULL, int recover = 1);

    int recover;
    ServerPlayer *who;
    const Card *card;
};

struct PindianStruct{
    PindianStruct();
    bool isSuccess() const;

    ServerPlayer *from;
    ServerPlayer *to;
    const Card *from_card;
    const Card *to_card;
    QString reason;
};

class JudgeStructPattern{
private:
    QString pattern;
    bool isRegex;

public:
    JudgeStructPattern();
    JudgeStructPattern &operator=(const QRegExp &rx);
    JudgeStructPattern &operator=(const QString &str);
    bool match(const Player *player, const Card *card) const;
};

struct JudgeStruct{
    JudgeStruct();
    bool isGood(const Card *card = NULL) const;
    bool isBad() const {return !isGood();}

    ServerPlayer *who;
    const Card *card;
    JudgeStructPattern pattern;
    bool good;
    QString reason;
    bool time_consuming;
};

struct PhaseChangeStruct{
    PhaseChangeStruct();
    Player::Phase from;
    Player::Phase to;
};

struct QiaogongStruct{
    QiaogongStruct();

    const Card *equip;
    bool wear; //true: equip; false: lost
    ServerPlayer * target; // who equip or lost
};

enum TriggerEvent{
    NonTrigger, //those two events actually trigger nothing

    GameStart,
    GameStarted,
    TurnStart,
    PhaseChange,
    InPhase,
    PhaseEnd,
    DrawNCards,
    DrawNCardsDone,
    HpRecover,
    HpRecovered,
    HpLost,
    HpChanged,
    MaxHpChanged,

    StartJudge,
    AskForRetrial,
    FinishJudge,

    Pindian,
    TurnedOver,
    ChainStateChange,
    PreConjuring,
    Conjured,

    Predamage,
    DamagedProceed,
    DamageProceed,
    Predamaged,
    DamageDone,
    Damage,
    Damaged,
    //DamageConclude,
    DamageComplete,

    Dying,
    AskForPeaches,
    AskForPeachesDone,
    PreDeath,
    Death,
    GameOverJudge,
    RewardAndPunish,

    SlashEffect,
    SlashEffected,
    SlashProceed,
    SlashHit,
    SlashMissed,

    JinkUsed,
    CardAsk,
    CardAsked,
    CardUseAsk,
    CardUsed,
    CardResponsed,
    CardThrow,
    CardDiscard,
    CardDiscarded,
    CardRecord,
    CardMoving,
    CardLost,
    CardLostDone,
    CardGot,
    CardGotDone,
    CardDrawing,
    CardDrawnDone,

    CardEffect,
    CardEffected,
    CardFinished,

    ChoiceMade,
    QiaogongTrigger,

    NumOfEvents
};

typedef const Card *CardStar;
typedef ServerPlayer *PlayerStar;
typedef JudgeStruct *JudgeStar;
typedef DamageStruct *DamageStar;
typedef PindianStruct *PindianStar;
typedef const CardMoveStruct *CardMoveStar;

Q_DECLARE_METATYPE(DamageStruct)
Q_DECLARE_METATYPE(CardEffectStruct)
Q_DECLARE_METATYPE(SlashEffectStruct)
Q_DECLARE_METATYPE(CardUseStruct)
Q_DECLARE_METATYPE(CardMoveStruct)
Q_DECLARE_METATYPE(CardMoveStar)
Q_DECLARE_METATYPE(CardStar)
Q_DECLARE_METATYPE(PlayerStar)
Q_DECLARE_METATYPE(DyingStruct)
Q_DECLARE_METATYPE(RecoverStruct)
Q_DECLARE_METATYPE(JudgeStar)
Q_DECLARE_METATYPE(DamageStar)
Q_DECLARE_METATYPE(PindianStar)
Q_DECLARE_METATYPE(PhaseChangeStruct)
Q_DECLARE_METATYPE(QiaogongStruct)

#endif // STRUCTS_H
