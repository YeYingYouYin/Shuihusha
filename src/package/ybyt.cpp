#include "ybyt.h"
#include "standard.h"
#include "skill.h"
#include "client.h"
#include "carditem.h"
#include "engine.h"
#include "ai.h"
#include "plough.h"
#include "maneuvering.h"

class SWPattern: public CardPattern{
public:
    virtual bool match(const Player *player, const Card *card) const{
        return card->inherits("Slash") || card->inherits("Weapon");
    }
    virtual bool willThrow() const{
        return false;
    }
};

YuanpeiCard::YuanpeiCard(){
    mute = true;
    once = true;
}

bool YuanpeiCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(!targets.isEmpty())
        return false;
    return to_select->getGeneral()->isMale() &&
            (!to_select->isKongcheng() || (to_select->isKongcheng() && to_select->getWeapon()));
}

void YuanpeiCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();
    room->playSkillEffect("yuanpei", 1);
    const Card *card = room->askForCard(effect.to, ".Yuanp", "@yuanpei:" + effect.from->objectName(), NonTrigger);
    if(card){
        effect.from->obtainCard(card);
        effect.from->drawCards(1);
        effect.to->drawCards(1);
    }
    else{
        room->setPlayerFlag(effect.from, "yuanpei");
        LogMessage lsp;
        lsp.type = "#Yuanpei";
        lsp.from = effect.from;
        lsp.to << effect.to;
        lsp.arg = "yuanpei";
        room->sendLog(lsp);
    }
}

class Yuanpei: public ViewAsSkill{
public:
    Yuanpei():ViewAsSkill("yuanpei"){
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        if(Self->hasFlag("yuanpei"))
            return selected.isEmpty() && !to_select->isEquipped();
        else
            return false;
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.length() == 1 && Self->hasFlag("yuanpei")){
            const Card *card = cards.first()->getCard();
            Card *slash = new Slash(card->getSuit(), card->getNumber());
            slash->addSubcard(card->getId());
            slash->setSkillName("yuanpei");
            return slash;
        }
        else if(cards.isEmpty())
            return new YuanpeiCard;
        else
            return NULL;
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        if(!player->hasUsed("YuanpeiCard"))
            return true;
        else
            return player->hasFlag("yuanpei") && Slash::IsAvailable(player);
    }
};

class Mengshi: public PhaseChangeSkill{
public:
    Mengshi():PhaseChangeSkill("mengshi"){
        frequency = Wake;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return PhaseChangeSkill::triggerable(target)
                && target->getMark("mengshi") == 0
                && target->getPhase() == Player::Start
                && target->getHandcardNum() < target->getAttackRange();
    }

    virtual bool onPhaseChange(ServerPlayer *qyyy) const{
        Room *room = qyyy->getRoom();

        LogMessage log;
        log.type = "#WakeUp";
        log.from = qyyy;
        log.arg = objectName();
        room->sendLog(log);
        room->playSkillEffect(objectName());
        room->broadcastInvoke("animate", "lightbox:$mengshi:1500");
        room->getThread()->delay(1500);

        qyyy->drawCards(3);
        room->acquireSkill(qyyy, "yinyu");
        room->setPlayerMark(qyyy, "mengshi", 1);
        return false;
    }
};

GuibingCard::GuibingCard(){

}

bool GuibingCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && Self->canSlash(to_select);
}

void GuibingCard::use(Room *room, ServerPlayer *gaolian, const QList<ServerPlayer *> &targets) const{
    ServerPlayer *to = targets.first();

    JudgeStruct judge;
    judge.pattern = QRegExp("(.*):(club|spade):(.*)");
    judge.good = true;
    judge.reason = objectName();
    judge.who = gaolian;

    room->judge(judge);

    if(judge.isGood()){
        Slash *slash = new Slash(Card::NoSuit, 0);
        slash->setSkillName("guibing");

        CardUseStruct use;
        use.from = gaolian;
        use.to << to;
        use.card = slash;
        room->useCard(use);
    }else
        room->setPlayerFlag(gaolian, "Guibing");
}

