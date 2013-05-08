#include "player.h"
#include "engine.h"
#include "room.h"
#include "client.h"
#include "standard.h"
#include "settings.h"

Player::Player(QObject *parent)
    :QObject(parent), owner(false), ready(false), general(NULL), general2(NULL),
    hp(-1), max_hp(-1), state("online"), seat(0), alive(true),
    phase(NotActive),
    weapon(NULL), armor(NULL), defensive_horse(NULL), offensive_horse(NULL),
    face_up(true), chained(false), player_statistics(new StatisticsStruct())
{
}

void Player::setScreenName(const QString &screen_name){
    this->screen_name = screen_name;
}

QString Player::screenName() const{
    return screen_name;
}

bool Player::isOwner() const{
    return owner;
}

void Player::setOwner(bool owner){
    if(this->owner != owner){
        this->owner = owner;
        emit owner_changed(owner);
    }
}

bool Player::isReady() const{
    return ready;
}

void Player::setReady(bool ready){
    if(this->ready != ready){
        this->ready = ready;
        emit ready_changed(ready);
    }
}

void Player::setHp(int hp){
    if(hp <= max_hp && this->hp != hp){
        this->hp = hp;
        emit state_changed();
    }
}

int Player::getHp() const{
    return hp;
}

int Player::getMaxHP() const{
    return max_hp;
}

int Player::getMaxHp() const{
    return getMaxHP();
}

void Player::setMaxHP(int max_hp){
    if(this->max_hp == max_hp)
        return;

    this->max_hp = max_hp;
    if(hp > max_hp)
        hp = max_hp;

    emit state_changed();
}

void Player::setMaxHp(int max_hp){
    setMaxHP(max_hp);
}

int Player::getLostHp(bool zeromax) const{
    int local_hp = zeromax ? qMax(hp, 0) : hp;
    return max_hp - local_hp;
}

bool Player::isWounded() const{
    if(hp < 0)
        return true;
    else
        return hp < max_hp;
}

General::Gender Player::getGender() const{
    if(general)
        return general->getGender();
    else
        return General::Neuter;
}

QString Player::getGenderString() const{
    if(general)
        return general->getGenderString();
    else
        return "neuter";
}

int Player::getSeat() const{
    return seat;
}

void Player::setSeat(int seat){
    this->seat = seat;
}

bool Player::isAlive() const{
    return alive;
}

bool Player::isDead() const{
    return !alive;
}

void Player::setAlive(bool alive){
    this->alive = alive;
}

QString Player::getFlags() const{
    QStringList flags_list;
    foreach(QString flag, flags)
        flags_list << flag;

    return flags_list.join("+");
}

QStringList Player::getClearFlags() const{
    QStringList flags_list;
    foreach(QString flag, flags){
        if(flag.startsWith("%"))
            flags_list << flag;
    }
    return flags_list;
}

void Player::setFlags(const QString &flag){
    static QChar unset_symbol('-');
    if(flag.startsWith(unset_symbol)){
        QString copy = flag;
        copy.remove(unset_symbol);
        flags.remove(copy);
    }else{
        flags.insert(flag);
    }
}

bool Player::hasFlag(const QString &flag) const{
    QString clflag = "%" + flag;
    return flags.contains(flag) || flags.contains(clflag);
}

void Player::clearFlags(){
    flags.clear();
}

int Player::getAttackRange(const Player *other, const Card *slash) const{
    int atkrg = getWeapon() ? getWeapon()->getRange() : 1;
    int extra = Sanguosha->correctClient("attackrange", this, other, slash);

    return extra >= 0 ? qMax(atkrg, extra): qAbs(extra);
}

bool Player::inMyAttackRange(const Player *other) const{
    return distanceTo(other) <= getAttackRange(other);
}

void Player::setFixedDistance(const Player *player, int distance){
    if(distance == -1)
        fixed_distance.remove(player);
    else
        fixed_distance.insert(player, distance);
}

bool Player::isBetweenAandB(const Player *a, const Player *b) const{
    int R = qAbs(a->seat - b->seat);
    int L = aliveCount() - R;
    int atob = qMin(R, L);
    R = qAbs(this->seat - a->seat);
    L = aliveCount() - R;
    int metoa = qMin(R, L);
    R = qAbs(this->seat - b->seat);
    L = aliveCount() - R;
    int metob = qMin(R, L);
    return metoa + metob == atob;
}

