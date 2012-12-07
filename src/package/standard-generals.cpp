#include "general.h"
#include "standard.h"
#include "skill.h"
#include "engine.h"
#include "client.h"
#include "carditem.h"
#include "serverplayer.h"
#include "room.h"
#include "standard-generals.h"
#include "plough.h"
#include "maneuvering.h"
#include "ai.h"

JianaiCard::JianaiCard(){
    will_throw = false;
}

void JianaiCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    ServerPlayer *target = NULL;
    if(targets.isEmpty()){
        foreach(ServerPlayer *player, room->getAlivePlayers()){
            if(player != source){
                target = player;
                break;
            }
        }
    }else
        target = targets.first();

    room->obtainCard(target, this, false);

    int old_value = source->getMark("jianai");
    int new_value = old_value + subcards.length();
    room->setPlayerMark(source, "jianai", new_value);

    if(old_value < 2 && new_value >= 2){
        RecoverStruct recover;
        recover.card = this;
        recover.who = source;
        room->recover(source, recover);
    }
}

class JianaiViewAsSkill:public ViewAsSkill{
public:
    JianaiViewAsSkill():ViewAsSkill("jianai"){
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        return !to_select->isEquipped();
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.isEmpty())
            return NULL;

        JianaiCard *jianai_card = new JianaiCard;
        jianai_card->addSubcards(cards);
        return jianai_card;
    }
};

class Jianai: public PhaseChangeSkill{
public:
    Jianai():PhaseChangeSkill("jianai"){
        view_as_skill = new JianaiViewAsSkill;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return PhaseChangeSkill::triggerable(target)
                && target->getPhase() == Player::NotActive
                && target->hasUsed("JianaiCard");
    }

    virtual bool onPhaseChange(ServerPlayer *target) const{
        target->getRoom()->setPlayerMark(target, "jianai", 0);
        return false;
    }
};

FeigongCard::FeigongCard(){
}

bool FeigongCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && Self->canSlash(to_select);
}

void FeigongCard::use(Room *room, ServerPlayer *jidan, const QList<ServerPlayer *> &targets) const{
    QList<ServerPlayer *> lieges = room->getLieges("xia", jidan);
    const Card *slash = NULL;

    QVariant tohelp = QVariant::fromValue((PlayerStar)jidan);
    foreach(ServerPlayer *liege, lieges){
        slash = room->askForCard(liege, "slash", "@feigong-slash:" + jidan->objectName(), tohelp);
        if(slash){
            CardUseStruct card_use;
            card_use.card = slash;
            card_use.from = jidan;
            card_use.to << targets.first();

            room->useCard(card_use);
            return;
        }
    }
}

class FeigongViewAsSkill:public ZeroCardViewAsSkill{
public:
    FeigongViewAsSkill():ZeroCardViewAsSkill("feigong$"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->hasLordSkill("feigong") && Slash::IsAvailable(player);
    }

    virtual const Card *viewAs() const{
        return new FeigongCard;
    }
};

class Feigong: public TriggerSkill{
public:
    Feigong():TriggerSkill("feigong$"){
        events << CardAsked;
        default_choice = "ignore";

        view_as_skill = new FeigongViewAsSkill;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target != NULL && target->hasLordSkill("feigong");
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *jidan, QVariant &data) const{
        if (jidan == NULL) return false;
        QString pattern = data.toString();
        if(pattern != "slash")
            return false;

        QList<ServerPlayer *> lieges = room->getLieges("xia", jidan);
        if(lieges.isEmpty())
            return false;

        if(!room->askForSkillInvoke(jidan, objectName()))
            return false;

        room->playSkillEffect(objectName(), getEffectIndex(jidan, NULL));

        QVariant tohelp = QVariant::fromValue((PlayerStar)jidan);
        foreach(ServerPlayer *liege, lieges){
            const Card *slash = room->askForCard(liege, "Slash", "@feigong-slash:" + jidan->objectName(), tohelp);
            if(slash){
                room->provide(slash);
                return true;
            }
        }

        return false;
    }

    virtual int getEffectIndex(const ServerPlayer *player, const Card *) const{
        int r = 1 + qrand() % 2;
        if(player->getGeneralName() == "jingtianming" || player->getGeneral2Name() == "jingtianming")
            r += 2;

        return r;
    }
};

class Moshou: public TriggerSkill{
public:
    Moshou():TriggerSkill("moshou"){
        events << CardEffected;
        frequency = Compulsory;
    }

    virtual bool trigger(TriggerEvent event, Room *room, ServerPlayer *tianming, QVariant &data) const{
        if(!tianming)
            return false;

        if(event == CardEffected){
            CardEffectStruct effect = data.value<CardEffectStruct>();
            if(!effect.card->isNDTrick() || effect.from == tianming)
                return false;

            room->playSkillEffect(objectName());

            LogMessage log;
            log.type = "#Moshou";
            log.from = effect.from;
            log.to << effect.to;
            log.arg = objectName();
            room->sendLog(log);

            if(!room->askForCard(effect.from, ".", "@moshou-discard", data, CardDiscarded))
                return true;
        }
        return false;
    }
};

class Shangxian: public PhaseChangeSkill{
public:
    Shangxian():PhaseChangeSkill("shangxian"){
    }

    virtual int getPriority() const{
        return 3;
    }

    virtual bool onPhaseChange(ServerPlayer *jingtianming) const{
        Room *room = jingtianming->getRoom();

        switch(jingtianming->getPhase()){
        case Player::Draw: {
            bool invoked = jingtianming->askForSkillInvoke(objectName());
            if(invoked)
                jingtianming->setFlags(objectName());

            return invoked;
        }
        case Player::NotActive: {
            if(jingtianming->hasFlag(objectName())){
                ServerPlayer *player = room->askForPlayerChosen(jingtianming, room->getOtherPlayers(jingtianming), objectName());

                room->playSkillEffect(objectName(), 2);
                LogMessage log;
                log.type = "#Shangxian";
                log.from = jingtianming;
                log.to << player;
                room->sendLog(log);

                PlayerStar p = player;
                room->setTag("ShangxianTarget", QVariant::fromValue(p));
            }
            break;
        }
        default:
            break;
        }
        return false;
    }
};

class ShangxianGive: public PhaseChangeSkill{
public:
    ShangxianGive():PhaseChangeSkill("#shangxian-give"){

    }

    virtual int getPriority() const{
        return -4;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target != NULL && target->getPhase() == Player::NotActive;
    }

    virtual bool onPhaseChange(ServerPlayer *jingtianming) const{
        Room *room = jingtianming->getRoom();
        if(!room->getTag("ShangxianTarget").isNull())
        {
            PlayerStar target = room->getTag("ShangxianTarget").value<PlayerStar>();
            room->removeTag("ShangxianTarget");
            if(target->isAlive())
                target->gainAnExtraTurn();
        }
        return false;
    }
};

class Tianzhi: public TriggerSkill{
public:
    Tianzhi():TriggerSkill("tianzhi$"){
        events << HpRecover;
        frequency = Compulsory;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target->hasLordSkill(objectName());
    }

    virtual bool trigger(TriggerEvent , Room *room, ServerPlayer *player, QVariant &data) const{
        RecoverStruct recover = data.value<RecoverStruct>();
        if(recover.who != player && recover.who->getKingdom() == "xia" && recover.card->inherits("Peach")){
            recover.recover ++;

            LogMessage log;
            log.type = "#Tianzhi";
            log.from = player;
            log.arg = objectName();
            room->sendLog(log);

            data = QVariant::fromValue(recover);
        }
        return false;
    }
};

