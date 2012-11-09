#include "boar.h"
#include "client.h"
#include "engine.h"
#include "carditem.h"
#include "settings.h"
#include "maneuvering.h"

class Jinqiang:public TriggerSkill{
public:
    Jinqiang():TriggerSkill("jinqiang"){
        events << Predamage;
        frequency = Compulsory;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *xuning, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        if(xuning != damage.to && damage.to->isChained() && damage.nature == DamageStruct::Normal){
            LogMessage log;
            log.type = "#TriggerSkill";
            log.from = xuning;
            log.arg = objectName();
            room->sendLog(log);
            room->playSkillEffect(objectName());

            damage.damage ++;
            data = QVariant::fromValue(damage);
            return false;
        }
        return false;
    }
};

class Jiebei: public TriggerSkill{
public:
    Jiebei():TriggerSkill("jiebei"){
        events << CardLost << FinishJudge;
        frequency = Frequent;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target->getRoom()->findPlayerBySkillName(objectName());
    }

    static void Callback(QVariant &data, ServerPlayer *xuning, ServerPlayer *from, const Card *armor){
        Room *room = xuning->getRoom();
        QList<ServerPlayer *> tos;
        foreach(ServerPlayer *tmp, room->getAllPlayers()){
            if(!tmp->getArmor())
                tos << tmp;
        }
        QString prompt = QString("@jiebei:%1::%2").arg(from->objectName()).arg(armor->objectName());
        if(!tos.isEmpty() && room->askForCard(xuning, ".", prompt, true, data, CardDiscarded)){
            room->playSkillEffect("jiebei", 1);
            xuning->obtainCard(armor);
            ServerPlayer *to = room->askForPlayerChosen(xuning, tos, "jiebei");
            room->moveCardTo(armor, to, Player::Equip);
        }
    }

    virtual bool trigger(TriggerEvent event, Room* room, ServerPlayer *player, QVariant &data) const{
        ServerPlayer *xuning = room->findPlayerBySkillName(objectName());
        if(event == CardLost){
            CardMoveStar move = data.value<CardMoveStar>();
            const Card *armor = Sanguosha->getCard(move->card_id);
            if(!armor->inherits("Armor"))
                return false;
            if(move->from_place == Player::Equip && move->from == xuning){
                if(room->askForSkillInvoke(xuning, objectName())){
                    room->playSkillEffect(objectName(), 2);
                    xuning->drawCards(2);
                }
            }
            if(room->getCardPlace(move->card_id) == Player::DiscardedPile){
                Callback(data, xuning, move->from, armor);
                return false;
            }
            return false;
        }else if(event == FinishJudge){
            JudgeStar judge = data.value<JudgeStar>();
            if(room->getCardPlace(judge->card->getEffectiveId()) == Player::DiscardedPile && judge->card->inherits("Armor")){
                Callback(data, xuning, judge->who, judge->card);
                return false;
            }
        }
        return false;
    }
};

class Shenchou: public MasochismSkill{
public:
    Shenchou():MasochismSkill("shenchou"){
        frequency = Compulsory;
    }

    virtual void onDamaged(ServerPlayer *wusong, const DamageStruct &damage) const{
        Room *room = wusong->getRoom();
        if(wusong->hasMark("shenchou"))
            return;
        const Card *card = damage.card;
        if(card && room->obtainable(card, wusong)){
            LogMessage log;
            log.type = "#TriggerSkill";
            log.from = wusong;
            log.arg = objectName();
            room->playSkillEffect(objectName());
            room->sendLog(log);

            wusong->addToPile("chou", card->getEffectiveId(), true);
        }
    }
};

class Wujie: public PhaseChangeSkill{
public:
    Wujie():PhaseChangeSkill("wujie"){
        frequency = Wake;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return PhaseChangeSkill::triggerable(target)
                && target->getPhase() == Player::Start
                && !target->hasMark("wujie")
                && target->getPile("chou").length() >= 3;
    }

    virtual bool onPhaseChange(ServerPlayer *wusong) const{
        Room *room = wusong->getRoom();

        room->broadcastInvoke("animate", "lightbox:$wujie");
        room->setPlayerMark(wusong, "wujie", 1);
        room->setPlayerProperty(wusong, "maxhp", QVariant(wusong->getMaxHp() + 1));

        room->playSkillEffect(objectName());
        room->acquireSkill(wusong, "zhusha");

        return false;
    }
};