class GuibingViewAsSkill:public ZeroCardViewAsSkill{
public:
    GuibingViewAsSkill():ZeroCardViewAsSkill("guibing"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->hasFlag("Guibing") && Slash::IsAvailable(player);
    }

    virtual const Card *viewAs() const{
        return new GuibingCard;
    }
};

class Guibing: public TriggerSkill{
public:
    Guibing():TriggerSkill("guibing"){
        events << CardAsked;
        view_as_skill = new GuibingViewAsSkill;
    }

    virtual bool trigger(TriggerEvent, ServerPlayer *gaolian, QVariant &data) const{
        QString pattern = data.toString();
        if(pattern != "slash")
            return false;

        Room *room = gaolian->getRoom();
        if(gaolian->askForSkillInvoke(objectName())){
            JudgeStruct judge;
            judge.pattern = QRegExp("(.*):(club|spade):(.*)");
            judge.good = true;
            judge.reason = objectName();
            judge.who = gaolian;

            room->playSkillEffect(objectName());
            room->judge(judge);

            if(judge.isGood()){
                Slash *slash = new Slash(Card::NoSuit, 0);
                slash->setSkillName(objectName());
                room->provide(slash);
                return true;
            }
        }
        return false;
    }
};

HeiwuCard::HeiwuCard(){
    target_fixed = true;
}

void HeiwuCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &) const{
    int num = getSubcards().length();
    room->moveCardTo(this, NULL, Player::DrawPile, true);
    QList<int> fog = room->getNCards(num, false);
    room->askForGuanxing(source, fog, false);
};

class Heiwu:public ViewAsSkill{
public:
    Heiwu():ViewAsSkill("heiwu"){
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        return !to_select->isEquipped();
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.isEmpty())
            return NULL;
        HeiwuCard *heiwu_card = new HeiwuCard;
        heiwu_card->addSubcards(cards);
        return heiwu_card;
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->isKongcheng();
    }
};

class Goulian: public TriggerSkill{
public:
    Goulian():TriggerSkill("goulian"){
        events << Predamage;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        PlayerStar xuning = damage.from;
        if(damage.to != xuning && damage.nature == DamageStruct::Normal && damage.to->isChained() &&
           xuning->askForSkillInvoke(objectName(), data)){
            Room *room = xuning->getRoom();

            damage.to->setChained(false);
            room->playSkillEffect(objectName());
            room->broadcastProperty(damage.to, "chained");
            if(!damage.to->faceUp())
                damage.to->turnOver();

            room->throwCard(damage.to->getDefensiveHorse());
            room->throwCard(damage.to->getOffensiveHorse());
        }
        return false;
    }
};

class Jinjia: public TriggerSkill{
public:
    Jinjia():TriggerSkill("jinjia"){
        events << Damaged << SlashEffected;
        frequency = Compulsory;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return TriggerSkill::triggerable(target) && !target->getArmor() && target->getMark("qinggang") == 0;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        if(event == SlashEffected){
            SlashEffectStruct effect = data.value<SlashEffectStruct>();
            if(effect.nature != DamageStruct::Normal){
                room->playSkillEffect(objectName(), 1);
                LogMessage log;
                log.from = player;
                log.type = "#JinjiaNullify";
                log.arg = objectName();
                log.arg2 = effect.slash->objectName();
                room->sendLog(log);

                return true;
            }
        }else if(event == Damaged){
            DamageStruct damage = data.value<DamageStruct>();
            if(damage.card && damage.card->inherits("Slash")){
                room->playSkillEffect(objectName(), 2);
                LogMessage log;
                log.type = "#ThrowJinjiaWeapon";
                log.from = player;
                log.arg = objectName();
                if(damage.from->getWeapon()){
                    room->sendLog(log);
                    room->throwCard(damage.from->getWeapon());
                }
            }
        }
        return false;
    }
};

