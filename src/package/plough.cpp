#include "plough.h"
#include "maneuvering.h"

Ecstasy::Ecstasy(Suit suit, int number): BasicCard(suit, number)
{
    setObjectName("ecstasy");
}

bool Ecstasy::IsAvailable(const Player *player){
    Ecstasy *ecs = new Ecstasy(Card::NoSuit, 0);
    ecs->deleteLater();
    return player->usedTimes("Ecstasy") <= Sanguosha->correctCardTarget(TargetModSkill::Residue, player, ecs);
    //return !player->hasUsed("Ecstasy");
}

bool Ecstasy::isAvailable(const Player *player) const{
    return IsAvailable(player) && BasicCard::isAvailable(player);
}

QString Ecstasy::getSubtype() const{
    return "attack_card";
}

bool Ecstasy::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    int total_num = 1 + Sanguosha->correctCardTarget(TargetModSkill::ExtraTarget, Self, this);
    if (targets.length() >= total_num)
        return false;
    return to_select != Self && Self->inMyAttackRange(to_select);
}

void Ecstasy::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();

    QString animation_str = QString("ecstasy:%1:%2")
                            .arg(effect.from->objectName()).arg(effect.to->objectName());
    room->broadcastInvoke("animate", animation_str);

    room->setPlayerFlag(effect.to, "ecst");
}

class EcstasySlash: public SlashSkill{
public:
    EcstasySlash():SlashSkill("#ecstasy-slash"){
        frequency = NotFrequent;
    }

    virtual int getSlashResidue(const Player *t) const{
        if(t->hasFlag("ecst"))
            return -998;
        else
            return 0;
    }
};

Drivolt::Drivolt(Suit suit, int number)
    :SingleTargetTrick(suit, number, true) {
    setObjectName("drivolt");
}

bool Drivolt::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    int trick_etargets = TrickCard::geteTargetsCount(Self, this);
    int trick_distance = TrickCard::geteRange(Self, this);

    trick_etargets ++;
    if(targets.length() >= trick_etargets)
        return false;
    if(to_select == Self)
        return false;
    if(to_select->getKingdom() == Self->getKingdom())
        return false;
    if(trick_distance != 0 && Self->distanceTo(to_select) > trick_distance)
        return false;
    // If the original is the infinite distance, it returns the new distance constraints
    return true;
}

void Drivolt::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.to->getRoom();
    room->loseHp(effect.to);
    if(effect.to->isAlive()){
        if(effect.to->getCardCount(true) <= 2){
            DummyCard *dummy = new DummyCard;
            foreach(const Card *card, effect.to->getCards("he"))
                dummy->addSubcard(card);
            room->throwCard(dummy, effect.to);
        }
        else
            room->askForDiscard(effect.to, "drivolt", qMin(2, effect.to->getCardCount(true)), false, true);
        effect.to->drawCards(3);
    }
}

Wiretap::Wiretap(Suit suit, int number)
    :SingleTargetTrick(suit, number, false) {
    setObjectName("wiretap");
}

bool Wiretap::targetsFeasible(const QList<const Player *> &targets, const Player *) const{
    QStringList skills;
    skills << "mitan" << "huace" << "linmo" << "fangzao";
    if(skills.contains(getSkillName()))
        return targets.length() == 1;
    else
        return targets.length() <= 1;
}

void Wiretap::onUse(Room *room, const CardUseStruct &card_use) const{
    if(card_use.to.isEmpty()){
        room->moveCardTo(this, NULL, Player::DiscardedPile);
        card_use.from->playCardEffect("@recast");
        room->setEmotion(card_use.from, "cards/recast");
        card_use.from->drawCards(1);
    }else
        TrickCard::onUse(room, card_use);
}

void Wiretap::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    source->playCardEffect(objectName());
    TrickCard::use(room, source, targets);
}

void Wiretap::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.to->getRoom();
    if(effect.to->isKongcheng())
        return;
    room->setTag("Wiretap", QVariant::fromValue(effect));
    QList<int> all = effect.to->handCards();
    //room->showAllCards(effect.to, effect.from);
    room->fillAG(all, effect.from);
    int mitan = room->askForAG(effect.from, all, true, "wiretap");
    if(effect.from->hasSkill("mitan") && mitan > -1){
        if(getSkillName() != "mitan")
            room->playSkillEffect("mitan", 2);
        room->showCard(effect.to, mitan);
    }
    effect.from->invoke("clearAG");
    room->removeTag("Wiretap");
}