ShengxueCard::ShengxueCard(){
    target_fixed = true;
    once = true;
}

void ShengxueCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &) const{
    room->throwCard(this, source);

    QList<int> card_ids = room->getNCards(3);
    room->fillAG(card_ids);

    int choice = 0;
    int card_id = room->askForAG(source, card_ids, true, "shengxue");
    if(card_id < 0)
        choice = 2;
    else{
        CardStar card = Sanguosha->getCard(card_id);
        if(card->inherits("BasicCard"))
            choice = 2;
        else
            choice = 1;
    }

    if(choice == 2){
        foreach(int tmp, card_ids){
            room->obtainCard(source, tmp);
        }
        room->loseHp(source);
    }
    else{
        QList<int> card_sids;
        foreach(int tmp, card_ids){
            if(!Sanguosha->getCard(card_id)->inherits("BasicCard"))
                card_sids << tmp;
        }
        room->fillAG(card_sids);
        room->askForAG(source, card_sids, true, "shengxue");
        foreach(int tmp, card_sids){
            room->obtainCard(source, tmp);
        }
    }
    room->broadcastInvoke("clearAG");
 }

class Shengxue: public OneCardViewAsSkill{
public:
    Shengxue(): OneCardViewAsSkill("shengxue"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("ShengxueCard");
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return !to_select->isEquipped();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        ShengxueCard *sheru_card = new ShengxueCard;
        sheru_card->addSubcard(card_item->getFilteredCard());

        return sheru_card;
    }
};

class Feiyue: public TriggerSkill{
public:
    Feiyue():TriggerSkill("feiyue"){
        frequency = Compulsory;
        events << CardAsked;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target != NULL && TriggerSkill::triggerable(target) && !target->getArmor()
                && !target->hasFlag("wuqian") && !target->hasMark("qinggang");
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *jiru, QVariant &data) const{
        QString pattern = data.toString();

        if(pattern != "jink")
            return false;

        if(jiru->askForSkillInvoke(objectName())){
            JudgeStruct judge;
            judge.pattern = QRegExp("(.*):(heart|diamond):(.*)");
            judge.good = true;
            judge.reason = objectName();
            judge.who = jiru;

            room->judge(judge);

            if(judge.isGood()){
                Jink *jink = new Jink(Card::NoSuit, 0);
                jink->setSkillName(objectName());
                room->provide(jink);
                //room->setEmotion(jiru, "good");
                return true;
            }else
                room->setEmotion(jiru, "bad");
        }

        return false;
    }
};

class Baihe: public TriggerSkill{
public:
    Baihe():TriggerSkill("baihe"){
        events << CardUsed << CardEffected;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent event, Room *room, ServerPlayer *player, QVariant &data) const{
        if(event == CardUsed){
            CardUseStruct use = data.value<CardUseStruct>();
            if(use.to.length() <= 1 || !use.card->inherits("TrickCard") || use.card->inherits("Collateral"))
                return false;
            QList<ServerPlayer *>guigus = room->findPlayersBySkillName(objectName());

            foreach(ServerPlayer *guigu, guigus){
                if(use.to.contains(guigu) && room->askForSkillInvoke(guigu, objectName(), data)){
                    guigu->tag["Baihe"] = use.card->getEffectiveId();

                    room->playSkillEffect(objectName());

                    LogMessage log;
                    log.type = "#BaiheAvoid";
                    log.from = guigu;
                    log.arg = use.card->objectName();
                    log.arg2 = objectName();
                    room->sendLog(log);

                    guigu->drawCards(1);
                }
            }
        }
        else{
            if(!player->isAlive() || !player->hasSkill(objectName()))
                return false;

            CardEffectStruct effect = data.value<CardEffectStruct>();
            if(player->tag["Baihe"].isNull() || player->tag["Baihe"].toInt() != effect.card->getEffectiveId())
                return false;

            player->tag["Baihe"] = QVariant(QString());
            return true;
        }

        return false;
    }
};

class Jiansheng:public OneCardViewAsSkill{
public:
    Jiansheng():OneCardViewAsSkill("jiansheng"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return Slash::IsAvailable(player);
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return  pattern == "slash";
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        const Card *card = to_select->getFilteredCard();

        return card->isRed();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *card = card_item->getCard();
        Card *slash = new Slash(card->getSuit(), card->getNumber());
        slash->addSubcard(card->getId());
        slash->setSkillName(objectName());
        return slash;
    }
};

class Yixian: public PhaseChangeSkill{
public:
    Yixian():PhaseChangeSkill("yixian"){
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target->isWounded();
    }

    virtual bool onPhaseChange(ServerPlayer *player) const{
        if(player->getPhase() != Player::Start)
            return false;
        Room *room = player->getRoom();
        QList<ServerPlayer *> ltys = room->findPlayersBySkillName(objectName());
        foreach(ServerPlayer *lingtianyi, ltys){
            if(!player->isWounded())
                break;
            if(lingtianyi->isKongcheng())
                continue;
            QVariant data = QVariant::fromValue((PlayerStar)player);
            QString prompt = "@yixian:" + player->objectName();
            const Card *card = room->askForCard(lingtianyi, ".|.|.|.|red", prompt, true, data, CardDiscarded);
            if(card){
                RecoverStruct lty;
                lty.card = card;
                lty.who = lingtianyi;

                room->playSkillEffect(objectName());
                LogMessage log;
                log.from = lingtianyi;
                log.to << player;
                log.type = "#UseSkill";
                log.arg = objectName();
                room->sendLog(log);

                room->recover(player, lty, true);
            }
        }
        return false;
    }
};

class Feiming: public TriggerSkill{
public:
    Feiming():TriggerSkill("feiming"){
        events << Dying;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *ren, QVariant &data) const{
        if(ren->faceUp() && ren->askForSkillInvoke(objectName())){
            room->playSkillEffect(objectName());
            LogMessage log;
            log.type = "#TriggerSkill";
            log.from = ren;
            log.arg = objectName();
            room->sendLog(log);

            ren->turnOver();
            RecoverStruct rev;
            rev.recover = ren->getLostHp(false) - ren->getMaxHp() + 1;
            rev.who = ren;
            room->recover(ren, rev);
            if(ren->getHp() != 1)
                room->setPlayerProperty(ren, "hp", 1);
        }
        return false;
    }
};

class Daowang: public ViewAsSkill{
public:
    Daowang():ViewAsSkill("daowang"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->hasFlag(objectName());
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        if(selected.isEmpty())
            return true;
        else if(selected.length() == 1){
            const Card *card = selected.first()->getFilteredCard();
            return to_select->getFilteredCard()->getSuit() == card->getSuit();
        }else
            return false;
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.length() == 2){
            const Card *first = cards.first()->getCard();
            Snatch *snatch = new Snatch(first->getSuit(), 0);
            snatch->addSubcard(first->getId());
            snatch->setSkillName(objectName());
            Self->setFlags(objectName());
            return snatch;
        }else
            return NULL;
    }
};

class Shenxing: public ClientSkill{
public:
    Shenxing():ClientSkill("shenxing"){
    }

