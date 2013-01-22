#include "roomthread.h"
#include "room.h"
#include "engine.h"
#include "gamerule.h"
#include "ai.h"
#include "scenario.h"
#include "jsonutils.h"
#include "settings.h"

#include <QTime>
#include <json/json.h>

using namespace QSanProtocol::Utils;

LogMessage::LogMessage()
    :from(NULL)
{
}

QString LogMessage::toString() const{
    QStringList tos;
    foreach(ServerPlayer *player, to)
        if (player != NULL) tos << player->objectName();

    return QString("%1:%2->%3:%4:%5:%6")
            .arg(type)
            .arg(from ? from->objectName() : "")
            .arg(tos.join("+"))
            .arg(card_str).arg(arg).arg(arg2);
}

DamageStruct::DamageStruct()
    :from(NULL), to(NULL), card(NULL), damage(1), nature(Normal), chain(false)
{
}

CardEffectStruct::CardEffectStruct()
    :card(NULL), from(NULL), to(NULL), multiple(false)
{
}

SlashEffectStruct::SlashEffectStruct()
    :slash(NULL), jink(NULL), from(NULL), to(NULL), drank(false), nature(DamageStruct::Normal)
{
}

DyingStruct::DyingStruct()
    :who(NULL), damage(NULL)
{
}

RecoverStruct::RecoverStruct()
    :recover(1), who(NULL), card(NULL)
{

}

PindianStruct::PindianStruct()
    :from(NULL), to(NULL), from_card(NULL), to_card(NULL)
{

}

bool PindianStruct::isSuccess() const{
    return from_card->getNumber() > to_card->getNumber();
}

JudgeStructPattern::JudgeStructPattern(){
}

bool JudgeStructPattern::match(const Player *player, const Card *card) const{
    if(pattern.isEmpty())
        return false;

    if(isRegex){
        QString class_name = card->metaObject()->className();
        Card::Suit suit = card->getSuit();
        /*if(player->hasSkill("hongyan") && suit == Card::Spade)
            suit = Card::Heart;*/

        QString number = card->getNumberString();
        QString card_str = QString("%1:%2:%3").arg(class_name).arg(Card::Suit2String(suit)).arg(number);

        return QRegExp(pattern).exactMatch(card_str);
    }else{
        const CardPattern *card_pattern = Sanguosha->getPattern(pattern);
        return card_pattern && card_pattern->match(player, card);
    }
}

JudgeStructPattern &JudgeStructPattern::operator =(const QRegExp &rx){
    pattern = rx.pattern();
    isRegex = true;

    return *this;
}

JudgeStructPattern &JudgeStructPattern::operator =(const QString &str){
    pattern = str;
    isRegex = false;

    return *this;
}

JudgeStruct::JudgeStruct()
    :who(NULL), card(NULL), good(true), time_consuming(false)
{

}

bool JudgeStruct::isGood(const Card *card) const{
    if(card == NULL)
        card = this->card;

    if(good)
        return pattern.match(who, card);
    else
        return !pattern.match(who, card);
}

bool JudgeStruct::isBad() const{
    return ! isGood();
}

PhaseChangeStruct::PhaseChangeStruct()
    :from(Player::NotActive), to(Player::NotActive)
{}

CardUseStruct::CardUseStruct()
    :card(NULL), from(NULL), mute(false)
{
}

bool CardUseStruct::isValid() const{
    return card != NULL;
}

bool CardUseStruct::tryParse(const Json::Value &usage, Room *room){
    if (usage.size() < 2 || !usage[0].isString() || !usage[1].isArray())
        return false;

    card = Card::Parse(toQString(usage[0]));

    const Json::Value &targets = usage[1];

    for (unsigned int i = 0; i < targets.size(); i++)
    {
        if (!targets[i].isString()) return false;
        this->to << room->findChild<ServerPlayer *>(toQString(targets[i]));
    }
    return true;
}

void CardUseStruct::parse(const QString &str, Room *room){
    QStringList words = str.split("->", QString::KeepEmptyParts);
    
    Q_ASSERT(words.length() == 1 || words.length() == 2);

    QString card_str = words.at(0);
    QString target_str = ".";
    
    //@todo: it's observed that when split on "a->."
    // only returns one QString, which is "a". Suspect
    // it's a bug with QT regular expression. Figure out
    // the cause of the issue.
    if (words.length() == 2 && !words.at(1).isEmpty()) 
        target_str = words.at(1);    

    card = Card::Parse(card_str);

    if(target_str != "."){
        QStringList target_names = target_str.split("+");
        foreach(QString target_name, target_names)
            to << room->findChild<ServerPlayer *>(target_name);
    }
}

QString EventTriplet::toString() const{
    return QString("event = %1, target = %2[%3], data = %4[%5]")
            .arg(event)
            .arg(target->objectName()).arg(target->getGeneralName())
            .arg(data->toString()).arg(data->typeName());
}

//@todo: setParent here is illegitimate in QT and is equivalent to calling
// setParent(NULL). Find another way to do it if we really need a parent.
RoomThread::RoomThread(Room *room)
    :room(room)
{
}

