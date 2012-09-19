#include "tiger.h"
#include "skill.h"
#include "standard.h"
#include "plough.h"
#include "maneuvering.h"
#include "clientplayer.h"
#include "carditem.h"
#include "engine.h"
/*
TaolueCard::TaolueCard(){
    once = true;
    mute = true;
    will_throw = false;
}

bool TaolueCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && !to_select->isKongcheng() && to_select != Self;
}

void TaolueCard::use(Room *room, ServerPlayer *player, const QList<ServerPlayer *> &targets) const{
    bool success = player->pindian(targets.first(), "Taolue", this);
    if(!success){
        room->playSkillEffect("taolue", 2);
        if(!player->isNude())
            room->askForDiscard(player, "taolue", 1, false, true);
        return;
    }
    room->playSkillEffect("taolue", 1);
    PlayerStar from = targets.first();
    if(from->getCards("ej").isEmpty())
        return;

    int card_id = room->askForCardChosen(player, from , "ej", "taolue");
    const Card *card = Sanguosha->getCard(card_id);
    Player::Place place = room->getCardPlace(card_id);

    int equip_index = -1;
    const DelayedTrick *trick = NULL;
    if(place == Player::Equip){
        const EquipCard *equip = qobject_cast<const EquipCard *>(card);
        equip_index = static_cast<int>(equip->location());
    }else{
        trick = DelayedTrick::CastFrom(card);
    }

    QList<ServerPlayer *> tos;
    foreach(ServerPlayer *p, room->getAlivePlayers()){
        if(equip_index != -1){
            if(p->getEquip(equip_index) == NULL)
                tos << p;
        }else{
            if(!player->isProhibited(p, trick) && !p->containsTrick(trick->objectName(), false))
                tos << p;
        }
    }
    if(trick && trick->isVirtualCard())
        delete trick;

    room->setTag("TaolueTarget", QVariant::fromValue(from));
    ServerPlayer *to = room->askForPlayerChosen(player, tos, "qiaobian");
    if(to)
        room->moveCardTo(card, to, place);
    room->removeTag("TaolueTarget");
}

class Taolue: public OneCardViewAsSkill{
public:
    Taolue():OneCardViewAsSkill("taolue"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->hasUsed("TaolueCard") && !player->isKongcheng();
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return !to_select->isEquipped();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        Card *card = new TaolueCard;
        card->addSubcard(card_item->getFilteredCard());
        return card;
    }
};

class Changsheng: public TriggerSkill{
public:
    Changsheng():TriggerSkill("changsheng"){
        events << Pindian;
        frequency = Compulsory;
    }

    virtual int getPriority() const{
        return -1;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *, QVariant &data) const{
        QList<ServerPlayer *> aokoo = room->findPlayersBySkillName(objectName());
        if(aokoo.isEmpty())
            return false;
        PindianStar pindian = data.value<PindianStar>();
        if(!aokoo.contains(pindian->from) && !aokoo.contains(pindian->to))
            return false;
        Card *pdcd;
        foreach(ServerPlayer *aoko, aokoo){
            bool invoke = false;
            if(pindian->from != aoko && pindian->to != aoko)
                continue;
            if(pindian->from != aoko && pindian->to_card->getSuit() == Card::Spade){
                pdcd = Sanguosha->cloneCard(pindian->to_card->objectName(), pindian->to_card->getSuit(), 13);
                pdcd->addSubcard(pindian->to_card);
                pdcd->setSkillName(objectName());
                pindian->to_card = pdcd;
                invoke = true;
                room->playSkillEffect(objectName(), 2);
            }
            else if(pindian->to != aoko && pindian->from_card->getSuit() == Card::Spade){
                pdcd = Sanguosha->cloneCard(pindian->from_card->objectName(), pindian->from_card->getSuit(), 13);
                pdcd->addSubcard(pindian->from_card);
                pdcd->setSkillName(objectName());
                pindian->from_card = pdcd;
                invoke = true;
                room->playSkillEffect(objectName(), 1);
            }

            if(invoke){
                LogMessage log;
                log.type = "#Changsheng";
                log.from = aoko;
                log.arg = objectName();
                room->sendLog(log);
            }

            data = QVariant::fromValue(pindian);
        }
        return false;
    }
};

class Zhanchi:public PhaseChangeSkill{
public:
    Zhanchi():PhaseChangeSkill("zhanchi"){
        frequency = Limited;
    }

    virtual bool onPhaseChange(ServerPlayer *opt) const{
        if(opt->getMark("@wings") > 0 && opt->getPhase() == Player::Judge){
            Room *room = opt->getRoom();
            if(opt->askForSkillInvoke(objectName())){
                room->playSkillEffect(objectName(), 1);
                room->broadcastInvoke("animate", "lightbox:$zhanchi");
                while(!opt->getJudgingArea().isEmpty())
                    room->throwCard(opt->getJudgingArea().first()->getId());
                room->acquireSkill(opt, "tengfei");
                opt->loseMark("@wings");
            }
        }
        return false;
    }
};

class Tengfei: public ClientSkill{
public:
    Tengfei():ClientSkill("tengfei"){
    }

    virtual int getAtkrg(const Player *op) const{
        return op->getHp();
    }
};

class TengfeiMain:public PhaseChangeSkill{
public:
    TengfeiMain():PhaseChangeSkill("#tengfei_main"){
        frequency = Compulsory;
    }

    virtual bool onPhaseChange(ServerPlayer *opt) const{
        if(opt->getPhase() == Player::NotActive){
            Room *room = opt->getRoom();
            if(opt->getMaxHP() > 3)
                room->playSkillEffect(objectName(), 1);
            else if(opt->getMaxHP() > 1)
                room->playSkillEffect(objectName(), 2);
            room->loseMaxHp(opt);

            if(opt->isAlive()){
                LogMessage log;
                log.type = "#Tengfei";
                log.from = opt;
                log.arg = objectName();
                room->sendLog(log);

                opt->gainAnExtraTurn(opt);
            }
        }
        return false;
    }
};
*/
class Guzong: public TriggerSkill{
public:
    Guzong():TriggerSkill("guzong"){
        events << CardLost;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target != NULL;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *, QVariant &data) const{
        ServerPlayer *leiheng = room->findPlayerBySkillName(objectName());
        ServerPlayer *current = room->getCurrent();

        if(!leiheng || leiheng == current)
            return false;
        if(current->getPhase() == Player::Discard){
            QVariantList guzong = leiheng->tag["Guzong"].toList();

            CardMoveStar move = data.value<CardMoveStar>();
                guzong << move->card_id;

            leiheng->tag["Guzong"] = guzong;
        }

        return false;
    }
};

