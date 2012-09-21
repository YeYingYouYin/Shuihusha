#include "standard.h"
#include "skill.h"
#include "sp.h"
#include "client.h"
#include "carditem.h"
#include "engine.h"
#include "ai.h"

BaoquanCard::BaoquanCard(){
    mute = true;
}

void BaoquanCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();
    room->playSkillEffect("baoquan", qrand() % 2 + 5);
    int fist = getSubcards().count();

    DamageStruct damage;
    damage.damage = fist;
    damage.from = effect.from;
    damage.to = effect.to;
    room->damage(damage);
}

class BaoquanViewAsSkill: public ViewAsSkill{
public:
    BaoquanViewAsSkill():ViewAsSkill("baoquan"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool viewFilter(const QList<CardItem *> &, const CardItem *to_select) const{
        return to_select->getCard()->inherits("EquipCard");
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.isEmpty())
            return NULL;
        BaoquanCard *card = new BaoquanCard;
        card->addSubcards(cards);
        return card;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return pattern == "@@baoquan";
    }
};

class Baoquan: public TriggerSkill{
public:
    Baoquan():TriggerSkill("baoquan"){
        events << PhaseChange << Damage;
        view_as_skill = new BaoquanViewAsSkill;
    }

    virtual QString getDefaultChoice(ServerPlayer *player) const{
        if(player->getLostHp() > 1)
            return "recover1hp";
        else
            return "draw2card";
    }

    virtual bool trigger(TriggerEvent e, Room* room, ServerPlayer *lusashi, QVariant &data) const{
        if(e == PhaseChange){
            if(lusashi->getPhase() == Player::RoundStart)
                room->setPlayerMark(lusashi, "@fist", 0);
            else if(lusashi->getPhase() == Player::Finish){
                int fist = lusashi->getMark("@fist");
                if(fist < 1)
                    return false;
                if(fist == 1 || fist == 2){
                    if(!lusashi->askForSkillInvoke(objectName())){
                        room->setPlayerMark(lusashi, "@fist", 0);
                        return false;
                    }
                }
                switch(fist){
                    case 1:{
                        room->playSkillEffect(objectName(), qrand() % 2 + 1);
                        lusashi->drawCards(1);
                        break;
                    }
                    case 2:{
                        room->playSkillEffect(objectName(), qrand() % 2 + 3);
                        PlayerStar target = room->askForPlayerChosen(lusashi, room->getAllPlayers(), objectName());
                        QString choice = !target->isWounded() ? "draw2card" :
                                         room->askForChoice(lusashi, objectName(), "draw2card+recover1hp");
                        if(choice == "draw2card")
                            target->drawCards(2);
                        else{
                            RecoverStruct rev;
                            rev.who = lusashi;
                            room->recover(target, rev);
                        }
                        break;
                    }
                    default:
                        room->askForUseCard(lusashi, "@@baoquan", "@baoquan", true);
                }
                room->setPlayerMark(lusashi, "@fist", 0);
            }
        }
        else{
            if(lusashi->getPhase() == Player::NotActive)
                return false;
            DamageStruct damage = data.value<DamageStruct>();
            lusashi->gainMark("@fist", damage.damage);
        }
        return false;
    }
};

class StrikeViewAsSkill: public ViewAsSkill{
public:
    StrikeViewAsSkill():ViewAsSkill("strike"){
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->getPhase() == Player::Play && Slash::IsAvailable(player);
    }

    virtual bool viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
        return selected.length() < 2 && !to_select->isEquipped();
    }

    virtual const Card *viewAs(const QList<CardItem *> &cards) const{
        if(cards.length() != 2)
            return NULL;

        const Card *first = cards.at(0)->getFilteredCard();
        const Card *second = cards.at(1)->getFilteredCard();

        Card::Suit suit = Card::NoSuit;
        if(first->isBlack() && second->isBlack())
            suit = Card::Spade;
        else if(first->isRed() && second->isRed())
            suit = Card::Heart;

        Slash *slash = new Slash(suit, 0);
        slash->setSkillName(objectName());
        slash->addSubcard(first);
        slash->addSubcard(second);

        return slash;
    }
};

class Strike: public TriggerSkill{
public:
    Strike():TriggerSkill("strike"){
        events << CardUsed;
        view_as_skill = new StrikeViewAsSkill;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        CardUseStruct use = data.value<CardUseStruct>();
        if(use.card->inherits("Slash") && use.card->isVirtualCard() && use.card->getSkillName() == objectName()){
            LogMessage log;
            log.type = "#Strike";
            log.from = use.from;
            log.arg = objectName();
            room->sendLog(log);
        }
        return false;
    }
};