SinueCard::SinueCard(){
}

bool SinueCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(!targets.isEmpty())
        return false;
    return Self->distanceTo(to_select) == 1;
}

void SinueCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();

    LogMessage log;
    log.type = "#UseSkill";
    log.from = effect.from;
    log.to << effect.to;
    log.arg = "sinue";
    room->sendLog(log);

    DamageStruct damage;
    damage.damage = 2;
    damage.from = effect.from;
    damage.to = effect.to;
    room->damage(damage);
}

class SinueViewAsSkill: public OneCardViewAsSkill{
public:
    SinueViewAsSkill():OneCardViewAsSkill("sinue"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return pattern == "@@sinue";
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        SinueCard *card = new SinueCard;
        card->addSubcard(card_item->getFilteredCard());
        return card;
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return ! to_select->isEquipped();
    }
};

class Sinue: public TriggerSkill{
public:
    Sinue():TriggerSkill("sinue"){
        events << Death;
        view_as_skill = new SinueViewAsSkill;
    }

    virtual int getPriority() const{
        return -1;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        DamageStar damage = data.value<DamageStar>();
        ServerPlayer *killer = damage ? damage->from : NULL;
        if(!killer || !killer->hasSkill(objectName()) || killer == player)
            return false;
        Room *room = killer->getRoom();
        if(killer->getPhase() == Player::Play && !killer->isKongcheng())
            room->askForUseCard(killer, "@@sinue", "@sinue");
        return false;
    }
};

class Xuandao: public TriggerSkill{
public:
    Xuandao():TriggerSkill("xuandao"){
        events << SlashMissed;
        frequency = Compulsory;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        SlashEffectStruct slash_effect = data.value<SlashEffectStruct>();
/*
        if(target->hasFlag("triggered")){
            target->setFlags("-triggered");
            return true;
        }
*/
        PlayerStar next = slash_effect.to->getNextAlive();
        room->playSkillEffect(objectName());
        LogMessage log;
        log.type = "#Xuandao";
        log.from = player;
        log.to << next;
        log.arg = objectName();
        room->sendLog(log);

        slash_effect.to = next;
        slash_effect.to->setFlags("triggered");
        room->setEmotion(next, "victim");
        room->slashEffect(slash_effect);
        return true;
    }
};

FangzaoCard::FangzaoCard(){
    once = true;
}

bool FangzaoCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && to_select != Self && !to_select->isKongcheng();
}

void FangzaoCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.to->getRoom();

    room->playSkillEffect("fangzao", 1);
    const Card *card = room->askForCardShow(effect.to, effect.from, "fangzao");
    int card_id = card->getEffectiveId();
    room->showCard(effect.to, card_id);

    if(card->getTypeId() == Card::Basic || card->isNDTrick()){
        room->setPlayerMark(effect.from, "fangzao", card_id);
        room->setPlayerFlag(effect.from, "fangzao");
    }else{
        room->setPlayerFlag(effect.from, "-fangzao");
    }
}

class Fangzao: public ViewAsSkill{
public:
    Fangzao():ViewAsSkill("fangzao"){

    }

    virtual int getEffectIndex(ServerPlayer *, const Card *card) const{
        if(card->getTypeId() == Card::Basic)
            return 2;
        else
            return 3;
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        if(player->hasUsed("FangzaoCard") && player->hasFlag("fangzao")){
            int card_id = player->getMark("fangzao");
            const Card *card = Sanguosha->getCard(card_id);
            return card->isAvailable(player);
        }else
            return true;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        if(player->getPhase() == Player::NotActive)
            return false;

        if(!player->hasFlag("fangzao"))
            return false;

        if(player->hasUsed("FangzaoCard")){
            int card_id = player->getMark("fangzao");
            const Card *card = Sanguosha->getCard(card_id);
            return pattern.contains(card->objectName());
        }else
            return false;
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        if(Self->hasUsed("FangzaoCard") && selected.isEmpty() && Self->hasFlag("fangzao")){
            return !to_select->isEquipped();
        }else
            return false;
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(Self->hasUsed("FangzaoCard")){
            if(!Self->hasFlag("fangzao"))
                return false;

            if(cards.length() != 1)
                return NULL;

            int card_id = Self->getMark("fangzao");
            const Card *card = Sanguosha->getCard(card_id);
            const Card *first = cards.first()->getFilteredCard();

            Card *new_card = Sanguosha->cloneCard(card->objectName(), first->getSuit(), first->getNumber());
            new_card->addSubcards(cards);
            new_card->setSkillName(objectName());
            return new_card;
        }else{
            return new FangzaoCard;
        }
    }
};

