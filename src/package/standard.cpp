#include "standard.h"
#include "serverplayer.h"
#include "room.h"
#include "skill.h"
#include "maneuvering.h"
#include "clientplayer.h"
#include "engine.h"
#include "client.h"
#include "exppattern.h"
#include "common-skillcards.h"

QString BasicCard::getType() const{
    return "basic";
}

Card::CardType BasicCard::getTypeId() const{
    return Basic;
}

TrickCard::TrickCard(Suit suit, int number, bool aggressive)
    :Card(suit, number), aggressive(aggressive),
    cancelable(true)
{
}

bool TrickCard::isAggressive() const{
    return aggressive;
}

void TrickCard::setCancelable(bool cancelable){
    this->cancelable = cancelable;
}

QString TrickCard::getType() const{
    return "trick";
}

Card::CardType TrickCard::getTypeId() const{
    return Trick;
}

bool TrickCard::isCancelable(const CardEffectStruct &effect) const{
    if(this->isNDTrick() && effect.from->hasSkill("pozhen"))
        return false;
    return cancelable;
}

TriggerSkill *EquipCard::getSkill() const{
    return skill;
}

QString EquipCard::getType() const{
    return "equip";
}

Card::CardType EquipCard::getTypeId() const{
    return Equip;
}

QString EquipCard::getEffectPath(bool is_male) const{
    return "audio/card/common/equip.ogg";
}

void EquipCard::onUse(Room *room, const CardUseStruct &card_use) const{
    if(card_use.to.isEmpty()){
        ServerPlayer *player = card_use.from;

        QVariant data = QVariant::fromValue(card_use);
        RoomThread *thread = room->getThread();
        thread->trigger(CardUsed, room, player, data);

        thread->trigger(CardFinished, room, player, data);
    }else
        Card::onUse(room, card_use);
}

void EquipCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    const EquipCard *equipped = NULL;
    ServerPlayer *target = targets.value(0, source);
    
    switch(location()){
    case WeaponLocation: equipped = target->getWeapon(); break;
    case ArmorLocation: equipped = target->getArmor(); break;
    case DefensiveHorseLocation: equipped = target->getDefensiveHorse(); break;
    case OffensiveHorseLocation: equipped = target->getOffensiveHorse(); break;
    }

    if(equipped)
        room->throwCard(equipped, source);

    LogMessage log;
    log.from = target;
    log.type = "$Install";
    log.card_str = QString::number(getEffectiveId());
    room->sendLog(log);

    room->moveCardTo(this, target, Player::Equip, true);
}

void EquipCard::onInstall(ServerPlayer *player) const{
    Room *room = player->getRoom();

    if(skill)
        room->getThread()->addTriggerSkill(skill);
}

void EquipCard::onUninstall(ServerPlayer *player) const{

}

QString GlobalEffect::getSubtype() const{
    return "global_effect";
}

void GlobalEffect::onUse(Room *room, const CardUseStruct &card_use) const{
    CardUseStruct use = card_use;
    use.to = room->getAllPlayers();
    TrickCard::onUse(room, use);
}

QString AOE::getSubtype() const{
    return "aoe";
}

bool AOE::isAvailable(const Player *player) const{
    QList<const Player *> players = player->getSiblings();
    foreach(const Player *p, players){
        if(p->isDead())
            continue;

        if(player->isProhibited(p, this))
            continue;

        return true;
    }

    return false;
}

void AOE::onUse(Room *room, const CardUseStruct &card_use) const{
    ServerPlayer *source = card_use.from;
    QList<ServerPlayer *> targets, other_players = room->getOtherPlayers(source);
    foreach(ServerPlayer *player, other_players){
        const ClientSkill *skill = room->isProhibited(source, player, this);
        if(skill){
            LogMessage log;
            log.type = "#SkillAvoid";
            log.from = player;
            log.arg = skill->objectName();
            log.arg2 = objectName();
            room->sendLog(log);

            room->playSkillEffect(skill->objectName());
        }else
            targets << player;
    }

    CardUseStruct use = card_use;
    use.to = targets;
    TrickCard::onUse(room, use);
}