    virtual int getCorrect(const Player *from, const Player *to) const{
        if(to->hasSkill(objectName()) && to->getHp() <= 2)
            return +1;
        else if(from->hasSkill(objectName()) && from->getHp() >= 3)
            return -1;
        else
            return 0;
    }
};

class Leishen: public SlashBuffSkill{
public:
    Leishen():SlashBuffSkill("leishen"){

    }

    virtual bool buff(const SlashEffectStruct &effect) const{
        ServerPlayer *chuizi = effect.from;
        Room *room = chuizi->getRoom();
        if(chuizi->getPhase() != Player::Play)
            return false;

        int num = effect.to->getHandcardNum();
        if(num >= chuizi->getHp() || num <= chuizi->getAttackRange()){
            if(chuizi->askForSkillInvoke(objectName(), QVariant::fromValue(effect))){
                room->playSkillEffect(objectName());
                room->slashResult(effect, NULL);

                return true;
            }
        }
        return false;
    }
};

ShangtongCard::ShangtongCard(){
    will_throw = false;
}

bool ShangtongCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(!targets.isEmpty() || to_select == Self)
        return false;

    const Card *card = Sanguosha->getCard(subcards.first());
    const EquipCard *equip = qobject_cast<const EquipCard *>(card);
    int equip_index = static_cast<int>(equip->location());
    return to_select->getEquip(equip_index) == NULL;
}

void ShangtongCard::onEffect(const CardEffectStruct &effect) const{
    ServerPlayer *bandashi = effect.from;
    bandashi->getRoom()->moveCardTo(this, effect.to, Player::Equip);
    bandashi->drawCards(1);
}

class Shangtong: public OneCardViewAsSkill{
public:
    Shangtong():OneCardViewAsSkill("shangtong"){

    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return !to_select->isEquipped() && to_select->getFilteredCard()->getTypeId() == Card::Equip;
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        ShangtongCard *shangtong_card = new ShangtongCard();
        shangtong_card->addSubcard(card_item->getFilteredCard());
        return shangtong_card;
    }
};

class Jieyong: public TriggerSkill{
public:
    Jieyong():TriggerSkill("jieyong"){
        events << CardLost;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target != NULL;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        ServerPlayer *bandashi = room->findPlayerBySkillName(objectName());
        ServerPlayer *current = room->getCurrent();

        if(bandashi == NULL)
            return false;
        if(bandashi == current)
            return false;
        if(current->getPhase() == Player::Discard){
            QVariantList jieyong = bandashi->tag["Jieyong"].toList();

            CardMoveStar move = data.value<CardMoveStar>();
                jieyong << move->card_id;

            bandashi->tag["Jieyong"] = jieyong;
        }

        return false;
    }
};

class JieyongGet: public PhaseChangeSkill{
public:
    JieyongGet():PhaseChangeSkill("#jieyong-get"){

    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target != NULL && !target->hasSkill("jieyong");
    }

    virtual int getPriority() const{
        return -1;
    }

    virtual bool onPhaseChange(ServerPlayer *player) const{
        if(player->isDead())
            return false;

        Room *room = player->getRoom();
        ServerPlayer *bandashi = room->findPlayerBySkillName(objectName());
        if(bandashi == NULL)
            return false;

        QVariantList jieyong_cards = bandashi->tag["Jieyong"].toList();
        bandashi->tag.remove("Jieyong");

        QList<int> cards;
        foreach(QVariant card_data, jieyong_cards){
            int card_id = card_data.toInt();
            if(room->getCardPlace(card_id) == Player::DiscardedPile)
                cards << card_id;
        }

        if(cards.isEmpty())
            return false;

        if(bandashi->askForSkillInvoke("jieyong", cards.length())){
            room->fillAG(cards, bandashi);

            int to_back = room->askForAG(bandashi, cards, false, objectName());
            player->obtainCard(Sanguosha->getCard(to_back));

            cards.removeOne(to_back);

            bandashi->invoke("clearAG");

            foreach(int card_id, cards)
                bandashi->obtainCard(Sanguosha->getCard(card_id));
        }

        return false;
    }
};

PofuCard::PofuCard(){
    target_fixed = true;
    once = true;
}

void PofuCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &) const{
    room->throwCard(this, source);
    if(source->isAlive())
        room->drawCards(source, subcards.length());
}


class Pofu:public ViewAsSkill{
public:
    Pofu():ViewAsSkill("pofu"){
    }

    virtual bool viewFilter(const QList<CardItem *> &, const CardItem *) const{
        return true;
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.isEmpty())
            return NULL;
        PofuCard *zhiheng_card = new PofuCard;
        zhiheng_card->addSubcards(cards);
        return zhiheng_card;
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->hasUsed("PofuCard");
    }
};

class Bieji: public TriggerSkill{
public:
    Bieji():TriggerSkill("bieji$"){
        events << Dying;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target != NULL && target->hasLordSkill("bieji");
    }

    virtual bool trigger(TriggerEvent , Room* room, ServerPlayer *xiangyu, QVariant &data) const{
        foreach(ServerPlayer *wu, room->getOtherPlayers(xiangyu)){
            if(wu->getKingdom() == "wang" && wu->askForSkillInvoke(objectName())){
                room->playSkillEffect("bieji", 1);
                RecoverStruct reo;
                reo.who = wu;
                reo.recover = wu->getHp();
                room->recover(xiangyu, reo, false);
                room->loseHp(wu, reo.recover);
                break;
            }
        }
        return false;
    }
};

class Yexing: public TriggerSkill{
public:
    Yexing():TriggerSkill("yexing"){
        events << DamagedProceed;
        frequency = Compulsory;
    }

    virtual int getPriority() const{
        return 2;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *zhuwu, QVariant &data) const{
        DamageStruct dmag = data.value<DamageStruct>();
        if(dmag.nature == DamageStruct::Thunder || zhuwu->faceUp())
            return false;
        LogMessage log;
        log.type = "#YexingForbid";
        log.from = zhuwu;
        log.arg = objectName();
        room->sendLog(log);
        return true;
    }
};

YoulanCard::YoulanCard(){
}

void YoulanCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    ServerPlayer *first = targets.first();
    first->obtainCard(source->wholeHandCards(), false);
    first->turnOver();
    int x = first->getHandcardNum() / 2;
    if(x == 0 || !first->askForSkillInvoke("youlan"))
        return;
    for(; x > 0; x--)
        source->obtainCard(room->askForCardShow(first, source, "youlan"));
    source->turnOver();
}

class Youlan: public ZeroCardViewAsSkill{
public:
    Youlan():ZeroCardViewAsSkill("youlan"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->hasUsed("YoulanCard") && !player->isKongcheng();
    }

    virtual const Card *viewAs() const{
        return new YoulanCard;
    }
};


SuoshaCard::SuoshaCard(){
    once = true;
    will_throw = false;
}

void SuoshaCard::onEffect(const CardEffectStruct &effect) const{
    effect.to->obtainCard(this);

    Room *room = effect.to->getRoom();
    QList<ServerPlayer *> targets;
    foreach(ServerPlayer *p, room->getOtherPlayers(effect.to)){
        if(effect.to->canSlash(p)){
            targets << p;
        }
    }
    if(!targets.isEmpty()){
        ServerPlayer *target = room->askForPlayerChosen(effect.from, targets, "suosha");
        Assassinate *ass = new Assassinate(Card::NoSuit, 0);
        ass->setCancelable(false);
        CardUseStruct use;
        use.from = effect.to;
        use.to << target;
        use.card = ass;
        room->useCard(use);
    }
}

