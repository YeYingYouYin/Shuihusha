#include "qjwm.h"
#include "skill.h"
#include "standard.h"
#include "tocheck.h"
#include "clientplayer.h"
#include "carditem.h"
#include "engine.h"

class Jingzhun: public SlashBuffSkill{
public:
    Jingzhun():SlashBuffSkill("jingzhun"){
        frequency = Compulsory;
    }

    virtual bool buff(const SlashEffectStruct &effect) const{
        ServerPlayer *huarong = effect.from;
        Room *room = huarong->getRoom();
        if(huarong->getPhase() != Player::Play)
            return false;

        if(huarong->distanceTo(effect.to) == huarong->getAttackRange()){
            room->playSkillEffect(objectName());
            LogMessage log;
            log.type = "#Jingzhun";
            log.from = huarong;
            log.to << effect.to;
            log.arg = objectName();
            room->sendLog(log);

            room->slashResult(effect, NULL);
            return true;
        }
        return false;
    }
};

class KaixianPattern: public CardPattern{
public:
    virtual bool match(const Player *player, const Card *card) const{
        return !player->hasEquip(card) &&
                card->getNumber() <= 5;
    }

    virtual bool willThrow() const{
        return false;
    }
};

class Kaixian: public PhaseChangeSkill{
public:
    Kaixian():PhaseChangeSkill("kaixian"){

    }

    virtual bool onPhaseChange(ServerPlayer *huarong) const{
        Room *room = huarong->getRoom();
        if(huarong->getPhase() == Player::Start && !huarong->isKongcheng()){
            bool caninvoke = false;
            foreach(const Card *cd, huarong->getHandcards()){
                if(cd->getNumber() <= 5){
                    caninvoke = true;
                    break;
                }
            }
            if(caninvoke && room->askForSkillInvoke(huarong, objectName())){
                const Card *card = room->askForCard(huarong, ".kaixian!", "@kaixian");
                room->setPlayerMark(huarong, "kaixian", card->getNumber());
                LogMessage log;
                log.type = "$Kaixian";
                log.from = huarong;
                log.card_str = card->getEffectIdString();
                room->sendLog(log);

                room->playSkillEffect(objectName());
            }
        }
        else if(huarong->getPhase() == Player::NotActive)
            room->setPlayerMark(huarong, "kaixian", 0);

        return false;
    }
};

class Kong1iang: public TriggerSkill{
public:
    Kong1iang():TriggerSkill("kong1iang"){
        events << PhaseChange;
    }

    static bool CompareBySuit(int card1, int card2){
        const Card *c1 = Sanguosha->getCard(card1);
        const Card *c2 = Sanguosha->getCard(card2);

        int a = static_cast<int>(c1->getSuit());
        int b = static_cast<int>(c2->getSuit());

        return a < b;
    }

    virtual bool trigger(TriggerEvent, ServerPlayer *liying, QVariant &data) const{
        Room *room = liying->getRoom();
        if(liying->getPhase() != Player::Draw)
            return false;
        if(room->askForSkillInvoke(liying, objectName())){
            room->playSkillEffect(objectName());
            liying->drawCards(liying->getMaxHP() + liying->getLostHp());

            QList<int> card_ids = liying->handCards();
            qSort(card_ids.begin(), card_ids.end(), CompareBySuit);
            int count = 0;
            while(!card_ids.isEmpty() && count < 2){
                room->fillAG(card_ids);
                int card_id = -1;
                do{
                    card_id = room->askForAG(liying, card_ids, false, objectName());
                }while(card_id < 0);
                room->throwCard(card_id);
                card_ids.removeOne(card_id);

                // throw the rest cards that matches the same suit
                const Card *card = Sanguosha->getCard(card_id);
                Card::Suit suit = card->getSuit();
                LogMessage ogg;
                ogg.type = "#Kongrice";
                ogg.from = liying;
                ogg.arg = card->getSuitString();
                room->sendLog(ogg);
                for(int i = card_ids.length() - 1; i > -1; i --){
                    const Card *c = Sanguosha->getCard(card_ids.at(i));
                    if(c->getSuit() == suit){
                        card_ids.removeAt(i);
                        room->throwCard(c);
                    }
                }
                count ++;
                room->broadcastInvoke("clearAG");
            }
            return true;
        }
        return false;
    }
};