Assassinate::Assassinate(Suit suit, int number)
    :SingleTargetTrick(suit, number, false) {
    setObjectName("assassinate");
}

void Assassinate::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();

    const Card *card1 = room->askForCard(effect.to, "jink", "@assas1:" + effect.from->objectName());
    const Card *card2;
    if(card1)
        card2 = room->askForCard(effect.to, "jink", "@assas2:" + effect.from->objectName());
    if(card1 && card2)
        effect.from->turnOver();
    else{
        DamageStruct dmae;
        dmae.card = this;
        dmae.from = effect.from;
        if(effect.to->hasSkill("huoshui")){
            room->broadcastInvoke("playAudio", "skill/scream");
            LogMessage ogg;
            ogg.type = "#Huoshui";
            ogg.from = effect.to;
            ogg.arg = "huoshui";
            ogg.arg2 = objectName();
            room->sendLog(ogg);
            dmae.damage = 2;
        }
        dmae.to = effect.to;
        room->damage(dmae);
    }
}

bool Assassinate::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    int trick_etargets = TrickCard::geteTargetsCount(Self, this);
    int trick_distance = TrickCard::geteRange(Self, this);

    trick_etargets ++;
    if(targets.length() >= trick_etargets)
        return false;
    if(to_select == Self)
        return false;
    if(trick_distance != 0 && Self->distanceTo(to_select) > trick_distance)
        return false;
    // If the original is the infinite distance, it returns the new distance constraints
    return true;
}

Counterplot::Counterplot(Suit suit, int number)
    :Nullification(suit, number){
    setObjectName("counterplot");
}

Provistore::Provistore(Suit suit, int number)
    :DelayedTrick(suit, number)
{
    setObjectName("provistore");
    target_fixed = false;

    judge.pattern = QRegExp("(.*):(diamond):(.*)");
    judge.good = false;
    judge.reason = objectName();
}

bool Provistore::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const
{
    int trick_distance = TrickCard::geteRange(Self, this);
    if(!targets.isEmpty())
        return false;
    if(to_select->containsTrick(objectName()))
        return false;

    if(trick_distance != 0 && Self->distanceTo(to_select) > trick_distance)
        return false;
    // If the original is the infinite distance, it returns the new distance constraints
    return true;
}

bool Provistore::targetsFeasible(const QList<const Player *> &targets, const Player *Self) const{
    if(targets.isEmpty() && !Self->containsTrick(objectName()))
        return true;
    return targets.length() == 1;
}

void Provistore::takeEffect(ServerPlayer *target, bool good) const{
    if(good)
        target->skip(Player::Discard);
}

Treasury::Treasury(Suit suit, int number):Disaster(suit, number){
    setObjectName("treasury");

    judge.pattern = QRegExp("(.*):(heart|diamond):([JQKA])");
    judge.good = true;
    judge.reason = objectName();
}

void Treasury::takeEffect(ServerPlayer *target, bool good) const{
    if(good){
        //room->broadcastInvoke("animate", "treasury:" + target->objectName());
        target->getRoom()->broadcastInvoke("playAudio", "card/treasury");
        target->drawCards(5);
    }
}

Tsunami::Tsunami(Suit suit, int number):Disaster(suit, number){
    setObjectName("tsunami");

    judge.pattern = QRegExp("(.*):(club|spade):([JQKA])");
    judge.good = false;
    judge.reason = objectName();
}

void Tsunami::takeEffect(ServerPlayer *target, bool good) const{
    if(!good){
        Room *room = target->getRoom();
        room->broadcastInvoke("playAudio", "card/tsunami");
        room->setEmotion(target, "tsunami");
        target->throwAllCards();
    }
}