class FangzaoMark: public TriggerSkill{
public:
    FangzaoMark():TriggerSkill("#fangzao_mark"){
        events << CardUsed;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *jindajian, QVariant &data) const{
        Room *room = jindajian->getRoom();
            CardUseStruct use = data.value<CardUseStruct>();
            if(use.card->getSkillName() == "fangzao"){
                    room->setPlayerFlag(jindajian, "-fangzao");
        }
        return false;
    }
};

class Jiangxin:public TriggerSkill{
public:
    Jiangxin():TriggerSkill("jiangxin"){
        frequency = Frequent;
        events << AskForRetrial;
    }

    virtual int getPriority() const{
        return -2;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *jindajian, QVariant &data) const{
        JudgeStar judge = data.value<JudgeStar>();
        CardStar card = judge->card;
        Room *room = jindajian->getRoom();

        if(card->inherits("BasicCard") && jindajian->askForSkillInvoke(objectName(), data)){
            room->playSkillEffect(objectName());
            jindajian->drawCards(1);
            return true;
        }
        return false;
    }
};

ShexinCard::ShexinCard(){
    once = true;
}

bool ShexinCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && !to_select->isKongcheng();
}

void ShexinCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this);

    ServerPlayer *target = targets.value(0, source);
    CardEffectStruct effect;
    effect.card = this;
    effect.from = source;
    effect.to = target;

    room->cardEffect(effect);
}

void ShexinCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();
    QList<int> cardes = effect.to->handCards();
    room->fillAG(cardes, effect.from);
    room->askForAG(effect.from, cardes, true, "shexin");
    QList<const Card *> cards = effect.to->getHandcards();
    foreach(const Card *card, cards){
        if(!card->inherits("BasicCard")){
            room->showCard(effect.to, card->getEffectiveId());
            room->getThread()->delay();
            room->throwCard(card);
        }else{
            room->showCard(effect.to, card->getEffectiveId());
            room->getThread()->delay();
        }
        effect.from->invoke("clearAG");
    }
}

class Shexin: public OneCardViewAsSkill{
public:
    Shexin():OneCardViewAsSkill("shexin"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("ShexinCard");
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getCard()->inherits("EquipCard") || to_select->getCard()->isNDTrick();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        ShexinCard *card = new ShexinCard;
        card->addSubcard(card_item->getFilteredCard());
        return card;
    }
};

SheyanCard::SheyanCard(){
    target_fixed = true;
}

void SheyanCard::onUse(Room *room, const CardUseStruct &card_use) const{
    room->throwCard(this);
    int card_id = getSubcards().first();
    Card::Suit suit = Sanguosha->getCard(card_id)->getSuit();
    int num = Sanguosha->getCard(card_id)->getNumber();

    CardUseStruct use;
    use.from = card_use.from;
    AmazingGrace *amazingGrace = new AmazingGrace(suit, num);
    amazingGrace->addSubcard(card_id);
    amazingGrace->setSkillName("sheyan");
    use.card = amazingGrace;
    room->useCard(use);
}

class Sheyan: public OneCardViewAsSkill{
public:
    Sheyan():OneCardViewAsSkill("sheyan"){

    }
    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("SheyanCard");
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getCard()->getSuit() == Card::Heart;
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        SheyanCard *card = new SheyanCard;
        card->addSubcard(card_item->getFilteredCard());
        return card;
    }
};

