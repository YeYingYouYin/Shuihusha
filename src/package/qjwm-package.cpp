#include "qjwm-package.h"
#include "skill.h"
#include "standard.h"
#include "maneuvering.h"
#include "clientplayer.h"
#include "carditem.h"
#include "engine.h"
#include "ai.h"

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

class Kongliang: public TriggerSkill{
public:
    Kongliang():TriggerSkill("kongliang"){
        events << DrawNCards << PhaseChange;
    }

    static bool CompareBySuit(int card1, int card2){
        const Card *c1 = Sanguosha->getCard(card1);
        const Card *c2 = Sanguosha->getCard(card2);

        int a = static_cast<int>(c1->getSuit());
        int b = static_cast<int>(c2->getSuit());

        return a < b;
    }

    virtual bool trigger(TriggerEvent event, ServerPlayer *liying, QVariant &data) const{
        Room *room = liying->getRoom();
        if(event == DrawNCards){
            if(room->askForSkillInvoke(liying, objectName())){
                room->playSkillEffect(objectName());
                data = liying->getMaxHP() + liying->getLostHp();
                liying->setFlags("kongliang");
            }
        }
        else if(event == PhaseChange && liying->hasFlag("kongliang") && liying->getPhase() == Player::Play){
            QList<int> card_ids;
            foreach(const Card *tmp, liying->getHandcards()){
                card_ids << tmp->getId();
            }
            qSort(card_ids.begin(), card_ids.end(), CompareBySuit);
            room->fillAG(card_ids);
            int count = 0;
            while(!card_ids.isEmpty() && count < 2){
                int card_id = room->askForAG(liying, card_ids, false, objectName());
                card_ids.removeOne(card_id);
                room->throwCard(card_id);
                room->takeAG(NULL, card_id);

                // throw the rest cards that matches the same suit
                const Card *card = Sanguosha->getCard(card_id);
                Card::Suit suit = card->getSuit();
                QMutableListIterator<int> itor(card_ids);
                while(itor.hasNext()){
                    const Card *c = Sanguosha->getCard(itor.next());
                    if(c->getSuit() == suit){
                        itor.remove();
                        room->throwCard(card_id);
                        room->takeAG(NULL, c->getId());
                    }
                }
                count ++;
            }
            room->broadcastInvoke("clearAG");
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
                const Card *card = room->askForCardShow(damage.to, luda, objectName());
                if(!card->inherits("BasicCard")){
                    room->throwCard(card->getId());
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
        ServerPlayer *wusong = room->findPlayerBySkillName(objectName());
        if(!wusong)
            return false;
        DamageStruct damage = data.value<DamageStruct>();

        if(damage.card && damage.card->inherits("Slash") && damage.to->isAlive()
            && !wusong->isKongcheng() && damage.from != wusong
                    && room->askForSkillInvoke(wusong, objectName(), data)){
            room->playSkillEffect(objectName());
            const Card *card = room->askForCard(wusong, ".basic", "@fuhu", data);
            if(!card)
                return false;
            Slash *slash = new Slash(card->getSuit(), card->getNumber());
            slash->setSkillName(objectName());
            CardUseStruct use;
            use.card = slash;
            use.from = wusong;
            use.to << damage.to;

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
               jiuwenlong->askForSkillInvoke(objectName()))
                jiuwenlong->obtainCard(weapon);
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
        ServerPlayer *jiuwenlong = room->findPlayerBySkillName(objectName());
        if(!jiuwenlong || jiuwenlong->isKongcheng())
            return false;
        DamageStruct damage = data.value<DamageStruct>();

        if(damage.nature == DamageStruct::Normal && damage.to->isAlive() && damage.damage > 0){
            bool caninvoke = false;
            foreach(const Card *cd, jiuwenlong->getHandcards()){
                if(cd->getTypeId() == Card::Equip){
                    caninvoke = true;
                    break;
                }
            }
            if(caninvoke && room->askForSkillInvoke(jiuwenlong, objectName(), data)){
                room->playSkillEffect(objectName());
                const Card *card = room->askForCard(jiuwenlong, ".equip", "@xiagu", data);
                if(card){
                    LogMessage log;
                    log.type = "$Xiagu";
                    log.from = jiuwenlong;
                    log.to << damage.to;
                    log.card_str = card->getEffectIdString();
                    room->sendLog(log);

                    damage.damage --;
                }
                data = QVariant::fromValue(damage);
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
        if(damage.to->hasFlag("dalei_target") && player->askForSkillInvoke(objectName())){
            ServerPlayer *target = room->askForPlayerChosen(player, room->getOtherPlayers(damage.to), objectName());
            RecoverStruct rev;
            rev.who = player;
            room->recover(target, rev);
        }
        return false;
    }
};

class Fuqin: public MasochismSkill{
public:
    Fuqin():MasochismSkill("fuqin"){
    }

    virtual void onDamaged(ServerPlayer *yan, const DamageStruct &damage) const{
        Room *room = yan->getRoom();
        int lstn = yan->getLostHp();
        QString choice = damage.from ?
                         room->askForChoice(yan, objectName(), "yan+qing+nil"):
                         room->askForChoice(yan, objectName(), "qing+nil");
        if(choice == "nil")
            return;
        if(choice == "yan"){
            for(int i = 0; i < lstn; i++){
                room->throwCard(room->askForCardChosen(damage.from, yan, "he", objectName()));
                if(damage.from->isNude())
                    break;
            }
        }
        else{
            ServerPlayer *target = room->askForPlayerChosen(yan, room->getAllPlayers(), objectName());
            target->drawCards(lstn);
        }
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
                zhuwu->loseMark("@buvr");
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
    will_throw = false;
}

bool TaolueCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && !to_select->isKongcheng() && to_select != Self;
}

void TaolueCard::use(Room *room, ServerPlayer *player, const QList<ServerPlayer *> &targets) const{
    bool success = player->pindian(targets.first(), "Taolue", this);
    if(!success){
        if(!player->isNude())
            room->askForDiscard(player, "taolue", 1, false, true);
        return;
    }
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
        ServerPlayer *aoko = room->findPlayerBySkillName(objectName());
        PindianStar pindian = data.value<PindianStar>();
        if(pindian->from != aoko && pindian->to != aoko)
            return false;
        Card *pdcd;
        if(pindian->from != aoko && pindian->to_card->getSuit() == Card::Spade){
            pdcd = Sanguosha->cloneCard(pindian->to_card->objectName(), pindian->to_card->getSuit(), 13);
            pdcd->addSubcard(pindian->to_card);
            pdcd->setSkillName(objectName());
            pindian->to_card = pdcd;
        }
        else if(pindian->to != aoko && pindian->from_card->getSuit() == Card::Spade){
            pdcd = Sanguosha->cloneCard(pindian->from_card->objectName(), pindian->from_card->getSuit(), 13);
            pdcd->addSubcard(pindian->from_card);
            pdcd->setSkillName(objectName());
            pindian->from_card = pdcd;
        }

        LogMessage log;
        log.type = "#Changsheng";
        log.from = aoko;
        log.arg = objectName();
        room->sendLog(log);

        data = QVariant::fromValue(pindian);
        return false;
    }
};

class Losthp: public TriggerSkill{
public:
    Losthp():TriggerSkill("#losthp"){
        events << GameStart;
    }

    virtual int getPriority() const{
        return 4;
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
            room->loseMaxHp(opt);

            LogMessage log;
            log.type = "#Tengfei";
            log.from = opt;
            log.arg = objectName();
            log.arg2 = QString::number(1);
            room->sendLog(log);

            room->setCurrent(opt);
            room->getThread()->trigger(TurnStart, opt);
        }
        return false;
    }
};

class Bribe:public TriggerSkill{
public:
    Bribe():TriggerSkill("bribe"){
        events << CardEffected;
        frequency = Frequent;
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *shien, QVariant &data) const{
        Room *room = shien->getRoom();
        CardEffectStruct effect = data.value<CardEffectStruct>();
        if(!effect.card->inherits("Slash"))
            return false;

        if(room->askForSkillInvoke(shien, objectName(), data)){
            QList<int> card_ids = room->getNCards(2);
            room->fillAG(card_ids);
            QMutableListIterator<int> itor(card_ids);
            while(itor.hasNext()){
                const Card *c = Sanguosha->getCard(itor.next());
                if(c->getTypeId() != Card::Basic){
                    itor.remove();
                    room->takeAG(NULL, c->getId());
                }
            }
            while(!card_ids.isEmpty()){
                int card_id = room->askForAG(shien, card_ids, false, objectName());
                card_ids.removeOne(card_id);
                ServerPlayer *target = room->askForPlayerChosen(shien, room->getAllPlayers(), objectName());
                room->takeAG(target, card_id);
            }
            room->broadcastInvoke("clearAG");
        }
        return false;
    }
};

XiaozaiCard::XiaozaiCard(){
}

bool XiaozaiCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(!targets.isEmpty())
        return false;

    return to_select->getMark("Xiaozai") > 0;
}

void XiaozaiCard::onEffect(const CardEffectStruct &effect) const{
    effect.from->tag["Xiaozai"] = QVariant::fromValue(effect.to);
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
        events << Predamaged;
    }

