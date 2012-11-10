#include "standard.h"
#include "skill.h"
#include "monkey.h"
#include "client.h"
#include "carditem.h"
#include "engine.h"
#include "ai.h"
#include "plough.h"

class Lianzang: public TriggerSkill{
public:
    Lianzang():TriggerSkill("lianzang"){
        events << Death;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        ServerPlayer *tenkei = room->findPlayerBySkillName(objectName());
        if(!tenkei)
            return false;
        QVariantList eatdeath_skills = tenkei->tag["EatDeath"].toList();
        if(room->askForSkillInvoke(tenkei, objectName(), data)){
            QStringList eatdeaths;
            foreach(QVariant tmp, eatdeath_skills)
                eatdeaths << tmp.toString();
            if(!eatdeaths.isEmpty()){
                QString choice = room->askForChoice(tenkei, objectName(), eatdeaths.join("+"));
                room->detachSkillFromPlayer(tenkei, choice);
                eatdeath_skills.removeOne(choice);
            }
            room->loseMaxHp(tenkei);
            QList<const Skill *> skills = player->getVisibleSkillList();
            foreach(const Skill *skill, skills){
                if(skill->parent() &&
                   skill->getFrequency() != Skill::Limited &&
                   skill->getFrequency() != Skill::Wake &&
                   !skill->isLordSkill()){
                    QString sk = skill->objectName();
                    room->acquireSkill(tenkei, sk);
                    eatdeath_skills << sk;
                }
            }
            tenkei->tag["EatDeath"] = eatdeath_skills;
        }

        return false;
    }
};

ShensuanCard::ShensuanCard(){
}

bool ShensuanCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.length() < subcardsLength() && to_select->isWounded();
}

bool ShensuanCard::targetsFeasible(const QList<const Player *> &targets, const Player *Self) const{
    return targets.length() <= subcardsLength();
}

void ShensuanCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    if(targets.isEmpty()){
        QList<ServerPlayer *> to;
        to << source;
        SkillCard::use(room, source, to);
    }else
        SkillCard::use(room, source, targets);
}

void ShensuanCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.to->getRoom();

    if(effect.to->isWounded()){
        RecoverStruct recover;
        recover.card = this;
        recover.who = effect.from;
        room->recover(effect.to, recover);
    }else
        effect.to->drawCards(2);
}

class ShensuanViewAsSkill: public ViewAsSkill{
public:
    ShensuanViewAsSkill():ViewAsSkill("shensuan"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return  pattern == "@@shensuan";
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        if(selected.length() >= 3)
            return false;
        int sum = 0;
        foreach(CardItem *item, selected){
            sum += item->getCard()->getNumber();
        }
        sum += to_select->getCard()->getNumber();
        return sum <= Self->getMark("shensuan");
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        int sum = 0;
        foreach(CardItem *item, cards){
            sum += item->getCard()->getNumber();
        }

        if(sum == Self->getMark("shensuan")){
            ShensuanCard *card = new ShensuanCard;
            card->addSubcards(cards);
            return card;
        }else
            return NULL;
    }
};

class Shensuan: public MasochismSkill{
public:
    Shensuan():MasochismSkill("shensuan"){
        view_as_skill = new ShensuanViewAsSkill;
    }

    virtual void onDamaged(ServerPlayer *jiangjing, const DamageStruct &damage) const{
        const Card *card = damage.card;
        if(card == NULL)
            return;

        int point = card->getNumber();
        if(point == 0)
            return;

        if(jiangjing->isNude())
            return;

        Room *room = jiangjing->getRoom();
        room->setPlayerMark(jiangjing, objectName(), point);

        QString prompt = QString("@shensuan:::%1").arg(point);
        room->askForUseCard(jiangjing, "@@shensuan", prompt, true);
    }
};

class ShensuanMC: public ClientSkill{
public:
    ShensuanMC():ClientSkill("#shensuan-mc"){
    }

    virtual int getExtra(const Player *target) const{
        if(target->hasSkill(objectName()))
            return 2;
        else
            return 0;
    }
};

class Gunzhu:public TriggerSkill{
public:
    Gunzhu():TriggerSkill("gunzhu"){
        frequency = Frequent;
        events << CardUsed << CardResponsed;
    }

