#include "bwqz.h"
#include "general.h"
#include "skill.h"
#include "carditem.h"
#include "engine.h"
#include "standard.h"
#include "clientplayer.h"
#include "plough.h"
#include "maneuvering.h"

class Zaochuan: public OneCardViewAsSkill{
public:
    Zaochuan():OneCardViewAsSkill("zaochuan"){
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getCard()->inherits("TrickCard");
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *card = card_item->getFilteredCard();
        IronChain *chain = new IronChain(card->getSuit(), card->getNumber());
        chain->addSubcard(card);
        chain->setSkillName(objectName());
        return chain;
    }
};

class Mengchong: public ClientSkill{
public:
    Mengchong():ClientSkill("mengchong"){
    }

    virtual int getCorrect(const Player *from, const Player *to) const{
        bool mengkang = from->hasSkill(objectName());
        foreach(const Player *player, from->getSiblings()){
            if(player->isAlive() && player->hasSkill(objectName())){
                mengkang = true;
                break;
            }
        }
        if(mengkang && !from->isChained() && to->isChained())
            return +1;
        else
            return 0;
    }
};

YuanyinCard::YuanyinCard(){
    mute = true;
}

bool YuanyinCard::targetsFeasible(const QList<const Player *> &targets, const Player *Self) const{
    if(targets.length() == 2)
        return true;
    return targets.length() == 1 && Self->canSlash(targets.first());
}

bool YuanyinCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(targets.isEmpty()){
        return to_select->getWeapon() && to_select != Self;
    }else if(targets.length() == 1){
        const Player *first = targets.first();
        return to_select != Self && first->getWeapon() && Self->canSlash(to_select);
    }else
        return false;
}

void YuanyinCard::onUse(Room *room, const CardUseStruct &card_use) const{
    QList<ServerPlayer *> targets = card_use.to;
    PlayerStar target; PlayerStar source = card_use.from;
    if(targets.length() > 1)
        target = targets.at(1);
    else if(targets.length() == 1 && source->canSlash(targets.first())){
        target = targets.first();
    }
    else
        return;

    const Card *weapon = target->getWeapon();
    if(weapon){
        Slash *slash = new Slash(weapon->getSuit(), weapon->getNumber());
        slash->setSkillName("yuanyin");
        slash->addSubcard(weapon);
        room->throwCard(weapon->getId(), target, source);
        CardUseStruct use;
        use.card = slash;
        use.from = source;
        use.to << target;
        room->useCard(use);
    }
}

class YuanyinViewAsSkill:public ZeroCardViewAsSkill{
public:
    YuanyinViewAsSkill():ZeroCardViewAsSkill("yuanyin"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return Slash::IsAvailable(player);
    }

    virtual const Card *viewAs() const{
        return new YuanyinCard;
    }
};

class Yuanyin: public TriggerSkill{
public:
    Yuanyin():TriggerSkill("yuanyin"){
        events << CardAsked;
        view_as_skill = new YuanyinViewAsSkill;
    }

    virtual int getPriority() const{
        return 2;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        QString asked = data.toString();
        if(asked != "slash" && asked != "jink")
            return false;
        QList<ServerPlayer *> playerAs, playerBs;
        foreach(ServerPlayer *tmp, room->getOtherPlayers(player)){
            if(asked == "slash" && tmp->getWeapon())
                playerAs << tmp;
            if(asked == "jink" &&
                    (tmp->getArmor() || tmp->getOffensiveHorse() || tmp->getDefensiveHorse()))
                playerBs << tmp;
        }
        if((asked == "slash" && playerAs.isEmpty()) || (asked == "jink" && playerBs.isEmpty()))
            return false;
        if(room->askForSkillInvoke(player, objectName(), data)){
            ServerPlayer *target = asked == "slash" ?
                                   room->askForPlayerChosen(player, playerAs, objectName()) :
                                   room->askForPlayerChosen(player, playerBs, objectName());
            const Card *card = NULL;
            if(asked == "slash")
                card = target->getWeapon();
            else if(asked == "jink"){
                if(target->getEquips().length() == 1 && !target->getWeapon())
                    card = target->getEquips().first();
                else if(target->getWeapon() && target->getEquips().length() == 2)
                    card = target->getEquips().at(1);
                else
                    card = Sanguosha->getCard(room->askForCardChosen(player, target, "e", objectName()));
            }
            if(asked == "jink" && target->getWeapon() && target->getWeapon()->getId() == card->getId())
                return false;
            if(asked == "slash"){
                Slash *yuanyin_card = new Slash(card->getSuit(), card->getNumber());
                yuanyin_card->setSkillName(objectName());
                yuanyin_card->addSubcard(card);
                room->provide(yuanyin_card);
            }
            else{
                Jink *yuanyin_card = new Jink(card->getSuit(), card->getNumber());
                yuanyin_card->setSkillName(objectName());
                yuanyin_card->addSubcard(card);
                room->provide(yuanyin_card);
            }
            room->setEmotion(player, "good");
        }
        return false;
    }
};