void RoomThread::addPlayerSkills(ServerPlayer *player, bool invoke_game_start){
    QVariant void_data;

    foreach(const TriggerSkill *skill, player->getTriggerSkills()){
        addTriggerSkill(skill);

        if(invoke_game_start && skill->getTriggerEvents().contains(GameStart))
            skill->trigger(GameStart, room, player, void_data);
    }
}

void RoomThread::constructTriggerTable(const GameRule *rule){
    foreach(ServerPlayer *player, room->getPlayers()){
        addPlayerSkills(player, false);
    }

    addTriggerSkill(rule);
}

static const int GameOver = 1;

void RoomThread::run3v3(){
    QList<ServerPlayer *> warm, cool;
    foreach(ServerPlayer *player, room->m_players){
        switch(player->getRoleEnum()){
        case Player::Lord: warm.prepend(player); break;
        case Player::Loyalist: warm.append(player); break;
        case Player::Renegade: cool.prepend(player); break;
        case Player::Rebel: cool.append(player); break;
        }
    }

    QString order = room->askForOrder(cool.first());
    QList<ServerPlayer *> *first, *second;

    if(order == "warm"){
        first = &warm;
        second = &cool;
    }else{
        first = &cool;
        second = &warm;
    }

    action3v3(first->first());

    forever{
        qSwap(first, second);

        QList<ServerPlayer *> targets;
        foreach(ServerPlayer *player, *first){
            if(!player->hasFlag("actioned") && player->isAlive())
                targets << player;
        }

        ServerPlayer *to_action = room->askForPlayerChosen(first->first(), targets, "3v3-action");
        if(to_action){
            action3v3(to_action);

            if(to_action != first->first()){
                ServerPlayer *another;
                if(to_action == first->last())
                    another = first->at(1);
                else
                    another = first->last();

                if(!another->hasFlag("actioned") && another->isAlive())
                    action3v3(another);
            }
        }
    }
}

void RoomThread::action3v3(ServerPlayer *player){
    room->setCurrent(player);
    trigger(TurnStart, room, room->getCurrent());
    room->setPlayerFlag(player, "actioned");

    bool all_actioned = true;
    foreach(ServerPlayer *player, room->m_alivePlayers){
        if(!player->hasFlag("actioned")){
            all_actioned = false;
            break;
        }
    }

    if(all_actioned){
        foreach(ServerPlayer *player, room->m_alivePlayers){
            room->setPlayerFlag(player, "-actioned");
        }
    }
}