int Player::distanceTo(const Player *other) const{
    if(this == other)
        return 0;

    if(fixed_distance.contains(other))
        return fixed_distance.value(other);

    int noseecount = 0;
    QList<const Player *> shuis;
    foreach(const Player *player, getSiblings()){
        if(player != other && player->hasSkill("shuizhan")){
            noseecount ++;
            shuis << player;
        }
    }

    int right = qAbs(seat - other->seat);
    int left = aliveCount() - right;
    int distance = qMin(left, right);

    if(noseecount > 0 && !this->hasSkill("shuizhan") && !other->hasSkill("shuizhan")){
        foreach(const Player *tmp, shuis){
            if(tmp->isBetweenAandB(this, other))
                distance --;
        }
    }
    distance += Sanguosha->correctClient("distance", this, other);

    // keep the distance >=1
    if(distance < 1)
        distance = 1;

    if(other->hasSkill("shuizhan"))
        return qMax(distance, 2);
    else
        return distance;
}

void Player::setGeneral(const General *new_general){
    if(this->general != new_general){
        this->general = new_general;

        if(new_general && kingdom.isEmpty())
            setKingdom(new_general->getKingdom());

        emit general_changed();
    }
}

void Player::setGeneralName(const QString &general_name){
    const General *new_general = Sanguosha->getGeneral(general_name);
    setGeneral(new_general);
}

QString Player::getGeneralName() const{
    if(general)
        return general->objectName();
    else
        return QString();
}

void Player::setGeneral2Name(const QString &general_name){
    const General *new_general = Sanguosha->getGeneral(general_name);
    if(general2 != new_general){
        general2 = new_general;

        emit general2_changed();
    }
}

QString Player::getGeneral2Name() const{
    if(general2)
        return general2->objectName();
    else
        return QString();
}

const General *Player::getGeneral2() const{
    return general2;
}

QString Player::getState() const{
    return state;
}

void Player::setState(const QString &state){
    if(this->state != state){
        this->state = state;
        emit state_changed();
    }
}

void Player::setRole(const QString &role){
    if(this->role != role){
        this->role = role;
        emit role_changed(role);
    }
}

QString Player::getRole() const{
    return role;
}

QString Player::getScreenRole() const{
    int index = Sanguosha->getRoleIndex();

    QMap<QString, QString> threeV3_mode, hegemony_mode, landlord_mode, warlords_mode;

    threeV3_mode["lord"] = threeV3_mode["renegade"] = "Marshal";
    threeV3_mode["loyalist"] = threeV3_mode["rebel"] = "Vanguard";

    hegemony_mode["lord"] = "guan";
    hegemony_mode["loyalist"] = "jiang";
    hegemony_mode["rebel"] = "min";
    hegemony_mode["renegade"] = "kou";

    landlord_mode["lord"] = "Landlord";
    landlord_mode["rebel"] = "Cottier";

    warlords_mode["lord"] = "Castellan";
    warlords_mode["loyalist"] = "Adviser";
    warlords_mode["rebel"] = "Peasant";

    switch(index){
    case 2: return threeV3_mode[role]; break;
    case 3: return hegemony_mode[role]; break;
    case 4: return landlord_mode[role]; break;
    case 5: return warlords_mode[role]; break;
    default:
        return role;
    }
    return role;
}

Player::Role Player::getRoleEnum() const{
    static QMap<QString, Role> role_map;
    if(role_map.isEmpty()){
        role_map.insert("lord", Lord);
        role_map.insert("loyalist", Loyalist);
        role_map.insert("rebel", Rebel);
        role_map.insert("renegade", Renegade);
    }

    return role_map.value(role);
}

const General *Player::getAvatarGeneral() const{
    if(general)
        return general;

    QString general_name = property("avatar").toString();
    if(general_name.isEmpty())
        return NULL;
    return Sanguosha->getGeneral(general_name);
}

const General *Player::getGeneral() const{
    return general;
}

bool Player::isLord() const{
    return getRole() == "lord";
}

bool Player::hasSkill(const QString &skill_name) const{
    if(hasMark("scarecrow")) //qimen
        return false;
    return hasInnateSkill(skill_name)
            || acquired_skills.contains(skill_name);
}

bool Player::hasInnateSkill(const QString &skill_name) const{
    if(general && general->hasSkill(skill_name))
        return true;

    if(general2 && general2->hasSkill(skill_name))
        return true;

    return false;
}