    virtual bool trigger(TriggerEvent event, Room* room, ServerPlayer *jiangjing, QVariant &data) const{
        CardStar card = NULL;
        if(event == CardUsed){
            CardUseStruct use = data.value<CardUseStruct>();
            card = use.card;
        }else if(event == CardResponsed)
            card = data.value<CardStar>();

        if(card->isNDTrick()){
            int num = card->getNumber();
            if(room->askForSkillInvoke(jiangjing, objectName())){
                room->playSkillEffect(objectName());
                JudgeStruct judge;
                judge.reason = objectName();
                judge.who = jiangjing;

                room->judge(judge);
                if(qAbs(judge.card->getNumber() - num) < 3)
                    jiangjing->drawCards(2);
            }
        }
        return false;
    }
};

class Touxi: public TriggerSkill{
public:
    Touxi():TriggerSkill("touxi"){
        events << CardResponsed;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return !target->hasSkill(objectName());
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        CardStar card_star = data.value<CardStar>();
        if(!card_star->inherits("Jink"))
            return false;
        ServerPlayer *duwei = room->findPlayerBySkillName(objectName());
        if(!duwei)
            return false;
        bool caninvoke = false;
        foreach(const Card *tp, duwei->getCards("he")){
            if(tp->inherits("Weapon") || tp->inherits("Armor")){
                caninvoke = true;
                break;
            }
        }
        const Card *card = caninvoke ? room->askForCard(duwei, "Weapon,Armor", "@touxi:" + player->objectName(), true, data, NonTrigger): NULL;
        if(card){
            Assassinate *ass = new Assassinate(card->getSuit(), card->getNumber());
            ass->setSkillName(objectName());
            ass->addSubcard(card);
            CardUseStruct use;
            use.card = ass;
            use.from = duwei;
            use.to << player;
            room->useCard(use);
        }
        return false;
    }
};

class Guanxing:public PhaseChangeSkill{
public:
    Guanxing():PhaseChangeSkill("guanxing"){
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

class Fanzhan:public PhaseChangeSkill{
public:
    Fanzhan():PhaseChangeSkill("fanzhan"){
    }

    virtual bool onPhaseChange(ServerPlayer *fuji) const{
        Room *room = fuji->getRoom();
        if(fuji->getPhase() == Player::Start){
            room->setTag("Fanzhan", false);
            return false;
        }
        if(fuji->getPhase() != Player::NotActive)
            return false;
        if(fuji->isWounded() || room->getAlivePlayers().length() <= 2)
            return false;
        if(fuji->askForSkillInvoke(objectName())){
            room->playSkillEffect(objectName());
            room->setTag("Fanzhan", true);
        }
        return false;
    }
};

class FanzhanClear: public TriggerSkill{
public:
    FanzhanClear():TriggerSkill("#fanzhan-clear"){
        events << Death;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target->hasSkill(objectName());
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &) const{
        room->removeTag("Fanzhan");
        return false;
    }
};

class Shuilao: public OneCardViewAsSkill{
public:
    Shuilao():OneCardViewAsSkill("shuilao"){

    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getCard()->isNDTrick();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *first = card_item->getCard();
        Indulgence *indulgence = new Indulgence(first->getSuit(), first->getNumber());
        indulgence->addSubcard(first->getId());
        indulgence->setSkillName(objectName());
        return indulgence;
    }
};

class Tancai:public PhaseChangeSkill{
public:
    Tancai():PhaseChangeSkill("tancai"){
    }

    virtual int getPriority() const{
        return 2;
    }

    virtual bool onPhaseChange(ServerPlayer *dujian) const{
        if(dujian->getPhase() == Player::Discard &&
           dujian->askForSkillInvoke(objectName())){
            dujian->playSkillEffect(objectName());
            dujian->drawCards(dujian->getLostHp() + 1, false);
        }
        return false;
    }
};

JingtianCard::JingtianCard(){
}

bool JingtianCard::targetFilter(const QList<const Player *> &targets, const Player *t, const Player *Self) const{
    return targets.isEmpty() && t != Self;
}

void JingtianCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();
    LogMessage log;
    log.type = "#Jingtian";
    log.from = effect.from;
    log.to << effect.to;
    log.arg = QString::number(effect.to->getHp());
    log.arg2 = QString::number(effect.from->getHp());

    room->sendLog(log);
    room->setPlayerProperty(effect.to, "hp", effect.from->getHp());
}

class Jingtian:public ViewAsSkill{
public:
    Jingtian():ViewAsSkill("jingtian"){
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        if(selected.isEmpty())
            return !to_select->isEquipped();
        else if(selected.length() == 1){
            const Card *card = selected.first()->getFilteredCard();
            return !to_select->isEquipped() && to_select->getFilteredCard()->getSuit() == card->getSuit();
        }else
            return false;
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.length() != 2)
            return NULL;
        JingtianCard *card = new JingtianCard;
        card->addSubcards(cards);
        return card;
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->hasUsed("JingtianCard");
    }
};