class Xiaofang: public TriggerSkill{
public:
    Xiaofang():TriggerSkill("xiaofang"){
        events << Predamaged << Damaged;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual int getPriority() const{
        return 2;
    }

    virtual bool trigger(TriggerEvent event, Room* room, ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        ServerPlayer *water = room->findPlayerBySkillName(objectName());
        if(!water || water->isKongcheng())
            return false;
        if(event == Predamaged){
            if(damage.nature == DamageStruct::Fire &&
               water->askForSkillInvoke(objectName()) &&
               room->askForDiscard(water, objectName(), 1)){
                LogMessage log;
                log.type = "#Xiaofan";
                log.from = water;
                log.arg = objectName();
                log.to << damage.to;
                room->sendLog(log);

                return true;
            }
        }
        else{
            if(damage.nature == DamageStruct::Thunder &&
               water->askForSkillInvoke(objectName()) &&
               room->askForDiscard(water, objectName(), 1)){
                ServerPlayer *forbider = damage.to;
                foreach(ServerPlayer *tmp, room->getOtherPlayers(water)){
                    if(tmp == forbider)
                        continue;
                    DamageStruct ailue;
                    ailue.from = water;
                    ailue.to = tmp;
                    ailue.nature = DamageStruct::Thunder;
                    room->damage(ailue);
                }
            }
        }
        return false;
    }
};

NushaCard::NushaCard(){
    once = true;
}

bool NushaCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    int i = 0;
    foreach(const Player *player, Self->getSiblings()){
        if(player->getHandcardNum() >= i){
            i = player->getHandcardNum();
        }
    }
    return targets.isEmpty() && to_select->getHandcardNum() == i && to_select != Self;
}

void NushaCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this, source);
    DamageStruct damage;
    damage.from = source;
    damage.to = targets.first();
    damage.card = this;
    room->damage(damage);
}

class Nusha: public OneCardViewAsSkill{
public:
    Nusha():OneCardViewAsSkill("nusha"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->hasUsed("NushaCard") && !player->isKongcheng();
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getCard()->inherits("Slash");
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        Card *card = new NushaCard;
        card->addSubcard(card_item->getFilteredCard());
        return card;
    }
};

class Wanku:public PhaseChangeSkill{
public:
    Wanku():PhaseChangeSkill("wanku"){
        frequency = Frequent;
    }

    virtual bool onPhaseChange(ServerPlayer *zhugeliang) const{
        if(zhugeliang->getPhase() == Player::Finish &&
           zhugeliang->getHp() > zhugeliang->getHandcardNum() &&
           zhugeliang->askForSkillInvoke(objectName())){
            zhugeliang->playSkillEffect(objectName());
            zhugeliang->drawCards(zhugeliang->getHp() - zhugeliang->getHandcardNum());
        }
        return false;
    }
};

QiaogongCard::QiaogongCard(){
    once = true;
}

void QiaogongCard::swapEquip(ServerPlayer *first, ServerPlayer *second, int index) const{
    const EquipCard *e1 = first->getEquip(index);
    const EquipCard *e2 = second->getEquip(index);

    Room *room = first->getRoom();

    if(e1)
        first->obtainCard(e1);

    if(e2)
        room->moveCardTo(e2, first, Player::Equip);

    if(e1)
        room->moveCardTo(e1, second, Player::Equip);
}