bool Player::hasLordSkill(const QString &skill_name) const{
    if(!isLord())
        return false;
    if(Config.NoLordSkill || Config.EnableAnzhan)
        return false;

    QString mode = getGameMode();
    if(mode == "06_3v3" || mode == "02_1v1")
        return false;

    if(Sanguosha->getScenarioLordSkill(mode))
        return false;

    return hasSkill(skill_name);
}

void Player::acquireSkill(const QString &skill_name){
    acquired_skills.insert(skill_name);
}

void Player::loseSkill(const QString &skill_name){
    acquired_skills.remove(skill_name);
}

void Player::loseAllSkills(){
    acquired_skills.clear();
}

QString Player::getPhaseString() const{
    switch(phase){
    case RoundStart: return "round_start";
    case Start: return "start";
    case Judge: return "judge";
    case Draw: return "draw";
    case Play: return "play";
    case Discard: return "discard";
    case Finish: return "finish";
    case NotActive:
    default:
        return "not_active";
    }
}

void Player::setPhaseString(const QString &phase_str){
    static QMap<QString, Phase> phase_map;
    if(phase_map.isEmpty()){
        phase_map.insert("round_start", RoundStart);
        phase_map.insert("start",Start);
        phase_map.insert("judge", Judge);
        phase_map.insert("draw", Draw);
        phase_map.insert("play", Play);
        phase_map.insert("discard", Discard);
        phase_map.insert("finish", Finish);
        phase_map.insert("not_active", NotActive);
    }

    setPhase(phase_map.value(phase_str, NotActive));
}

void Player::setEquip(const EquipCard *card){
    switch(card->location()){
    case EquipCard::WeaponLocation: weapon = qobject_cast<const Weapon*>(card); break;
    case EquipCard::ArmorLocation: armor = qobject_cast<const Armor*>(card); break;
    case EquipCard::DefensiveHorseLocation: defensive_horse = qobject_cast<const Horse*>(card); break;
    case EquipCard::OffensiveHorseLocation: offensive_horse = qobject_cast<const Horse*>(card); break;
    }
}

void Player::removeEquip(const EquipCard *equip){
    switch(equip->location()){
    case EquipCard::WeaponLocation: weapon = NULL; break;
    case EquipCard::ArmorLocation: armor = NULL; break;
    case EquipCard::DefensiveHorseLocation: defensive_horse = NULL; break;
    case EquipCard::OffensiveHorseLocation:offensive_horse = NULL; break;
    }
}

bool Player::hasEquip(const Card *card) const{
    if(hasSkill("qiaogong")){
        QString perty = QString("qiaogong_%1").arg(card->getSubtype().left(1));
        CardStar car = tag[perty].value<CardStar>();
        if(car == card)
            return true;
    }
    return weapon == card || armor == card || defensive_horse == card || offensive_horse == card;
}

bool Player::hasEquip() const{
    if(hasSkill("qiaogong")){
        QStringList nn;
        nn << "w" << "a" << "d" << "o";
        foreach(QString n, nn){
            QString perty = QString("qiaogong_%1").arg(n);
            CardStar car = tag[perty].value<CardStar>();
            if(car)
                return true;
        }
    }
    return weapon || armor || defensive_horse || offensive_horse;
}

bool Player::hasEquip(const QString &name, bool inherit) const{
    //for example: hasEquip("axe") ;  hasEquip("Weapon", true)
    bool ok = false;
    foreach(const Card *equip, getEquips()){
        if((!inherit && equip->objectName() == name) ||
           (inherit && equip->inherits(name.toLocal8Bit().data()))){
            ok = true;
            break;
        }
    }
    return ok;
}

const Weapon *Player::getWeapon(bool trueequip) const{
    if(trueequip)
        return weapon;
    if(hasSkill("qiaogong")){
        CardStar car = tag["qiaogong_w"].value<CardStar>();
        if(car)
            return qobject_cast<const Weapon*>(car);
    }
    return weapon;
}

const Armor *Player::getArmor(bool trueequip) const{
    if(trueequip)
        return armor;
    if(hasSkill("qiaogong")){
        CardStar car = tag["qiaogong_a"].value<CardStar>();
        if(car)
            return qobject_cast<const Armor*>(car);
    }
    return armor;
}