void RoomThread::run(){
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    if(setjmp(env) == GameOver){        
        return;
    }

    // start game, draw initial 4 cards
    foreach(ServerPlayer *player, room->getPlayers()){
        if(!player->getWakeSkills().isEmpty()){ //init the wake icon
            player->setFlags("init_wake");
            player->setFlags("-init_wake");
        }
        trigger(GameStart, room, player);
        trigger(GameStarted, room, player);
    }

    //if(room->scenario){
    //  room->scenario->run(room);
    if(room->getMode() == "dusong"){
        ServerPlayer *shenlvbu = room->getLord();
        if(shenlvbu->getGeneralName() == "zhang1dong"){
            QList<ServerPlayer *> league = room->getPlayers();
            league.removeOne(shenlvbu);

            forever{
                foreach(ServerPlayer *player, league){
                    if(player->hasFlag("actioned"))
                        room->setPlayerFlag(player, "-actioned");
                }

                foreach(ServerPlayer *player, league){
                    room->setCurrent(player);
                    trigger(TurnStart, room, room->getCurrent());

                    if(!player->hasFlag("actioned"))
                        room->setPlayerFlag(player, "actioned");

                    if(shenlvbu->getGeneralName() == "zhang2dong")
                        goto second_phase;

                    if(player->isAlive()){
                        room->setCurrent(shenlvbu);
                        trigger(TurnStart, room, room->getCurrent());

                        if(shenlvbu->getGeneralName() == "zhang2dong")
                            goto second_phase;
                    }
                }
            }

        }else{
            second_phase:

            foreach(ServerPlayer *player, room->getPlayers()){
                if(player != shenlvbu){
                    if(player->hasFlag("actioned"))
                        room->setPlayerFlag(player, "-actioned");

                    if(player->getPhase() != Player::NotActive){
                        PhaseChangeStruct phase;
                        phase.from = player->getPhase();
                        room->setPlayerProperty(player, "phase", "not_active");
                        phase.to = player->getPhase();
                        QVariant data = QVariant::fromValue(phase);
                        trigger(PhaseChange, room, player, data);
                    }
                }
            }

            room->setCurrent(shenlvbu);

            forever{
                trigger(TurnStart, room, room->getCurrent());
                room->setCurrent(room->getCurrent()->getNext());
            }
        }
    }else if(room->getMode() == "changban"){
        ServerPlayer *cbzhaoyun = room->getLord();
        ServerPlayer *cbzhangfei = cbzhaoyun;
        foreach(ServerPlayer *p, room->m_players){
            if(p->getRole() == "loyalist")
                cbzhangfei = p;
        }

        if(cbzhaoyun->getGeneralName() == "cbzhaoyun1"){
            QList<ServerPlayer *> league = room->m_players;
            league.removeOne(cbzhaoyun);
            league.removeOne(cbzhangfei);

            forever{
                foreach(ServerPlayer *player, league){
                    if(player->hasFlag("actioned"))
                        room->setPlayerFlag(player, "-actioned");
                }

                int i = 0;
                foreach(ServerPlayer *player, league){
                    room->setCurrent(player);
                    trigger(TurnStart, room, room->getCurrent());

                    if(!player->hasFlag("actioned"))
                        room->setPlayerFlag(player, "actioned");

                    if(cbzhaoyun->getGeneralName() == "cbzhaoyun2")
                        goto cbsecond_phase;

                    if(player->isAlive()){
                        if(i % 2 == 0){
                            room->setCurrent(cbzhaoyun);
                            trigger(TurnStart, room, room->getCurrent());

                            if(cbzhaoyun->getGeneralName() == "cbzhaoyun2")
                                goto cbsecond_phase;
                        }else{
                            room->setCurrent(cbzhangfei);
                            trigger(TurnStart, room, room->getCurrent());

                            if(cbzhaoyun->getGeneralName() == "cbzhaoyun2")
                                goto cbsecond_phase;
                        }

                        i++;
                    }
                }

                if(i == 1){
                    room->setCurrent(cbzhangfei);
                    trigger(TurnStart, room, room->getCurrent());

                    if(cbzhaoyun->getGeneralName() == "cbzhaoyun2")
                        goto cbsecond_phase;
                }
            }

        }else{
            cbsecond_phase:
            foreach(ServerPlayer *player, room->m_players){
                if(player != cbzhaoyun){
                    if(player->hasFlag("actioned"))
                        room->setPlayerFlag(player, "-actioned");

                    if(player->getPhase() != Player::NotActive){
                        PhaseChangeStruct phase;
                        phase.from = player->getPhase();
                        room->setPlayerProperty(player, "phase", "not_active");
                        phase.to = player->getPhase();
                        QVariant data = QVariant::fromValue(phase);
                        trigger(PhaseChange, room, player, data);
                    }
                }
            }

            room->setCurrent(cbzhaoyun);

            forever{
                trigger(TurnStart, room, room->getCurrent());
                room->setCurrent(room->getCurrent()->getNext());
            }
        }
    }
    else if(room->mode == "06_3v3")
        run3v3();
    else{
        if(room->getMode() == "02_1v1")
            room->setCurrent(room->getPlayers().at(1));
        forever {
            trigger(TurnStart, room, room->getCurrent());
            if (room->isFinished()) break;
            room->setCurrent(room->getCurrent()->getNextAlive());
        }
    }
}

const QList<EventTriplet> *RoomThread::getEventStack() const{
    return &event_stack;
}

class CompareByPriority{
public:
    CompareByPriority(TriggerEvent event){
        e = event;
    }

    void setEvent(TriggerEvent event){
        e = event;
    }

    bool operator()(const TriggerSkill *a, const TriggerSkill *b){
        return a->getPriority(e) > b->getPriority(e);
    }

private:
    TriggerEvent e;
};

bool RoomThread::trigger(TriggerEvent event, Room* room, ServerPlayer *target, QVariant &data){
    Q_ASSERT(QThread::currentThread() == this);

    // push it to event stack
    EventTriplet triplet(event, room, target, &data);
    event_stack.push_back(triplet);

    bool broken = false;
    foreach(const TriggerSkill *skill, skill_table[event]){
        if(skill->triggerable(target)){
            broken = skill->trigger(event, room, target, data);
            if(broken)
                break;
        }
    }

    if(target){
        foreach(AI *ai, room->ais){
            ai->filterEvent(event, target, data);
        }
    }

    delay(1);
    // pop event stack
    event_stack.pop_back();

    return broken;
}

bool RoomThread::trigger(TriggerEvent event, Room* room, ServerPlayer *target){
    QVariant data;
    return trigger(event, room, target, data);
}

void RoomThread::addTriggerSkill(const TriggerSkill *skill){
    if(skillSet.contains(skill))
        return;

    skillSet << skill;

    QList<TriggerEvent> events = skill->getTriggerEvents();
    foreach(TriggerEvent event, events){
        QList<const TriggerSkill *> &table = skill_table[event];

        table << skill;
        CompareByPriority comparator(event);
        qStableSort(table.begin(), table.end(), comparator);
    }

    if(skill->isVisible()){
        foreach(const Skill *skill, Sanguosha->getRelatedSkills(skill->objectName())){
            const TriggerSkill *trigger_skill = qobject_cast<const TriggerSkill *>(skill);
            addTriggerSkill(trigger_skill);
        }
    }
}

void RoomThread::delay(unsigned long secs){
    if(room->property("to_test").toString().isEmpty()&& Config.AIDelay>0)
        msleep(secs);
}

void RoomThread::end(){
    longjmp(env, GameOver);
}

QiaogongStruct::QiaogongStruct()
    :equip(NULL), wear(true), target(NULL)
{
}