class Jiayao:public TriggerSkill{
public:
    Jiayao():TriggerSkill("jiayao"){
        events << CardEffect;
    }

    virtual int getPriority() const{
        return 2;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        CardEffectStruct effect = data.value<CardEffectStruct>();
        if(!effect.card->inherits("AmazingGrace"))
            return false;
        ServerPlayer *sq = room->findPlayerBySkillName(objectName());
        if(!sq || room->getTag("Jiayao").toInt() == 1 || !sq->askForSkillInvoke(objectName()))
            return false;
        room->setTag("Jiayao", 1);
        sq->drawCards(1);
        room->playSkillEffect(objectName());
        QVariantList ag_list = room->getTag("AmazingGrace").toList();
        int a = 0;
        foreach(QVariant card_id, ag_list){
            const Card *card = Sanguosha->getCard(card_id.toInt());
            if(card->inherits("Peach") || card->inherits("Analeptic"))
                a ++;
        }
        RecoverStruct rev;
        rev.recover = a;
        room->recover(sq, rev);
        return false;
    }
};

MaiyiCard::MaiyiCard(){

}

bool MaiyiCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(targets.length() >= 2)
        return false;
    return true;
}

bool MaiyiCard::targetsFeasible(const QList<const Player *> &targets, const Player *Self) const{
    return targets.length() == 2 || targets.isEmpty();
}

void MaiyiCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this);
    room->playSkillEffect(objectName());
    if(targets.isEmpty())
        room->setPlayerFlag(source, "maiyi");
    else{
        foreach(ServerPlayer *target, targets)
            target->drawCards(2);
    }
}

class MaiyiViewAsSkill: public ViewAsSkill{
public:
    MaiyiViewAsSkill(): ViewAsSkill("maiyi"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("MaiyiCard");
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        if(selected.length() >= 3)
            return false;

        foreach(CardItem *item, selected){
            if(selected.length() == 1 && item->getCard()->inherits("EquipCard") && to_select->getCard()->inherits("EquipCard"))
                return true;
            if(to_select->getFilteredCard()->getSuit() == item->getFilteredCard()->getSuit())
                return false;
        }
        return true;
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        bool can = false;
        if(cards.length() == 3)
            can = true;
        if(cards.length() == 2){
            if(cards.first()->getCard()->inherits("EquipCard") &&
               cards.last()->getCard()->inherits("EquipCard"))
                can = true;
        }
        if(!can)
            return NULL;
        MaiyiCard *card = new MaiyiCard;
        card->addSubcards(cards);
        return card;
    }
};

class Maiyi: public PhaseChangeSkill{
public:
    Maiyi():PhaseChangeSkill("maiyi"){
        view_as_skill = new MaiyiViewAsSkill;
    }

    virtual int getPriority() const{
        return 3;
    }

    virtual bool onPhaseChange(ServerPlayer *xueyong) const{
        if(xueyong->getPhase() != Player::NotActive || !xueyong->hasFlag("maiyi"))
            return false;
        Room *room = xueyong->getRoom();

        ServerPlayer *maiyier = room->askForPlayerChosen(xueyong, room->getAllPlayers(), objectName());
        LogMessage log;
        log.type = "#MaiyiCanInvoke";
        log.to << maiyier;
        log.from = xueyong;
        log.arg = objectName();
        room->sendLog(log);

        maiyier->gainAnExtraTurn(xueyong);
        return false;
    }
};