class GuzongGet: public PhaseChangeSkill{
public:
    GuzongGet():PhaseChangeSkill("#guzong-get"){

    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target != NULL && !target->hasSkill("guzong");
    }

    virtual int getPriority() const{
        return -1;
    }

    virtual bool onPhaseChange(ServerPlayer *player) const{
        if(player->isDead() || !player->isWounded() || player->getPhase() != Player::Finish)
            return false;

        Room *room = player->getRoom();
        ServerPlayer *leiheng = room->findPlayerBySkillName("guzong");
        if(!leiheng)
            return false;

        QVariantList guzong_cards = leiheng->tag["Guzong"].toList();
        leiheng->tag.remove("Guzong");

        QList<int> cards;
        foreach(QVariant card_data, guzong_cards){
            int card_id = card_data.toInt();
            if(room->getCardPlace(card_id) == Player::DiscardedPile)
                cards << card_id;
        }

        if(cards.isEmpty())
            return false;

        if(!leiheng->isNude() && leiheng->askForSkillInvoke("guzong", QVariant::fromValue(cards.length()))){
            room->fillAG(cards, leiheng);

            while(!leiheng->isNude() && !cards.isEmpty()){
                int to_back = room->askForAG(leiheng, cards, true, "guzong");
                if(to_back < 0)
                    break;
                room->askForDiscard(leiheng, "guzong", 1, false, true);
                player->obtainCard(Sanguosha->getCard(to_back));
                room->throwCard(room->askForCardChosen(leiheng, player, "he", "guzong"), leiheng);
                cards.removeOne(to_back);
                room->broadcastInvoke("clearAG");
                room->fillAG(cards, leiheng);
            }

            leiheng->invoke("clearAG");

            foreach(int card_id, cards)
                room->throwCard(card_id);
        }
        return false;
    }
};

class Neiying:public ViewAsSkill{
public:
    Neiying():ViewAsSkill("neiying"){
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        if(selected.isEmpty())
            return true;
        else if(selected.length() == 1){
            const Card *card = selected.first()->getCard();
            return to_select->getCard()->getColor() == card->getColor();
        }else
            return false;
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.length() == 2){
            const Card *first = cards.first()->getCard();
            Counterplot *aa = new Counterplot(first->getSuit(), 0);
            aa->addSubcards(cards);
            aa->setSkillName(objectName());
            return aa;
        }else
            return NULL;
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *, const QString &pattern) const{
        //return pattern == "nullification";
        return pattern == "counterplot";
    }
};

JintangCard::JintangCard(){
}

bool JintangCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && to_select != Self;
}

void JintangCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();
    CardStar card = effect.from->tag["Jintg"].value<CardStar>();
    const EquipCard *equipped = qobject_cast<const EquipCard *>(card);
    QList<ServerPlayer *> targets;
    targets << effect.to;
    equipped->use(room, effect.from, targets);
}