class Liba: public TriggerSkill{
public:
    Liba():TriggerSkill("liba"){
        events << Predamage;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *luda, QVariant &data) const{
        Room *room = luda->getRoom();
        if(luda->getPhase() != Player::Play)
            return false;
        DamageStruct damage = data.value<DamageStruct>();

        if(damage.card && damage.card->inherits("Slash") && damage.to->isAlive()
            && !damage.to->isKongcheng()){
            if(room->askForSkillInvoke(luda, objectName(), data)){
                room->playSkillEffect(objectName());
                int card_id = damage.to->getRandomHandCardId();
                const Card *card = Sanguosha->getCard(card_id);
                room->showCard(damage.to, card_id);
                room->getThread()->delay();
                if(!card->inherits("BasicCard")){
                    room->throwCard(card_id);
                    LogMessage log;
                    log.type = "$ForceDiscardCard";
                    log.from = luda;
                    log.to << damage.to;
                    log.card_str = card->getEffectIdString();
                    room->sendLog(log);

                    damage.damage ++;
                }
                data = QVariant::fromValue(damage);
            }
        }
        return false;
    }
};

class Fuhu: public TriggerSkill{
public:
    Fuhu():TriggerSkill("fuhu"){
        events << DamageComplete;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        DamageStruct damage = data.value<DamageStruct>();
        if(!damage.from || !damage.card || !damage.card->inherits("Slash"))
            return false;
        QList<ServerPlayer *> wusOng = room->findPlayersBySkillName(objectName());
        if(wusOng.isEmpty())
            return false;

        foreach(ServerPlayer *wusong, wusOng){
            if(wusong->canSlash(damage.from, false)
                    && !wusong->isKongcheng() && damage.from != wusong){
                const Card *card = room->askForCard(wusong, ".basic", "@fuhu:" + damage.from->objectName(), data);
                if(!card)
                    continue;
                Slash *slash = new Slash(Card::NoSuit, 0);
                slash->setSkillName(objectName());
                CardUseStruct use;
                use.card = slash;
                use.from = wusong;
                use.to << damage.from;

                if(card->inherits("Analeptic")){
                    LogMessage log;
                    log.type = "$Fuhu";
                    log.from = wusong;
                    log.card_str = card->getEffectIdString();
                    room->sendLog(log);

                    room->setPlayerFlag(wusong, "drank");
                }
                room->useCard(use);
            }
        }
        return false;
    }
};

class Wubang: public TriggerSkill{
public:
    Wubang():TriggerSkill("wubang"){
        events << CardLost;
        frequency = Frequent;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual int getPriority() const{
        return 2;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        ServerPlayer *jiuwenlong = room->findPlayerBySkillName(objectName());
        if(!jiuwenlong || player == jiuwenlong)
            return false;
        CardMoveStar move = data.value<CardMoveStar>();
        if(move->to_place == Player::DiscardedPile){
            const Card *weapon = Sanguosha->getCard(move->card_id);
            if(weapon->inherits("Weapon") &&
               jiuwenlong->askForSkillInvoke(objectName())){
                room->playSkillEffect(objectName());
                jiuwenlong->obtainCard(weapon);
            }
        }
        return false;
    }
};

class EquipPattern: public CardPattern{
public:
    virtual bool match(const Player *player, const Card *card) const{
        return card->getTypeId() == Card::Equip;
    }
};

class Xiagu: public TriggerSkill{
public:
    Xiagu():TriggerSkill("xiagu"){
        events << Predamage;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        QList<ServerPlayer *> jiuwennong = room->findPlayersBySkillName(objectName());
        if(jiuwennong.isEmpty())
            return false;

        DamageStruct damage = data.value<DamageStruct>();
        foreach(ServerPlayer *jiuwenlong, jiuwennong){
            if(!jiuwenlong->isNude() && damage.nature == DamageStruct::Normal &&
               damage.to->isAlive() && damage.damage > 0){
                bool caninvoke = false;
                foreach(const Card *cd, jiuwenlong->getCards("he")){
                    if(cd->getTypeId() == Card::Equip){
                        caninvoke = true;
                        break;
                    }
                }
                if(caninvoke){
                    const Card *card = room->askForCard(jiuwenlong, ".equip", "@xiagu", data);
                    if(card){
                        LogMessage log;
                        log.type = "$Xiagu";
                        log.from = jiuwenlong;
                        log.to << damage.to;
                        log.card_str = card->getEffectIdString();
                        room->sendLog(log);
                        room->playSkillEffect(objectName());

                        damage.damage --;
                    }
                    data = QVariant::fromValue(damage);
                }
            }
        }
        return false;
    }
};

DaleiCard::DaleiCard(){
    once = true;
    will_throw = false;
}

bool DaleiCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && to_select->getGeneral()->isMale() &&
            !to_select->isKongcheng() && to_select != Self;
}

