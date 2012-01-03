#include "standard.h"
#include "skill.h"
#include "sp-package.h"
#include "client.h"
#include "carditem.h"
#include "engine.h"
#include "ai.h"

// skill cards
/*
GuidaoCard::GuidaoCard(){
    target_fixed = true;
    will_throw = false;
}

void GuidaoCard::use(Room *room, ServerPlayer *zhangjiao, const QList<ServerPlayer *> &targets) const{

}

LeijiCard::LeijiCard(){

}

bool LeijiCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty();
}

void LeijiCard::onEffect(const CardEffectStruct &effect) const{
    ServerPlayer *zhangjiao = effect.from;
    ServerPlayer *target = effect.to;

    Room *room = zhangjiao->getRoom();
    room->setEmotion(target, "bad");

    JudgeStruct judge;
    judge.pattern = QRegExp("(.*):(.*):(.*)");
    judge.good = false;
    judge.reason = "leiji";
    judge.who = target;

    room->judge(judge);

    if(judge.isBad()){
        DamageStruct damage;
        damage.card = NULL;
        damage.damage = 2;
        damage.from = zhangjiao;
        damage.to = target;
        damage.nature = DamageStruct::Thunder;

        room->damage(damage);
    }else
        room->setEmotion(zhangjiao, "bad");
}

HuangtianCard::HuangtianCard(){
    once = true;
}

void HuangtianCard::use(Room *room, ServerPlayer *, const QList<ServerPlayer *> &targets) const{
    ServerPlayer *zhangjiao = targets.first();
    if(zhangjiao->hasSkill("huangtian")){
        zhangjiao->obtainCard(this);
        room->setEmotion(zhangjiao, "good");
    }
}

bool HuangtianCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && to_select->hasLordSkill("huangtian") && to_select != Self;
}

class GuidaoViewAsSkill:public OneCardViewAsSkill{
public:
    GuidaoViewAsSkill():OneCardViewAsSkill(""){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return  pattern == "@guidao";
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getFilteredCard()->isBlack();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        GuidaoCard *card = new GuidaoCard;
        card->addSubcard(card_item->getFilteredCard());

        return card;
    }
};

class Guidao: public TriggerSkill{
public:
    Guidao():TriggerSkill("guidao"){
        view_as_skill = new GuidaoViewAsSkill;

        events << AskForRetrial;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        if(!TriggerSkill::triggerable(target))
            return false;

        if(target->isKongcheng()){
            bool has_black = false;
            int i;
            for(i=0; i<4; i++){
                const EquipCard *equip = target->getEquip(i);
                if(equip && equip->isBlack()){
                    has_black = true;
                    break;
                }
            }

            return has_black;
        }else
            return true;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        JudgeStar judge = data.value<JudgeStar>();

        QStringList prompt_list;
        prompt_list << "@guidao-card" << judge->who->objectName()
                << "" << judge->reason << judge->card->getEffectIdString();
        QString prompt = prompt_list.join(":");

        player->tag["Judge"] = data;
        const Card *card = room->askForCard(player, "@guidao", prompt, data);

        if(card){
            // the only difference for Guicai & Guidao
            player->obtainCard(judge->card);

            judge->card = Sanguosha->getCard(card->getEffectiveId());
            room->moveCardTo(judge->card, NULL, Player::Special);

            LogMessage log;
            log.type = "$ChangedJudge";
            log.from = player;
            log.to << judge->who;
            log.card_str = card->getEffectIdString();
            room->sendLog(log);

            room->sendJudgeResult(judge);
        }

        return false;
    }
};

class HuangtianViewAsSkill: public OneCardViewAsSkill{
public:
    HuangtianViewAsSkill():OneCardViewAsSkill("huangtianv"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->hasUsed("HuangtianCard") && player->getKingdom() == "kou";
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        const Card *card = to_select->getCard();
        return card->objectName() == "jink" || card->objectName() == "lightning";
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        HuangtianCard *card = new HuangtianCard;
        card->addSubcard(card_item->getFilteredCard());

        return card;
    }
};

class Huangtian: public GameStartSkill{
public:
    Huangtian():GameStartSkill("huangtian$"){

    }

    virtual void onGameStart(ServerPlayer *zhangjiao) const{
        Room *room = zhangjiao->getRoom();
        QList<ServerPlayer *> players = room->getAlivePlayers();
        foreach(ServerPlayer *player, players){
            room->attachSkillToPlayer(player, "huangtianv");
        }
    }
};

class LeijiViewAsSkill: public ZeroCardViewAsSkill{
public:
    LeijiViewAsSkill():ZeroCardViewAsSkill("leiji"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return  pattern == "@@leiji";
    }

    virtual const Card *viewAs() const{
        return new LeijiCard;
    }
};

class Leiji: public TriggerSkill{
public:
    Leiji():TriggerSkill("leiji"){
        events << CardResponsed;
        view_as_skill = new LeijiViewAsSkill;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *zhangjiao, QVariant &data) const{
        CardStar card_star = data.value<CardStar>();
        if(!card_star->inherits("Jink"))
            return false;

        Room *room = zhangjiao->getRoom();
        room->askForUseCard(zhangjiao, "@@leiji", "@leiji");

        return false;
    }
};

ShensuCard::ShensuCard(){
    mute = true;
}

bool ShensuCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(!targets.isEmpty())
        return false;

    if(to_select->hasSkill("kongcheng") && to_select->isKongcheng())
        return false;

    if(to_select == Self)
        return false;

    return true;
}

void ShensuCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this);

    Slash *slash = new Slash(Card::NoSuit, 0);
    slash->setSkillName("shensu");
    CardUseStruct use;
    use.card = slash;
    use.from = source;
    use.to = targets;

    room->useCard(use);
}

class ShensuViewAsSkill: public ViewAsSkill{
public:
    ShensuViewAsSkill():ViewAsSkill("shensu"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        if(ClientInstance->getPattern().endsWith("1"))
            return false;
        else
            return selected.isEmpty() && to_select->getCard()->inherits("EquipCard");
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return  pattern.startsWith("@@shensu");
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(ClientInstance->getPattern().endsWith("1")){
            if(cards.isEmpty())
                return new ShensuCard;
            else
                return NULL;
        }else{
            if(cards.length() != 1)
                return NULL;

            ShensuCard *card = new ShensuCard;
            card->addSubcards(cards);

            return card;
        }
    }
};

class Shensu: public PhaseChangeSkill{
public:
    Shensu():PhaseChangeSkill("shensu"){
        view_as_skill = new ShensuViewAsSkill;
    }

    virtual bool onPhaseChange(ServerPlayer *xiahouyuan) const{
        Room *room = xiahouyuan->getRoom();

        if(xiahouyuan->getPhase() == Player::Judge){
            if(room->askForUseCard(xiahouyuan, "@@shensu1", "@shensu1")){
                xiahouyuan->skip(Player::Draw);
                return true;
            }
        }else if(xiahouyuan->getPhase() == Player::Play){
            if(room->askForUseCard(xiahouyuan, "@@shensu2", "@shensu2")){
                return true;
            }
        }

        return false;
    }
};

class Jushou: public PhaseChangeSkill{
public:
    Jushou():PhaseChangeSkill("jushou"){

    }

    virtual bool onPhaseChange(ServerPlayer *target) const{
        if(target->getPhase() == Player::Finish){
            Room *room = target->getRoom();
            if(room->askForong->getRoom();
        if(huangzhong->getPhase() != Player::Play)
            return false;

        int num = effect.to->getHandcardNum();
        if(num >= huangzhong->getHp() || num <= huangzhong->getAttackRange()){
            if(huangzhong->askForSkillInvoke(objectName(), QVariant::fromValue(effect))){
                room->playSkillEffect(objectName());
                room->slashResult(effect, NULL);

                return true;
            }
        }

        return false;
    }
};
*/

