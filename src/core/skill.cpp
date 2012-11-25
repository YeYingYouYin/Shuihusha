#include "skill.h"
#include "engine.h"
#include "player.h"
#include "room.h"
#include "client.h"
#include "standard.h"
#include "scenario.h"

#include <QFile>

Skill::Skill(const QString &name, Frequency frequency)
    :frequency(frequency), default_choice("no")
{
    static QChar lord_symbol('$');

    if(name.endsWith(lord_symbol)){
        QString copy = name;
        copy.remove(lord_symbol);
        setObjectName(copy);
        lord_skill = true;
    }else{
        setObjectName(name);
        lord_skill = false;
    }
}

bool Skill::isLordSkill() const{
    return lord_skill;
}

QString Skill::getDescription() const{
    if(!Sanguosha->isDuplicated(objectName()))
        return Sanguosha->translate("::");
    return Sanguosha->translate(":" + objectName());
}

QString Skill::getText() const{
    QString skill_name = Sanguosha->translate(objectName());

    switch(frequency){
    case Skill::NotFrequent:
    case Skill::Frequent: break;
    case Skill::Limited: skill_name.append(Sanguosha->translate("[Limited]")); break;
    case Skill::Compulsory: skill_name.append(Sanguosha->translate("[Compulsory]")); break;
    case Skill::Wake: skill_name.append(Sanguosha->translate("[Wake]")); break;
    default: break;
    }

    return skill_name;
}

bool Skill::isVisible() const{
    return ! objectName().startsWith("#");
}

QString Skill::getDefaultChoice(ServerPlayer *) const{
    return default_choice;
}

int Skill::getEffectIndex(const ServerPlayer *, const Card *) const{
    return -1;
}

void Skill::initMediaSource(){
    sources.clear();

    int i;
    for(i=1; ;i++){
        QString effect_file = QString("audio/skill/%1%2.ogg").arg(objectName()).arg(i);
        if(!QFile::exists(effect_file))
            effect_file = QString("extensions/audio/skill/%1%2.ogg").arg(objectName()).arg(i);
        if(QFile::exists(effect_file))
            sources << effect_file;
        else
            break;
    }

    if(sources.isEmpty()){
        QString effect_file = QString("audio/skill/%1.ogg").arg(objectName());
        if(!QFile::exists(effect_file))
            effect_file = QString("extensions/audio/skill/%1.ogg").arg(objectName());
        if(QFile::exists(effect_file))
            sources << effect_file;
    }
}

Skill::Location Skill::getLocation() const{
    return parent() ? Right : Left;
}

void Skill::playEffect(int index) const{
    if(!sources.isEmpty()){
        if(index == -1)
            index = qrand() % sources.length();
        else
            index --;

        // check length
        QString filename;
        if(index >= 0 && index < sources.length())
            filename = sources.at(index);
        else
            filename = sources.first();

        Sanguosha->playEffect(filename);
        if(ClientInstance)
            ClientInstance->setLines(filename);
    }
}

void Skill::setFlag(ServerPlayer *player) const{
    player->getRoom()->setPlayerFlag(player, objectName());
}

void Skill::unsetFlag(ServerPlayer *player) const{
    player->getRoom()->setPlayerFlag(player, "-" + objectName());
}

Skill::Frequency Skill::getFrequency() const{
    return frequency;
}

QStringList Skill::getSources() const{
    return sources;
}

QDialog *Skill::getDialog() const{
    return NULL;
}

ViewAsSkill::ViewAsSkill(const QString &name)
    :Skill(name)
{

}

bool ViewAsSkill::isAvailable() const{
    switch(ClientInstance->getStatus()){
    case Client::Playing: return isEnabledAtPlay(Self);
    case Client::Responsing: return isEnabledAtResponse(Self, ClientInstance->getPattern());
    default:
        return false;
    }
}

bool ViewAsSkill::isEnabledAtPlay(const Player *) const{
    return true;
}

bool ViewAsSkill::isEnabledAtResponse(const Player *, const QString &) const{
    return false;
}

ZeroCardViewAsSkill::ZeroCardViewAsSkill(const QString &name)
    :ViewAsSkill(name)
{

}

const Card *ZeroCardViewAsSkill::viewAs(const QList<CardItem *> &cards) const{
    if(cards.isEmpty())
        return viewAs();
    else
        return NULL;
}

bool ZeroCardViewAsSkill::viewFilter(const QList<CardItem *> &, const CardItem *) const{
    return false;
}

OneCardViewAsSkill::OneCardViewAsSkill(const QString &name)
    :ViewAsSkill(name)
{

}

bool OneCardViewAsSkill::viewFilter(const QList<CardItem *> &selected, const CardItem *to_select) const{
    return selected.isEmpty() && viewFilter(to_select);
}

const Card *OneCardViewAsSkill::viewAs(const QList<CardItem *> &cards) const{
    if(cards.length() != 1)
        return NULL;
    else
        return viewAs(cards.first());
}

FilterSkill::FilterSkill(const QString &name)
    :OneCardViewAsSkill(name)
{
    frequency = Compulsory;
}

TriggerSkill::TriggerSkill(const QString &name)
    :Skill(name), view_as_skill(NULL)
{

}

const ViewAsSkill *TriggerSkill::getViewAsSkill() const{
    return view_as_skill;
}