class Suosha: public OneCardViewAsSkill{
public:
    Suosha():OneCardViewAsSkill("suosha"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("SuoshaCard");
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        const Card *c = to_select->getCard();
        return c->getTypeId() == Card::Equip;
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        SuoshaCard *card = new SuoshaCard;
        card->addSubcard(card_item->getFilteredCard());

        return card;
    }
};

class Podou: public TriggerSkill{
public:
    Podou():TriggerSkill("podou"){
        events << Dying << AskForPeachesDone << RewardAndPunish;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent event, Room* room, ServerPlayer *a, QVariant &data) const{
        PlayerStar b = room->getCurrent();
        if(event == Dying){
            ServerPlayer *fanjian = room->findPlayerBySkillName(objectName());
            if(fanjian && fanjian->askForSkillInvoke(objectName())){
                room->playSkillEffect(objectName());
                b->drawCards(3);
                room->setPlayerFlag(b, objectName());
            }
        }
        else if(event == AskForPeachesDone){
            if(a->isAlive() && b->hasFlag(objectName())){
                a->obtainCard(b->getRandomHandCard(), false);
                a->obtainCard(b->getRandomHandCard(), false);
                room->setPlayerFlag(b, "-podou");
            }
            else if(a->isDead()){
                DyingStruct dying = data.value<DyingStruct>();
                DamageStruct *damage = dying.damage;
                if(damage && damage->from)
                    room->setPlayerFlag(damage->from, "podoucut");
            }
        }
        else{
            DamageStar damage = data.value<DamageStar>();
            ServerPlayer *killer = damage->from;
            if(killer->hasFlag("podoucut")){
                killer->setFlags("podoucut");
                return true;
            }
        }
        return false;
    }
};

class Yunchou:public PhaseChangeSkill{
public:
    Yunchou():PhaseChangeSkill("yunchou"){
        frequency = Frequent;
    }

    virtual bool onPhaseChange(ServerPlayer *zhuge) const{
        if(zhuge->getPhase() == Player::Start &&
           zhuge->askForSkillInvoke(objectName()))
        {
            Room *room = zhuge->getRoom();
            room->playSkillEffect(objectName());

            int n = qMin(5, room->alivePlayerCount());
            room->askForGuanxing(zhuge, room->getNCards(n, false), false);
        }

        return false;
    }
};

class Bigu: public ClientSkill{
public:
    Bigu():ClientSkill("bigu"){

    }

    virtual bool isProhibited(const Player *from, const Player *to, const Card *card) const{
        if(card->inherits("Slash") || card->inherits("Duel"))
            return to->isKongcheng();
        else
            return false;
    }
};

class BiguEffect: public TriggerSkill{
public:
    BiguEffect():TriggerSkill("#bigu-effect"){
        frequency = Compulsory;
        events << CardLost;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        if(player->isKongcheng()){
            CardMoveStar move = data.value<CardMoveStar>();
            if(move->from_place == Player::Hand)
                room->playSkillEffect("bigu");
        }

        return false;
    }
};

class Yaojian: public TriggerSkill{
public:
    Yaojian():TriggerSkill("yaojian"){
        events << Damage;
    }

    virtual int getPriority() const{
        return -1;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        if(!damage.card->inherits("Slash") && !damage.card->inherits("Assassinate"))
            return false;
        if(damage.to && !player->isKongcheng() && !damage.to->isKongcheng() &&
           player->askForSkillInvoke(objectName())){
            bool success = player->pindian(damage.to, objectName());
            if(success){
                int card_id = room->askForCardChosen(player, damage.to, "he", objectName());
                room->obtainCard(player, card_id, room->getCardPlace(card_id) != Player::Hand);
            }
        }
        return false;
    }
};

class Zhendu: public OneCardViewAsSkill{
public:
    Zhendu():OneCardViewAsSkill("zhendu"){

    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getCard()->getSuit() == Card::Diamond;
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *first = card_item->getCard();
        Indulgence *indulgence = new Indulgence(first->getSuit(), first->getNumber());
        indulgence->addSubcard(first->getId());
        indulgence->setSkillName(objectName());
        return indulgence;
    }
};

HuomeiCard::HuomeiCard(){
    owner_discarded = true;
}


bool HuomeiCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(!targets.isEmpty())
        return false;

    if(to_select->hasFlag("slash_source"))
        return false;

    if(!Self->canSlash(to_select))
        return false;

    int card_id = subcards.first();
    if(Self->getWeapon() && Self->getWeapon()->getId() == card_id)
        return Self->distanceTo(to_select) <= 1;
    else if(Self->getOffensiveHorse() && Self->getOffensiveHorse()->getId() == card_id)
        return Self->distanceTo(to_select) <= (Self->getWeapon()?Self->getWeapon()->getRange():1);
    else
        return true;
}

void HuomeiCard::onEffect(const CardEffectStruct &effect) const{
    effect.to->getRoom()->setPlayerFlag(effect.to, "huomei_target");
}

class HuomeiViewAsSkill: public OneCardViewAsSkill{
public:
    HuomeiViewAsSkill():OneCardViewAsSkill("huomei"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return pattern == "@@huomei";
    }

    virtual bool viewFilter(const CardItem *) const{
        return true;
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        HuomeiCard *huomei_card = new HuomeiCard;
        huomei_card->addSubcard(card_item->getFilteredCard());

        return huomei_card;
    }
};

class Huomei: public TriggerSkill{
public:
    Huomei():TriggerSkill("huomei"){
        view_as_skill = new HuomeiViewAsSkill;
        events << CardUsed;
    }

    virtual bool triggerable(const ServerPlayer *) const{
        return true;
    }

    virtual bool trigger(TriggerEvent, Room *room, ServerPlayer *player, QVariant &data) const{
        CardUseStruct use = data.value<CardUseStruct>();
        ServerPlayer *chilian = room->findPlayerBySkillName(objectName());
        if(!chilian || !use.to.contains(chilian)) return false;

        if(use.card->inherits("Slash") && !chilian->isNude() && room->alivePlayerCount() > 2){
            QList<ServerPlayer *> players = room->getOtherPlayers(chilian);
            players.removeOne(use.from);

            bool can_invoke = false;
            foreach(ServerPlayer *p, players){
                if(chilian->inMyAttackRange(p)){
                    can_invoke = true;
                    break;
                }
            }

            if(can_invoke){
                QString prompt = "@huomei:" + use.from->objectName();
                room->setPlayerFlag(use.from, "slash_source");
                if(room->askForUseCard(chilian, "@@huomei", prompt)){
                    foreach(ServerPlayer *p, players){
                        if(p->hasFlag("huomei_target")){
                            use.to.insert(use.to.indexOf(chilian), p);
                            use.to.removeOne(chilian);

                            data = QVariant::fromValue(use);

                            room->setPlayerFlag(use.from, "-slash_source");
                            room->setPlayerFlag(p, "-huomei_target");
                            break;
                        }
                    }
                }
            }
        }
        return false;
    }
};

class Chenxu: public TriggerSkill{
public:
    Chenxu(): TriggerSkill("chenxu"){
        events << CardResponsed;
    }

    virtual int getPriority() const{
        return 2;
    }