class Longao: public TriggerSkill{
public:
    Longao():TriggerSkill("longao"){
        events << CardEffected;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();

        ServerPlayer *zouyuan = room->findPlayerBySkillName(objectName());
        CardEffectStruct effect = data.value<CardEffectStruct>();
        if(!zouyuan || !effect.from || effect.from == zouyuan ||
           effect.multiple || !effect.card->isNDTrick())
            return false;

        if(!zouyuan->isNude() && room->askForSkillInvoke(zouyuan, objectName(), data)){
            room->askForDiscard(zouyuan, objectName(), 1, false, true);

            QList<ServerPlayer *> players = room->getOtherPlayers(effect.from), targets;
            foreach(ServerPlayer *player, players){
                if(player != effect.to)
                    targets << player;
            }

            QString choice = room->askForChoice(zouyuan, objectName(), "zhuan+qi");
            if(choice == "zhuan" && targets.length() > 0){
                room->playSkillEffect(objectName(), 1);
                ServerPlayer *target = room->askForPlayerChosen(zouyuan, targets, objectName());

                effect.from = effect.from;
                effect.to = target;
                data = QVariant::fromValue(effect);
            }
            if(choice == "qi" && !effect.from->isNude()){
                room->playSkillEffect(objectName(), 2);
                int to_throw = room->askForCardChosen(zouyuan, effect.from, "he", objectName());
                room->throwCard(to_throw);
            }
        }
        return false;
    }
};

HunjiuCard::HunjiuCard(){

}

bool HunjiuCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && Self->inMyAttackRange(to_select);
}

bool HunjiuCard::targetsFeasible(const QList<const Player *> &targets, const Player *Self) const{
    return (targets.isEmpty() && Analeptic::IsAvailable(Self)) ||
            (targets.length() == 1 && Ecstasy::IsAvailable(Self));
}

void HunjiuCard::onUse(Room *room, const CardUseStruct &card_use) const{
    room->throwCard(this);
    int card_id = getSubcards().first();
    Card::Suit suit = Sanguosha->getCard(card_id)->getSuit();
    int num = Sanguosha->getCard(card_id)->getNumber();
    CardUseStruct use;
    use.from = card_use.from;
    if(card_use.to.isEmpty()){
        Analeptic *a = new Analeptic(suit, num);
        a->setSkillName("hunjiu");
        a->addSubcard(card_id);
        use.card = a;
    }
    else{
        Ecstasy *e = new Ecstasy(suit, num);
        e->setSkillName("hunjiu");
        e->addSubcard(card_id);
        use.card = e;
        use.to << card_use.to.first();
    }
    room->useCard(use);
}

class Hunjiu:public OneCardViewAsSkill{
public:
    Hunjiu():OneCardViewAsSkill("hunjiu"){

    }

    virtual bool viewFilter(const CardItem *to_select) const{
        const Card *card = to_select->getFilteredCard();

        switch(ClientInstance->getStatus()){
        case Client::Playing:{
                return card->inherits("Peach") || card->inherits("Analeptic") || card->inherits("Ecstasy");
            }

        case Client::Responsing:{
                QString pattern = ClientInstance->getPattern();
                if(pattern.contains("analeptic"))
                    return card->inherits("Ecstasy") || card->inherits("Peach");
            }
        default:
            return false;
        }
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return pattern.contains("analeptic");
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        HunjiuCard *card = new HunjiuCard;
        card->addSubcard(card_item->getFilteredCard());
        return card;
    }
};

class Guitai: public TriggerSkill{
public:
    Guitai():TriggerSkill("guitai"){
        events << CardEffected;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();

        ServerPlayer *zhufu = room->findPlayerBySkillName(objectName());
        CardEffectStruct effect = data.value<CardEffectStruct>();
        if(effect.to == zhufu)
            return false;

        if(!zhufu || zhufu->getPhase() != Player::NotActive)
            return false;

        if(!effect.card->inherits("Peach"))
            return false;

        if(!zhufu->isNude() && zhufu->isWounded()){
            const Card *card = room->askForCard(zhufu, ".|heart", "@guitai:" + effect.to->objectName(), data, CardDiscarded);
            if(card){
                room->playSkillEffect(objectName());
                LogMessage log;
                log.type = "#Guitai";
                log.from = zhufu;
                log.to << effect.to;
                log.arg = objectName();
                log.arg2 = effect.card->objectName();
                room->sendLog(log);

                effect.from = effect.from;
                effect.to = zhufu;
                data = QVariant::fromValue(effect);
            }
        }
        return false;
    }
};