const Horse *Player::getDefensiveHorse(bool trueequip) const{
    if(trueequip)
        return defensive_horse;
    if(hasSkill("qiaogong")){
        CardStar car = tag["qiaogong_d"].value<CardStar>();
        if(car)
            return qobject_cast<const Horse*>(car);
    }
    return defensive_horse;
}

const Horse *Player::getOffensiveHorse(bool trueequip) const{
    if(trueequip)
        return offensive_horse;
    if(hasSkill("qiaogong")){
        CardStar car = tag["qiaogong_o"].value<CardStar>();
        if(car)
            return qobject_cast<const Horse*>(car);
    }
    return offensive_horse;
}

QList<const Card *> Player::getEquips(bool trueequip) const{
    QList<const Card *> equips;
    if(weapon)
        equips << weapon;
    if(armor)
        equips << armor;
    if(defensive_horse)
        equips << defensive_horse;
    if(offensive_horse)
        equips << offensive_horse;

    if(!trueequip && hasSkill("qiaogong")){
        QStringList nn;
        nn << "w" << "a" << "d" << "o";
        foreach(QString n, nn){
            QString proty = QString("qiaogong_%1").arg(n);
            CardStar car = tag[proty].value<CardStar>();
            if(car)
                equips << car;
        }
    }

    return equips;
}

const EquipCard *Player::getEquip(int index) const{
    switch(index){
    case 0: return weapon; break;
    case 1: return armor; break;
    case 2: return defensive_horse; break;
    case 3: return offensive_horse; break;
    default:
        break;
    }

    return NULL;
}

bool Player::hasWeapon(const QString &weapon_name) const{
    if(hasSkill("qiaogong")){
        CardStar car = tag["qiaogong_w"].value<CardStar>();
        return car && car->objectName() == weapon_name;
    }
    return weapon && weapon->objectName() == weapon_name;
}

bool Player::hasArmorEffect(const QString &armor_name) const{
    if(hasSkill("qiaogong")){
        CardStar car = tag["qiaogong_a"].value<CardStar>();
        return car && !isPenetrated() && car->objectName() == armor_name;
    }
    return armor && !isPenetrated() && armor->objectName() == armor_name;
}

QList<const Card *> Player::getJudgingArea() const{
    return judging_area;
}

Player::Phase Player::getPhase() const{
    return phase;
}

void Player::setPhase(Phase phase){
    this->phase = phase;

    emit phase_changed();
}

bool Player::faceUp() const{
    return face_up;
}

void Player::setFaceUp(bool face_up){
    if(this->face_up != face_up){
        this->face_up = face_up;

        emit state_changed();
    }
}

int Player::getMaxCards() const{
    int rule = 0, total = 0, extra = 0;
    if(Config.MaxHpScheme == 2 && general2){
        total = general->getMaxHp() + general2->getMaxHp();
        if(total % 2 != 0)
            rule = 1;
    }
    extra += Sanguosha->correctClient("maxcard", this);

    if(extra < 0)
        return qAbs(extra);
    else
        return (qMax(hp,0) + rule + extra);
}

int Player::getSlashTarget(const Player *other, const Card *slash) const{
    int rule = 1;
    int extra = Sanguosha->correctClient("extragoals", this, other, slash);
    return rule + extra;
}

QString Player::getKingdom() const{
    if(kingdom.isEmpty() && general)
        return general->getKingdom();
    else
        return kingdom;
}

void Player::setKingdom(const QString &kingdom){
    if(this->kingdom != kingdom){
        this->kingdom = kingdom;
        emit kingdom_changed();
    }
}

QString Player::getKingdomIcon() const{
    return QString("image/kingdom/icon/%1.png").arg(kingdom);
}

QString Player::getKingdomFrame() const{
    return QString("image/kingdom/frame/%1.png").arg(kingdom);
}

bool Player::isKongcheng() const{
    return getHandcardNum() == 0;
}

bool Player::isNude() const{
    return isKongcheng() && !hasEquip();
}

bool Player::isAllNude() const{
    return isNude() && judging_area.isEmpty();
}

void Player::addDelayedTrick(const Card *trick){
    judging_area << trick;
    const DelayedTrick *t_rick = DelayedTrick::CastFrom(trick);
    if(t_rick)
        delayed_tricks << t_rick;
}

void Player::removeDelayedTrick(const Card *trick){
    int index = judging_area.indexOf(trick);
    if(index >= 0){
        judging_area.removeAt(index);
        delayed_tricks.removeAt(index);
    }
}

