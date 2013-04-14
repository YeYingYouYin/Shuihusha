#include "purgatory.h"
#include "maneuvering.h"

Shit::Shit(Suit suit, int number):
    BasicCard(suit, number){
    setObjectName("shit");
    target_fixed = true;
}

QString Shit::getSubtype() const{
    return "specially";
}

void Shit::onUse(Room *room, const CardUseStruct &) const{
    room->moveCardTo(this, NULL, Player::DiscardedPile);
}

void Shit::onMove(const CardMoveStruct &move) const{
    PlayerStar from = move.from;
    if(!from)
        return;
    Room *room = from->getRoom();
    if(from && move.from_place == Player::Hand &&
       room->getCurrent() == move.from
       && (move.to_place == Player::DiscardedPile || move.to_place == Player::Special)
       && move.to == NULL
       && from->isAlive()){

        LogMessage log;
        log.type = "$ShitHint";
        log.card_str = getEffectIdString();
        log.from = from;
        room->sendLog(log);

        if(getSuit() == Spade)
            room->loseHp(from);
        else{
            DamageStruct damage;
            damage.from = damage.to = from;
            damage.card = this;

            switch(getSuit()){
            case Club: damage.nature = DamageStruct::Thunder; break;
            case Heart: damage.nature = DamageStruct::Fire; break;
            default: damage.nature = DamageStruct::Normal;
            }
            room->damage(damage);
        }
    }
}

bool Shit::HasShit(const Card *card){
    if(card->isVirtualCard()){
        QList<int> card_ids = card->getSubcards();
        foreach(int card_id, card_ids){
            const Card *c = Sanguosha->getCard(card_id);
            if(c->objectName() == "shit")
                return true;
        }

        return false;
    }else
        return card->objectName() == "shit";
}

Mastermind::Mastermind(Suit suit, int number)
    :SingleTargetTrick(suit, number, false) {
    setObjectName("mastermind");
}

bool Mastermind::targetsFeasible(const QList<const Player *> &targets, const Player *) const{
    return targets.length() == 2;
}

bool Mastermind::targetFilter(const QList<const Player *> &targets, const Player *, const Player *) const{
    return targets.length() <= 1;
}

void Mastermind::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    targets.last()->tag["DtoL"] = QVariant::fromValue((PlayerStar)targets.first());
    QList<ServerPlayer *> tgs;
    tgs << targets.last();
    TrickCard::use(room, source, tgs);
}

void Mastermind::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();
    PlayerStar death = effect.to;
    PlayerStar life = death->tag["DtoL"].value<PlayerStar>();

    foreach(ServerPlayer *t, room->getAllPlayers()){
        t->loseAllMarks("@death");
        t->loseAllMarks("@life");
    }

    death->gainMark("@death");
    life->gainMark("@life");
}

QString Mastermind::getEffectPath(bool ) const{
    return Card::getEffectPath();
}

SpinDestiny::SpinDestiny(Suit suit, int number)
    :GlobalEffect(suit, number)
{
    setObjectName("spin_destiny");
}

void SpinDestiny::onUse(Room *room, const CardUseStruct &card_use) const{
    CardUseStruct use = card_use;
    use.to = room->getAllPlayers(false);
    QList<ServerPlayer *> deads;
    foreach(ServerPlayer *dead, room->getAllPlayers(true)){
        if(dead->isDead())
            deads << dead;
    }
    TrickCard::onUse(room, use);
    foreach(ServerPlayer *dead, deads){
        if(dead->getMaxHp() <= 0)
            room->setPlayerProperty(dead, "maxhp", 1);
        room->setPlayerProperty(dead, "hp", 1);
        room->revivePlayer(dead);
    }
}

void SpinDestiny::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();
    room->loseHp(effect.to);
}

QString SpinDestiny::getEffectPath(bool ) const{
    return Card::getEffectPath();
}

EdoTensei::EdoTensei(Suit suit, int number)
    :SingleTargetTrick(suit, number, false){
    setObjectName("edo_tensei");
    target_fixed = true;
}

void EdoTensei::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();
    QStringList targets, targets_object;
    foreach(ServerPlayer *target, room->getAllPlayers(true)){
        if(target->isDead()){
            targets << target->getGeneralName();
            targets_object << target->objectName();
        }
    }
    if(targets.isEmpty())
        return;
    QString hcoi = targets.count() == 1 ? targets.first() :
                   room->askForChoice(effect.from, "edo_tensei", targets.join("+"));
    int index = targets.indexOf(hcoi);
    PlayerStar revivd = room->findPlayer(targets_object.at(index), true);
    room->setPlayerProperty(revivd, "hp", 1);
    revivd->drawCards(3);
    room->revivePlayer(revivd);
}

QString EdoTensei::getEffectPath(bool ) const{
    return Card::getEffectPath();
}

bool EdoTensei::isAvailable(const Player *) const{
    return false;
}

class ProudBannerSkill: public ArmorSkill{
public:
    ProudBannerSkill():ArmorSkill("renwang_shield"){
        events << TurnedOver << ChainStateChange << PreConjuring;
    }

    virtual bool trigger(TriggerEvent event, Room *, ServerPlayer *player, QVariant &data) const{
        if(event == TurnedOver)
            return player->faceUp();
        else if(event == ChainStateChange)
            return !player->isChained();
        else if(event == PreConjuring)
            return qrand() % 2 == 0;
        return false;
    }
};

ProudBanner::ProudBanner(Suit suit, int number)
    :Armor(suit, number)
{
    setObjectName("proud_banner");
    skill = new ProudBannerSkill;
}

class LashGunSkill : public WeaponSkill{
public:
    LashGunSkill():WeaponSkill("lash_gun"){
        events << Damage;
    }

    virtual int getPriority(TriggerEvent) const{
        return -1;
    }

    static QList<ServerPlayer *> getNextandPrevious(ServerPlayer *target){
        QList<ServerPlayer *> targets;
        targets << target->getNextAlive();
        if(target->getRoom()->getAlivePlayers().count() > 2){
            foreach(ServerPlayer *tmp, target->getRoom()->getOtherPlayers(target)){
                if(tmp->getNextAlive() == target){
                    targets << tmp;
                    break;
                }
            }
        }
        return targets;
    }

    virtual bool trigger(TriggerEvent, Room*, ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        if(damage.card->isKindOf("Slash") && damage.nature != DamageStruct::Normal){
            player->playCardEffect("Elash_gun", "weapon");
            foreach(ServerPlayer *tmp, getNextandPrevious(damage.to))
                tmp->gainJur("dizzy_jur", 2);
        }
        return false;
    }
};

LashGun::LashGun(Suit suit, int number)
    :Weapon(suit, number, 6)
{
    setObjectName("lash_gun");
    skill = new LashGunSkill;
}

PurgatoryPackage::PurgatoryPackage()
    :CardPackage("purgatory")
{
    QList<Card *> cards;

    cards
            /*<< new Shit(Card::Club, 1)
            << new Shit(Card::Heart, 8)
            << new Shit(Card::Diamond, 13)*/
            << new Shit(Card::Spade, 10)
            << new Mastermind(Card::Heart, 5)
            << new Mastermind(Card::Heart, 5)
            << new SpinDestiny(Card::Diamond, 5)
            << new EdoTensei(Card::Club, 5)
            << new EdoTensei(Card::Spade, 5)
            << new LashGun(Card::Club, 4)
            << new ProudBanner(Card::Heart, 1)
            ;

    foreach(Card *card, cards)
        card->setParent(this);
}

ADD_PACKAGE(Purgatory)