class Cuihuo: public TriggerSkill{
public:
    Cuihuo():TriggerSkill("cuihuo"){
        events << CardLost;
        frequency = Frequent;
    }

    virtual bool trigger(TriggerEvent, ServerPlayer *sunshangxiang, QVariant &data) const{
        CardMoveStar move = data.value<CardMoveStar>();
        if(move->from_place == Player::Equip){
            Room *room = sunshangxiang->getRoom();
            if(room->askForSkillInvoke(sunshangxiang, objectName())){
                room->playSkillEffect(objectName());
                sunshangxiang->drawCards(2);
            }
        }
        return false;
    }
};

class Jintang: public MasochismSkill{
public:
    Jintang():MasochismSkill("jintang"){
        frequency = Compulsory;
    }

    virtual void onDamaged(ServerPlayer *player, const DamageStruct &damage) const{
        Room *room = player->getRoom();
        if(!damage.from || !damage.from->getWeapon())
            return;
        int percent = 30 + player->getEquips().length() * 15;
        if(qrand() % 100 < percent){
            LogMessage log;
            log.from = player;
            log.type = "#TriggerSkill";
            log.arg = objectName();
            room->sendLog(log);
            DamageStruct damage2 = damage;
            damage2.from = player;
            damage2.to = damage.from;
            room->damage(damage2);
        }
    }
};

YBYTPackage::YBYTPackage()
    :Package("YBYT")
{
    General *qiongying = new General(this, "qiongying", "jiang", 3, false);
    qiongying->addSkill(new Yuanpei);
    patterns[".Yuanp"] = new SWPattern;
    qiongying->addSkill(new Mengshi);
    related_skills.insertMulti("mengshi", "yinyu");

    General *xuning = new General(this, "xuning", "jiang");
    xuning->addSkill(new Goulian);
    xuning->addSkill(new Jinjia);

    General *baoxu = new General(this, "baoxu", "kou");
    baoxu->addSkill(new Sinue);

    General *xiangchong = new General(this, "xiangchong", "jiang");
    xiangchong->addSkill(new Xuandao);

    General *jindajian = new General(this, "jindajian", "min", 3);
    jindajian->addSkill(new Fangzao);
    jindajian->addSkill(new FangzaoMark);
    jindajian->addSkill(new Jiangxin);
    related_skills.insertMulti("fangzao", "#fangzao_mark");

    General *yangchun = new General(this, "yangchun", "kou");
    yangchun->addSkill(new Shexin);

    General *songqing = new General(this, "songqing", "min", 3);
    songqing->addSkill(new Sheyan);
    songqing->addSkill(new Jiayao);

    General *xueyong = new General(this, "xueyong", "min");
    xueyong->addSkill(new Maiyi);
    xueyong->addSkill("#losthp");

    General *tanglong = new General(this, "tanglong", "jiang", 3);
    tanglong->addSkill(new Cuihuo);
    tanglong->addSkill(new Jintang);

    General *zouyan = new General(this, "zouyuan", "min");
    zouyan->addSkill(new Longao);

    General *zhufu = new General(this, "zhufu", "min", 3);
    zhufu->addSkill(new Hunjiu);
    zhufu->addSkill(new Guitai);

    General *gaolian = new General(this, "gaolian", "guan", 3);
    gaolian->addSkill(new Guibing);
    gaolian->addSkill(new Heiwu);

    addMetaObject<YuanpeiCard>();
    addMetaObject<GuibingCard>();
    addMetaObject<HeiwuCard>();
    addMetaObject<SinueCard>();
    addMetaObject<FangzaoCard>();
    addMetaObject<ShexinCard>();
    addMetaObject<MaiyiCard>();
    addMetaObject<SheyanCard>();
    addMetaObject<HunjiuCard>();
}

ADD_PACKAGE(YBYT);