QString SingleTargetTrick::getSubtype() const{
    return "single_target_trick";
}

bool SingleTargetTrick::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(to_select == Self)
        return false;
    if(objectName() == "assassinate" || objectName() == "duel"){
        if(to_select->hasSkill("jueming") && to_select->getHp() == 1)
            return false;
    }

    return targets.isEmpty();
}

DelayedTrick::DelayedTrick(Suit suit, int number, bool movable)
    :TrickCard(suit, number, true), movable(movable)
{
}

void DelayedTrick::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    ServerPlayer *target = targets.value(0, source);
    room->moveCardTo(this, target, Player::Judging, true);
}

QString DelayedTrick::getSubtype() const{
    return "delayed_trick";
}

void DelayedTrick::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.to->getRoom();

    if(!movable)
        room->throwCard(this);

    LogMessage log;
    log.from = effect.to;
    log.type = "#DelayedTrick";
    log.arg = effect.card->objectName();
    room->sendLog(log);

    JudgeStruct judge_struct = judge;
    judge_struct.who = effect.to;
    room->judge(judge_struct);

    bool burst = false;
    if(judge_struct.reason == "treasury" ||
        judge_struct.reason == "provistore")
        burst = judge_struct.isGood();
    else
        burst = judge_struct.isBad();

    if(burst){
        room->throwCard(this);
        takeEffect(effect.to, judge_struct.isGood());
    }
    else if(movable){
        if(objectName() != "tsunami" || !effect.to->hasEquip("haiqiu"))
            onNullified(effect.to);
    }
}

void DelayedTrick::onNullified(ServerPlayer *target) const{
    Room *room = target->getRoom();
    if(movable){
        QList<ServerPlayer *> players = room->getOtherPlayers(target);
        players << target;

        foreach(ServerPlayer *player, players){
            if(player->containsTrick(objectName()))
                continue;

            if(room->isProhibited(target, player, this))
                continue;

            room->moveCardTo(this, player, Player::Judging, true);
            break;
        }
    }else
        room->throwCard(this);
}

const DelayedTrick *DelayedTrick::CastFrom(const Card *card){
    DelayedTrick *trick = NULL;
    Card::Suit suit = card->getSuit();
    int number = card->getNumber();
    if(card->inherits("DelayedTrick"))
        return qobject_cast<const DelayedTrick *>(card);
    else if(card->getSuit() == Card::Diamond){
        trick = new Indulgence(suit, number);
        trick->addSubcard(card->getId());
    }
    else if(card->isBlack() && (card->inherits("BasicCard") || card->inherits("EquipCard"))){
        trick = new SupplyShortage(suit, number);
        trick->addSubcard(card->getId());
    }

    return trick;
}

Weapon::Weapon(Suit suit, int number, int range)
    :EquipCard(suit, number), range(range), attach_skill(false)
{
}

int Weapon::getRange() const{
    return range;
}

QString Weapon::getSubtype() const{
    return "weapon";
}

EquipCard::Location Weapon::location() const{
    return WeaponLocation;
}

QString Weapon::label() const{
    return QString("%1(%2)").arg(getName()).arg(range);
}

void Weapon::onInstall(ServerPlayer *player) const{
    EquipCard::onInstall(player);
    Room *room = player->getRoom();

    if(attach_skill)
        room->attachSkillToPlayer(player, objectName());
}

bool Weapon::hasSkill() const{
    return attach_skill;
}

void Weapon::onUninstall(ServerPlayer *player) const{
    EquipCard::onUninstall(player);
    Room *room = player->getRoom();

    if(attach_skill)
        room->detachSkillFromPlayer(player, objectName(), false);
}

QString Armor::getSubtype() const{
    return "armor";
}

EquipCard::Location Armor::location() const{
    return ArmorLocation;
}

QString Armor::label() const{
    return getName();
}

Horse::Horse(Suit suit, int number, int correct)
    :EquipCard(suit, number), correct(correct)
{
}

int Horse::getCorrect() const{
    return correct;
}

