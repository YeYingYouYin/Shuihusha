#include "bwqz-package.h"
#include "general.h"
#include "skill.h"
#include "carditem.h"
#include "engine.h"
#include "standard.h"
#include "clientplayer.h"
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

YuanyinCard::YuanyinCard(){
}

bool YuanyinCard::targetsFeasible(const QList<const Player *> &targets, const Player *Self) const{
    return targets.length() == 2 || targets.length() == 1;
}

bool YuanyinCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(targets.isEmpty()){
        return to_select->getWeapon() && to_select != Self;
    }else if(targets.length() == 1){
        const Player *first = targets.first();
        return first != Self && first->getWeapon() && Self->canSlash(to_select);
    }else
        return false;
}

void YuanyinCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    const Card *weapon = targets.at(0)->getWeapon();
    ServerPlayer *target;
    if(targets.length() == 1){
        if(!Self->canSlash(targets.first()))
            return;
        else
            target = targets.first();
    }
    else
        target = targets.at(1);

    if(weapon){
        room->throwCard(weapon->getId());
        Slash *slash = new Slash(weapon->getSuit(), weapon->getNumber());
        slash->setSkillName("yuanyin");
        slash->addSubcard(weapon);
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

    virtual bool trigger(TriggerEvent, ServerPlayer *player, QVariant &data) const{
        QString asked = data.toString();
        if(asked != "slash" && asked != "jink")
            return false;
        Room *room = player->getRoom();
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
            int card_id = asked == "slash" ?
                          target->getWeapon()->getId() :
                          room->askForCardChosen(player, target, "e", objectName());
            if(asked == "jink" && target->getWeapon() && target->getWeapon()->getId() == card_id)
                return false;
            const Card *card = Sanguosha->getCard(card_id);
            if(asked == "slash"){
                Slash *yuanyin_card = new Slash(card->getSuit(), card->getNumber());
                yuanyin_card->setSkillName(objectName());
                yuanyin_card->addSubcard(card);
                room->provide(yuanyin_card);
            }
            else if(asked == "jink"){
                Jink *yuanyin_card = new Jink(card->getSuit(), card->getNumber());
                yuanyin_card->setSkillName(objectName());
                yuanyin_card->addSubcard(card);
                room->provide(yuanyin_card);
            }
        }
        return false;
    }
};

/*
class Weidi:public ZeroCardViewAsSkill{
public:
    Weidi():ZeroCardViewAsSkill("weidi"){
        frequency = Compulsory;
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->hasLordSkill("ji"jiang"") && Slash::IsAvailable(player);
    }

    virtual const Card *viewAs() const{
        return Sanguosha->cloneSkillCard("JijiangCard");
    }
};*/

class Xiaofang: public TriggerSkill{
public:
    Xiaofang():TriggerSkill("xiaofang"){
        events << Predamaged;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual int getPriority() const{
        return 2;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        Room *room = player->getRoom();
        ServerPlayer *water = room->findPlayerBySkillName(objectName());
        if(!water || water->isKongcheng() || damage.nature != DamageStruct::Fire)
            return false;
        if(water->askForSkillInvoke(objectName()) && room->askForDiscard(water, objectName(), 1)){
            LogMessage log;
            log.type = "#Xiaofang";
            log.from = water;
            log.arg = objectName();
            log.to << damage.to;
            room->sendLog(log);

            damage.nature = DamageStruct::Normal;
            data = QVariant::fromValue(damage);
        }
        return false;
    }
};

/*
class Xiuluo: public PhaseChangeSkill{
public:
    Xiuluo():PhaseChangeSkill("xiuluo"){

    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return PhaseChangeSkill::triggerable(target)
                && target->getPhase() == Player::Start
                && !target->isKongcheng()
                && !target->getJudgingArea().isEmpty();
    }

    virtual bool onPhaseChange(ServerPlayer *target) const{
        if(!target->askForSkillInvoke(objectName()))
            return false;

        Room *room = target->getRoom();
        int card_id = room->askForCardChosen(target, target, "j", objectName());
        const Card *card = Sanguosha->getCard(card_id);

        QString suit_str = card->getSuitString();
        QString pattern = QString(".%1").arg(suit_str.at(0).toUpper());
        QString prompt = QString("@xiuluo:::%1").arg(suit_str);
        if(room->askForCard(target, pattern, prompt)){
            room->throwCard(card);
        }

        return false;
    }
};

class Shenwei: public DrawCardsSkill{
public:
    Shenwei():DrawCardsSkill("shenwei"){
        frequency = Compulsory;
    }

    virtual int getDrawNum(ServerPlayer *player, int n) const{
        return n + 2;
    }
};

class Danji: public PhaseChangeSkill{
public:
    Danji():PhaseChangeSkill("danji"){
        frequency = Wake;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return PhaseChangeSkill::triggerable(target)
                && target->getPhase() == Player::Start
                && target->getMark("danji") == 0
                && target->getHandcardNum() > target->getHp();
    }

    virtual bool onPhaseChange(ServerPlayer *guanyu) const{
        Room *room = guanyu->getRoom();
        ServerPlayer *the_lord = room->getLord();
        if(the_lord && the_lord->isCaoCao()){
            LogMessage log;
            log.type = "#DanjiWake";
            log.from = guanyu;
            log.arg = QString::number(guanyu->getHandcardNum());
            log.arg2 = QString::number(guanyu->getHp());
            room->sendLog(log);

            guanyu->setMark("danji", 1);

            room->loseMaxHp(guanyu);
            room->acquireSkill(guanyu, "mashu");
        }

        return false;
    }
};

SPCardPackage::SPCardPackage()
    :Package("sp_cards")
{
    (new SPMoonSpear)->setParent(this);

    type = CardPack;
}

ADD_PACKAGE(SPCard)
*/

class Zhuying: public FilterSkill{
public:
    Zhuying():FilterSkill("zhuying"){

    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return !to_select->isEquipped() && to_select->getCard()->objectName() == "analeptic";
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *c = card_item->getCard();
        Peach *peach = new Peach(c->getSuit(), c->getNumber());
        peach->setSkillName(objectName());
        peach->addSubcard(card_item->getCard());

        return peach;
    }
};

class Banzhuang: public OneCardViewAsSkill{
public:
    Banzhuang():OneCardViewAsSkill("banzhuang"){
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return !to_select->isEquipped() && to_select->getFilteredCard()->getSuit() == Card::Heart;
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *card = card_item->getCard();
        ExNihilo *ex_nihilo = new ExNihilo(card->getSuit(), card->getNumber());
        ex_nihilo->addSubcard(card->getId());
        ex_nihilo->setSkillName(objectName());
        return ex_nihilo;
    }
};

ShougeCard::ShougeCard(){
    will_throw = false;
    target_fixed = true;
    mute = true;
}

void ShougeCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->playSkillEffect("shouge", qrand() % 2 + 1);
    source->addToPile("vege", this->getSubcards().first());
}