    virtual bool triggerable(const ServerPlayer *) const{
        return true;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        CardStar card_star = data.value<CardStar>();
        if(!card_star->inherits("Jink"))
            return false;
        ServerPlayer *wolf = room->findPlayerBySkillName(objectName());
        if(wolf && wolf != player){
            if(room->askForCard(wolf, "EquipCard", "@chenxu:" + player->objectName(), data, CardDiscarded)){
                Duel *dl = new Duel(Card::NoSuit, 0);
                dl->setCancelable(false);
                CardUseStruct use;
                use.from = wolf;
                use.to << player;
                use.card = dl;
                room->useCard(use);
            }
        }
        return false;
    }
};

class Wushuang: public TriggerSkill{
public:
    Wushuang():TriggerSkill("wushuang"){
        events << SlashProceed;
        frequency = Compulsory;
    }

    virtual bool trigger(TriggerEvent, Room *room, ServerPlayer *lvbu, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();
        room->playSkillEffect(objectName());

        QString slasher = lvbu->objectName();

        const Card *first_jink = NULL, *second_jink = NULL;
        first_jink = room->askForCard(effect.to, "jink", "@wushuang-jink-1:" + slasher, QVariant(), JinkUsed);
        if(first_jink)
            second_jink = room->askForCard(effect.to, "jink", "@wushuang-jink-2:" + slasher, QVariant(), JinkUsed);

        Card *jink = NULL;
        if(first_jink && second_jink){
            jink = new DummyCard;
            jink->addSubcard(first_jink);
            jink->addSubcard(second_jink);
        }

        room->slashResult(effect, jink);

        return true;
    }
};

class Duoding:public MasochismSkill{
public:
    Duoding():MasochismSkill("duoding"){
    }

    virtual void onDamaged(ServerPlayer *yingzheng, const DamageStruct &damage) const{
        Room *room = yingzheng->getRoom();
        const Card *card = damage.card;
        if(!room->obtainable(card, yingzheng))
            return;

            QVariant data = QVariant::fromValue(card);
            if(room->askForSkillInvoke(yingzheng, "duoding", data)){
            room->playSkillEffect(objectName());
            yingzheng->obtainCard(card);
        }
    }
};

class Hujia:public TriggerSkill{
public:
    Hujia():TriggerSkill("hujia$"){
        events << CardAsked;
        default_choice = "ignore";
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target != NULL && target->hasLordSkill("hujia");
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *yingzheng, QVariant &data) const{
        QString pattern = data.toString();
        if(pattern != "jink")
            return false;

        QList<ServerPlayer *> lieges = room->getLieges("di", yingzheng);
        if(lieges.isEmpty())
            return false;

        if(!room->askForSkillInvoke(yingzheng, objectName()))
            return false;

        room->playSkillEffect(objectName());
        QVariant tohelp = QVariant::fromValue((PlayerStar)yingzheng);
        foreach(ServerPlayer *liege, lieges){
            const Card *jink = room->askForCard(liege, "jink", "@hujia-jink:" + yingzheng->objectName(), tohelp);
            if(jink){
                room->provide(jink);
                return true;
            }
        }

        return false;
    }
};

class Congjun: public TriggerSkill{
public:
    Congjun():TriggerSkill("congjun"){
        frequency = Compulsory;
        events << SlashEffected;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target != NULL && TriggerSkill::triggerable(target) && !target->getArmor()
                && !target->hasFlag("wuqian") && target->getMark("qinggang") == 0;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();
        if(effect.slash->isBlack()){
            LogMessage log;
            log.type = "#CongjunNullify";
            log.from = player;
            log.arg = objectName();
            log.arg2 = effect.slash->objectName();
            room->sendLog(log);

            return true;
        }else
            return false;
    }
};

class Dayi: public OneCardViewAsSkill{
public:
    Dayi():OneCardViewAsSkill("dayi"){

    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getFilteredCard()->inherits("EquipCard") && !to_select->isEquipped();
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *, const QString &pattern) const{
        return pattern == "nullification" || pattern == "nulliplot";
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *first = card_item->getFilteredCard();
        Card *ncard = new Nullification(first->getSuit(), first->getNumber());
        ncard->addSubcard(first);
        ncard->setSkillName("dayi");

        return ncard;
    }
};

ShenwuCard::ShenwuCard(){
    mute = true;
}

bool ShenwuCard::targetsFeasible(const QList<const Player *> &targets, const Player *Self) const{
    if(Self->getPhase() == Player::Draw)
        return targets.length() <= 2;
    else if(Self->getPhase() == Player::Play)
        return targets.length() <= 1;
    else
        return targets.isEmpty();
}

bool ShenwuCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(Self->getPhase() == Player::Draw)
        return targets.length() < 2 && to_select != Self && !to_select->isKongcheng();
    else if(Self->getPhase() == Player::Play){
        return targets.isEmpty() &&
                (!to_select->getJudgingArea().isEmpty() || !to_select->getEquips().isEmpty());
    }else
        return false;
}

void ShenwuCard::use(Room *room, ServerPlayer *mengtian, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this, mengtian);

    if(mengtian->getPhase() == Player::Draw){
        room->playSkillEffect("shenwu", 2);
        if(targets.isEmpty())
            return;

        QList<ServerPlayer *> players = targets;
        //qSort(players.begin(), players.end(), ServerPlayer::CompareByActionOrder);
        foreach(ServerPlayer *target, players){
            room->cardEffect(this, mengtian, target);
        }
    }else if(mengtian->getPhase() == Player::Play){
        room->playSkillEffect("shenwu", 3);
        if(targets.isEmpty())
            return;

        PlayerStar from = targets.first();
        if(!from->hasEquip() && from->getJudgingArea().isEmpty())
            return;

        int card_id = room->askForCardChosen(mengtian, from , "ej", "shenwu");
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
                if(!mengtian->isProhibited(p, trick) && !p->containsTrick(trick->objectName()))
                    tos << p;
            }
        }

        if(trick && trick->isVirtualCard())
            delete trick;

        room->setTag("ShenwuTarget", QVariant::fromValue(from));
        ServerPlayer *to = room->askForPlayerChosen(mengtian, tos, "shenwu");
        if(to)
            room->moveCardTo(card, to, place);
        room->removeTag("ShenwuTarget");
    }
    else if(mengtian->getPhase() == Player::Judge)
        room->playSkillEffect("shenwu", 1);
    else
        room->playSkillEffect("shenwu", 4);
}

void ShenwuCard::onEffect(const CardEffectStruct &effect) const{
    if(effect.from->getPhase() == Player::Draw){
        Room *room = effect.from->getRoom();
        if(!effect.to->isKongcheng()){
            int card_id = room->askForCardChosen(effect.from, effect.to, "h", "shenwu");
            room->obtainCard(effect.from, card_id, false);

            room->setEmotion(effect.to, "bad");
            room->setEmotion(effect.from, "good");
        }
    }
}

class ShenwuViewAsSkill: public OneCardViewAsSkill{
public:
    ShenwuViewAsSkill():OneCardViewAsSkill(""){

    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return !to_select->isEquipped();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        ShenwuCard *card = new ShenwuCard;
        card->addSubcard(card_item->getFilteredCard());
        return card;
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return pattern == "@shenwu";
    }
};

class Shenwu: public PhaseChangeSkill{
public:
    Shenwu():PhaseChangeSkill("shenwu"){
        view_as_skill = new ShenwuViewAsSkill;
    }