ZhushaCard::ZhushaCard(){
    target_fixed = true;
}

void ZhushaCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    PlayerStar wusong = source;
    QList<int> chous = wusong->getPile("chou");

    int card_id = -1;
    if(chous.length() == 1)
        card_id = chous.first();
    else{
        room->fillAG(chous, wusong);
        card_id = room->askForAG(wusong, chous, true, "zhusha");
        wusong->invoke("clearAG");

        if(card_id == -1)
            return;
    }

    const Card *card = Sanguosha->getCard(card_id);
    room->moveCardTo(card, NULL, Player::DiscardedPile, true);
    room->loseMaxHp(wusong, 1);
    Self->setFlags("zhusha_effect");
}

class ZhushaDiscard: public ZeroCardViewAsSkill{
public:
    ZhushaDiscard():ZeroCardViewAsSkill("zhusha"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->hasUsed("ZhushaCard") && !player->getPile("chou").isEmpty();
    }

    virtual const Card *viewAs() const{
        return new ZhushaCard;
    }
};

class Zhusha:public TriggerSkill{
public:
    Zhusha():TriggerSkill("zhusha"){
        events << CardUsed;
        view_as_skill = new ZhushaDiscard;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *wusong, QVariant &data) const{
        CardUseStruct use = data.value<CardUseStruct>();
        if(use.card->inherits("Slash") && Self->hasFlag("zhusha_effect")){
            room->playSkillEffect(objectName());
            LogMessage ogg;
            ogg.type = "#Zhusha";
            ogg.from = wusong;
            ogg.arg = objectName();

            Self->setFlags("-zhusha_effect");
            use.to.clear();
            foreach(ServerPlayer *p, room->getOtherPlayers(wusong))
                use.to << p;
            ogg.to = use.to;
            room->sendLog(ogg);

            data = QVariant::fromValue(use);
            return false;
        }
        return false;
    }
};

DuanbiCard::DuanbiCard(){

}

bool DuanbiCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty();
}

void DuanbiCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    room->throwCard(this);
    ServerPlayer *target = targets.first();
    room->broadcastInvoke("animate", "lightbox:$duanbi");
    source->loseMark("@arm");
    room->setPlayerProperty(source, "maxhp", 1);
    DamageStruct damage;
    damage.from = source;
    damage.to = target;
    damage.damage = 2;
    room->damage(damage);
    room->detachSkillFromPlayer(source, "shenchou");
    source->addMark("shenchou");
}

class Duanbi: public ZeroCardViewAsSkill{
public:
    Duanbi():ZeroCardViewAsSkill("duanbi"){
        frequency = Limited;
    }

    virtual const Card *viewAs() const{
        return new DuanbiCard;
    }

protected:
    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->hasMark("@arm")
                && player->getHp() == 1
                && player->getMaxHP() > 1;
    }
};

class XianjiClear: public PhaseChangeSkill{
public:
    XianjiClear():PhaseChangeSkill("#xianji-clear"){

    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool onPhaseChange(ServerPlayer *target) const{
        if(target->getPhase() == Player::NotActive){
            Room *room = target->getRoom();
            QList<ServerPlayer *> players = room->getAllPlayers();
            foreach(ServerPlayer *player, players){
                if(player->hasFlag("jilei")){
                    player->jilei(".");
                    player->invoke("jilei");

                    LogMessage log;
                    log.type = "#XianjiClear";
                    log.from = player;
                    room->sendLog(log);
                }
            }
        }

        return false;
    }
};

class Xianji: public PhaseChangeSkill{
public:
    Xianji():PhaseChangeSkill("xianji"){
    }

    virtual int getPriority() const{
        return 2;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return !target->hasSkill(objectName());
    }