class JintangViewAsSkill: public ZeroCardViewAsSkill{
public:
    JintangViewAsSkill():ZeroCardViewAsSkill("jintang"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return pattern == "@@jintang";
    }

    virtual const Card *viewAs() const{
        return new JintangCard;
    }
};

class Jintang: public TriggerSkill{
public:
    Jintang(): TriggerSkill("jintang"){
        events << Predamaged << Death;
        view_as_skill = new JintangViewAsSkill;
        frequency = Compulsory;
    }

    virtual bool trigger(TriggerEvent event, Room* room, ServerPlayer *player, QVariant &data) const{
        if(event == Predamaged){
            DamageStruct damage = data.value<DamageStruct>();
            LogMessage log;
            log.from = player;
            log.arg = objectName();
            if(player->getHp() == 1 && damage.nature == DamageStruct::Normal){
                log.type = "#JintangForb";
                room->sendLog(log);
                return true;
            }
            if(player->getHp() <= 2 && damage.damage > 1){
                log.type = "#JintangCut";
                log.arg2 = QString::number(damage.damage);
                room->sendLog(log);

                damage.damage = 1;
                data = QVariant::fromValue(damage);
            }
        }
        else if(event == Death){
            foreach(CardStar equip, player->getEquips()){
                player->tag["Jintg"] = QVariant::fromValue(equip);
                room->askForUseCard(player, "@@jintang", "@jintang:" + equip->objectName(), true);
            }
        }
        return false;
    }
};

class Wuzhou:public TriggerSkill{
public:
    Wuzhou():TriggerSkill("wuzhou"){
        events << CardLostDone;
        frequency = Frequent;
    }

    virtual int getPriority() const{
        return -1;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        if(room->getCurrent() == player && player->getPhase() == Player::Play){
            if(player->getEquips().isEmpty())
                return false;
            int x = 5 - player->getEquips().count();
            if(player->getHandcardNum() < x && player->askForSkillInvoke(objectName()))
                player->drawCards(x - player->getHandcardNum());
        }
        return false;
    }
};

HuweiCard::HuweiCard(){
    will_throw = false;
}

void HuweiCard::onEffect(const CardEffectStruct &effect) const{
    ServerPlayer *frog = effect.to;
    if(frog->hasLordSkill("huwei")){
        const Card *card = Sanguosha->getCard(getSubcards().first());
        const EquipCard *equipped = qobject_cast<const EquipCard *>(card);
        QList<ServerPlayer *> targets;
        targets << frog;
        equipped->use(frog->getRoom(), effect.from, targets);
        effect.from->drawCards(1);
    }
}

bool HuweiCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && to_select->hasLordSkill("huwei")
            && to_select != Self && to_select->getEquips().isEmpty();
}

class HuweiViewAsSkill: public OneCardViewAsSkill{
public:
    HuweiViewAsSkill():OneCardViewAsSkill("huweiv"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->getKingdom() == "jiang";
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getCard()->inherits("EquipCard");
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        HuweiCard *card = new HuweiCard;
        card->addSubcard(card_item->getFilteredCard());
        return card;
    }
};

class Huwei: public GameStartSkill{
public:
    Huwei():GameStartSkill("huwei$"){
    }

    virtual void onGameStart(ServerPlayer *tigger) const{
        if(!tigger->isLord())
            return;
        Room *room = tigger->getRoom();
        QList<ServerPlayer *> players = room->getAlivePlayers();
        foreach(ServerPlayer *player, players){
            room->attachSkillToPlayer(player, "huweiv");
        }
    }

    virtual void onIdied(ServerPlayer *tigger) const{
        Room *room = tigger->getRoom();
        if(room->findPlayerBySkillName("huwei"))
            return;
        QList<ServerPlayer *> players = room->getAlivePlayers();
        foreach(ServerPlayer *player, players){
            room->detachSkillFromPlayer(player, "huweiv", false);
        }
    }
};

class Jielue:public TriggerSkill{
public:
    Jielue():TriggerSkill("jielue"){
        events << FinishJudge;
    }

    virtual bool triggerable(const ServerPlayer *tat) const{
        return !tat->hasSkill(objectName());
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        //JudgeStar judge = data.value<JudgeStar>();
        QList<ServerPlayer *> zhangs = room->findPlayersBySkillName(objectName());
        foreach(ServerPlayer *zhah, zhangs){
            if(zhah->getMark("fuhun") > 0 || player->isKongcheng())
                continue;
            if(zhah->askForSkillInvoke(objectName(), data))
                room->obtainCard(zhah, room->askForCardChosen(zhah, player, "h", objectName()), false);
        }
        return false;
    }
};

class Fuhun: public TriggerSkill{
public:
    Fuhun():TriggerSkill("fuhun"){
        events << Death;
        frequency = Wake;
    }