void DaleiCard::use(Room *room, ServerPlayer *xiaoyi, const QList<ServerPlayer *> &targets) const{
    bool success = xiaoyi->pindian(targets.first(), "dalei", this);
    if(success){
        room->setPlayerFlag(xiaoyi, "dalei_success");
        room->setPlayerFlag(targets.first(), "dalei_target");
    }else{
        DamageStruct damage;
        damage.from = targets.first();
        damage.to = xiaoyi;
        room->damage(damage);
    }
}

class DaleiViewAsSkill: public OneCardViewAsSkill{
public:
    DaleiViewAsSkill():OneCardViewAsSkill("dalei"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->hasUsed("DaleiCard") && !player->isKongcheng();
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return !to_select->isEquipped();
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        Card *card = new DaleiCard;
        card->addSubcard(card_item->getFilteredCard());
        return card;
    }
};

class Dalei: public TriggerSkill{
public:
    Dalei():TriggerSkill("dalei"){
        view_as_skill = new DaleiViewAsSkill;
        events << Damage << PhaseChange;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        if(!player->hasFlag("dalei_success"))
            return false;
        if(event == PhaseChange){
            if(player->getPhase() == Player::NotActive){
                room->setPlayerFlag(player, "-dalei_success");
                foreach(ServerPlayer *tmp, room->getAllPlayers()){
                    if(tmp->hasFlag("dalei_target"))
                        room->setPlayerFlag(tmp, "-dalei_target");
                }
            }
            return false;
        }
        DamageStruct damage = data.value<DamageStruct>();
        if(damage.to->hasFlag("dalei_target")){
            RecoverStruct rev;
            rev.who = player;
            for(int p = 0; p < damage.damage; p++){
                if(player->askForSkillInvoke(objectName()))
                    room->recover(room->askForPlayerChosen(player, room->getOtherPlayers(damage.to), objectName()), rev);
            }
        }
        return false;
    }
};

class Fuqin: public MasochismSkill{
public:
    Fuqin():MasochismSkill("fuqin"){
    }

    virtual QString getDefaultChoice(ServerPlayer *player) const{
        if(player->getLostHp() > 2)
            return "qing";
        else
            return "yan";
    }

    virtual void onDamaged(ServerPlayer *yan, const DamageStruct &damage) const{
        Room *room = yan->getRoom();
        int lstn = yan->getLostHp();
        if(damage.from){
            PlayerStar from = damage.from;
            yan->tag["FuqinSource"] = QVariant::fromValue(from);
        }
        QString choice = damage.from ?
                         room->askForChoice(yan, objectName(), "yan+qing+nil"):
                         room->askForChoice(yan, objectName(), "qing+nil");
        if(choice == "nil")
            return;
        LogMessage log;
        log.from = yan;
        log.arg = objectName();
        if(choice == "yan"){
            room->playSkillEffect(objectName(), 1);
            if(!damage.from || damage.from->isNude())
                return;
            int i = 0;
            for(; i < lstn; i++){
                int card_id = room->askForCardChosen(yan, damage.from, "he", objectName());
                room->throwCard(card_id);
                if(damage.from->isNude())
                    break;
            }
            log.to << damage.from;
            log.arg2 = QString::number(i);
            log.type = "#FuqinYan";
        }
        else{
            room->playSkillEffect(objectName(), 2);
            ServerPlayer *target = room->askForPlayerChosen(yan, room->getAllPlayers(), objectName());
            target->drawCards(lstn);
            log.to << target;
            log.arg2 = QString::number(lstn);
            log.type = "#FuqinQin";
        }
        room->sendLog(log);
        yan->tag.remove("FuqinSource");
    }
};