    virtual bool onPhaseChange(ServerPlayer *player) const{
        PlayerStar target = player;
        if(target->getPhase() != Player::Start)
            return false;
        Room *room = target->getRoom();
        QList<ServerPlayer *> waster = room->findPlayersBySkillName(objectName());
        const Card *card = NULL;
        if(waster.isEmpty())
            return false;
        foreach(ServerPlayer *waste, waster){
            if(!waste->isNude())
                card = room->askForCard(waste, "..", "@xianji", true, QVariant::fromValue(target), CardDiscarded);
            if(!card)
               continue;
            QString choice = card->getType();
            //QString choice = card->getSuitString();
            room->playSkillEffect(objectName());

            target->jilei(choice);
            target->invoke("jilei", choice);
            target->setFlags("jilei");

            LogMessage log;
            log.type = "#Xianji";
            log.from = waste;
            log.to << target;
            log.arg2 = objectName();
            log.arg = choice;
            room->sendLog(log);
        }
        return false;
    }
};

class Houlue: public TriggerSkill{
public:
    Houlue():TriggerSkill("houlue"){
        events << CardLost;
        frequency = Frequent;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return !target->hasSkill(objectName());
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        ServerPlayer *lese = room->findPlayerBySkillName(objectName());
        if(!lese)
            return false;
        CardMoveStar move = data.value<CardMoveStar>();
        if(move->to_place == Player::DiscardedPile){
            const Card *card = Sanguosha->getCard(move->card_id);
            if(card->isNDTrick() && lese->askForSkillInvoke(objectName())){
                JudgeStruct judge;
                judge.pattern = QRegExp("(.*):(.*):(.*)");
                judge.good = true;
                judge.reason = objectName();
                judge.who = lese;

                room->judge(judge);
                if(!judge.card->inherits("TrickCard")){
                    lese->obtainCard(card);
                    room->playSkillEffect(objectName(), 1);
                }
                else{
                    lese->obtainCard(judge.card);
                    room->playSkillEffect(objectName(), 2);
                }
            }
        }
        return false;
    }
};

FeihuangCard::FeihuangCard(){
    target_fixed = true;
    will_throw = false;
}

void FeihuangCard::use(Room *, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    foreach(int card_id, this->getSubcards())
        source->addToPile("stone", card_id);
}

class FeihuangViewAsSkill: public ViewAsSkill{
public:
    FeihuangViewAsSkill(): ViewAsSkill("feihuang"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        int n = Self->getHandcardNum() - Self->getHp();
        if(selected.length() >= n)
            return false;

        if(to_select->isEquipped())
            return false;

        return true;
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.length() != Self->getHandcardNum() - Self->getHp())
            return NULL;

        FeihuangCard *card = new FeihuangCard;
        card->addSubcards(cards);
        return card;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return pattern == "@@feihuang";
    }
};

class Feihuang: public PhaseChangeSkill{
public:
    Feihuang():PhaseChangeSkill("feihuang"){
        view_as_skill = new FeihuangViewAsSkill;
    }

    virtual bool onPhaseChange(ServerPlayer *vq) const{
        if(vq->getPhase() != Player::Discard)
            return false;
        Room *room = vq->getRoom();
        if(vq->getHandcardNum() > vq->getHp() && room->askForUseCard(vq, "@@feihuang", "@feihuang", true))
            return true;
        return false;
    }
};

MeiyuCard::MeiyuCard(){
    mute = true;
}

bool MeiyuCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && Self->canSlash(to_select, false);
}

void MeiyuCard::onUse(Room *room, const CardUseStruct &card_use) const{
    room->throwCard(card_use.from->getPile("stone").last());
    Slash *slash = new Slash(Card::NoSuit, 0);
    slash->setSkillName(skill_name);
    CardUseStruct use;
    use.card = slash;
    use.from = card_use.from;
    use.to = card_use.to;
    room->useCard(use, false);
}

class MeiyuViewAsSkill: public ZeroCardViewAsSkill{
public:
    MeiyuViewAsSkill():ZeroCardViewAsSkill("meiyu"){
    }

    virtual const Card *viewAs() const{
        return new MeiyuCard;
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->getPile("stone").isEmpty();
    }
};

class Meiyu: public TriggerSkill{
public:
    Meiyu():TriggerSkill("meiyu"){
        events << Predamage;
        view_as_skill = new MeiyuViewAsSkill;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *moyujian, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        if(!damage.card || !damage.card->inherits("Slash") || damage.damage < 1)
            return false;
        if(damage.card->getSkillName() == "meiyu" && damage.to->isAlive()){
            room->loseMaxHp(damage.to, damage.damage);
            return true;
        }
        return false;
    }
};

HuafoCard::HuafoCard(){

}

bool HuafoCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && Self->inMyAttackRange(to_select);
            /* && Slash::targetFilter(targets, to_select, Self);*/
}

bool HuafoCard::targetsFeasible(const QList<const Player *> &targets, const Player *Self) const{
    return (targets.isEmpty() && Analeptic::IsAvailable(Self)) ||
            (!targets.isEmpty() && Slash::IsAvailable(Self));
}