QString Horse::getEffectPath(bool) const{
    return "audio/card/common/horse.ogg";
}

void Horse::onInstall(ServerPlayer *) const{

}

void Horse::onUninstall(ServerPlayer *) const{

}

QString Horse::label() const{
    QString format;

    if(correct > 0)
        format = "%1(+%2)";
    else
        format = "%1(%2)";

    return format.arg(getName()).arg(correct);
}

OffensiveHorse::OffensiveHorse(Card::Suit suit, int number, int correct)
    :Horse(suit, number, correct)
{

}

QString OffensiveHorse::getSubtype() const{
    return "offensive_horse";
}

DefensiveHorse::DefensiveHorse(Card::Suit suit, int number, int correct)
    :Horse(suit, number, correct)
{

}

QString DefensiveHorse::getSubtype() const{
    return "defensive_horse";
}

EquipCard::Location Horse::location() const{
    if(correct > 0)
        return DefensiveHorseLocation;
    else
        return OffensiveHorseLocation;
}

class NamePattern: public CardPattern{
public:
    NamePattern(const QString &name)
        :name(name){
    }
    virtual bool match(const Player *player, const Card *card) const{
        return ! player->hasEquip(card) && card->objectName() == name;
    }
private:
    QString name;
};

class NothrowHandcardsPattern: public CardPattern{
public:
    virtual bool match(const Player *player, const Card *card) const{
        return !player->hasEquip(card) ;
    }
    virtual bool willThrow() const{
        return false;
    }
};

class NothrowPattern: public CardPattern{
public:
    virtual bool match(const Player *player, const Card *card) const{
        return true;
    }
    virtual bool willThrow() const{
        return false;
    }
};

#include "carditem.h"
// zhuanshi-mode
class Sacrifice:public ZeroCardViewAsSkill{
public:
    Sacrifice():ZeroCardViewAsSkill("sacrifice"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        if(player->hasUsed("SacrificeCard"))
            return false;
        foreach(const Player *p, player->getSiblings())
            if(p->isDead())
                return !player->isKongcheng();
        return false;
    }

    virtual const Card *viewAs() const{
        return new SacrificeCard;
    }
};

// test main
class Ubuna: public ClientSkill{
public:
    Ubuna():ClientSkill("#ubuna"){
    }

    virtual int getExtra(const Player *target) const{
        if(target->hasSkill(objectName()))
            return 1358;
        else
            return 0;
    }
};

class Ubunb:public ZeroCardViewAsSkill{
public:
    Ubunb():ZeroCardViewAsSkill("ubunb"){
    }

    virtual const Card *viewAs() const{
        return new UbunbCard;
    }
};

class Ubunc:public ZeroCardViewAsSkill{
public:
    Ubunc():ZeroCardViewAsSkill("ubunc"){
    }

    virtual const Card *viewAs() const{
        return new UbuncCard;
    }
};

class Ubund:public ZeroCardViewAsSkill{
public:
    Ubund():ZeroCardViewAsSkill("ubund"){
    }

    virtual const Card *viewAs() const{
        return new UbundCard;
    }
};

class UbuneVAS: public OneCardViewAsSkill{
public:
    UbuneVAS():OneCardViewAsSkill("ubune"){
    }

    virtual bool isDTE(CardStar card) const{
        return (card->inherits("DelayedTrick") ||
                card->getTypeId() == Card::Equip);
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return isDTE(to_select->getCard());
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        UbuneCard *card = new UbuneCard;
        card->addSubcard(card_item->getCard()->getId());
        return card;
    }
};

class Ubune: public PhaseChangeSkill{
public:
    Ubune():PhaseChangeSkill("ubune"){
        view_as_skill = new UbuneVAS;
    }