class Lift: public TriggerSkill{
public:
    Lift(): TriggerSkill("lift"){
        events << SlashMissed;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();

        if(player->askForSkillInvoke(objectName(), data)){
            player->turnOver();
            LogMessage log;
            log.type = "#Lift";
            log.from = player;
            log.to << effect.to;
            log.arg = objectName();
            room->sendLog(log);

            room->slashResult(effect, NULL);
        }

        return false;
    }
};

class Exterminate: public TriggerSkill{
public:
    Exterminate():TriggerSkill("exterminate"){
        events << DamageComplete;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        if(!damage.from || !damage.from->hasSkill(objectName()) || damage.from == damage.to)
            return false;
        ServerPlayer *hanae = damage.from;
        if(hanae->getMark("@kacha") > 0 && hanae->askForSkillInvoke(objectName(), data)){
            room->playSkillEffect(objectName());

            QList<ServerPlayer *> targets;
            foreach(ServerPlayer *tmp, room->getAllPlayers()){
                if(damage.to->distanceTo(tmp) == 1)
                    targets << tmp;
            }
            LogMessage log;
            log.type = "#Exterminate";
            log.from = hanae;
            log.to = targets;
            log.arg = objectName();
            room->sendLog(log);

            room->loseMaxHp(hanae);
            hanae->loseMark("@kacha");
            DamageStruct dama = damage;
            foreach(ServerPlayer *tmp, targets){
                dama.to = tmp;
                room->damage(dama);
            }
        }
        return false;
    }
};
/*
class Shemi: public TriggerSkill{
public:
    Shemi():TriggerSkill("shemi"){
        events << PhaseChange << TurnedOver;
    }

    virtual bool trigger(TriggerEvent e, Room* room, ServerPlayer *emperor, QVariant &data) const{
        if(e == PhaseChange){
            if(emperor->getPhase() == Player::Discard &&
               emperor->askForSkillInvoke(objectName(), data)){
                emperor->turnOver();
                return true;
            }
        }
        else{
            if(!emperor->hasFlag("NongQ")){
                int index = emperor->faceUp() ? 2: 1;
                room->playSkillEffect(objectName(), index);
            }
            int x = emperor->getLostHp();
            x = qMax(qMin(x,2),1);
            emperor->drawCards(x);
        }
        return false;
    }
};

class Lizheng: public ClientSkill{
public:
    Lizheng():ClientSkill("lizheng"){
    }

    virtual int getCorrect(const Player *from, const Player *to) const{
        if(to->hasSkill(objectName()) && !to->faceUp())
            return +1;
        else
            return 0;
    }
};

class Nongquan:public PhaseChangeSkill{
public:
    Nongquan():PhaseChangeSkill("nongquan$"){
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target->getKingdom() == "guan" && !target->hasLordSkill(objectName());
    }

    virtual bool onPhaseChange(ServerPlayer *otherguan) const{
        Room *room = otherguan->getRoom();
        if(otherguan->getPhase() != Player::Draw)
            return false;
        ServerPlayer *head = room->getLord();
        if(head->hasLordSkill(objectName()) && otherguan->getKingdom() == "guan"
           && otherguan->askForSkillInvoke(objectName())){
            room->playSkillEffect(objectName());
            room->setPlayerFlag(head, "NongQ");
            head->turnOver();
            room->setPlayerFlag(head, "-NongQ");
            return true;
        }
        return false;
    }
};

JiebaoCard::JiebaoCard(){
}

bool JiebaoCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if(targets.length() >= 2)
        return false;

    if(to_select == Self)
        return false;

    return !to_select->isNude();
}

void JiebaoCard::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();
    int card_id = room->askForCardChosen(effect.from, effect.to, "he", "jiebao");
    room->obtainCard(effect.from, card_id, room->getCardPlace(card_id) != Player::Hand);

    room->setEmotion(effect.to, "bad");
    room->setEmotion(effect.from, "good");
}

class JiebaoViewAsSkill: public ZeroCardViewAsSkill{
public:
    JiebaoViewAsSkill():ZeroCardViewAsSkill("jiebao"){
    }

    virtual const Card *viewAs() const{
        return new JiebaoCard;
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return false;
    }

    virtual bool isEnabledAtResponse(const Player *player, const QString &pattern) const{
        return pattern == "@@jiebao";
    }
};

class Jiebao: public TriggerSkill{
public:
    Jiebao():TriggerSkill("jiebao"){
        events << Death;
        view_as_skill = new JiebaoViewAsSkill;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return !target->hasSkill(objectName());
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        ServerPlayer *tianwang = room->findPlayerBySkillName(objectName());
        if(!tianwang)
            return false;
        bool can_invoke = false;
        QList<ServerPlayer *> other_players = room->getOtherPlayers(tianwang);
        foreach(ServerPlayer *player, other_players){
            if(!player->isNude()){
                can_invoke = true;
                break;
            }
        }
        if(can_invoke)
            room->askForUseCard(tianwang, "@@jiebao", "@jiebao:" + player->objectName());
        return false;
    }
};

class Dushi: public TriggerSkill{
public:
    Dushi():TriggerSkill("dushi"){
        events << Death;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target->hasSkill("dushi");
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        DamageStar damage = data.value<DamageStar>();
        if(damage && damage->to == player && damage->from && damage->from != player){
            if(!player->askForSkillInvoke(objectName(), QVariant::fromValue(damage)))
                return false;
            QList<ServerPlayer *> targets = room->getOtherPlayers(damage->from);
            targets.removeOne(player);
            if(targets.isEmpty())
                return false;
            ServerPlayer *target = room->askForPlayerChosen(player, targets, objectName());
            room->showAllCards(target);
            int kuro = 0;
            foreach(const Card *tcard, target->getHandcards()){
                if(tcard->isBlack())
                    kuro ++;
            }
            if(kuro > 0){
                room->playSkillEffect(objectName());
                DamageStruct uct;
                uct.from = target;
                uct.to = damage->from;
                uct.damage = kuro;
                room->damage(uct);
            }
        }
        return false;
    }
};

class Shaxue: public PhaseChangeSkill{
public:
    Shaxue():PhaseChangeSkill("shaxue$"){
        frequency = Compulsory;
    }

    virtual bool onPhaseChange(ServerPlayer *player) const{
        if(player->getPhase() == Player::Discard &&
           player->getHandcardNum() > player->getHp() && player->getHandcardNum() <= player->getMaxCards())
            player->getRoom()->playSkillEffect(objectName());
        return false;
    }
};

class ShaxueMaxCard: public ClientSkill{
public:
    ShaxueMaxCard():ClientSkill("#shaxue-maxcard"){
    }

    virtual int getExtra(const Player *target) const{
        int shaxue = 0;
        if(target->hasLordSkill("shaxue")){
            QList<const Player *> players = target->getSiblings();
            foreach(const Player *player, players){
                if(player->isDead() && player->getKingdom() == "kou")
                    shaxue += 2;
            }
        }
        return shaxue;
    }
};

class Shuntian: public TriggerSkill{
public:
    Shuntian():TriggerSkill("shuntian"){
        events << GameStart;
        frequency = Compulsory;
    }

    virtual bool trigger(TriggerEvent, Room* rom, ServerPlayer *player, QVariant &data) const{
        QString kim;
        if(!player->isLord())
            kim = rom->getLord()->getKingdom();
        rom->setPlayerProperty(player, "kingdom", kim);
        return false;
    }
};

YuzhongCard::YuzhongCard(){
    mute = true;
}

bool YuzhongCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    int num = Self->getKingdoms();
    return targets.length() < num;
}

bool YuzhongCard::targetsFeasible(const QList<const Player *> &targets, const Player *Self) const{
    int num = Self->getKingdoms();
    return targets.length() <= num;
}

void YuzhongCard::onEffect(const CardEffectStruct &effect) const{
    effect.to->drawCards(1);
}

class YuzhongViewAsSkill: public ZeroCardViewAsSkill{
public:
    YuzhongViewAsSkill():ZeroCardViewAsSkill("yuzhong"){

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

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        int num = room->getKingdoms();
        DamageStar damage = data.value<DamageStar>();
        if(damage && damage->from != damage->to && damage->from->hasSkill(objectName())){
            ServerPlayer *source = damage->from;
            QString choice = room->askForChoice(source, objectName(), "hp+card+cancel");
            if(choice == "cancel")
                return false;
            LogMessage log;
            log.type = "#InvokeSkill";
            log.from = source;
            log.arg = objectName();
            room->sendLog(log);
            if(choice == "hp"){
                room->playSkillEffect(objectName(), 1);
                RecoverStruct rev;
                rev.who = source;
                rev.recover = num;
                room->recover(room->getLord(), rev);
            }
            else if(choice == "card"){
                room->playSkillEffect(objectName(), 2);
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

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *player, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        if(!damage.to->isLord())
            return false;
        QString choice = room->askForChoice(player, "yuzhong", "all+me+cancel");
        if(choice == "cancel")
            return false;
        LogMessage log;
        log.type = "#InvokeSkill";
        log.from = player;
        log.arg = "yuzhong";
        room->sendLog(log);
        if(choice == "all"){
            if(!room->askForUseCard(player, "@@yuzhong", "@yuzhong"))
                choice = "me";
            else
                room->playSkillEffect(objectName(), 3);
        }
        if(choice == "me"){
            room->playSkillEffect(objectName(), 4);
            LogMessage log;
            log.type = "#InvokeSkill";
            log.from = player;
            log.arg = "yuzhong";
            room->sendLog(log);
            player->drawCards(2);
        }

        return false;
    }
};

class Chengfu: public TriggerSkill{
public:
    Chengfu():TriggerSkill("chengfu"){
        events << CardLost << CardAsk << CardUseAsk << PhaseChange;
        frequency = Compulsory;
    }

    virtual bool trigger(TriggerEvent t, Room* room, ServerPlayer *conan, QVariant &data) const{
        if(t == PhaseChange){
            if(conan->getPhase() == Player::NotActive)
                room->setPlayerCardLock(conan, "Slash");
            else if(conan->getPhase() == Player::RoundStart)
                room->setPlayerCardLock(conan, "-Slash");
            return false;
        }
        else if((t == CardAsk || t == CardUseAsk) &&
                conan->getPhase() == Player::NotActive){
            QString asked = data.toString();
            if(asked == "slash"){
                room->playSkillEffect(objectName(), qrand() % 2 + 3);
                LogMessage log;
                log.type = "#ChengfuEffect";
                log.from = conan;
                log.arg = asked;
                log.arg2 = objectName();
                room->sendLog(log);
                return true;
            }
        }
        else if(t == CardLost && conan->getPhase() == Player::NotActive){
            CardMoveStar move = data.value<CardMoveStar>();
            if(conan->isDead())
                return false;
            if(move->from_place == Player::Hand || move->from_place == Player::Equip){
                room->playSkillEffect(objectName(), qrand() % 2 + 1);
                LogMessage log;
                log.type = "#TriggerSkill";
                log.from = conan;
                log.arg = objectName();
                room->sendLog(log);

                conan->drawCards(1);
            }
        }
        return false;
    }
};

class Xiaduo: public TriggerSkill{
public:
    Xiaduo():TriggerSkill("xiaduo"){
        events << DamageComplete;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return true;
    }

    virtual bool trigger(TriggerEvent, Room* room, ServerPlayer *, QVariant &data) const{
        DamageStruct damage = data.value<DamageStruct>();
        if(!damage.from || !damage.from->hasSkill(objectName()))
            return false;
        if(damage.from->distanceTo(damage.to) != 1)
            return false;
        ServerPlayer *wanglun = damage.from;
        QList<ServerPlayer *> ones;
        foreach(ServerPlayer *tmp, room->getOtherPlayers(damage.to))
            if(wanglun->distanceTo(tmp) == 1)
                ones << tmp;
        if(!ones.isEmpty() && room->askForCard(wanglun, "EquipCard", "@xiaduo", data, CardDiscarded)){
            room->playSkillEffect(objectName());
            ServerPlayer *target = room->askForPlayerChosen(wanglun, ones, objectName());
            LogMessage log;
            log.type = "#UseSkill";
            log.from = wanglun;
            log.to << target;
            log.arg = objectName();
            room->sendLog(log);

            DamageStruct dama;
            dama.from = wanglun;
            dama.to = target;
            room->damage(dama);
        }
        return false;
    }
};
*/
SPPackage::SPPackage()
    :Package("sp")
{
    General *luda = new General(this, "luda", "guan");
    luda->addSkill(new Baoquan);

    General *tora = new General(this, "tora", "god", 4, false);
    tora->addSkill(new Strike);
    tora->addSkill(new Lift);
    tora->addSkill(new Exterminate);
    tora->addSkill(new MarkAssignSkill("@kacha", 1));
    related_skills.insertMulti("exterminate", "#@kacha-1");

/*
    General *zhaoji = new General(this, "zhaoji$", "guan", 3);
    zhaoji->addSkill(new Shemi);
    zhaoji->addSkill(new Lizheng);
    zhaoji->addSkill(new Nongquan);

    General *chaogai = new General(this, "chaogai", "kou");
    chaogai->addSkill(new Jiebao);
    chaogai->addSkill(new Dushi);
    chaogai->addSkill(new Shaxue);
    chaogai->addSkill(new ShaxueMaxCard);
    related_skills.insertMulti("shaxue", "#shaxue-maxcard");

    General *jiangsong = new General(this, "jiangsong", "guan");
    jiangsong->addSkill(new Yuzhong);
    jiangsong->addSkill(new Yuzhong2);
    related_skills.insertMulti("yuzhong", "#yuzh0ng");
    jiangsong->addSkill(new Shuntian);

    General *wanglun = new General(this, "wanglun", "kou", 3);
    wanglun->addSkill(new Chengfu);
    wanglun->addSkill(new Xiaduo);

    addMetaObject<YuzhongCard>();
    addMetaObject<JiebaoCard>();
*/
    addMetaObject<BaoquanCard>();
}

ADD_PACKAGE(SP);