void HuafoCard::onUse(Room *room, const CardUseStruct &card_use) const{
    room->throwCard(this);
    int card_id = getSubcards().first();
    Card::Suit suit = Sanguosha->getCard(card_id)->getSuit();
    int num = Sanguosha->getCard(card_id)->getNumber();
    CardUseStruct use;
    use.from = card_use.from;
    if(card_use.to.isEmpty()){
        Analeptic *a = new Analeptic(suit, num);
        a->setSkillName("huafo");
        a->addSubcard(card_id);
        use.card = a;
    }
    else{
        Slash *e = new Slash(suit, num);
        e->setSkillName("huafo");
        e->addSubcard(card_id);
        use.card = e;
        use.to = card_use.to;
    }
    room->useCard(use);
}

class Huafo:public OneCardViewAsSkill{
public:
    Huafo():OneCardViewAsSkill("huafo"){

    }

    virtual bool viewFilter(const CardItem *to_select) const{
        const Card *card = to_select->getFilteredCard();

        switch(ClientInstance->getStatus()){
        case Client::Playing:{
                return card->inherits("BasicCard");
            }

        case Client::Responsing:{
                QString pattern = ClientInstance->getPattern();
                if(pattern.contains("analeptic"))
                    return card->inherits("BasicCard");
            }
        default:
            return false;
        }
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return pattern.contains("analeptic");
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        HuafoCard *card = new HuafoCard;
        card->addSubcard(card_item->getFilteredCard());
        return card;
    }
};

class Dunwu: public TriggerSkill{
public:
    Dunwu():TriggerSkill("dunwu"){
        events << Damage;
        frequency = Compulsory;
    }

    virtual int getPriority() const{
        return -1;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        if(damage.damage > 0){
            LogMessage log;
            log.type = "#Dunwu";
            log.from = player;
            log.arg = QString::number(damage.damage);
            log.arg2 = objectName();
            room->sendLog(log);

            room->playSkillEffect(objectName());
        }
        for(int i = 0; i < damage.damage; i++){
            int card_id = room->drawCard();
            const Card *card = Sanguosha->getCard(card_id);
            room->moveCardTo(card, NULL, Player::Special, true);
            room->getThread()->delay();

            if(card->inherits("Tsunami"))
                room->killPlayer(player);
            else
                player->addToPile("jiyan", card_id);

            if(player->getPile("jiyan").length() >= 4){
                LogMessage log;
                log.type = "#DunwuB";
                log.from = player;
                log.arg = "jiyan";
                log.arg2 = QString::number(4);
                room->sendLog(log);

                player->clearPile("jiyan");
                room->loseMaxHp(player);
            }
        }
        return false;
    }
};

BoarPackage::BoarPackage()
    :GeneralPackage("boar")
{
    General *shenwusong = new General(this, "shenwusong", "god", 5);
    shenwusong->addSkill(new Shenchou);
    shenwusong->addSkill(new Wujie);
    shenwusong->addRelateSkill("zhusha");
    skills << new Zhusha;
    shenwusong->addSkill(new Duanbi);
    shenwusong->addSkill(new MarkAssignSkill("@arm", 1));
    related_skills.insertMulti("duanbi", "#@arm-1");

    General *shenwuyong = new General(this, "shenwuyong", "god", 3);
    shenwuyong->addSkill(new Xianji);
    shenwuyong->addSkill(new XianjiClear);
    shenwuyong->addSkill(new Houlue);
    related_skills.insertMulti("xianji", "#xianji-clear");

    General *shenzhangqing = new General(this, "shenzhangqing", "god", 4);
    shenzhangqing->addSkill(new Feihuang);
    shenzhangqing->addSkill(new Meiyu);

    General *shenluzhishen = new General(this, "shenluzhishen", "god", 5);
    shenluzhishen->addSkill(new Dunwu);
    shenluzhishen->addSkill(new Huafo);

    General *shenxuning = new General(this, "shenxuning", "god");
    shenxuning->addSkill(new Jiebei);
    shenxuning->addSkill(new Jinqiang);

    addMetaObject<ZhushaCard>();
    addMetaObject<DuanbiCard>();
    addMetaObject<FeihuangCard>();
    addMetaObject<MeiyuCard>();
    addMetaObject<HuafoCard>();
}

ADD_PACKAGE(Boar)