    virtual int getPriority() const{
        return 3;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target != NULL && PhaseChangeSkill::triggerable(target) && !target->isKongcheng();
    }

    virtual bool onPhaseChange(ServerPlayer *mengtian) const{
        Room *room = mengtian->getRoom();

        switch(mengtian->getPhase()){
        case Player::RoundStart:
        case Player::Start:
        case Player::Finish:
        case Player::NotActive: return false;

        case Player::Judge: return room->askForUseCard(mengtian, "@shenwu", "@shenwu-judge", 1);
        case Player::Draw: return room->askForUseCard(mengtian, "@shenwu", "@shenwu-draw", 2);
        case Player::Play: return room->askForUseCard(mengtian, "@shenwu", "@shenwu-play", 3);
        case Player::Discard: return room->askForUseCard(mengtian, "@shenwu", "@shenwu-discard", 1);
        }

        return false;
    }
};

class Shujian:public MasochismSkill{
public:
    Shujian():MasochismSkill("shujian"){
        frequency = Frequent;
    }

    virtual void onDamaged(ServerPlayer *lisi, const DamageStruct &damage) const{
        Room *room = lisi->getRoom();

        if(!room->askForSkillInvoke(lisi, objectName()))
            return;

        room->playSkillEffect(objectName());

        int x = damage.damage, i;
        for(i=0; i<x; i++){
            lisi->drawCards(2, true, objectName());
            QList<int> shujian_cards;
            foreach(const Card *card, lisi->getHandcards()){
                if(card->hasFlag(objectName())){
                    room->setCardFlag(card, "-" + objectName());
                    shujian_cards << card->getEffectiveId();
                }
            }

            if(shujian_cards.isEmpty())
                continue;

            while(room->askForYiji(lisi, shujian_cards))
                ; // empty loop
        }

    }
};

DushaCard::DushaCard(){
    will_throw = false;
    once = true;
}

bool DushaCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(!targets.isEmpty())
        return false;
    return to_select->getHandcardNum() > Self->getHandcardNum();
}

void DushaCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->showCard(source, getEffectiveId());
    PlayerStar target = targets.first();
    QString choice = room->askForChoice(targets.first(), "dusha", "dis+los");
    if(choice == "los"){
        QString suit = getSuitString();
        if(room->askForCard(target, ".|" + suit + "|.|hand", "@dusha:" + source->objectName(), QVariant::fromValue((CardStar)this), CardDiscarded)){
            DamageStruct ddd;
            ddd.from = target;
            ddd.to = source;
            room->damage(ddd);
            return;
        }
    }
    room->loseHp(target);
}

class Dusha: public OneCardViewAsSkill{
public:
    Dusha():OneCardViewAsSkill("dusha"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("DushaCard");
    }

    virtual bool viewFilter(const CardItem *lij) const{
        return !lij->isEquipped();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        DushaCard *card = new DushaCard;
        card->addSubcard(card_item->getCard()->getId());
        return card;
    }
};

class Sixing:public TriggerSkill{
public:
    Sixing():TriggerSkill("sixing"){
        events << PhaseChange << FinishJudge;

        frequency = Frequent;
    }

    virtual bool trigger(TriggerEvent event, Room* room, ServerPlayer *yueshen, QVariant &data) const{
        if(event == PhaseChange && yueshen->getPhase() == Player::Start){
            while(yueshen->askForSkillInvoke("sixing")){
                room->playSkillEffect(objectName());

                JudgeStruct judge;
                judge.pattern = QRegExp("(.*):(spade|club):(.*)");
                judge.good = true;
                judge.reason = objectName();
                judge.who = yueshen;
                judge.time_consuming = true;

                room->judge(judge);
                if(judge.isBad())
                    break;
            }

        }else if(event == FinishJudge){
            JudgeStar judge = data.value<JudgeStar>();
            if(judge->reason == objectName()){
                if(judge->card->isBlack()){
                    yueshen->obtainCard(judge->card);
                    return true;
                }
            }
        }

        return false;
    }
};

class Yixing:public OneCardViewAsSkill{
public:
    Yixing():OneCardViewAsSkill("yixing"){

    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getFilteredCard()->isBlack() && !to_select->isEquipped();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *card = card_item->getCard();
        Jink *jink = new Jink(card->getSuit(), card->getNumber());
        jink->setSkillName(objectName());
        jink->addSubcard(card->getId());
        return jink;
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return  pattern == "jink";
    }
};

ShouyaoCard::ShouyaoCard(){
    once = true;
}

bool ShouyaoCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(!to_select->getGeneral()->isMale())
        return false;

    if(targets.isEmpty() && to_select->hasSkill("kongcheng") && to_select->isKongcheng()){
        return false;
    }

    return true;
}

bool ShouyaoCard::targetsFeasible(const QList<const Player *> &targets, const Player *Self) const{
    return targets.length() == 2;
}

void ShouyaoCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this, source);
    room->playSkillEffect("shouyao");

    ServerPlayer *to = targets.at(0);
    ServerPlayer *from = targets.at(1);

    Duel *duel = new Duel(Card::NoSuit, 0);
    duel->setCancelable(false);

    CardUseStruct use;
    use.from = from;
    use.to << to;
    use.card = duel;
    room->useCard(use);
}

class Shouyao: public OneCardViewAsSkill{
public:
    Shouyao():OneCardViewAsSkill("shouyao"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("ShouyaoCard");
    }

    virtual bool viewFilter(const CardItem *) const{
        return true;
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        ShouyaoCard *shouyao_card = new ShouyaoCard;
        shouyao_card->addSubcard(card_item->getCard()->getId());

        return shouyao_card;
    }

    virtual int getEffectIndex(const ServerPlayer *, const Card *) const{
        return 0;
    }
};

class Zheyao: public PhaseChangeSkill{
public:
    Zheyao():PhaseChangeSkill("zheyao"){
        frequency = Frequent;
    }

    virtual bool onPhaseChange(ServerPlayer *dasiming) const{
        if(dasiming->getPhase() == Player::Finish){
            Room *room = dasiming->getRoom();
            if(room->askForSkillInvoke(dasiming, objectName())){
                room->playSkillEffect(objectName());
                dasiming->drawCards(1);
            }
        }

        return false;
    }
};

LuoshengCard::LuoshengCard(){
    once = true;
    mute = true;
    owner_discarded = true;
}

bool LuoshengCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(!targets.isEmpty())
        return false;

    return to_select->getGeneral()->isMale() && to_select->isWounded();
}

void LuoshengCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();

    RecoverStruct recover;
    recover.card = this;
    recover.who = effect.from;

    room->recover(effect.from, recover, true);
    room->recover(effect.to, recover, true);

    int index = -1;
    if(effect.from->getGeneral()->isMale()){
        if(effect.from == effect.to)
            index = 5;
        else if(effect.from->getHp() >= effect.to->getHp())
            index = 3;
        else
            index = 4;
    }else{
        index = 1 + qrand() % 2;
    }

    room->playSkillEffect("luosheng", index);
}

class Luosheng: public ViewAsSkill{
public:
    Luosheng():ViewAsSkill("luosheng"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("LuoshengCard");
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        if(selected.length() > 2)
            return false;

        return !to_select->isEquipped();
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.length() != 2)
            return NULL;

        LuoshengCard *luosheng_card = new LuoshengCard();
        luosheng_card->addSubcards(cards);