QList<TriggerEvent> TriggerSkill::getTriggerEvents() const{
    return events;
}

int TriggerSkill::getPriority() const{
    switch(frequency){
    case Compulsory:
    case Wake: return 2;
    default:
        return 1;
    }
}

bool TriggerSkill::triggerable(const ServerPlayer *target) const{
    return target->isAlive() && target->hasSkill(objectName());
}

ScenarioRule::ScenarioRule(Scenario *scenario)
    :TriggerSkill(scenario->objectName())
{
    setParent(scenario);
}

int ScenarioRule::getPriority() const{
    return 3;
}

bool ScenarioRule::triggerable(const ServerPlayer *) const{
    return true;
}

MasochismSkill::MasochismSkill(const QString &name)
    :TriggerSkill(name)
{
    events << Damaged;
}

int MasochismSkill::getPriority() const{
    return -1;
}

bool MasochismSkill::trigger(TriggerEvent, Room*, ServerPlayer *player, QVariant &data) const{
    DamageStruct damage = data.value<DamageStruct>();

    if(player->isAlive())
        onDamaged(player, damage);

    return false;
}

PhaseChangeSkill::PhaseChangeSkill(const QString &name)
    :TriggerSkill(name)
{
    events << PhaseChange;
}

bool PhaseChangeSkill::trigger(TriggerEvent, Room*, ServerPlayer *player, QVariant &) const{
    bool skipped = onPhaseChange(player);
    if(skipped)
        player->skip(player->getPhase());

    return skipped;
}

DrawCardsSkill::DrawCardsSkill(const QString &name)
    :TriggerSkill(name)
{
    events << DrawNCards << DrawNCardsDone;
}

void DrawCardsSkill::drawDone(ServerPlayer *, int) const{
    return;
}

bool DrawCardsSkill::trigger(TriggerEvent event, Room*, ServerPlayer *player, QVariant &data) const{
    int n = data.toInt();
    if(event == DrawNCards)
        data = getDrawNum(player, n);
    else
        drawDone(player, n);
    return false;
}

SlashBuffSkill::SlashBuffSkill(const QString &name)
    :TriggerSkill(name)
{
    events << SlashProceed;
}

bool SlashBuffSkill::trigger(TriggerEvent, Room*, ServerPlayer *player, QVariant &data) const{
    if(data.canConvert<SlashEffectStruct>()){
        SlashEffectStruct effect = data.value<SlashEffectStruct>();

        if(player->isAlive())
            return buff(effect);
    }

    return false;
}

GameStartSkill::GameStartSkill(const QString &name)
    :TriggerSkill(name)
{
    events << GameStart << Death;
}

bool GameStartSkill::triggerable(const ServerPlayer *target) const{
    return target->hasSkill(objectName());
}

bool GameStartSkill::trigger(TriggerEvent event, Room*, ServerPlayer *player, QVariant &) const{
    if (event == GameStart)
        onGameStart(player);
    else
        onIdied(player);

    return false;
}

void GameStartSkill::onIdied(ServerPlayer *) const{
    return;
}

ClientSkill::ClientSkill(const QString &name)
    :Skill(name, Skill::Compulsory)
{
}

int ClientSkill::getExtra(const Player *) const{
    return 0;
}

int ClientSkill::getCorrect(const Player *, const Player *) const{
    return 0;
}

int ClientSkill::getAtkrg(const Player *) const{
    return 0;
}

int ClientSkill::getSlashResidue(const Player *target) const{
    return qMax(1 - target->getSlashCount(), 0);
}

bool ClientSkill::isProhibited(const Player *from, const Player *to, const Card *card) const{
    return false;
}
/*
ProhibitSkill::ProhibitSkill(const QString &name)
    :ClientSkill(name){
}
*/
DistanceSkill::DistanceSkill(const QString &name)
    :ClientSkill(name){
}
/*
MaxCardsSkill::MaxCardsSkill(const QString &name)
    :ClientSkill(name){
}
*/
WeaponSkill::WeaponSkill(const QString &name)
    :TriggerSkill(name)
{
}

bool WeaponSkill::triggerable(const ServerPlayer *target) const{
    if (!target) return false;
    return target->hasWeapon(objectName());
}

ArmorSkill::ArmorSkill(const QString &name)
    :TriggerSkill(name)
{

}

bool ArmorSkill::triggerable(const ServerPlayer *target) const{
    if (!target) return false;
    return target->hasArmorEffect(objectName()) && target->getArmor()->getSkill() == this;
}

MarkAssignSkill::MarkAssignSkill(const QString &mark, int n)
    :GameStartSkill(QString("#%1-%2").arg(mark).arg(n)), mark_name(mark), n(n)
{
}

int MarkAssignSkill::getPriority() const{
    return -1;
}

void MarkAssignSkill::onGameStart(ServerPlayer *player) const{
    player->gainMark(mark_name, n);
}

CutHpSkill::CutHpSkill(int n)
    :GameStartSkill(QString("#hp-%1").arg(n)), n(n)
{
}

int CutHpSkill::getPriority() const{
    return -1;
}

void CutHpSkill::onGameStart(ServerPlayer *player) const{
    player->getRoom()->setPlayerProperty(player, "hp", player->getHp() - n);
}