class DoubleWhipSkill : public WeaponSkill{
public:
    DoubleWhipSkill():WeaponSkill("double_whip"){
        events << CardEffect;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        CardEffectStruct effect = data.value<CardEffectStruct>();
        if(effect.card->inherits("Slash") && player->askForSkillInvoke("double_whip", data)){
            if(!effect.to->isChained())
                player->playCardEffect("Edouble_whip1", "weapon");
            else
                player->playCardEffect("Edouble_whip2", "weapon");
            room->setPlayerChained(effect.to);
        }
        return false;
    }
};

DoubleWhip::DoubleWhip(Suit suit, int number)
    :Weapon(suit, number, 2)
{
    setObjectName("double_whip");
    skill = new DoubleWhipSkill;
}

class MeteorSwordSkill : public WeaponSkill{
public:
    MeteorSwordSkill():WeaponSkill("meteor_sword"){
        events << Predamage;
    }

    virtual int getPriority(TriggerEvent) const{
        return -1;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        if(damage.card->inherits("Slash") && damage.to->isAlive()){
            player->playCardEffect("Emeteor_sword", "weapon");
            room->loseHp(damage.to, damage.damage);
            return true;
        }
        return false;
    }
};

MeteorSword::MeteorSword(Suit suit, int number)
    :Weapon(suit, number, 3)
{
    setObjectName("meteor_sword");
    skill = new MeteorSwordSkill;
}

class SunBowSkill: public SlashSkill{
public:
    SunBowSkill():SlashSkill("sun_bow"){
    }

    virtual int getSlashExtraGoals(const Player *from, const Player *to, const Card *slash) const{
        if(from->hasWeapon("sun_bow") && slash->isRed() && slash->objectName() == "slash")
            return 1;
        else
            return 0;
    }
};

SunBow::SunBow(Suit suit, int number)
    :Weapon(suit, number, 5)
{
    setObjectName("sun_bow");
}

class GoldArmorSkill: public ArmorSkill{
public:
    GoldArmorSkill():ArmorSkill("gold_armor"){
        events << Damaged << SlashEffected;
    }

    virtual bool trigger(TriggerEvent event, Room* room, ServerPlayer *player, QVariant &data) const{
        if(event == SlashEffected){
            SlashEffectStruct effect = data.value<SlashEffectStruct>();
            if(effect.nature != DamageStruct::Normal){
                player->playCardEffect("Egold_armor1", "armor");

                LogMessage log;
                log.from = player;
                log.type = "#ArmorNullify";
                log.arg = objectName();
                log.arg2 = effect.slash->objectName();
                room->sendLog(log);

                return true;
            }
        }else if(event == Damaged){
            DamageStruct damage = data.value<DamageStruct>();
            if(damage.card && damage.card->inherits("Slash")){
                LogMessage log;
                log.type = "#ThrowWeapon";
                log.from = player;
                log.arg = objectName();
                if(damage.from->getWeapon()){
                    player->playCardEffect("Egold_armor2", "armor");
                    room->sendLog(log);
                    room->throwCard(damage.from->getWeapon(), damage.from);
                }
            }
        }
        return false;
    }
};

GoldArmor::GoldArmor(Suit suit, int number):Armor(suit, number){
    setObjectName("gold_armor");
    skill = new GoldArmorSkill;
}