class Pozhen: public TriggerSkill{
public:
    Pozhen():TriggerSkill("pozhen"){
        events << CardUsed;
        frequency = Compulsory;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        CardUseStruct use = data.value<CardUseStruct>();
        if(use.card->isNDTrick()){
            room->playSkillEffect(objectName());
            LogMessage log;
            log.type = "#Pozhen";
            log.from = use.from;
            log.arg = objectName();
            log.arg2 = use.card->objectName();
            room->sendLog(log);
        }
        return false;
    }
};

BuzhenCard::BuzhenCard(){
}

bool BuzhenCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(targets.length() >= 2)
        return false;
    return true;
}

bool BuzhenCard::targetsFeasible(const QList<const Player *> &targets, const Player *Self) const{
    return targets.length() == 2;
}

void BuzhenCard::use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const{
    ServerPlayer *first = targets.first();
    room->swapSeat(first, targets.last());
}

class BuzhenViewAsSkill: public ZeroCardViewAsSkill{
public:
    BuzhenViewAsSkill():ZeroCardViewAsSkill("buzhen"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return pattern == "@@buzhen";
    }

    virtual const Card *viewAs() const{
        return new BuzhenCard;
    }
};

class Buzhen:public PhaseChangeSkill{
public:
    Buzhen():PhaseChangeSkill("buzhen"){
        view_as_skill = new BuzhenViewAsSkill;
        frequency = Limited;
    }

    virtual bool onPhaseChange(ServerPlayer *zhuwu) const{
        if(zhuwu->getMark("@buvr") > 0 && zhuwu->getPhase() == Player::Play){
            Room *room = zhuwu->getRoom();
            if(room->askForUseCard(zhuwu, "@@buzhen", "@buzhen")){
                room->broadcastInvoke("animate", "lightbox:$buzhen:5000");
                zhuwu->loseMark("@buvr");
                room->getThread()->delay(4500);
                return true;
            }
        }
        return false;
    }
};

class Fangzhen: public ProhibitSkill{
public:
    Fangzhen():ProhibitSkill("fangzhen"){
    }

    virtual bool isProhibited(const Player *from, const Player *to, const Card *card) const{
        if(card->inherits("Slash") || card->inherits("Duel"))
            return from->getHp() > to->getHp();
        else
            return false;
    }
};

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
            if(!player->isProhibited(p, trick) && !p->containsTrick(trick->objectName()))
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

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
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

class Losthp: public TriggerSkill{
public:
    Losthp():TriggerSkill("#losthp"){
        events << GameStart;
    }

    virtual int getPriority() const{
        return -1;
    }

    virtual bool trigger(TriggerEvent, ServerPlayer *player, QVariant &data) const{
        player->getRoom()->setPlayerProperty(player, "hp", player->getHp() - 1);
        return false;
    }
};

class Zhanchi:public PhaseChangeSkill{
public:
    Zhanchi():PhaseChangeSkill("zhanchi"){
        frequency = Limited;
    }

    virtual bool onPhaseChange(ServerPlayer *opt) const{
        if(opt->getMark("@vfui") > 0 && opt->getPhase() == Player::Judge){
            Room *room = opt->getRoom();
            if(opt->askForSkillInvoke(objectName())){
                room->playSkillEffect(objectName(), 1);
                room->broadcastInvoke("animate", "lightbox:$zhanchi1");
                while(!opt->getJudgingArea().isEmpty())
                    room->throwCard(opt->getJudgingArea().first()->getId());
                room->acquireSkill(opt, "tengfei");
                opt->loseMark("@vfui");
            }
        }
        return false;
    }
};

class Tengfei:public PhaseChangeSkill{
public:
    Tengfei():PhaseChangeSkill("tengfei"){
        frequency = Compulsory;
    }