class Wodao: public FilterSkill{
public:
    Wodao():FilterSkill("wodao"){

    }

    virtual bool viewFilter(const CardItem *to_select) const{
        const Card *card = to_select->getCard();
        return (card->getSuit() == Card::Spade && card->inherits("TrickCard")) ||
                card->inherits("EventsCard") ||
                card->getSuit() == Card::Heart;
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        const Card *c = card_item->getCard();
        if(c->getSuit() == Card::Heart && !c->inherits("EventsCard")){
            Slash *slash = new Slash(c->getSuit(), c->getNumber());
            slash->setSkillName(objectName());
            slash->addSubcard(card_item->getCard());
            return slash;
        }
        else{
            Duel *duel = new Duel(c->getSuit(), c->getNumber());
            duel->setSkillName(objectName());
            duel->addSubcard(card_item->getCard());
            return duel;
        }
    }
};

class Tongmou: public SlashBuffSkill{
public:
    Tongmou():SlashBuffSkill("tongmou"){
    }

    virtual bool buff(const SlashEffectStruct &effect) const{
        ServerPlayer *fuan = effect.from;
        Room *room = fuan->getRoom();
        QList<ServerPlayer *> targets;
        foreach(ServerPlayer *tmp, room->getOtherPlayers(fuan)){
            if(tmp->inMyAttackRange(effect.to))
                targets << tmp;
        }
        if(targets.isEmpty() || !fuan->askForSkillInvoke(objectName()))
            return false;
        ServerPlayer *target = room->askForPlayerChosen(fuan, targets, objectName());
        if(room->askForCard(target, "slash", "@tongmou:" + fuan->objectName())){
            room->playSkillEffect(objectName());
            LogMessage log;
            log.type = "#Tongmou";
            log.from = effect.to;
            log.to << fuan << target;
            log.arg = objectName();
            room->sendLog(log);

            room->slashResult(effect, NULL);
            return true;
        }
        return false;
    }
};

XianhaiCard::XianhaiCard(){
}

bool XianhaiCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty();
}

void XianhaiCard::onUse(Room *room, const CardUseStruct &card_use) const{
    const Card *c = Sanguosha->getCard(getSubcards().first());
    Drivolt *drive = new Drivolt(c->getSuit(), c->getNumber());
    drive->setSkillName("xianhai");
    drive->addSubcard(c);
    CardUseStruct use;
    use.card = drive;
    use.from = card_use.from;
    use.to = card_use.to;
    room->useCard(use);
}

class XianhaiViewAsSkill: public OneCardViewAsSkill{
public:
    XianhaiViewAsSkill():OneCardViewAsSkill("xianhai"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return pattern == "@@xianhai";
    }

    virtual bool viewFilter(const CardItem *n) const{
        return !n->isEquipped() && n->getCard()->getSuit() == Card::Spade;
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        XianhaiCard *card = new XianhaiCard;
        card->addSubcard(card_item->getCard()->getId());
        return card;
    }
};

class Xianhai: public MasochismSkill{
public:
    Xianhai():MasochismSkill("xianhai"){
        view_as_skill = new XianhaiViewAsSkill;
    }

    virtual void onDamaged(ServerPlayer *an, const DamageStruct &damage) const{
        Room *room = an->getRoom();
        room->askForUseCard(an, "@@xianhai", "@xianhai", true);
    }
};