const DelayedTrick *Player::topDelayedTrick() const{
    if(delayed_tricks.isEmpty())
        return NULL;
    else
        return delayed_tricks.last();
}

QList<const DelayedTrick *> Player::delayedTricks() const{
    return delayed_tricks;
}

bool Player::containsTrick(const QString &trick_name, bool consi_hq) const{
    foreach(const DelayedTrick *trick, delayed_tricks){
        if(trick->objectName() == trick_name)
            return true;
    }

    if(hasEquip("haiqiu") && consi_hq)
        return trick_name != "tsunami";
    return false;
}

bool Player::isChained() const{
    return chained;
}

void Player::setChained(bool chained){
    if(this->chained != chained){
        this->chained = chained;
        emit state_changed();
    }
}

void Player::addMark(const QString &mark){
    int value = marks.value(mark, 0);
    value++;
    setMark(mark, value);
}

void Player::removeMark(const QString &mark){
    int value = marks.value(mark, 0);
    value--;
    value = qMax(0, value);
    setMark(mark, value);
}

void Player::setMark(const QString &mark, int value){
    if(marks[mark] != value)
        marks[mark] = value;
}

int Player::getMark(const QString &mark) const{
    int n = marks.value(mark, 0);
    if(mark.startsWith("_")){
        // wake mark "skill_wake", getMark("_wake") to get all mark endswith "wake"
        QMap<QString, int>::const_iterator i;
        for(i = marks.constBegin(); i != marks.constEnd(); ++i){
            QString key = i.key();
            if(key.endsWith(mark))
                n = n + marks.value(key, 0);
        }
    }
    return n;
}

bool Player::hasMark(const QString &mark) const{
    return marks.value(mark, 0) > 0;
}

QStringList Player::getAllMarkName(int flag, const QString &part) const{
    //flag = 1 : startwith; flag = 2 : all; flag = 3 : endwith
    QStringList marknames;
    QMap<QString, int>::const_iterator i;
    for(i = marks.constBegin(); i != marks.constEnd(); ++i){
        QString key = i.key();
        switch(flag){
        case 1:
            if(key.startsWith(part) && hasMark(key)) //like "@skull"
                marknames << key;
            break;
        case 3:
            if(key.endsWith(part) && hasMark(key)) //like "aoxiang_wake"
                marknames << key;
            break;
        case 2:
        default:
            marknames << key;
        }
    }
    return marknames;
}

bool Player::canSlash(const Player *other, const Card *slash, bool distance_limit) const{
    if(slash == NULL)
        slash = Sanguosha->cloneCard("slash", Card::NoSuit, 0);

    if(isProhibited(other, slash))
        return false;

    if(other == this)
        return false;

    bool target_rule = distance_limit ? distanceTo(other) <= getAttackRange(other, slash) : true;
    if(slash->getSkillName() == "strike")
        return !target_rule;
    else
        return target_rule;
}

bool Player::canSlash(const Player *other, bool distance_limit) const{
    const Card *slash = Sanguosha->cloneCard("slash", Card::NoSuit, 0);
    return canSlash(other, slash, distance_limit);
}

int Player::getCardCount(bool include_equip) const{
    int count = getHandcardNum();

    if(include_equip){
        if(weapon)
            count ++;

        if(armor)
            count ++;

        if(defensive_horse)
            count ++;

        if(offensive_horse)
            count ++;
    }

    return count;
}

QList<int> Player::getPile(const QString &pile_name) const{
    return piles[pile_name];
}

QStringList Player::getPileNames() const{
    QStringList names;
    foreach(QString pile_name, piles.keys())
        names.append(pile_name);
    return names;
}

QString Player::getPileName(int card_id) const{
    foreach(QString pile_name, piles.keys()){
        QList<int> pile = piles[pile_name];
        if(pile.contains(card_id))
            return pile_name;
    }

    return QString();
}

void Player::addHistory(const QString &name, int times){
    history[name] += times;
}

int Player::usedTimes(const QString &card_class, int init) const{
    return history.value(card_class, init);
}

int Player::getSlashCount() const{
    return usedTimes("Slash") + usedTimes("ThunderSlash") + usedTimes("FireSlash");
}

QStringList Player::getHistorys() const{
    return history.keys();
}

void Player::clearHistory(const QString &name){
    if(!getHistorys().contains(name))
        history.clear();
    else
        history.remove(name);
}