PloughPackage::PloughPackage()
    :CardPackage("plough")
{
    QList<Card *> cards;

    cards
    // spade
            << new Assassinate(Card::Spade, 1)
            << new Tsunami(Card::Spade, 2)
            << new MeteorSword(Card::Spade, 3)
            << new Ecstasy(Card::Spade, 4)
            << new Ecstasy(Card::Spade, 5)
            << new Ecstasy(Card::Spade, 6)
            << new Slash(Card::Spade, 7)
            << new Slash(Card::Spade, 8)
            << new ThunderSlash(Card::Spade, 9)
            << new ThunderSlash(Card::Spade, 10)
            << new ThunderSlash(Card::Spade, 11)
            << new Wiretap(Card::Spade, 12)
            << new Drivolt(Card::Spade, 13)

    // diamond
            << new Dismantlement(Card::Diamond, 1)
            << new Peach(Card::Diamond, 2)
            << new FireSlash(Card::Diamond, 3)
            << new Slash(Card::Diamond, 4)
            << new Slash(Card::Diamond, 5)
            << new Jink(Card::Diamond, 6)
            << new Jink(Card::Diamond, 7)
            << new Treasury(Card::Diamond, 8)
            << new Analeptic(Card::Diamond, 9)
            << new Slash(Card::Diamond, 10)
            << new SunBow(Card::Diamond, 11)
            << new Assassinate(Card::Diamond, 12)
            << new Counterplot(Card::Diamond, 13)

    // club
            << new Provistore(Card::Club, 1)
            << new Ecstasy(Card::Club, 2)
            << new Ecstasy(Card::Club, 3)
            << new Slash(Card::Club, 4)
            << new Slash(Card::Club, 5)
            << new ThunderSlash(Card::Club, 6)
            << new DoubleWhip(Card::Club, 7)
            << new Analeptic(Card::Club, 9)
            << new GoldArmor(Card::Club, 10)
            << new Wiretap(Card::Club, 11)
            << new IronChain(Card::Club, 12)
            << new IronChain(Card::Club, 13)

    // heart
            << new Drivolt(Card::Heart, 1)
            << new FireSlash(Card::Heart, 2)
            << new Slash(Card::Heart, 3)
            << new Provistore(Card::Heart, 4)
            << new Jink(Card::Heart, 5)
            << new Jink(Card::Heart, 6)
            << new Jink(Card::Heart, 7)
            << new Jink(Card::Heart, 8)
            << new Analeptic(Card::Heart, 9)
            << new Peach(Card::Heart, 10)
            << new Peach(Card::Heart, 11)
            << new Counterplot(Card::Heart, 13);

    skills << new EcstasySlash << new SunBowSkill;
    DefensiveHorse *jade = new DefensiveHorse(Card::Heart, 12);
    jade->setObjectName("jade");
    OffensiveHorse *brown = new OffensiveHorse(Card::Club, 8);
    brown->setObjectName("brown");

    cards << jade << brown;
    foreach(Card *card, cards)
        card->setParent(this);
}

// ex
Inspiration::Inspiration(Suit suit, int number)
    :GlobalEffect(suit, number)
{
    setObjectName("inspiration");
}

bool Inspiration::isCancelable(const CardEffectStruct &effect) const{
    return effect.to->isWounded();
}

void Inspiration::onEffect(const CardEffectStruct &effect) const{
    int x = qMin(3, effect.to->getLostHp());
    if(x > 0)
        effect.to->drawCards(x);
}

Haiqiu::Haiqiu(Card::Suit suit, int number)
    :OffensiveHorse(suit, number)
{
    setObjectName("haiqiu");
}

QString Haiqiu::getEffectPath(bool ) const{
    return "audio/card/common/haiqiu.ogg";
}

Yuukirito::Yuukirito(Suit suit, int number)
    :SingleTargetTrick(suit, number, false) {
    setObjectName("yuukirito");
    target_fixed = true;
}

void Yuukirito::onUse(Room *room, const CardUseStruct &card_use) const{
    if(room->getMode() != "02_1v1"){
        room->moveCardTo(this, NULL, Player::DiscardedPile);
        card_use.from->playCardEffect("@recast");
        room->setEmotion(card_use.from, "cards/recast");
        card_use.from->drawCards(1);
    }else
        TrickCard::onUse(room, card_use);
}

void Yuukirito::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &) const{
    source->playCardEffect(objectName());
    ServerPlayer *target = room->getOtherPlayers(source).first();
    QString choice = !target->hasEquip() ? "hvp" :
                     room->askForChoice(target, "yuukirito", "eqp+hvp", QVariant());
    if(choice == "hvp")
        room->loseHp(target);
    else
        target->throwAllEquips();
}

New3v3CardPackage::New3v3CardPackage()
    :CardPackage("new_3v3_card")
{
    QList<Card *> cards;
    cards << new SupplyShortage(Card::Spade, 1)
          << new SupplyShortage(Card::Club, 12)
          << new Nullification(Card::Heart, 12)

          << new Assassinate(Card::Spade, 2)
          << new Wiretap(Card::Heart, 2)
          << new Provistore(Card::Diamond, 12)
          << new Counterplot(Card::Club, 12);

    foreach(Card *card, cards)
        card->setParent(this);

    type = CardPack;
}

ADD_PACKAGE(New3v3Card)
ADD_PACKAGE(Plough)