    virtual int getPriority() const{
        return -1;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return !target->hasSkill(objectName());
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        QList<ServerPlayer *> zhangs = room->findPlayersBySkillName(objectName());
        foreach(ServerPlayer *zhang, zhangs){
            if(zhang->getMark("fuhun") > 0)
                continue;
            LogMessage log;
            log.type = "#WakeUp";
            log.from = zhang;
            log.arg = objectName();
            room->sendLog(log);
            room->playSkillEffect(objectName());
            RecoverStruct r;
            room->recover(zhang, r);
            room->broadcastInvoke("animate", "lightbox:$fuhun:1500");
            room->getThread()->delay(1500);

            room->acquireSkill(zhang, "lihun");
            foreach(const Skill *skill, player->getVisibleSkillList()){
                if(skill->getLocation() == Skill::Right &&
                   skill->getFrequency() != Skill::Limited &&
                   skill->getFrequency() != Skill::Wake &&
                   !skill->isLordSkill()){
                    room->acquireSkill(zhang, skill->objectName());
                }
            }
            room->detachSkillFromPlayer(zhang, "jielue");
            room->setPlayerMark(zhang, "fuhun", 1);
        }
        return false;
    }
};

class Liehuo: public TriggerSkill{
public:
    Liehuo():TriggerSkill("liehuo"){
        events << SlashMissed << Damage;
    }

    virtual bool trigger(TriggerEvent event, Room* room, ServerPlayer *bao, QVariant &data) const{
        PlayerStar target;
        if(event == SlashMissed){
            SlashEffectStruct effect = data.value<SlashEffectStruct>();
            target = effect.to;
        }
        else{
            DamageStruct damage = data.value<DamageStruct>();
            if(damage.to && damage.card->inherits("Slash"))
                target = damage.to;
            else
                return false;
        }
        if(target && !target->isKongcheng() &&
           target->getHandcardNum() > bao->getHandcardNum() &&
           room->askForSkillInvoke(bao, objectName(), QVariant::fromValue(target))){
            room->playSkillEffect(objectName());
            bao->obtainCard(target->getRandomHandCard(), false);
        }
        return false;
    }
};

class Longluo:public TriggerSkill{
public:
    Longluo():TriggerSkill("longluo"){
        events << CardEffected;
        frequency = Frequent;
    }

    virtual int getPriority() const{
        return 3;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *shien, QVariant &data) const{
        CardEffectStruct effect = data.value<CardEffectStruct>();
        if(!effect.card->inherits("Slash"))
            return false;

        if(room->askForSkillInvoke(shien, objectName(), data)){
            room->playSkillEffect(objectName());
            for(int i = 0; i < 2; i++){
                int card_id = room->drawCard();
                room->moveCardTo(Sanguosha->getCard(card_id), NULL, Player::Special, true);
                room->getThread()->delay();

                CardStar card = Sanguosha->getCard(card_id);
                LogMessage lolo;
                lolo.from = shien;
                lolo.card_str = card->getEffectIdString();
                if(!card->inherits("BasicCard")){
                    lolo.type = "$Longluo1";
                    room->throwCard(card_id);
                    room->sendLog(lolo);
                }else{
                    lolo.type = "$Longluo2";
                    shien->tag["LongluoCard"] = QVariant::fromValue(card);
                    room->sendLog(lolo);
                    ServerPlayer *target = room->askForPlayerChosen(shien, room->getAllPlayers(), objectName());
                    if(!target)
                        target = shien;
                    room->obtainCard(target, card_id);
                    shien->tag.remove("LongluoCard");
                }
            }
        }
        return false;
    }
};

XiaozaiCard::XiaozaiCard(){
    will_throw = false;
}

bool XiaozaiCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(!targets.isEmpty())
        return false;

    if(to_select == Self)
        return false;

    return !to_select->hasFlag("Xiaozai");
}

void XiaozaiCard::onEffect(const CardEffectStruct &effect) const{
    effect.to->getRoom()->obtainCard(effect.to, this, false);
    PlayerStar target = effect.to;
    effect.from->tag["Xiaozai"] = QVariant::fromValue(target);
}

class XiaozaiViewAsSkill: public ViewAsSkill{
public:
    XiaozaiViewAsSkill():ViewAsSkill("xiaozai"){
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        return !to_select->isEquipped() && selected.length() < 2;
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return pattern == "@@xiaozai";
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.length() != 2)
            return NULL;
        XiaozaiCard *card = new XiaozaiCard;
        card->addSubcards(cards);
        return card;
    }
};