class Shuntian: public TriggerSkill{
public:
    Shuntian():TriggerSkill("shuntian"){
        events << GameStart;
        frequency = Compulsory;
    }

    virtual bool trigger(TriggerEvent, ServerPlayer *player, QVariant &data) const{
        Room *rom = player->getRoom();
        QString kim = player->isLord() ?
                      player->getKingdom() == "sp" ? "guan" : player->getKingdom() :
                      rom->getLord()->getKingdom();
        rom->setPlayerProperty(player, "kingdom", kim);
        return false;
    }
};

YuzhongCard::YuzhongCard(){

}

bool YuzhongCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    int num = Self->getMark("YuZy");
    return targets.length() < num;
}

bool YuzhongCard::targetsFeasible(const QList<const Player *> &targets, const Player *Self) const{
    int num = Self->getMark("YuZy");
    return targets.length() <= num;
}

void YuzhongCard::onEffect(const CardEffectStruct &effect) const{
    effect.to->drawCards(1);
}

class YuzhongViewAsSkill: public ZeroCardViewAsSkill{
public:
    YuzhongViewAsSkill():ZeroCardViewAsSkill("Yuzhong"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *, const QString &pattern) const{
        return pattern == "@@yuzhong";
    }

    virtual const Card *viewAs() const{
        return new YuzhongCard;
    }
};