    virtual bool onPhaseChange(ServerPlayer *p) const{
        Room *room = p->getRoom();
        if(p->getPhase() == Player::Judge && !p->getJudgingArea().isEmpty() &&
           p->askForSkillInvoke(objectName())){
            ServerPlayer *target = room->askForPlayerChosen(p, room->getOtherPlayers(p), objectName());
            DummyCard *dummy1 = new DummyCard;
            foreach(const Card *card, target->getJudgingArea())
                dummy1->addSubcard(card->getId());
            DummyCard *dummy2 = new DummyCard;
            foreach(const Card *card, p->getJudgingArea())
                dummy2->addSubcard(card->getId());
            room->moveCardTo(dummy2, target, Player::Judging);
            room->playSkillEffect(objectName(), 2);
            delete dummy2;
            room->moveCardTo(dummy1, p, Player::Judging);
            delete dummy1;
        }
        return false;
    }
};

class Ubunf: public TriggerSkill{
public:
    Ubunf():TriggerSkill("ubunf"){
        events << Dying << PreDeath;
    }

    virtual bool trigger(TriggerEvent event, Room* room, ServerPlayer *player, QVariant &data) const{
        if(event == PreDeath)
            return player->getHp() > 0;

        DyingStruct dying = data.value<DyingStruct>();
        if(dying.who == player && player->askForSkillInvoke(objectName())){
            room->playSkillEffect(objectName());
            RecoverStruct rev;
            rev.who = player;
            rev.recover = player->getMaxHP();
            room->recover(player, rev, true);
            room->setPlayerProperty(player, "hp", player->getMaxHp());
        }
        return false;
    }
};

QiapaiCard::QiapaiCard(){
    target_fixed = true;
    mute = true;
}

void QiapaiCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    QList<int> card_ids;
    QList<const Card *> cards = source->getCards("he");
    foreach(const Card *tmp, cards)
        card_ids << tmp->getId();
    room->fillAG(card_ids, source);
    int card_id = room->askForAG(source, card_ids, false, objectName());
    card_ids.removeOne(card_id);
    room->takeAG(source, card_id);
    room->broadcastInvoke("clearAG");
    room->playSkillEffect(skill_name);
}

class Qiapai: public ZeroCardViewAsSkill{
public:
    Qiapai():ZeroCardViewAsSkill("qiapai"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->isNude();
    }

    virtual const Card *viewAs() const{
        return new QiapaiCard;
    }
};

TestPackage::TestPackage()
    :Package("test")
{
    skills << new Sacrifice;
    addMetaObject<SacrificeCard>();

    General *ubuntenkei = new General(this, "ubuntenkei", "god", 4, false, true);
    ubuntenkei->addSkill(new Ubuna);
    ubuntenkei->addSkill(new Qiapai);
    addMetaObject<QiapaiCard>();
    ubuntenkei->addSkill(new Ubune);
    addMetaObject<UbuneCard>();
    ubuntenkei->addSkill(new Ubunb);
    addMetaObject<UbunbCard>();
    related_skills.insertMulti("ubunb", "#ubuna");
    ubuntenkei->addSkill(new Ubunc);
    addMetaObject<UbuncCard>();
    ubuntenkei->addSkill(new Ubund);
    addMetaObject<UbundCard>();
    ubuntenkei->addSkill(new Ubunf);

    new General(this, "sujiang", "god", 5, true, true);
    new General(this, "sujiangf", "god", 5, false, true);
    new General(this, "anjiang", "god", 4, true, true, true);

    patterns["."] = new ExpPattern(".|.|.|hand");
    patterns[".S"] = new ExpPattern(".|spade|.|hand");
    patterns[".C"] = new ExpPattern(".|club|.|hand");
    patterns[".H"] = new ExpPattern(".|heart|.|hand");
    patterns[".D"] = new ExpPattern(".|diamond|.|hand");

    //patterns[".red"] = new ExpPattern(".|.|.|hand|red");

    patterns[".."] = new ExpPattern(".");
    //patterns["..D"] = new ExpPattern(".|diamond");

    patterns["slash"] = new ExpPattern("Slash");
    patterns["jink"] = new ExpPattern("Jink");
    patterns["peach"] = new ExpPattern("Peach");
    patterns["nullification"] = new NamePattern("nullification");
    patterns["nulliplot"] = new ExpPattern("Nullification");
    patterns["peach+analeptic"] = new ExpPattern("Peach,Analeptic");
}

ADD_PACKAGE(Test)