class Shenyong:public TriggerSkill{
public:
    Shenyong():TriggerSkill("shenyong"){
        events << CardAsked;
    }

    virtual int getPriority() const{
        return 2;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *shibao, QVariant &data) const{
        QString asked = data.toString();
        if(asked == "jink" && shibao->askForSkillInvoke(objectName())){
            if(room->askForUseCard(shibao, "slash", "@askforslash", true)){
                Jink *jink = new Jink(Card::NoSuit, 0);
                jink->setSkillName(objectName());
                room->provide(jink);
                room->setEmotion(shibao, "good");
            }
        }
        return false;
    }
};

class Qingdong: public TriggerSkill{
public:
    Qingdong():TriggerSkill("qingdong"){
        events << CardLost;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return !target->hasSkill(objectName()) &&
                target->getGeneral()->isMale() &&
                target->getPhase() == Player::NotActive;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *conan, QVariant &data) const{
        ServerPlayer *yulan = room->findPlayerBySkillName(objectName());
        CardMoveStar move = data.value<CardMoveStar>();
        if(conan->isDead() || !yulan)
            return false;
        if(move->from_place == Player::Hand && yulan->askForSkillInvoke(objectName())){
            room->playSkillEffect(objectName());
            JudgeStruct judge;
            judge.pattern = QRegExp("(.*):(diamond):(.*)");
            judge.good = false;
            judge.reason = objectName();
            judge.who = conan;

            room->judge(judge);
            if(judge.isGood()){
                int cd = conan->getMaxHP() - conan->getHandcardNum();
                conan->drawCards(qMin(4, qMax(0, cd)));
            }
        }
        return false;
    }
};

class Qing5hang: public TriggerSkill{
public:
    Qing5hang():TriggerSkill("qing5hang"){
        events << Death;
        frequency = Compulsory;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target->hasSkill(objectName());
    }

    virtual bool trigger(TriggerEvent, Room*, ServerPlayer *player, QVariant &data) const{
        DamageStar damage = data.value<DamageStar>();
        ServerPlayer *killer = damage ? damage->from : NULL;

        if(killer){
            if(killer != player)
                killer->gainMark("@shang");
        }
        return false;
    }
};

MonkeyPackage::MonkeyPackage()
    :GeneralPackage("monkey")
{
    General *caiqing = new General(this, "caiqing", "jiang", 5);
    caiqing->addSkill(new Lianzang);

    General *shenjiangjing = new General(this, "shenjiangjing", "god", 3);
    shenjiangjing->addSkill(new Shensuan);
    shenjiangjing->addSkill(new ShensuanMC);
    related_skills.insertMulti("shensuan", "#shensuan-mc");
    shenjiangjing->addSkill(new Gunzhu);

    General *duwei = new General(this, "duwei", "jiang");
    duwei->addSkill(new Touxi);

    General *puwenying = new General(this, "puwenying", "guan", 3);
    puwenying->addSkill(new Guanxing);
    puwenying->addSkill(new Fanzhan);
    puwenying->addSkill(new FanzhanClear);
    related_skills.insertMulti("fanzhan", "#fanzhan-clear");

    General *tongmeng = new General(this, "tongmeng", "min", 3);
    tongmeng->addSkill(new Shuilao);
    tongmeng->addSkill(new Skill("shuizhan", Skill::Compulsory));

    General *zhangmengfang = new General(this, "zhangmengfang", "guan");
    zhangmengfang->addSkill(new Tancai);

    General *litianrun = new General(this, "litianrun", "jiang");
    litianrun->addSkill(new Jingtian);

    General *duxue = new General(this, "duxue", "kou");
    duxue->addSkill(new Wodao);

    General *fuan = new General(this, "fuan", "guan", 3);
    fuan->addSkill(new Tongmou);
    fuan->addSkill(new Xianhai);

    General *shibao = new General(this, "shibao", "jiang");
    shibao->addSkill(new Shenyong);

    General *yulan = new General(this, "yulan", "guan", 3, false);
    yulan->addSkill(new Qingdong);
    yulan->addSkill(new Qing5hang);

    addMetaObject<ShensuanCard>();
    addMetaObject<JingtianCard>();
    addMetaObject<XianhaiCard>();
}

ADD_PACKAGE(Monkey)