class Yuzhong: public TriggerSkill{
public:
    Yuzhong():TriggerSkill("yuzhong"){
        events << Death;
        view_as_skill = new YuzhongViewAsSkill;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    static int getKingdoms(Room *room){
        QSet<QString> kingdom_set;
        foreach(ServerPlayer *p, room->getAlivePlayers()){
            kingdom_set << p->getKingdom();
        }
        return kingdom_set.size();
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        int num = getKingdoms(room);
        DamageStar damage = data.value<DamageStar>();
        if(damage->from != damage->to && damage->from->hasSkill(objectName())){
            ServerPlayer *source = damage->from;
            QString choice = room->askForChoice(source, objectName(), "hp+card+cancel");
            if(choice != "cancel"){
                LogMessage log;
                log.type = "#InvokeSkill";
                log.from = source;
                log.arg = "yuzhong";
                room->sendLog(log);
            }
            if(choice == "hp"){
                RecoverStruct rev;
                rev.who = source;
                rev.recover = num;
                room->recover(room->getLord(), rev);
            }
            else if(choice == "card"){
                room->getLord()->drawCards(num);
            }
        }
        return false;
    }
};

class Yuzhong2: public TriggerSkill{
public:
    Yuzhong2():TriggerSkill("#yuzh0ng"){
        events << Damage;
    }

    virtual bool trigger(TriggerEvent, ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        room->setPlayerMark(player, "YuZy", Yuzhong::getKingdoms(room));
        DamageStruct damage = data.value<DamageStruct>();
        if(!damage.to->isLord())
            return false;
        QString choice = room->askForChoice(player, "yuzhong", "all+me+cancel");
        if(choice == "cancel")
            return false;
        if(choice == "all")
            if(!room->askForUseCard(player, "@@yuzhong", "@yuzhong"))
                choice = "me";
        if(choice == "me"){
            LogMessage log;
            log.type = "#InvokeSkill";
            log.from = player;
            log.arg = "yuzhong";
            room->sendLog(log);
            player->drawCards(2);
        }

        room->setPlayerMark(player, "YuZy", 0);
        return false;
    }
};

SPPackage::SPPackage()
    :Package("sp")
{
    General *jiangsong = new General(this, "jiangsong", "sp");
    jiangsong->addSkill(new Yuzhong);
    jiangsong->addSkill(new Yuzhong2);
    related_skills.insertMulti("yuzhong", "#yuzh0ng");
    jiangsong->addSkill(new Shuntian);

/*
    General *sp_diaochan = new General(this, "sp_diaochan", "qun", 3, false, true);
    sp_diaochan->addSkill("Qianxian");
    sp_diaochan->addSkill("biyue");
    sp_diaochan->addSkill(new Xuwei);

    General *sp_sunshangxiang = new General(this, "sp_sunshangxiang", "shu", 3, false, true);
    sp_sunshangxiang->addSkill("jieyin");
    sp_sunshangxiang->addSkill("xiaoji");

    General *sp_guanyu = new General(this, "sp_guanyu", "wei", 4);
    sp_guanyu->addSkill("wusheng");
    sp_guanyu->addSkill(new Danji);

    General *sp_caiwenji = new General(this, "sp_caiwenji", "wei", 3, false, true);
    sp_caiwenji->addSkill("beige");
    sp_caiwenji->addSkill("duanchang");

    General *sp_machao = new General(this, "sp_machao", "qun", 4, true, true);
    sp_machao->addSkill("mashu");
    sp_machao->addSkill("tieji");
*/
    addMetaObject<YuzhongCard>();
}

ADD_PACKAGE(SP);