class ShougeViewAsSkill: public OneCardViewAsSkill{
public:
    ShougeViewAsSkill():OneCardViewAsSkill("shouge"){

    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getCard()->inherits("Peach") ||
                to_select->getCard()->inherits("Analeptic");
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        ShougeCard *card = new ShougeCard;
        card->addSubcard(card_item->getFilteredCard());
        return card;
    }
};

class Shouge: public TriggerSkill{
public:
    Shouge():TriggerSkill("shouge"){
        view_as_skill = new ShougeViewAsSkill;
        events << CardLost << HpLost;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target->getPhase() == Player::NotActive;
    }

    static bool doDraw(Room *room, ServerPlayer *vgqq){
        room->throwCard(vgqq->getPile("vege").last());
        room->playSkillEffect("shouge", qrand() % 2 + 3);
        vgqq->drawCards(3);
        return vgqq->getPile("vege").isEmpty();
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *vgqq, QVariant &data) const{
        Room *room = vgqq->getRoom();
        if(vgqq->getPile("vege").isEmpty())
            return false;
        if(event == CardLost){
            CardMoveStar move = data.value<CardMoveStar>();
            if(move->from_place == Player::Hand && move->to != vgqq
               && vgqq->askForSkillInvoke(objectName())){
                doDraw(room, vgqq);
            }
        }
        else{
            int lose = data.toInt();
            for(; lose > 0; lose --){
                if(vgqq->askForSkillInvoke(objectName()))
                    if(doDraw(room, vgqq))
                        break;
            }
        }
        return false;
    }
};

class Qiongtu: public PhaseChangeSkill{
public:
    Qiongtu():PhaseChangeSkill("qiongtu"){
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return !target->hasSkill(objectName());
    }

    virtual bool onPhaseChange(ServerPlayer *target) const{
        Room *room = target->getRoom();
        ServerPlayer *zhangqing = room->findPlayerBySkillName(objectName());
        if(zhangqing && target->getPhase() == Player::Finish){
            if(target->getHandcardNum() <= 1 && !target->isNude()
                && zhangqing->askForSkillInvoke(objectName())){
                room->playSkillEffect(objectName());
                int card_id = room->askForCardChosen(zhangqing, target, "he", objectName());
                room->obtainCard(zhangqing, card_id);
            }
        }
        return false;
    }
};

BWQZPackage::BWQZPackage()
    :Package("BWQZ")
{
    General *houjian = new General(this, "houjian", "min", 3);
    houjian->addSkill(new Yuanyin);

    General *mengkang = new General(this, "mengkang", "kou", 4);
    mengkang->addSkill(new Zaochuan);
    mengkang->addSkill(new Skill("mengchong", Skill::Compulsory));

    General *jiaoting = new General(this, "jiaoting", "kou", 4);
    jiaoting->addSkill(new Skill("qinlong"));

    General *shantinggui = new General(this, "shantinggui", "jiang", 5);
    shantinggui->addSkill(new Xiaofang);

    General *qingzhang = new General(this, "qingzhang", "kou", 3);
    qingzhang->addSkill(new Shouge);
    qingzhang->addSkill(new Qiongtu);

    General *jiashi = new General(this, "jiashi", "min", 3, false);
    jiashi->addSkill(new Zhuying);
    jiashi->addSkill(new Banzhuang);

    addMetaObject<YuanyinCard>();
    addMetaObject<ShougeCard>();
}

ADD_PACKAGE(BWQZ);