class Xiaozai: public TriggerSkill{
public:
    Xiaozai():TriggerSkill("xiaozai"){
        events << DamagedProceed;
        view_as_skill = new XiaozaiViewAsSkill;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        if(damage.from)
            room->setPlayerFlag(damage.from, "Xiaozai");
        if(player->getHandcardNum() > 1 && room->askForUseCard(player, "@@xiaozai", "@xiaozai", true)){
            ServerPlayer *cup = player->tag["Xiaozai"].value<PlayerStar>();
            if(cup){
                damage.to = cup;
                room->damage(damage);
                return true;
            }
        }
        if(damage.from){
            room->setPlayerFlag(damage.from, "-Xiaozai");
            player->tag.remove("Xiaozai");
        }
        return false;
    }
};

class Huxiao: public OneCardViewAsSkill{
public:
    Huxiao():OneCardViewAsSkill("huxiao"){

    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getFilteredCard()->inherits("EquipCard");
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *card = card_item->getCard();
        SavageAssault *savage_assault = new SavageAssault(card->getSuit(), card->getNumber());
        savage_assault->addSubcard(card->getId());
        savage_assault->setSkillName(objectName());
        return savage_assault;
    }
};

class Tanse:public TriggerSkill{
public:
    Tanse():TriggerSkill("tanse"){
        events << CardEffected;
    }

    virtual int getPriority() const{
        return 2;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        CardEffectStruct effect = data.value<CardEffectStruct>();
        if(effect.card->isNDTrick() && effect.from->getGeneral()->isFemale()){
            QString choice = effect.from->getEquips().isEmpty() ? "tan1+cancel" : "tan1+se2+cancel";
            choice = room->askForChoice(player, objectName(), choice);
            if(choice == "cancel")
                return false;
            LogMessage log;
            log.type = "#Tanse";
            log.from = player;
            log.to << effect.from;
            log.arg = objectName();
            log.arg2 = choice;
            room->sendLog(log);
            player->drawCards(1);
            if(choice == "tan1"){
                const Card *equip = room->askForCard(player, "EquipCard", "@tanse:" + effect.from->objectName(), false, data, NonTrigger);
                if(equip)
                    effect.from->obtainCard(equip);
                else
                    return false;
            }
            else
                room->obtainCard(player, room->askForCardChosen(player, effect.from, "e", objectName()));
        }
        return false;
    }
};

class HoufaViewAsSkill: public ViewAsSkill{
public:
    HoufaViewAsSkill():ViewAsSkill("houfa"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return Slash::IsAvailable(player);
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        if(selected.length() >= 2)
            return false;
        return to_select->getCard()->inherits("Slash");
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.length() != 2)
            return NULL;
        const Card *card = cards.first()->getCard();
        Slash *slash = new Slash(card->getSuit(), 0);
        slash->addSubcards(cards);
        slash->setSkillName(objectName());
        return slash;
    }
};

class Houfa: public TriggerSkill{
public:
    Houfa():TriggerSkill("houfa"){
        events << CardDiscarded;
        //frequency = Frequent;
        view_as_skill = new HoufaViewAsSkill;
    }

    virtual bool triggerable(const ServerPlayer *pa) const{
        return !pa->hasSkill(objectName());
    }

    virtual int getPriority() const{
        return 3;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        ServerPlayer *selang = room->findPlayerBySkillName(objectName());
        if(!selang)
            return false;
        CardStar slash = data.value<CardStar>();
        if(slash->isVirtualCard()){
            bool hasslash = false;
            foreach(int card_id, slash->getSubcards()){
                if(Sanguosha->getCard(card_id)->inherits("Slash")){
                    hasslash = true;
                    break;
                }
            }
            if(hasslash && selang->askForSkillInvoke(objectName())){
                room->playSkillEffect(objectName());
                foreach(int card_id, slash->getSubcards()){
                    if(Sanguosha->getCard(card_id)->inherits("Slash"))
                        room->obtainCard(selang, card_id);
                }
            }
        }
        else if(slash->inherits("Slash") && selang->askForSkillInvoke(objectName())){
            room->playSkillEffect(objectName());
            selang->obtainCard(slash);
        }
        return false;
    }
};

class HoufaSlash: public TriggerSkill{
public:
    HoufaSlash():TriggerSkill("#houfa-slash"){
        events << SlashProceed;
    }

    virtual bool trigger(TriggerEvent , Room* room, ServerPlayer *, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();
        if(effect.slash->getSkillName() == "houfa"){
            room->slashResult(effect, NULL);
            return true;
        }
        return false;
    }
};

class Losthp: public TriggerSkill{
public:
    Losthp():TriggerSkill("#losthp"){
        events << GameStart;
    }

    virtual int getPriority() const{
        return -1;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        room->setPlayerProperty(player, "hp", player->getHp() - 1);
        return false;
    }
};

class Linse: public ClientSkill{
public:
    Linse():ClientSkill("linse"){
    }

    virtual bool isProhibited(const Player *, const Player *, const Card *card) const{
        return card->inherits("Snatch") || card->inherits("Dismantlement");
    }