        return luosheng_card;
    }
};

class Lingyi: public TriggerSkill{
public:
    Lingyi():TriggerSkill("lingyi"){
        events << CardLost;

        frequency = Frequent;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *shaosiming, QVariant &data) const{
        if (shaosiming == NULL) return false;
        CardMoveStar move = data.value<CardMoveStar>();
        if(move->from_place == Player::Equip){
            if(room->askForSkillInvoke(shaosiming, objectName())){
                room->playSkillEffect(objectName());
                shaosiming->drawCards(2);
            }
        }

        return false;
    }
};

class Badao: public TriggerSkill{
public:
    Badao():TriggerSkill("badao"){
        frequency = Frequent;
        events << Damaged;
    }

    virtual bool triggerable(const ServerPlayer *) const{
        return true;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *other, QVariant &data) const{
        QList<ServerPlayer *> lolita = room->findPlayersBySkillName(objectName());
        DamageStruct damage = data.value<DamageStruct>();
        foreach(ServerPlayer *loli, lolita){
            if(damage.nature == DamageStruct::Fire && loli->askForSkillInvoke(objectName())){
                room->setEmotion(loli, "draw-card");
                room->drawCards(loli, damage.damage);
            }
        }
        return false;
    }
};

EnchouCard::EnchouCard(){
}

void EnchouCard::onEffect(const CardEffectStruct &effect) const{
    effect.to->drawCards(1);
    FireAttack *fa = new FireAttack(Card::NoSuit, 0);
    fa->setCancelable(false);
    CardUseStruct use;
    use.card = fa;
    use.from = effect.from;
    use.to << effect.to;
    effect.from->getRoom()->useCard(use);
}

class Enchou:public ZeroCardViewAsSkill{
public:
    Enchou():ZeroCardViewAsSkill("enchou"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->hasUsed("EnchouCard");
    }

    virtual const Card *viewAs() const{
        return new EnchouCard;
    }
};

class Zhumie:public TriggerSkill{
public:
    Zhumie():TriggerSkill("zhumie"){
        events << CardUsed;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *nana, QVariant &data) const{
        CardUseStruct use = data.value<CardUseStruct>();
        CardStar card = use.card;
        if(card->inherits("Slash") && card->isBlack()){
            foreach(ServerPlayer *useto, use.to){
                if(room->askForSkillInvoke(nana, objectName())){
                    int card_id = room->askForCardChosen(nana, useto, "he", objectName());
                    room->throwCard(card_id, useto, nana);
                }
            }
        }
        return false;
    }
};

class Guibian:public TriggerSkill{
public:
    Guibian():TriggerSkill("guibian"){
        frequency = Frequent;
        events << CardUsed << CardResponsed;
    }

    virtual bool trigger(TriggerEvent event, Room* room, ServerPlayer *yueying, QVariant &data) const{
        if (yueying == NULL) return false;
        CardStar card = NULL;
        if(event == CardUsed){
            CardUseStruct use = data.value<CardUseStruct>();
            card = use.card;
        }else if(event == CardResponsed)
            card = data.value<CardStar>();

        if(card->isNDTrick()){
            if(room->askForSkillInvoke(yueying, objectName())){
                room->playSkillEffect(objectName());
                yueying->drawCards(1);
            }
        }

        return false;
    }
};

class Bishi: public TriggerSkill{
public:
    Bishi():TriggerSkill("bishi"){
        events << CardEffect << CardEffected;
        frequency = Compulsory;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target != NULL;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        CardEffectStruct effect = data.value<CardEffectStruct>();

        if(effect.to == effect.from)
            return false;

        if(effect.card->getTypeId() == Card::Trick){
            if(effect.from && effect.from->hasSkill(objectName())){
                LogMessage log;
                log.type = "#BishiBaD";
                log.from = effect.from;
                log.to << effect.to;
                log.arg = effect.card->objectName();
                log.arg2 = objectName();

                room->sendLog(log);

                room->playSkillEffect("bishi");

                return true;
            }

            if(effect.to->hasSkill(objectName()) && effect.from){
                LogMessage log;
                log.type = "#BishiGooD";
                log.from = effect.to;
                log.to << effect.from;
                log.arg = effect.card->objectName();
                log.arg2 = objectName();

                room->sendLog(log);

                room->playSkillEffect("bishi");

                return true;
            }
        }

        return false;
    }
};

ZhiyanCard::ZhiyanCard(){
    will_throw = false;
    once = true;
}

void ZhiyanCard::onEffect(const CardEffectStruct &effect) const{
    effect.to->obtainCard(this);
    Room *room = effect.from->getRoom();
    int card_id = room->askForCardChosen(effect.from, effect.to, "he", "zhiyan");
    room->obtainCard(effect.from, card_id, room->getCardPlace(card_id) != Player::Hand);
    room->showCard(effect.from, card_id);
    if(Sanguosha->getCard(card_id)->inherits("TrickCard") && effect.from->askForSkillInvoke("zhiyan")){
        room->throwCard(card_id, effect.to, effect.from);
        RecoverStruct tec;
        room->recover(effect.from, tec, true);
    }
}

class Zhiyan: public ViewAsSkill{
public:
    Zhiyan(): ViewAsSkill("zhiyan"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("ZhiyanCard");
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        return to_select->getCard()->inherits("TrickCard");
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.isEmpty())
            return NULL;
        ZhiyanCard *card = new ZhiyanCard;
        card->addSubcards(cards);
        return card;
    }
};

GuirouCard::GuirouCard(){
    owner_discarded = true;
}

void GuirouCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.to->getRoom();
    DamageStruct damage = effect.from->tag["GuirouDamage"].value<DamageStruct>();
    damage.to = effect.to;
    room->damage(damage);

    if(damage.to->isAlive())
        damage.to->drawCards(damage.to->getLostHp());
}

class GuirouViewAsSkill: public OneCardViewAsSkill{
public:
    GuirouViewAsSkill():OneCardViewAsSkill("guirou"){

    }

    virtual bool isEnabledAtPlay(const Player *) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return pattern == "@@guirou";
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return !to_select->isEquipped() && to_select->getFilteredCard()->getSuit() == Card::Heart;
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        GuirouCard *card = new GuirouCard;
        card->addSubcard(card_item->getFilteredCard());

        return card;
    }
};

class Guirou: public TriggerSkill{
public:
    Guirou():TriggerSkill("guirou"){
        events << Predamaged;
        view_as_skill = new GuirouViewAsSkill;
    }

    virtual int getPriority() const{
        return 2;
    }

    virtual bool trigger(TriggerEvent, Room *room, ServerPlayer *xiaoyao, QVariant &data) const{
        if(!xiaoyao->isKongcheng()){
            DamageStruct damage = data.value<DamageStruct>();

            xiaoyao->tag["GuirouDamage"] = QVariant::fromValue(damage);
            if(room->askForUseCard(xiaoyao, "@@guirou", "@guirou-card"))
                return true;
        }

        return false;
    }
};

YangshengCard::YangshengCard(){
    will_throw = false;
    once = true;
}

bool YangshengCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(!targets.isEmpty())
        return false;
    return to_select->hasSkill("yangsheng");
}