    virtual int getPriority() const{
        return -1;
    }

    virtual bool trigger(TriggerEvent, ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        Room *room = player->getRoom();
        room->setPlayerMark(damage.from, "Xiaozai", 1);
        if(player->getHandcardNum() > 1 && room->askForUseCard(player, "@@xiaozai", "@xiaozai")){
            ServerPlayer *cup = player->tag.value("Xiaozai", NULL).value<ServerPlayer *>();
            room->setPlayerMark(damage.from, "Xiaozai", 0);
            damage.to = cup;
            room->damage(damage);

            return true;
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
            JudgeStruct judge;
            judge.pattern = QRegExp("(.*):(heart):(.*)");
            judge.good = true;
            judge.reason = objectName();
            judge.who = ren;

            Room *room = ren->getRoom();
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

QJWMPackage::QJWMPackage():Package("QJWM"){

    General *huarong = new General(this, "huarong", "wei", 4); //guan == wei
    huarong->addSkill(new Jingzhun);
    huarong->addSkill(new Kaixian);
    patterns.insert(".kaixian!", new KaixianPattern);

    General *liying = new General(this, "liying", "wei");
    liying->addSkill(new Kongliang);

    General *luzhishen = new General(this, "luzhishen", "qun"); //kou == qun
    luzhishen->addSkill(new Liba);
    luzhishen->addSkill(new Skill("zuohua", Skill::Compulsory));

    General *wusong = new General(this, "wusong", "qun");
    wusong->addSkill(new Fuhu);

    General *shijin = new General(this, "shijin", "qun");
    shijin->addSkill(new Wubang);
    shijin->addSkill(new Xiagu);
    patterns[".equip"] = new EquipPattern;

    General *yanqing = new General(this, "yanqing", "wu"); //min == wu
    yanqing->addSkill(new Dalei);
    yanqing->addSkill(new Fuqin);

    General *zhuwu = new General(this, "zhuwu", "qun", 3);
    zhuwu->addSkill(new Buzhen);
    zhuwu->addSkill(new MarkAssignSkill("@buvr", 1));
    related_skills.insertMulti("buzhen", "#@buvr");
    zhuwu->addSkill(new Fangzhen);

    General *hantao = new General(this, "hantao", "wei");
    hantao->addSkill(new Taolue);
    hantao->addSkill(new Changsheng);

    General *oupeng = new General(this, "oupeng", "wu", 5); //jiang == shu
    oupeng->addSkill(new Losthp);
    oupeng->addSkill(new Zhanchi);
    oupeng->addSkill(new MarkAssignSkill("@vfui", 1));
    related_skills.insertMulti("zhanchi", "#@vfui");
    skills << new Tengfei;

    General *shin = new General(this, "shin", "wu");
    shin->addSkill(new Bribe);
    shin->addSkill(new Xiaozai);

    General *luozhenren = new General(this, "luozhenren", "qun");
    luozhenren->addSkill(new Huaxian);

    General *wangqing = new General(this, "wangqing", "wu");
    wangqing->addSkill(new Qibing);
    /*

    addMetaObject<JujianCard>();
    addMetaObject<MingceCard>();
    addMetaObject<GanluCard>();
    */
    addMetaObject<DaleiCard>();
    addMetaObject<BuzhenCard>();
    addMetaObject<TaolueCard>();
    addMetaObject<XiaozaiCard>();
}

ADD_PACKAGE(QJWM)