bool Player::hasUsed(const QString &card_class) const{
    return history.value(card_class, 0) > 0;
}

QSet<const TriggerSkill *> Player::getTriggerSkills() const{
    QSet<const TriggerSkill *> skills;
    if(general)
        skills += general->getTriggerSkills();

    if(general2)
        skills += general2->getTriggerSkills();

    foreach(QString skill_name, acquired_skills){
        const TriggerSkill *skill = Sanguosha->getTriggerSkill(skill_name);
        if(skill)
            skills << skill;
    }

    return skills;
}

QSet<const Skill *> Player::getVisibleSkills() const{
    return getVisibleSkillList().toSet();
}

QList<const Skill *> Player::getVisibleSkillList() const{
    QList<const Skill *> skills;
    if(general)
        skills << general->getVisibleSkillList();

    if(general2)
        skills << general2->getVisibleSkillList();

    foreach(QString skill_name, acquired_skills){
        const Skill *skill = Sanguosha->getSkill(skill_name);
        if(skill->getFrequency() == Skill::NotSkill)
            continue;
        if(skill->isVisible())
            skills << skill;
    }

    return skills;
}

QStringList Player::getVisibleSkillList(const QString &exclude) const{
    QList<const Skill *> skills;
    if(general)
        skills << general->getVisibleSkillList();
    if(general2)
        skills << general2->getVisibleSkillList();
    foreach(QString skill_name, acquired_skills){
        const Skill *skill = Sanguosha->getSkill(skill_name);
        if(skill->getFrequency() == Skill::NotSkill)
            continue;
        if(skill->isVisible())
            skills << skill;
    }

    QStringList skis;
    foreach(const Skill *skill, skills){
        if(skill->parent() && !skill->objectName().startsWith(exclude))
            skis << skill->objectName();
    }

    return skis;
}

QStringList Player::getWakeSkills() const{
    QStringList list;
    foreach(const Skill *skil, getVisibleSkillList()){
        if(skil->getFrequency() == Skill::Wake)
            list << skil->objectName();
    }
    if(hasSkill("wanghuan"))
        list << "aoxiang";
    return list;
}

int Player::getKingdoms() const{
    QSet<QString> kingdom_set;
    kingdom_set << getKingdom();
    foreach(const Player *tmp, getSiblings())
        kingdom_set << tmp->getKingdom();
    return kingdom_set.size();
}

QSet<QString> Player::getAcquiredSkills() const{
    return acquired_skills;
}

QString Player::getAllSkillDescription() const{
    if(!getGeneral())
        return QString();
    QString local_desc = tr("<font color=red size=4>Main:</font><br/>%1").arg(getGeneral()->getSkillDescription());
    QString local_desc2 = getGeneral2() ? tr("<font color=blue size=4>Extra:</font><br/>%1").arg(getGeneral2()->getSkillDescription())
        : QString();
    QString acquired_desc = QString();
    if(!acquired_skills.isEmpty()){
        acquired_desc = tr("<font color=green size=4>Acquired:</font><br/>");
        foreach(QString skill_name, acquired_skills){
            const Skill *skill = Sanguosha->getSkill(skill_name);
            if(skill){
                if(skill->getFrequency() == Skill::NotSkill)
                    continue;
                if(getGeneral()->hasSkill(skill_name) ||
                   (getGeneral2() && getGeneral2()->hasSkill(skill_name)))
                    continue;

                QString skill_name = Sanguosha->translate(skill->objectName());
                QString desc = skill->getDescription();
                desc.replace("\n", "<br/>");
                acquired_desc.append(QString("<b>%1</b>: %2 <br/> <br/>").arg(skill_name).arg(desc));
            }
        }
    }
    QString conjur_desc = QString();
    if(!getAllMarkName(3, "_jur").isEmpty()){
        QString conjur = getAllMarkName(3, "_jur").first();
        conjur_desc = tr("<font color=deeppink size=4>Conjuring:</font><br/>");
        conjur_desc.append(QString("<b>%1</b>: %2 <br/> <br/>").arg(Sanguosha->translate(conjur)).arg(Sanguosha->translate(":" + conjur)));
    }
    QString fin = QString("%1%2%3%4").arg(local_desc).arg(local_desc2).arg(acquired_desc).arg(conjur_desc);
    fin.replace("<br/> <br/>", "<br/>");
    return fin;
}