void YangshengCard::onEffect(const CardEffectStruct &effect) const{
    effect.to->obtainCard(this);
    Room *room = effect.from->getRoom();
    if(effect.from->isWounded() && room->askForDiscard(effect.to, "yangsheng", 1, true)){
        RecoverStruct rec;
        rec.who = effect.to;
        room->recover(effect.from, rec, true);
    };
}

class YangshengViewAsSkill: public OneCardViewAsSkill{
public:
    YangshengViewAsSkill():OneCardViewAsSkill("baoyang"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("YangshengCard");
    }

    virtual bool viewFilter(const CardItem *watch) const{
        return !watch->isEquipped() && watch->getCard()->getSuit() == Card::Heart;
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        YangshengCard *card = new YangshengCard;
        card->addSubcard(card_item->getCard()->getId());
        return card;
    }
};

class Yangsheng: public GameStartSkill{
public:
    Yangsheng():GameStartSkill("yangsheng"){
    }

    virtual void onGameStart(ServerPlayer *yangvi) const{
        Room *room = yangvi->getRoom();
        QList<ServerPlayer *> players = room->getAlivePlayers();
        foreach(ServerPlayer *player, players){
            room->attachSkillToPlayer(player, "baoyang");
        }
    }

    virtual void onIdied(ServerPlayer *yangvi) const{
        Room *room = yangvi->getRoom();
        if(room->findPlayerBySkillName("yangsheng"))
            return;
        QList<ServerPlayer *> players = room->getAlivePlayers();
        foreach(ServerPlayer *player, players){
            room->detachSkillFromPlayer(player, "baoyang", false);
        }
    }
};

StandardPackage::StandardPackage()
    :Package("standard"){
//--------
    General *jidan = new General(this, "jidan$", "xia");
    jidan->addSkill(new Jianai);
    jidan->addSkill(new Feigong);

    General *jingtianming = new General(this, "jingtianming$", "xia");
    jingtianming->addSkill(new Moshou);
    jingtianming->addSkill(new Shangxian);
    jingtianming->addSkill(new ShangxianGive);
    jingtianming->addSkill(new Tianzhi);
    related_skills.insertMulti("shangxian", "#shangxian-give");

    General *jiru = new General(this, "jiru", "xia", 3, false);
    jiru->addSkill(new Shengxue);
    jiru->addSkill(new Feiyue);
    //jiru->addSkill(new Dayi);

    General *genie = new General(this, "genie", "xia");
    genie->addSkill(new Baihe);
    genie->addSkill(new Jiansheng);

    General *duanmurong = new General(this, "duanmurong", "xia", 3, false);
    duanmurong->addSkill(new Yixian);
    duanmurong->addSkill(new Feiming);

    General *daozhi = new General(this, "daozhi", "xia");
    daozhi->addSkill(new Daowang);
    daozhi->addSkill(new Shenxing);

    General *datiechui = new General(this, "datiechui", "xia");
    datiechui->addSkill(new Leishen);

    General *bandashi = new General(this, "bandashi", "xia", 3);
    bandashi->addSkill(new Shangtong);
    bandashi->addSkill(new Jieyong);
    bandashi->addSkill(new JieyongGet);
    related_skills.insertMulti("jieyong", "#jieyong-get");
//--------
    General *xiangyu = new General(this, "xiangyu$", "wang");
    xiangyu->addSkill(new Pofu);
    xiangyu->addSkill(new Bieji);

    General *shilan = new General(this, "shilan", "wang", 3, false);
    shilan->addSkill(new Yexing);
    shilan->addSkill(new Youlan);

    General *fanzeng = new General(this, "fanzeng", "wang", 3);
    fanzeng->addSkill(new Suosha);
    fanzeng->addSkill(new Podou);

    General *zhangliang = new General(this, "zhangliang", "wang", 3);
    zhangliang->addSkill(new Yunchou);
    zhangliang->addSkill(new Bigu);
    zhangliang->addSkill(new BiguEffect);
    related_skills.insertMulti("bigu", "#bigu-effect");

    General *weizhuang = new General(this, "weizhuang", "wang");
    weizhuang->addSkill("baihe");
    weizhuang->addSkill(new Yaojian);

    General *chilian = new General(this, "chilian", "wang", 3, false);
    chilian->addSkill(new Zhendu);
    chilian->addSkill(new Huomei);

    General *canglangwang = new General(this, "canglangwang", "wang");
    canglangwang->addSkill(new Chenxu);

    General *jiguanwushuang = new General(this, "jiguanwushuang", "wang");
    jiguanwushuang->addSkill(new Wushuang);
//--------
    General *yingzheng = new General(this, "yingzheng$", "di");
    yingzheng->addSkill(new Duoding);
    yingzheng->addSkill(new Hujia);

    General *fusu = new General(this, "fusu", "di");
    fusu->addSkill(new Congjun);
    fusu->addSkill(new Dayi);

    General *mengtian = new General(this, "mengtian", "di");
    mengtian->addSkill(new Shenwu);

    General *lisi = new General(this, "lisi", "di", 3);
    lisi->addSkill(new Shujian);
    lisi->addSkill(new Dusha);

    General *yueshen = new General(this, "yueshen", "di", 3, false);
    yueshen->addSkill(new Sixing);
    yueshen->addSkill(new Yixing);

    General *dasiming = new General(this, "dasiming", "di", 3, false);
    dasiming->addSkill(new Shouyao);
    dasiming->addSkill(new Zheyao);
    //dasiming->addSkill(new SPConvertSkill("tuoqiao", "dasiming", "sp_dasiming"));

    General *shaosiming = new General(this, "shaosiming", "di", 3, false);
    shaosiming->addSkill(new Luosheng);
    shaosiming->addSkill(new Lingyi);
    //shaosiming->addSkill(new SPConvertSkill("chujia", "shaosiming", "sp_shaosiming"));
//--------
    General *gongshuchou = new General(this, "gongshuchou", "free", 3);
    gongshuchou->addSkill(new Badao);
    gongshuchou->addSkill(new Enchou);

    General *shengqi = new General(this, "shengqi", "free");
    shengqi->addSkill(new Zhumie);

    General *gongsunlinglong = new General(this, "gongsunlinglong", "free", 3, false);
    gongsunlinglong->addSkill(new Guibian);
    gongsunlinglong->addSkill(new Skill("baima", Skill::Compulsory));

    General *chunangong = new General(this, "chunangong", "free", 3);
    chunangong->addSkill(new Bishi);
    chunangong->addSkill(new Zhiyan);

    General *xiaoyaozi = new General(this, "xiaoyaozi", "free", 3);
    xiaoyaozi->addSkill(new Guirou);
    xiaoyaozi->addSkill(new Yangsheng);

    // for skill cards
    addMetaObject<JianaiCard>();
    addMetaObject<FeigongCard>();
    addMetaObject<ShengxueCard>();
    addMetaObject<YoulanCard>();
    addMetaObject<SuoshaCard>();
    addMetaObject<HuomeiCard>();
    addMetaObject<ShangtongCard>();
    addMetaObject<PofuCard>();
    addMetaObject<ShenwuCard>();
    addMetaObject<DushaCard>();
    addMetaObject<LuoshengCard>();
    addMetaObject<EnchouCard>();
    addMetaObject<ShouyaoCard>();
    addMetaObject<ZhiyanCard>();
    addMetaObject<GuirouCard>();
    addMetaObject<YangshengCard>();
    //addMetaObject<QingnangCard>();
}

ADD_PACKAGE(Standard)