    virtual bool onPhaseChange(ServerPlayer *opt) const{
        if(opt->getPhase() == Player::NotActive){
            Room *room = opt->getRoom();
            if(opt->getMaxHP() > 3)
                room->playSkillEffect("zhanchi", 2);
            else if(opt->getMaxHP() > 1)
                room->playSkillEffect("zhanchi", 3);
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

class Longluo:public TriggerSkill{
public:
    Longluo():TriggerSkill("longluo"){
        events << CardEffected;
        frequency = Frequent;
    }

    virtual int getPriority() const{
        return 3;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *shien, QVariant &data) const{
        Room *room = shien->getRoom();
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
        view_as_skill = new XiaozaiViewAsSkill;
        events << Predamaged;
    }

    virtual bool trigger(TriggerEvent, ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        Room *room = player->getRoom();
        if(damage.from)
            room->setPlayerFlag(damage.from, "Xiaozai");
        if(player->getHandcardNum() > 1 && room->askForUseCard(player, "@@xiaozai", "@xiaozai")){
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

ButianCard::ButianCard(){
    target_fixed = true;
    mute = true;
}

void ButianCard::use(Room *, ServerPlayer *, const QList<ServerPlayer *> &) const{
}

class ButianViewAsSkill:public OneCardViewAsSkill{
public:
    ButianViewAsSkill():OneCardViewAsSkill("butian"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return true;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return pattern == "@butian";
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        Card *card = new ButianCard;
        card->addSubcard(card_item->getFilteredCard());

        return card;
    }
};

class Butian: public TriggerSkill{
public:
    Butian():TriggerSkill("butian"){
        view_as_skill = new ButianViewAsSkill;
        events << AskForRetrial;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return TriggerSkill::triggerable(target) && !target->isNude();
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        JudgeStar judge = data.value<JudgeStar>();

        QStringList prompt_list;
        prompt_list << "@butian-card" << judge->who->objectName()
                << "" << judge->reason << judge->card->getEffectIdString();
        QString prompt = prompt_list.join(":");

        player->tag["Judge"] = data;
        const Card *card = room->askForCard(player, "@butian", prompt, data);

        if(card){
            int index = qrand() % 2 + 1;
            if(player->getMark("wudao") == 0)
                room->playSkillEffect(objectName(), index);
            else
                room->playSkillEffect(objectName(), index + 2);
            room->throwCard(judge->card);

            QList<int> card_ids = room->getNCards(3);
            room->fillAG(card_ids, player);
            int card_id = room->askForAG(player, card_ids, false, objectName());
            if(card_id == -1)
                return false;
            int locat = card_ids.indexOf(card_id);
            card_ids.replace(locat, judge->card->getId());
            player->invoke("clearAG");

            card_ids.swap(0, 2);
            foreach(int tmp, card_ids){
                room->throwCard(tmp);
                room->moveCardTo(Sanguosha->getCard(tmp), NULL, Player::DrawPile);
            }
            room->getThread()->delay();

            judge->card = Sanguosha->getCard(card_id);
            room->moveCardTo(judge->card, NULL, Player::Special);

            LogMessage log;
            log.type = "$ChangedJudge";
            log.from = player;
            log.to << judge->who;
            log.card_str = QString::number(card_id);
            room->sendLog(log);

            room->sendJudgeResult(judge);
        }
        return false;
    }
};

class Huaxian: public TriggerSkill{
public:
    Huaxian():TriggerSkill("huaxian"){
        events << Dying;
        frequency = Frequent;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *ren, QVariant &data) const{
        DyingStruct dying = data.value<DyingStruct>();
        if(dying.who == ren && ren->askForSkillInvoke(objectName())){
            Room *room = ren->getRoom();
            room->playSkillEffect(objectName());

            JudgeStruct judge;
            judge.pattern = QRegExp("(.*):(heart):(.*)");
            judge.good = true;
            judge.reason = objectName();
            judge.who = ren;

            room->judge(judge);
            if(judge.isGood()){
                RecoverStruct rev;
                rev.card = judge.card;
                rev.who = ren;
                room->recover(ren, rev);
            }
        }
        return false;
    }
};

class Qibing:public DrawCardsSkill{
public:
    Qibing():DrawCardsSkill("qibing"){
        frequency = Compulsory;
    }

    virtual int getDrawNum(ServerPlayer *wangq, int n) const{
        Room *room = wangq->getRoom();
        room->playSkillEffect(objectName());
        LogMessage log;
        log.type = "#TriggerSkill";
        log.from = wangq;
        log.arg = objectName();
        room->sendLog(log);

        return qMin(wangq->getHp(), 4);
    }
};

class Jiachu:public MasochismSkill{
public:
    Jiachu():MasochismSkill("jiachu$"){
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return !target->hasLordSkill(objectName()) && target->getKingdom() == "min";
    }

    virtual void onDamaged(ServerPlayer *player, const DamageStruct &damage) const{
        Room *room = player->getRoom();
        ServerPlayer *wangqing = room->getLord();
        if(!wangqing || !wangqing->hasLordSkill(objectName()))
            return;
        int x = damage.damage, i;
        for(i=0; i<x; i++){
            if(wangqing->isWounded() && player->getKingdom() == "min"
               && room->askForCard(player, "..H", "@jiachu:" + wangqing->objectName())){
                RecoverStruct rev;
                rev.who = player;
                room->playSkillEffect(objectName());

                LogMessage log;
                log.type = "#InvokeSkill";
                log.from = player;
                log.arg = "jiachu";
                room->sendLog(log);
                room->recover(wangqing, rev);
            }
        }
    }
};

QJWMPackage::QJWMPackage()
    :Package("QJWM"){

    General *huarong = new General(this, "huarong", "guan", 4);
    huarong->addSkill(new Jingzhun);
    huarong->addSkill(new Kaixian);
    patterns.insert(".kaixian!", new KaixianPattern);

    General *liying = new General(this, "liying", "guan");
    liying->addSkill(new Kong1iang);

    General *luzhishen = new General(this, "luzhishen", "kou");
    luzhishen->addSkill(new Liba);
    luzhishen->addSkill(new Skill("zuohua", Skill::Compulsory));

    General *wusong = new General(this, "wusong", "kou");
    wusong->addSkill(new Fuhu);

    General *shijin = new General(this, "shijin", "kou");
    shijin->addSkill(new Wubang);
    shijin->addSkill(new Xiagu);
    patterns[".equip"] = new EquipPattern;

    General *yanqing = new General(this, "yanqing", "min", 3);
    yanqing->addSkill(new Dalei);
    yanqing->addSkill(new Fuqin);

    General *zhuwu = new General(this, "zhuwu", "kou", 3);
    zhuwu->addSkill(new Pozhen);
    zhuwu->addSkill(new Buzhen);
    zhuwu->addSkill(new MarkAssignSkill("@buvr", 1));
    related_skills.insertMulti("buzhen", "#@buvr-1");
    zhuwu->addSkill(new Fangzhen);

    General *hantao = new General(this, "hantao", "guan");
    hantao->addSkill(new Taolue);
    hantao->addSkill(new Changsheng);

    General *oupeng = new General(this, "oupeng", "jiang", 5);
    oupeng->addSkill(new Losthp);
    oupeng->addSkill(new Zhanchi);
    oupeng->addSkill(new MarkAssignSkill("@vfui", 1));
    related_skills.insertMulti("zhanchi", "#@vfui-1");
    skills << new Tengfei;

    General *shien = new General(this, "shien", "min", 3);
    shien->addSkill(new Longluo);
    shien->addSkill(new Xiaozai);

    General *wangqing = new General(this, "wangqing$", "min");
    wangqing->addSkill(new Qibing);
    wangqing->addSkill(new Jiachu);

    General *luozhenren = new General(this, "luozhenren", "kou", 3);
    luozhenren->addSkill(new Butian);
    luozhenren->addSkill(new Huaxian);

    addMetaObject<DaleiCard>();
    addMetaObject<BuzhenCard>();
    addMetaObject<TaolueCard>();
    addMetaObject<XiaozaiCard>();
    addMetaObject<ButianCard>();
}

ADD_PACKAGE(QJWM)