bool Player::isProhibited(const Player *to, const Card *card) const{
    return Sanguosha->isProhibited(this, to, card);
}

bool Player::isPenetrated(const Player *from, const Card *card) const{
    return Sanguosha->isPenetrate(from, this, card);
}

bool Player::canSlashWithoutCrossbow() const{
    return Slash::IsAvailable(this);
}

void Player::jilei(const QString &type){
    if(type == ".")
        jilei_set.clear();
    else if(type == "basic")
        jilei_set << "BasicCard";
    else if(type == "trick")
        jilei_set << "TrickCard";
    else if(type == "equip")
        jilei_set << "EquipCard";
    else
        jilei_set << type;
}

bool Player::isJilei(const Card *card) const{
    if(card->getTypeId() == Card::Skill){
        if(!card->canJilei())
            return false;

        foreach(int card_id, card->getSubcards()){
            const Card *c = Sanguosha->getCard(card_id);
            foreach(QString pattern, jilei_set.toList()){
                ExpPattern p(pattern);
                if(p.match(this,c) && !hasEquip(c)) return true;
            }
        }
    }
    else{
        if(card->getSubcards().isEmpty())
            foreach(QString pattern, jilei_set.toList()){
                ExpPattern p(pattern);
                if(p.match(this,card)) return true;
            }
        else{
            foreach(int card_id, card->getSubcards()){
                const Card *c = Sanguosha->getCard(card_id);
                foreach(QString pattern, jilei_set.toList()){
                    ExpPattern p(pattern);
                    if(p.match(this,card) && !hasEquip(c)) return true;
                }
            }
        }
    }

    return false;
}

void Player::setCardLocked(const QString &name){
    static QChar unset_symbol('-');
    if(name.isEmpty())
        return;
    else if(name == ".")
        lock_card.clear();
    else if(name.startsWith(unset_symbol)){
        QString copy = name;
        copy.remove(unset_symbol);
        lock_card.remove(copy);
    }
    else if(!lock_card.contains(name))
        lock_card << name;
}

bool Player::isLocked(const Card *card) const{
    foreach(QString card_name, lock_card){
        if(card->inherits(card_name.toStdString().c_str()))
            return true;
    }

    return false;
}

bool Player::hasCardLock(const QString &card_str) const{
    return lock_card.contains(card_str);
}

StatisticsStruct *Player::getStatistics() const{
    return player_statistics;
}

void Player::setStatistics(StatisticsStruct *statistics){
    player_statistics = statistics;
}

void Player::copyFrom(Player* p)
{
    Player *b = this;
    Player *a = p;

    b->marks            = QMap<QString, int> (a->marks);
    b->piles            = QMap<QString, QList<int> > (a->piles);
    b->acquired_skills  = QSet<QString> (a->acquired_skills);
    b->flags            = QSet<QString> (a->flags);
    b->history          = QHash<QString, int> (a->history);

    b->hp               = a->hp;
    b->max_hp           = a->max_hp;
    b->kingdom          = a->kingdom;
    b->role             = a->role;
    b->seat             = a->seat;
    b->alive            = a->alive;

    b->phase            = a->phase;
    b->weapon           = a->weapon;
    b->armor            = a->armor;
    b->defensive_horse  = a->defensive_horse;
    b->offensive_horse  = a->offensive_horse;
    b->face_up          = a->face_up;
    b->chained          = a->chained;
    b->judging_area     = QList<const Card *> (a->judging_area);
    b->delayed_tricks   = QList<const DelayedTrick *> (a->delayed_tricks);
    b->fixed_distance   = QHash<const Player *, int> (a->fixed_distance);
    b->jilei_set        = QSet<QString> (a->jilei_set);

    b->tag              = QVariantMap(a->tag);
}

QList<const Player *> Player::getSiblings() const{
    QList<const Player *> siblings;
    if(parent()){
        siblings = parent()->findChildren<const Player *>();
        siblings.removeOne(this);
    }

    return siblings;
}

const Player *Player::findPlayer(const QString &objectname) const{
    if(parent()){
        QList<const Player *> siblings = parent()->findChildren<const Player *>();
        foreach(const Player *player, siblings){
            if(player->objectName() == objectname || player->getGeneralName() == objectname)
                return player;
        }
    }
    return NULL;
}

void Player::playAudio(const QString &name) const{
    Sanguosha->playAudio(name);
}