bool QiaogongCard::targetsFeasible(const QList<const Player *> &targets, const Player *Self) const{
    return targets.length() == 2;
}

bool QiaogongCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    switch(targets.length()){
    case 0: return true;
    case 1: {
            int n1 = targets.first()->getEquips().length();
            int n2 = to_select->getEquips().length();
            return qAbs(n1-n2) <= Self->getLostHp();
        }

    default:
        return false;
    }
}

void QiaogongCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    ServerPlayer *first = targets.first();
    ServerPlayer *second = targets.at(1);

    int i;
    for(i=0; i<4; i++)
        swapEquip(first, second, i);

    LogMessage log;
    log.type = "#QiaogongSwap";
    log.from = source;
    log.to = targets;
    room->sendLog(log);
}

class Qiaogong: public ZeroCardViewAsSkill{
public:
    Qiaogong():ZeroCardViewAsSkill("qiaogong"){

    }

    virtual const Card *viewAs() const{
        return new QiaogongCard;
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("QiaogongCard");
    }
};

class Manli: public TriggerSkill{
public:
    Manli():TriggerSkill("manli"){
        events << DamageProceed;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *turtle, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        const Card *reason = damage.card;
        if(reason == NULL)
            return false;
        if((reason->inherits("Slash") || reason->inherits("Duel"))
            && turtle->getWeapon() && turtle->getArmor()
            && turtle->askForSkillInvoke(objectName(), data)){
            room->playSkillEffect(objectName());
            LogMessage log;
            log.type = "#ManliBuff";
            log.from = turtle;
            log.to << damage.to;
            log.arg = QString::number(damage.damage);
            log.arg2 = QString::number(damage.damage + 1);
            room->sendLog(log);

            damage.damage ++;
            data = QVariant::fromValue(damage);
        }
        return false;
    }
};

class Kongying: public TriggerSkill{
public:
    Kongying():TriggerSkill("kongying"){
        events << CardResponsed;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        CardStar card_star = data.value<CardStar>();
        if(!card_star->inherits("Jink"))
            return false;

        if(player->askForSkillInvoke(objectName())){
            room->playSkillEffect(objectName());
            ServerPlayer *target = room->askForPlayerChosen(player, room->getOtherPlayers(player), objectName());
            if(!room->askForCard(target, "jink", "@kongying:" + player->objectName())){
                DamageStruct damage;
                damage.from = player;
                damage.to = target;
                room->damage(damage);
            }
        }
        return false;
    }
};

class Jibu: public ClientSkill{
public:
    Jibu():ClientSkill("jibu"){
    }

    virtual int getCorrect(const Player *from, const Player *to) const{
        if(to->hasSkill(objectName()))
            return +1;
        else if(from->hasSkill(objectName()))
            return -1;
        else
            return 0;
    }
};

BWQZPackage::BWQZPackage()
    :Package("BWQZ")
{
    General *houjian = new General(this, "houjian", "kou", 2);
    houjian->addSkill(new Yuanyin);

    General *mengkang = new General(this, "mengkang", "kou");
    mengkang->addSkill(new Zaochuan);
    mengkang->addSkill(new Mengchong);

    General *jiaoting = new General(this, "jiaoting", "kou");
    jiaoting->addSkill(new Skill("qinlong"));

    General *shantinggui = new General(this, "shantinggui", "jiang", 5, true, true);
    shantinggui->addSkill(new Xiaofang);

    General *kongliang = new General(this, "kongliang", "kou", 3);
    kongliang->addSkill(new Nusha);
    kongliang->addSkill(new Wanku);

    General *taozongwang = new General(this, "taozongwang", "min", 3);
    taozongwang->addSkill(new Qiaogong);
    taozongwang->addSkill(new Manli);

    General *wangdingliu = new General(this, "wangdingliu", "kou", 3);
    wangdingliu->addSkill(new Kongying);
    wangdingliu->addSkill(new Jibu);

    addMetaObject<YuanyinCard>();
    addMetaObject<NushaCard>();
    addMetaObject<QiaogongCard>();
}

ADD_PACKAGE(BWQZ);