    virtual int getExtra(const Player *target) const{
        if(target->hasSkill(objectName()))
            return target->getMaxHp();
        else
            return 0;
    }
};

class LinseEffect: public PhaseChangeSkill{
public:
    LinseEffect():PhaseChangeSkill("#linse_effect"){
    }

    virtual bool onPhaseChange(ServerPlayer *lz) const{
        if(lz->getPhase() == Player::Discard &&
           lz->getHandcardNum() > lz->getHp() && lz->getHandcardNum() <= lz->getMaxCards())
            lz->getRoom()->playSkillEffect("linse");
        return false;
    }
};
/*
class Yueli:public TriggerSkill{
public:
    Yueli():TriggerSkill("yueli"){
        frequency = Frequent;
        events << FinishJudge;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *yuehe, QVariant &data) const{
        JudgeStar judge = data.value<JudgeStar>();
        CardStar card = judge->card;

        QVariant data_card = QVariant::fromValue(card);
        if(judge->card->inherits("BasicCard") && yuehe->askForSkillInvoke(objectName(), data_card)){
            if(card->objectName() == "shit"){
                QString result = room->askForChoice(yuehe, objectName(), "yes+no");
                if(result == "no"){
                    room->playSkillEffect(objectName(), 2);
                    return false;
                }
            }
            yuehe->obtainCard(judge->card);
            if(judge->reason != "taohui")
                room->playSkillEffect(objectName(), 1);
            return true;
        }
        if(judge->reason != "taohui")
            room->playSkillEffect(objectName(), 2);
        return false;
    }
};

class Taohui:public TriggerSkill{
public:
    Taohui():TriggerSkill("taohui"){
        events << PhaseChange << FinishJudge;
        frequency = Frequent;
    }

    virtual bool trigger(TriggerEvent event, Room* room, ServerPlayer *yuehe, QVariant &data) const{
        if(event == PhaseChange && yuehe->getPhase() == Player::Finish){
            while(yuehe->askForSkillInvoke(objectName())){
                room->playSkillEffect(objectName());

                JudgeStruct judge;
                judge.reason = objectName();
                judge.who = yuehe;
                judge.time_consuming = true;

                room->judge(judge);
                if(judge.card->inherits("BasicCard"))
                    break;
            }
        }
        else if(event == FinishJudge){
            JudgeStar judge = data.value<JudgeStar>();
            if(judge->reason == objectName()){
                if(!judge->card->inherits("BasicCard")){
                    Room *room = yuehe->getRoom();
                    room->throwCard(judge->card->getId());
                    ServerPlayer *target = room->askForPlayerChosen(yuehe, room->getAllPlayers(), objectName());
                    target->drawCards(1);
                    return true;
                }
            }
        }
        return false;
    }
};

class Wuzu: public TriggerSkill{
public:
    Wuzu():TriggerSkill("wuzu"){
        events << CardUsed << CardFinished;
        frequency = Compulsory;
    }

    static bool isWuzuEffectCard(CardStar card){
        return card->inherits("Slash") ||
                card->inherits("AOE") ||
                card->inherits("FireAttack");
    }

    virtual bool trigger(TriggerEvent e, Room* room, ServerPlayer *player, QVariant &data) const{
        CardUseStruct use = data.value<CardUseStruct>();
        if(e == CardUsed){
            bool play = false;
            foreach(ServerPlayer *tmp, use.to){
                if(tmp->getArmor()){
                    tmp->addMark("qinggang");
                    LogMessage log;
                    log.type = "$IgnoreArmor";
                    log.from = player;
                    log.to << tmp;
                    log.card_str = tmp->getArmor()->getEffectIdString();
                    room->sendLog(log);

                    play = true;
                }
            }
            if(play && isWuzuEffectCard(use.card))
                room->playSkillEffect(objectName());
        }
        else{
            foreach(ServerPlayer *tmp, use.to)
                tmp->removeMark("qinggang");
        }
        return false;
    }
};

class Qiangqu:public TriggerSkill{
public:
    Qiangqu():TriggerSkill("qiangqu"){
        events << DamageProceed;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        if(damage.card && damage.card->inherits("Slash") && damage.to->getGeneral()->isFemale()
            && damage.to->isWounded() && !damage.to->isNude() && player->askForSkillInvoke(objectName(), data)){
            int card_id = room->askForCardChosen(damage.from, damage.to, "he", objectName());
            RecoverStruct re;
            re.card = Sanguosha->getCard(card_id);
            re.who = player;
            room->obtainCard(player, card_id, false);

            LogMessage log;
            log.from = player;
            log.type = "#Qiangqu";
            log.to << damage.to;
            room->sendLog(log);
            room->recover(damage.to, re, true);
            room->playSkillEffect(objectName());
            room->recover(damage.from, re, true);
            return true;
        }
        return false;
    }
};

class Huatian:public TriggerSkill{
public:
    Huatian():TriggerSkill("huatian"){
        events << Damaged << HpRecovered;
    }

    virtual bool trigger(TriggerEvent event, Room* room, ServerPlayer *player, QVariant &data) const{
        if(event == Damaged){
            DamageStruct damage = data.value<DamageStruct>();
            for(int i = 0; i < damage.damage; i++){
                QList<ServerPlayer *> wounders;
                foreach(ServerPlayer *tmp, room->getOtherPlayers(damage.to)){
                    if(tmp->isWounded())
                        wounders << tmp;
                }
                if(!wounders.isEmpty()){
                    room->setPlayerMark(player, "HBTJ", 1);
                    if(!damage.to->askForSkillInvoke(objectName())){
                        room->setPlayerMark(player, "HBTJ", 0);
                        break;
                    }
                    ServerPlayer *target = room->askForPlayerChosen(player, wounders, objectName());
                    room->setPlayerMark(player, "HBTJ", 0);
                    RecoverStruct recovvv;
                    recovvv.who = player;
                    room->playSkillEffect(objectName(), qrand() % 2 + 1);
                    room->recover(target, recovvv, true);
                }
            }
            return false;
        }
        RecoverStruct rec = data.value<RecoverStruct>();
        for(int i = rec.recover; i > 0; i--){
            if(!player->askForSkillInvoke(objectName()))
                break;
            room->setPlayerMark(player, "HBTJ", 2);
            ServerPlayer *target = room->askForPlayerChosen(player, room->getOtherPlayers(player), objectName());
            room->setPlayerMark(player, "HBTJ", 0);

            room->playSkillEffect(objectName(), qrand() % 2 + 3);
            DamageStruct damage;
            damage.from = player;
            damage.to = target;
            room->damage(damage);
        }
        return false;
    }
};

class Tiansuan: public TriggerSkill{
public:
    Tiansuan():TriggerSkill("tiansuan"){
        events << Pindian;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *, QVariant &data) const{
        ServerPlayer *pianzi = room->findPlayerBySkillName(objectName());
        if(!pianzi || !pianzi->askForSkillInvoke(objectName()))
            return false;
        PindianStar pindian = data.value<PindianStar>();
        QStringList choices;
        QString from_card = Sanguosha->translate(QString("@tiansuan:%1:%2:%3:%4").
                            arg(pindian->from->getGeneralName()).
                            arg(pindian->from_card->getSuitString()).
                            arg(pindian->from_card->getNumberString()).
                            arg(pindian->from_card->objectName()));
        QString to_card = Sanguosha->translate(QString("@tiansuan:%1:%2:%3:%4").
                          arg(pindian->to->getGeneralName()).
                          arg(pindian->to_card->getSuitString()).
                          arg(pindian->to_card->getNumberString()).
                          arg(pindian->to_card->objectName()));
        choices << from_card
                << to_card;
        QString choice = room->askForChoice(pianzi, objectName(), choices.join("+"));
        if(choice == from_card){
            int omiga = room->drawCard();
            //room->moveCardTo(Sanguosha->getCard(omiga), pindian->from, Player::Hand, false);
            room->moveCardTo(pindian->from_card, NULL, Player::DrawPile, true);
            pindian->from_card = Sanguosha->getCard(omiga);
        }
        else{
            int omiga = room->drawCard();
            //room->moveCardTo(Sanguosha->getCard(omiga), pindian->to, Player::Hand, false);
            room->moveCardTo(pindian->to_card, NULL, Player::DrawPile, true);
            pindian->to_card = Sanguosha->getCard(omiga);
        }
        data = QVariant::fromValue(pindian);
        return false;
    }
};

HuazhuCard::HuazhuCard(){
    once = true;
    will_throw = false;
}

bool HuazhuCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && !to_select->isKongcheng() && to_select != Self;
}

void HuazhuCard::use(Room *, ServerPlayer *iszjj, const QList<ServerPlayer *> &targets) const{
    iszjj->pindian(targets.first(), "huazhu", this);
}

class HuazhuViewAsSkill: public OneCardViewAsSkill{
public:
    HuazhuViewAsSkill():OneCardViewAsSkill("huazhu"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->hasUsed("HuazhuCard") && !player->isKongcheng();
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return !to_select->isEquipped();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        Card *card = new HuazhuCard;
        card->addSubcard(card_item->getFilteredCard());
        return card;
    }
};

class Huazhu: public TriggerSkill{
public:
    Huazhu():TriggerSkill("huazhu"){
        events << Pindian;
        view_as_skill = new HuazhuViewAsSkill;
    }

    virtual int getPriority() const{
        return -1;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *, QVariant &data) const{
        PindianStar pindian = data.value<PindianStar>();
        if(pindian->reason != "huazhu" || !pindian->from->hasSkill(objectName()))
            return false;
        if(pindian->isSuccess()){
            int x = (pindian->from_card->getNumber() + 1) / 2;
            ServerPlayer *target = room->askForPlayerChosen(pindian->from, room->getAllPlayers(), objectName());
            if(target->getHandcardNum() > x)
                room->askForDiscard(target, objectName(), target->getHandcardNum() - x);
            else if(target->getHandcardNum() < x)
                target->drawCards(x - target->getHandcardNum());
        }
        return false;
    }
};

class Fengxing: public TriggerSkill{
public:
    Fengxing():TriggerSkill("fengxing"){
        events << PhaseChange << CardLost;
        frequency = Compulsory;
    }

    virtual bool trigger(TriggerEvent v, Room* room, ServerPlayer *player, QVariant &data) const{
        if(v == CardLost){
            CardMoveStar move = data.value<CardMoveStar>();
            if(move->from_place == Player::Hand && player->isAlive()){
                if(player->getHandcardNum() < player->getMaxHP()){
                    LogMessage log;
                    log.type = "#TriggerSkill";
                    log.from = player;
                    log.arg = objectName();
                    room->sendLog(log);
                    player->drawCards(1);
                }
            }
        }
        else{
            if(player->getPhase() == Player::Judge ||
               player->getPhase() == Player::Draw ||
               player->getPhase() == Player::Discard)
                return true;
        }
        return false;
    }
};
*/
TigerPackage::TigerPackage()
    :Package("tiger"){
/*
    General *hantao = new General(this, "hantao", "guan");
    hantao->addSkill(new Taolue);
    hantao->addSkill(new Changsheng);

    General *oupeng = new General(this, "oupeng", "jiang", 5);
    oupeng->addSkill("#losthp");
    oupeng->addSkill(new Zhanchi);
    oupeng->addSkill(new MarkAssignSkill("@wings", 1));
    related_skills.insertMulti("zhanchi", "#@wings-1");
    oupeng->addRelateSkill("tengfei");
    skills << new Tengfei << new TengfeiMain;
    related_skills.insertMulti("tengfei", "#tengfei_main");
*/
    General *leiheng = new General(this, "leiheng", "guan");
    leiheng->addSkill(new Guzong);
    leiheng->addSkill(new GuzongGet);
    related_skills.insertMulti("guzong", "#guzong-get");

    General *sunli = new General(this, "sunli", "guan");
    sunli->addSkill(new Neiying);

    General *wuyanguang = new General(this, "wuyanguang", "guan");

    General *tianhu = new General(this, "tianhu$", "jiang");
    tianhu->addSkill(new Wuzhou);
    tianhu->addSkill(new Huwei);
    skills << new HuweiViewAsSkill;

    General *zhangheng = new General(this, "zhangheng", "min", 3);
    zhangheng->addSkill(new Jielue);
    zhangheng->addSkill(new Fuhun);

    General *xiebao = new General(this, "xiebao", "min");
    xiebao->addSkill(new Liehuo);

    General *shien = new General(this, "shien", "min", 3);
    shien->addSkill(new Longluo);
    shien->addSkill(new Xiaozai);

    General *yanshun = new General(this, "yanshun", "kou");
    yanshun->addSkill(new Huxiao);

    General *wangying = new General(this, "wangying", "kou", 3);
    wangying->addSkill(new Tanse);
    wangying->addSkill(new Houfa);
    wangying->addSkill(new HoufaSlash);
    related_skills.insertMulti("houfa", "#houfa-slash");

    General *lizhong = new General(this, "lizhong", "kou", 4);
    lizhong->addSkill(new Losthp);
    lizhong->addSkill(new Linse);
    lizhong->addSkill(new LinseEffect);
    related_skills.insertMulti("linse", "#linse_effect");

/*
    General *yuehe = new General(this, "yuehe", "min", 3);
    yuehe->addSkill(new Yueli);
    yuehe->addSkill(new Taohui);

    General *muhong = new General(this, "muhong", "jiang");
    muhong->addSkill(new Wuzu);
    muhong->addSkill("huqi");

    General *zhoutong = new General(this, "zhoutong", "kou", 3);
    zhoutong->addSkill(new Qiangqu);
    zhoutong->addSkill(new Huatian);

    General *jiangjing = new General(this, "jiangjing", "jiang");
    jiangjing->addSkill(new Tiansuan);
    jiangjing->addSkill(new Huazhu);

    General *maling = new General(this, "maling", "jiang", 3);
    maling->addSkill(new Fengxing);

    addMetaObject<TaolueCard>();
    addMetaObject<HuazhuCard>();
*/
    addMetaObject<HuweiCard>();
    addMetaObject<XiaozaiCard>();
}

ADD_PACKAGE(Tiger)
