#include "gamerule.h"
#include "serverplayer.h"
#include "room.h"
#include "standard.h"
#include "engine.h"
#include "settings.h"

#include <QTime>

GameRule::GameRule(QObject *)
    :TriggerSkill("game_rule")
{
    //@todo: this setParent is illegitimate in QT and is equivalent to calling
    // setParent(NULL). So taking it off at the moment until we figure out
    // a way to do it.
    //setParent(parent);

    events << GameStart << TurnStart << PhaseChange << CardUsed
            << CardAsk << CardUseAsk << CardFinished
            << CardEffected << HpRecover << HpLost << AskForPeachesDone
            << AskForPeaches << Death << Dying << GameOverJudge
            << PreDeath << RewardAndPunish << TurnedOver << ChainStateChange
            << SlashHit << SlashMissed << SlashEffected << SlashProceed
            << DamageDone << DamageComplete
            << StartJudge << FinishJudge << Pindian;
}

bool GameRule::triggerable(const ServerPlayer *) const{
    return true;
}

int GameRule::getPriority(TriggerEvent) const{
    return 0;
}

void GameRule::onPhaseChange(ServerPlayer *player) const{
    Room *room = player->getRoom();
    switch(player->getPhase()){
    case Player::RoundStart:{
            break;
        }
    case Player::Start: {
            player->setMark("SlashCount", 0);
            if(player->hasMark("@shang"))
                room->loseHp(player, player->getMark("@shang"));
            break;
        }
    case Player::Judge: {
            QList<const DelayedTrick *> tricks = player->delayedTricks();
            while(!tricks.isEmpty() && player->isAlive()){
                const DelayedTrick *trick = tricks.takeLast();
                bool on_effect = room->cardEffect(trick, NULL, player);
                if(!on_effect)
                    trick->onNullified(player);
            }

            break;
        }
    case Player::Draw: {
            QVariant num = 2;
            if(room->getTag("FirstRound").toBool()){
                room->setTag("FirstRound", false);
                if(room->getMode() == "02_1v1")
                    num = 1;
            }

            room->getThread()->trigger(DrawNCards, room, player, num);
            int n = num.toInt();
            if(n > 0)
                player->drawCards(n, false);
            room->getThread()->trigger(DrawNCardsDone, room, player, num);
            break;
        }

    case Player::Play: {
            player->clearHistory();

            while(player->isAlive()){
                CardUseStruct card_use;
                room->activate(player, card_use);
                if(card_use.isValid()){
                    room->useCard(card_use);
                }else
                    break;
            }
            break;
        }

    case Player::Discard:{
            int discard_num = player->getHandcardNum() - player->getMaxCards();
            if(player->hasFlag("jilei")){
                QSet<const Card *> jilei_cards;
                QList<const Card *> handcards = player->getHandcards();
                foreach(const Card *card, handcards){
                    if(player->isJilei(card))
                        jilei_cards << card;
                }

                if(jilei_cards.size() > player->getMaxCards()){
                    // show all his cards
                    room->showAllCards(player);

                    DummyCard *dummy_card = new DummyCard;
                    foreach(const Card *card, handcards.toSet() - jilei_cards){
                        dummy_card->addSubcard(card);
                    }
                    room->throwCard(dummy_card, player);

                    return;
                }
            }

            if(discard_num > 0)
                room->askForDiscard(player, "gamerule", discard_num);
            break;
        }
    case Player::Finish: {
            break;
        }

    case Player::NotActive:{
            if(player->hasFlag("drank")){
                LogMessage log;
                log.type = "#UnsetDrankEndOfTurn";
                log.from = player;
                room->sendLog(log);

                room->setPlayerFlag(player, "-drank");
            }
            foreach(ServerPlayer *tmp, room->getAllPlayers()){
                if(tmp->hasFlag("ecst")){
                    LogMessage log;
                    log.type = "#UnsetEcstEndOfTurn";
                    log.from = player;
                    log.to << tmp;
                    room->sendLog(log);
                    room->setPlayerFlag(tmp, "-ecst");
                }
                foreach(QString clear_flag, tmp->getClearFlags())
                    room->setPlayerFlag(tmp, "-" + clear_flag);
                // % mean clear this flag after each turn
            }

            player->clearFlags();
            player->clearHistory();

            return;
        }
    }
}

void GameRule::setGameProcess(Room *room) const{
    int good = 0, bad = 0;
    QList<ServerPlayer *> players = room->getAlivePlayers();
    foreach(ServerPlayer *player, players){
        switch(player->getRoleEnum()){
        case Player::Lord:
        case Player::Loyalist: good ++; break;
        case Player::Rebel: bad++; break;
        case Player::Renegade: break;
        }
    }

    QString process;
    if(good == bad)
        process = "Balance";
    else if(good > bad)
        process = "LordSuperior";
    else
        process = "RebelSuperior";

    room->setTag("GameProcess", process);
}

bool GameRule::trigger(TriggerEvent event, Room* room, ServerPlayer *player, QVariant &data) const{
    if(room->getTag("SkipGameRule").toBool()){
        room->removeTag("SkipGameRule");
        return false;
    }

    switch(event){
    case GameStart: {
        bool sun_moon = player->getGeneral()->getKingdom() == "sun" || player->getGeneral()->getKingdom() == "moon";
        if((player->getGeneral()->getKingdom() == "god" && player->getGeneralName() != "anjiang") || sun_moon){
            QString new_kingdom = room->askForKingdom(player, !sun_moon);
            room->setPlayerProperty(player, "kingdom", new_kingdom);

            LogMessage log;
            log.type = "#ChooseKingdom";
            log.from = player;
            log.arg = new_kingdom;
            room->sendLog(log);
        }

        if(player->isLord() && Config.EnableAnzhan && !room->getTag("AnzhanInit").toBool()){
            LogMessage log;
            log.type = "#AnzhanShuffle";
            log.from = player;
            log.arg = "anzhan";
            room->sendLog(log);

            QStringList roles;
            foreach(ServerPlayer *plr, room->getAllPlayers()){
                roles << plr->getRole();
            }
            qShuffle(roles);
            foreach(ServerPlayer *plr, room->getAllPlayers()){
                plr->setRole(roles.first());
                roles.removeFirst();
            }
            if(player->isLord()){
                player->setRole(player->getNextAlive()->getRole());
                player->getNextAlive()->setRole("lord");
            }
            room->updateStateItem();
            room->broadcastProperty(player, "role");
            log.type = "#AnzhanShow";
            if(room->getPlayerCount() > 4 && !Config.value("AnzhanEqual", false).toBool()){
                room->setPlayerProperty(player, "maxhp", player->getMaxHp() + 1);
                log.type = "#AnzhanShow2";
            }
            log.arg = player->getRole();
            room->sendLog(log);

            room->setTag("AnzhanInit", true);
        }

        if(player->isLord())
            setGameProcess(room);

        room->setTag("FirstRound", true);
        player->drawCards(4, false);
        if(Config.value("Cheat/GamblingCards", false).toBool()){
            if(player->getState() == "online" && player->askForSkillInvoke("gambling")){
                player->throwAllHandCards();
                player->drawCards(4, false);
            }
        }

        break;
    }

    case TurnStart:{
        player = room->getCurrent();
        if(Config.value("Cheat/FreeShowRole", false).toBool()){
            LogMessage log;
            log.type = "#ShowRole";
            log.from = player;
            log.arg = player->getRole();
            room->sendLog(log);
        }
        if(!player->faceUp())
            player->turnOver();
        else if(player->isAlive())
            player->play();

        break;
    }

    case PhaseChange: onPhaseChange(player); break;
    case CardUsed: {
        if(data.canConvert<CardUseStruct>()){
            CardUseStruct card_use = data.value<CardUseStruct>();
            const Card *card = card_use.card;

            if(!card->getSkillName().isEmpty() && Config.EnableSkillEmotion)
                room->setEmotion(player, "skill/" + card->getSkillName());
            room->playExtra(CardUsed, data);
            card->use(room, card_use.from, card_use.to);
        }

        break;
    }
    case CardAsk :
    case CardUseAsk: {
        if(player->hasFlag("ecst") && (data.toString() == "slash" || data.toString() == "jink")){
            LogMessage log;
            log.type = "#EcstasyEffect";
            log.from = player;
            log.arg = data.toString();
            room->sendLog(log);
            return true;
        }
        break;
    }
    case CardFinished: {
        CardUseStruct use = data.value<CardUseStruct>();

        room->clearCardFlag(use.card);
        break;
    }

    case HpRecover:{
        RecoverStruct recover_struct = data.value<RecoverStruct>();
        int recover = recover_struct.recover;

        room->setPlayerStatistics(player, "recover", recover);

        int new_hp = qMin(player->getHp() + recover, player->getMaxHP());
        room->setPlayerProperty(player, "hp", new_hp);
        room->broadcastInvoke("hpChange", QString("%1:%2").arg(player->objectName()).arg(recover));

        break;
    }

    case HpLost:{
        int lose = data.toInt();

        LogMessage log;
        log.type = "#LoseHp";
        log.from = player;
        log.arg = QString::number(lose);
        room->sendLog(log);

        room->setPlayerProperty(player, "hp", player->getHp() - lose);
        QString str = QString("%1:%2L").arg(player->objectName()).arg(-lose);
        room->broadcastInvoke("hpChange", str);

        if(player->getHp() <= 0)
            room->enterDying(player, NULL);

        if(Config.EnableEndless)
            player->gainMark("@endless", lose);

        break;
    }

    case Dying:{
        if(player->getHp() > 0){
            player->setFlags("-dying");
            break;
        }

        DyingStruct dying = data.value<DyingStruct>();

        if(room->getAllPlayers().count() != room->getAllPlayers(true).count()){
            QList<ServerPlayer *> sources = room->findPlayersWhohasCard("edo_tensei");
            foreach(ServerPlayer *source, sources){
                //ai: hasflag("dying")
                if(room->askForUseCard(source, "EdoTensei", "@edo:" + player->objectName())){
                    room->killPlayer(player);
                    return true;
                }
            }
        }

        LogMessage log;
        log.type = "#AskForPeaches";
        log.from = player;
        log.to = dying.savers;
        log.arg = QString::number(1 - player->getHp());
        room->sendLog(log);

        RoomThread *thread = room->getThread();
        foreach(ServerPlayer *saver, dying.savers){
            if(player->getHp() > 0)
                break;

            thread->trigger(AskForPeaches, room, saver, data);
        }

        player->setFlags("-dying");
        thread->trigger(AskForPeachesDone, room, player, data);

        break;
    }

    case AskForPeaches:{
        DyingStruct dying = data.value<DyingStruct>();

        if(player->hasFlag("%zhaoan"))
            return true;
        while(dying.who->getHp() <= 0){
            if(dying.who->isDead())
                break;
            const Card *peach = room->askForSinglePeach(player, dying.who);
            if(!peach)
                break;

            CardUseStruct use;
            use.card = peach;
            use.from = player;
            if(player != dying.who)
                use.to << dying.who;

            room->useCard(use, false);

            if(player != dying.who && dying.who->getHp() > 0)
                room->setPlayerStatistics(player, "save", 1);
        }

        break;
    }

    case AskForPeachesDone:{
        if(player->getHp() <= 0 && player->isAlive()){
            DyingStruct dying = data.value<DyingStruct>();
            room->killPlayer(player, dying.damage);
        }

        break;
    }

    case DamageDone:{
        DamageStruct damage = data.value<DamageStruct>();
        room->sendDamageLog(damage);

        if(damage.to->hasFlag("ecst")){
            LogMessage log;
            log.type = "#UnsetEcst";
            log.from = damage.to;
            room->sendLog(log);

            room->setPlayerFlag(damage.to, "-ecst");
        }
        if(damage.from)
            room->setPlayerStatistics(damage.from, "damage", damage.damage);

        room->applyDamage(player, damage);
        if(player->getHp() <= 0)
            room->enterDying(player, &damage);

        break;
    }

    case DamageComplete:{
        if(room->getMode() == "02_1v1" && player->isDead()){
            QString new_general = player->tag["1v1ChangeGeneral"].toString();
            if(!new_general.isEmpty())
                changeGeneral1v1(player);
        }
        DamageStruct damage = data.value<DamageStruct>();

        if(Config.EnableEndless){
            if(damage.from)
                damage.from->gainMark("@endless", damage.damage);
            else
                damage.to->gainMark("@endless", damage.damage);
        }

        bool chained = player->isChained();
        if(!chained)
            break;

        if(damage.nature != DamageStruct::Normal){
            room->setPlayerProperty(player, "chained", false);

            // iron chain effect
            QList<ServerPlayer *> chained_players = room->getOtherPlayers(player);
            foreach(ServerPlayer *chained_player, chained_players){
                if(chained_player->isChained()){
                    room->getThread()->delay();
                    room->setPlayerProperty(chained_player, "chained", false);

                    LogMessage log;
                    log.type = "#IronChainDamage";
                    log.from = chained_player;
                    room->sendLog(log);

                    DamageStruct chain_damage = damage;
                    chain_damage.to = chained_player;
                    chain_damage.chain = true;

                    room->damage(chain_damage);
                }
            }
        }

        //if(player->getHp() <= 0 && player->isAlive())
        //    room->enterDying(player, &damage);
        break;
    }

    case CardEffected:{
        if(data.canConvert<CardEffectStruct>()){
            CardEffectStruct effect = data.value<CardEffectStruct>();
            if(room->isCanceled(effect))
                return true;

            effect.card->onEffect(effect);
        }

        break;
    }

    case SlashEffected:{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();

        QVariant data = QVariant::fromValue(effect);
        room->getThread()->trigger(SlashProceed, room, effect.from, data);

        break;
    }

    case SlashProceed:{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();

        QString slasher = effect.from->objectName();
        const Card *jink = room->askForCard(effect.to, "jink", "slash-jink:" + slasher, data, JinkUsed);
        room->slashResult(effect, jink);

        break;
    }

    case SlashHit:{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();

        DamageStruct damage;
        damage.card = effect.slash;

        damage.damage = 1;
        if(effect.drank){
            LogMessage log;
            log.type = "#AnalepticBuff";
            log.from = effect.from;
            log.to << effect.to;
            log.arg = "analeptic";
            room->sendLog(log);

            damage.damage ++;
        }
        if(effect.from->hasFlag("drunken")){
            LogMessage log;
            log.type = "#DrunkenBuff";
            log.from = effect.from;
            log.to << effect.to;
            log.arg = "jiangjieshi";
            room->sendLog(log);

            damage.damage ++;
            room->setPlayerFlag(player, "-drunken");
        }

        damage.from = effect.from;
        damage.to = effect.to;
        damage.nature = effect.nature;

        room->damage(damage);

        effect.to->removeMark("qinggang");
        break;
    }

    case SlashMissed:{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();
        effect.to->removeMark("qinggang");

        break;
    }

    case GameOverJudge:{
        if(room->getMode() == "02_1v1"){
            QStringList list = player->tag["1v1Arrange"].toStringList();

            if(!list.isEmpty())
                return false;
        }

        QString winner = getWinner(player);
        if(!winner.isNull()){
            room->gameOver(winner);
            return true;
        }

        break;
    }

    case PreDeath:{
        DamageStar damage = data.value<DamageStar>();
        ServerPlayer *killer = damage ? damage->from : NULL;

        if(Config.EnableEndless){
            if(player->getMaxHp() <= 0)
                room->setPlayerProperty(player, "maxhp", player->getGeneral()->getMaxHp());
            if(player->getHp() <= 0)
                room->setPlayerProperty(player, "hp", 1);
            if(player->hasMark("@endless")){
                if(killer)
                    killer->gainMark("@endless", player->getMark("@endless") / 2);
                player->loseMark("@endless", player->getMark("@endless") / 2);
            }
            return true;
        }

        if(player->getState() == "online"){
            if(Config.value("Cheat/FreeUnDead", false).toBool()){
                if(player->getMaxHp() <= 0)
                    room->setPlayerProperty(player, "maxhp", player->getGeneral()->getMaxHp());
                if(player->getHp() <= 0)
                    room->setPlayerProperty(player, "hp", 1);
                LogMessage log;
                log.type = "#Undead";
                log.from = player;
                room->sendLog(log);
                return true;
            }
            if(Config.value("Cheat/HandsUp", false).toBool() && room->getMode() != "02_1v1"){
                bool allrobot = true;
                QStringList winners;
                foreach(ServerPlayer *robot, room->getOtherPlayers(player)){
                    if(robot->getState() != "robot" && allrobot)
                        allrobot = false;
                    if(robot->isAlive())
                        winners << robot->objectName();
                }
                if(allrobot && player->askForSkillInvoke("goaway")){
                    room->gameOver(winners.join("+"));
                    return true;
                }
            }
        }
        break;
    }

    case Death:{
        player->bury();

        if(room->getTag("SkipNormalDeathProcess").toBool())
            return false;

        DamageStar damage = data.value<DamageStar>();
        ServerPlayer *killer = damage ? damage->from : NULL;
        if(killer)
            room->getThread()->trigger(RewardAndPunish, room, player, data);
        else if(player->hasSkill("zuohua"))
            room->playSkillEffect("zuohua", 2);

        setGameProcess(room);

        if(room->getMode() == "02_1v1"){
            QStringList list = player->tag["1v1Arrange"].toStringList();

            if(!list.isEmpty()){
                player->tag["1v1ChangeGeneral"] = list.takeFirst();
                player->tag["1v1Arrange"] = list;

                DamageStar damage = data.value<DamageStar>();

                if(damage == NULL){
                    changeGeneral1v1(player);
                    return false;
                }
            }
        }
        break;
    }
    case RewardAndPunish:{
        if(data.canConvert<DamageStar>()){
            DamageStar damage = data.value<DamageStar>();
            PlayerStar killer = damage->from;
            PlayerStar victim = player;

            if(killer->isDead())
                return true;

            if(room->getMode() == "06_3v3")
                killer->drawCards(3);
            else{
                if(victim->getRole() == "rebel" && killer != victim){
                    killer->drawCards(3);
                }else if(victim->getRole() == "loyalist" && killer->getRole() == "lord"){
                    killer->throwAllEquips();
                    killer->throwAllHandCards();
                }
            }
        }
        break;
    }

    case TurnedOver:{
        player->setFaceUp(!player->faceUp());
        room->broadcastProperty(player, "faceup");

        LogMessage log;
        log.type = "#TurnOver";
        log.from = player;
        log.arg = player->faceUp() ? "face_up" : "face_down";
        room->sendLog(log);
        break;
    }

    case ChainStateChange:{
        bool chained = !player->isChained();
        player->setChained(chained);
        room->broadcastProperty(player, "chained");
        room->setEmotion(player, "chain");
        break;
    }

    case StartJudge:{
        int card_id = room->drawCard();

        JudgeStar judge = data.value<JudgeStar>();
        judge->card = Sanguosha->getCard(card_id);
        room->moveCardTo(judge->card, NULL, Player::Special);

        LogMessage log;
        log.type = "$InitialJudge";
        log.from = player;
        log.card_str = judge->card->getEffectIdString();
        room->sendLog(log);

        room->sendJudgeResult(judge);

        int delay = Config.AIDelay;
        if(judge->time_consuming)
            delay /= 4;
        room->getThread()->delay(delay);

        break;
    }

    case FinishJudge:{
        JudgeStar judge = data.value<JudgeStar>();
        room->throwCard(judge->card);

        LogMessage log;
        log.type = "$JudgeResult";
        log.from = player;
        log.card_str = judge->card->getEffectIdString();
        room->sendLog(log);

        room->sendJudgeResult(judge, true);
        room->broadcastInvoke("playAudio", judge->isBad() ? "adversity" : "lucky");

        room->getThread()->delay();
        break;
    }

    case Pindian:{
        PindianStar pindian = data.value<PindianStar>();

        room->setEmotion(pindian->from, "pindian");
        room->setEmotion(pindian->to, "pindian");
        room->broadcastInvoke("playAudio", "pindian");
        LogMessage log;

        room->moveCardTo(pindian->from_card, NULL, Player::DiscardedPile);
        log.type = "$PindianResult";
        log.from = pindian->from;
        log.card_str = pindian->from_card->getEffectIdString();
        room->sendLog(log);
        room->getThread()->delay();

        room->moveCardTo(pindian->to_card, NULL, Player::DiscardedPile);
        log.type = "$PindianResult";
        log.from = pindian->to;
        log.card_str = pindian->to_card->getEffectIdString();
        room->sendLog(log);
        room->getThread()->delay();
    }

    default:
        break;
    }

    return false;
}

void GameRule::changeGeneral1v1(ServerPlayer *player) const{
    Room *room = player->getRoom();
    QString new_general = player->tag["1v1ChangeGeneral"].toString();
    player->tag.remove("1v1ChangeGeneral");
    room->transfigure(player, new_general, true, true);
    room->revivePlayer(player);

    if(player->getKingdom() != player->getGeneral()->getKingdom())
        room->setPlayerProperty(player, "kingdom", player->getGeneral()->getKingdom());

    room->broadcastInvoke("revealGeneral",
                          QString("%1:%2").arg(player->objectName()).arg(new_general),
                          player);

    if(!player->faceUp())
        player->turnOver();

    if(player->isChained())
        room->setPlayerProperty(player, "chained", false);

    player->drawCards(4);
}

QString GameRule::getWinner(ServerPlayer *victim) const{
    Room *room = victim->getRoom();
    QString winner;

    if(room->getMode() == "06_3v3"){
        switch(victim->getRoleEnum()){
        case Player::Lord: winner = "renegade+rebel"; break;
        case Player::Renegade: winner = "lord+loyalist"; break;
        default:
            break;
        }
    }else if(Config.EnableHegemony){
        bool has_anjiang = false, has_diff_kingdoms = false;
        QString init_kingdom;
        foreach(ServerPlayer *p, room->getAlivePlayers()){
            if(room->getTag(p->objectName()).toStringList().size()){
                has_anjiang = true;
            }

            if(init_kingdom.isEmpty()){
                init_kingdom = p->getKingdom();
            }
            else if(init_kingdom != p->getKingdom()){
                has_diff_kingdoms = true;
            }
        }

        if(!has_anjiang && !has_diff_kingdoms){
            QStringList winners;
            QString aliveKingdom = room->getAlivePlayers().first()->getKingdom();
            foreach(ServerPlayer *p, room->getPlayers()){
                if(p->isAlive())winners << p->objectName();
                if(p->getKingdom() == aliveKingdom)
                {
                    QStringList generals = room->getTag(p->objectName()).toStringList();
                    if(generals.size()&&!Config.Enable2ndGeneral)continue;
                    if(generals.size()>1)continue;

                    //if someone showed his kingdom before death,
                    //he should be considered victorious as well if his kingdom survives
                    winners << p->objectName();
                }
            }

            winner = winners.join("+");
        }
    }else{
        QStringList alive_roles = room->aliveRoles(victim);
        switch(victim->getRoleEnum()){
        case Player::Lord:{
                if(alive_roles.length() == 1 && alive_roles.first() == "renegade")
                    winner = room->getAlivePlayers().first()->objectName();
                else
                    winner = "rebel";
                break;
            }

        case Player::Rebel:
        case Player::Renegade:
            {
                if(!alive_roles.contains("rebel") && !alive_roles.contains("renegade")){
                    winner = "lord+loyalist";
                    if(victim->getRole() == "renegade" && !alive_roles.contains("loyalist"))
                        room->setTag("RenegadeInFinalPK", true);
                }
                break;
            }

        default:
            break;
        }
    }

    return winner;
}

BasaraMode::BasaraMode(QObject *parent)
    :GameRule(parent)
{
    setObjectName("basara_mode");

    events << CardLost << Predamaged;
    skill_mark["shenchou"] = "@chou";
}

QString BasaraMode::getMappedRole(const QString &role){
    static QMap<QString, QString> roles;
    if(roles.isEmpty()){
        roles["guan"] = "lord";
        roles["jiang"] = "loyalist";
        roles["min"] = "rebel";
        roles["kou"] = "renegade";
    }
    return roles[role];
}

int BasaraMode::getPriority(TriggerEvent) const{
    return 5;
}

void BasaraMode::playerShowed(ServerPlayer *player) const{
    Room *room = player->getRoom();
    QStringList names = room->getTag(player->objectName()).toStringList();
    if(names.isEmpty())
        return;

    if(Config.EnableHegemony){
        QMap<QString, int> kingdom_roles;
        foreach(ServerPlayer *p, room->getOtherPlayers(player)){
            kingdom_roles[p->getKingdom()]++;
        }

        if(kingdom_roles[Sanguosha->getGeneral(names.first())->getKingdom()] >= 2
                && player->getGeneralName() == "anjiang")
            return;
    }

    QString answer = room->askForChoice(player, "RevealGeneral", "yes+no");
    if(answer == "yes"){

        QString general_name = room->askForGeneral(player,names);

        generalShowed(player,general_name);
        if (Config.EnableHegemony) room->getThread()->trigger(GameOverJudge, room, player);
        playerShowed(player);
    }
}

void BasaraMode::generalShowed(ServerPlayer *player, QString general_name) const
{
    Room * room = player->getRoom();
    QStringList names = room->getTag(player->objectName()).toStringList();
    if(names.isEmpty())return;

    if(player->getGeneralName() == "anjiang")
    {
        QString transfigure_str = QString("%1:%2").arg(player->getGeneralName()).arg(general_name);
        player->invoke("transfigure", transfigure_str);
        room->setPlayerProperty(player,"general",general_name);

        foreach(QString skill_name, skill_mark.keys()){
            if(player->hasSkill(skill_name))
                room->setPlayerMark(player, skill_mark[skill_name], 1);
        }
    }
    else{
        QString transfigure_str = QString("%1:%2").arg(player->getGeneral2Name()).arg(general_name);
        player->invoke("transfigure", transfigure_str);
        room->setPlayerProperty(player,"general2",general_name);
    }

    room->setPlayerProperty(player, "kingdom", player->getGeneral()->getKingdom());
    if(Config.EnableHegemony)room->setPlayerProperty(player, "role", getMappedRole(player->getGeneral()->getKingdom()));

    names.removeOne(general_name);
    room->setTag(player->objectName(),QVariant::fromValue(names));

    LogMessage log;
    log.type = "#BasaraReveal";
    log.from = player;
    log.arg  = player->getGeneralName();
    log.arg2 = player->getGeneral2Name();

    room->sendLog(log);
    room->broadcastInvoke("playAudio", "choose-item");
}

bool BasaraMode::trigger(TriggerEvent event, Room* room, ServerPlayer *player, QVariant &data) const{
    player->tag["event"] = event;
    player->tag["event_data"] = data;

    switch(event){
    case GameStart:{
        if(player->isLord()){
            if(Config.EnableHegemony)
                room->setTag("SkipNormalDeathProcess", true);

            foreach(ServerPlayer* sp, room->getAlivePlayers())
            {
                QString transfigure_str = QString("%1:%2").arg(sp->getGeneralName()).arg("anjiang");
                sp->invoke("transfigure", transfigure_str);
                room->setPlayerProperty(sp,"general","anjiang");
                room->setPlayerProperty(sp,"kingdom","god");

                LogMessage log;
                log.type = "#BasaraGeneralChosen";
                log.arg = room->getTag(sp->objectName()).toStringList().at(0);

                if(Config.Enable2ndGeneral)
                {

                    transfigure_str = QString("%1:%2").arg(sp->getGeneral2Name()).arg("anjiang");
                    sp->invoke("transfigure", transfigure_str);
                    room->setPlayerProperty(sp,"general2","anjiang");

                    log.arg2 = room->getTag(sp->objectName()).toStringList().at(1);
                }

                sp->invoke("log",log.toString());
                sp->tag["roles"] = room->getTag(sp->objectName()).toStringList().join("+");
            }
        }

        break;
    }
    case CardEffected:{
        if(player->getPhase() == Player::NotActive){
            CardEffectStruct ces = data.value<CardEffectStruct>();
            if(ces.card)
                if(ces.card->inherits("TrickCard") ||
                        ces.card->inherits("Slash"))
                playerShowed(player);

            const ClientSkill *prohibit = room->isProhibited(ces.from, ces.to, ces.card);
            if(prohibit){
                LogMessage log;
                log.type = "#SkillAvoid";
                log.from = ces.to;
                log.arg  = prohibit->objectName();
                log.arg2 = ces.card->objectName();

                room->sendLog(log);

                return true;
            }
        }
        break;
    }

    case PhaseChange:{
        if(player->getPhase() == Player::RoundStart)
            playerShowed(player);

        break;
    }
    case Predamaged:{
        playerShowed(player);
        break;
    }
    case GameOverJudge:{
        if(Config.EnableHegemony){
            if(player->getGeneralName() == "anjiang"){
                QStringList generals = room->getTag(player->objectName()).toStringList();
                room->setPlayerProperty(player, "general", generals.at(0));
                if(Config.Enable2ndGeneral)room->setPlayerProperty(player, "general2", generals.at(1));
                room->setPlayerProperty(player, "kingdom", player->getGeneral()->getKingdom());
                room->setPlayerProperty(player, "role", getMappedRole(player->getKingdom()));
            }
        }
        break;
    }

    case Death:{
        if(Config.EnableHegemony){
            DamageStar damage = data.value<DamageStar>();
            ServerPlayer *killer = damage ? damage->from : NULL;
            if(killer && killer->getKingdom() == damage->to->getKingdom()){
                killer->throwAllEquips();
                killer->throwAllHandCards();
            }
            else if(killer && killer->isAlive()){
                killer->drawCards(3);
            }
        }

        break;
    }

    default:
        break;
    }

    return false;
}

EventsRule::EventsRule(QObject *parent)
    :GameRule(parent)
{
    setObjectName("events_rule");
    events << Predamaged << Damaged << DamageProceed << AskForRetrial;
}

int EventsRule::getPriority(TriggerEvent event) const{
    if(event == DamageComplete)
        return 2;
    else
        return -1;
}

bool EventsRule::trigger(TriggerEvent event, Room* room, ServerPlayer *player, QVariant &data) const{
    switch(event){
    case Predamaged:{
        DamageStruct damage = data.value<DamageStruct>();
        if(damage.damage > 1){
            ServerPlayer *source = room->findPlayerWhohasCard("ninedaygirl");
            if(source == damage.to){
                room->setPlayerFlag(damage.to, "NineGirl");
                QString prompt = QString("@ninedaygirl:::%1").arg(damage.damage);
                bool girl = room->askForUseCard(damage.to, "NinedayGirl", prompt);
                room->setPlayerFlag(damage.to, "-NineGirl");
                if(girl){
                    LogMessage log;
                    log.from = damage.to;
                    log.type = "#NineGirl";
                    log.arg = QString::number(damage.damage);
                    room->sendLog(log);
                    return true;
                }
            }
        }
        break;
    }
    case DamageProceed:{
        DamageStruct damage = data.value<DamageStruct>();
        if(!damage.from || !damage.to || player->isKongcheng())
            break;
        ServerPlayer *source = room->findPlayerWhohasCard("xiaobawang");
        if(source && source != player && player->getGender() != damage.to->getGender()){
            QString prompt = QString("@xiaobawang1:%1:%2").arg(damage.from->objectName()).arg(damage.to->objectName());
            source->tag["Xiaob"] = QVariant::fromValue((PlayerStar)damage.from);
            room->askForUseCard(source, "Xiaobawang", prompt);
            source->tag.remove("Xiaob");
        }
        break;
    }
    case Damaged:{
        DamageStruct damage = data.value<DamageStruct>();
        ServerPlayer *source = room->findPlayerWhohasCard("xiaobawang");
        if(damage.from && damage.from == source){
            if(damage.to && !damage.to->isNude() &&
               damage.from->getGender() == damage.to->getGender()){
                const Card *e = room->askForCard(source, "Xiaobawang", "@xiaobawang2:" + damage.to->objectName(), data, CardDiscarded);
                if(e){
                    source->playCardEffect("@xiaobawang2");
                    int card_id = room->askForCardChosen(source, damage.to, "he", "xiaobawang");
                    room->throwCard(card_id, damage.to, source);
                }
            }
            else{
                QString prompt = QString("@xiaobawang3:::%1").arg(damage.damage);
                const Card *e = room->askForCard(source, "Xiaobawang", prompt, data, CardDiscarded);
                if(e){
                    source->playCardEffect("@xiaobawang2");
                    source->drawCards(damage.damage);
                }
            }
        }
        break;
    }
    case DamageComplete:{
        DamageStruct damage = data.value<DamageStruct>();
        ServerPlayer *source = room->findPlayerWhohasCard("nanastars");
        if(damage.from && damage.from != player && source == player && !damage.from->isNude()){
            if(room->askForCard(source, "NanaStars", "@7stars:" + damage.from->objectName(), data, CardDiscarded)){
                int x = qMax(qAbs(source->getHp() - damage.from->getHp()), 1);
                source->playCardEffect("@nanastars2");

                LogMessage log;
                log.type = "#NanaStars";
                log.from = source;
                log.to << damage.from;
                log.arg = "nanastars";
                room->sendLog(log);
                while(!damage.from->isNude()){
                    int card_id = room->askForCardChosen(source, damage.from, "he", "nanastars");
                    room->obtainCard(source, card_id, room->getCardPlace(card_id) != Player::Hand);
                    x --;
                    if(x == 0)
                        break;
                }
            }
        }
        break;
    }
    case CardFinished:{
        if(data.canConvert<CardUseStruct>()){
            CardUseStruct use = data.value<CardUseStruct>();
            if(use.card->isKindOf("Snatch")){
                ServerPlayer *source = room->findPlayerWhohasCard("daojia");
                if(source){
                    room->setPlayerFlag(source, "Daojia");
                    room->askForUseCard(source, "Daojia", "@daojia");
                    room->setPlayerFlag(source, "-Daojia");
                }
            }
            if(use.card->inherits("Analeptic")){
                ServerPlayer *source = room->findPlayerWhohasCard("tifanshi");
                if(source && source == use.from){
                    room->setPlayerFlag(source, "Tifanshi");
                    room->askForUseCard(source, "Tifanshi", "@tifanshi");
                    room->setPlayerFlag(source, "-Tifanshi");
                }
            }
            if(use.card->inherits("Ecstasy")){
                ServerPlayer *source = room->findPlayerWhohasCard("nanastars");
                if(source){
                    bool invoke = false;
                    foreach(ServerPlayer *tmp, room->getAlivePlayers()){
                        if(tmp->containsTrick("treasury", false)){
                            invoke = true;
                            break;
                        }
                    }
                    if(invoke)
                        room->askForUseCard(source, "NanaStars", "@nanastars");
                }
            }
        }
        break;
    }
    case PhaseChange:{
        if(player->getPhase() == Player::NotActive){
            ServerPlayer *source = room->findPlayerWhohasCard("jiefachang");
            if(source && player == source){
                bool face = false;
                foreach(ServerPlayer *tmp, room->getAlivePlayers())
                    if(!tmp->faceUp()){
                        face = true;
                        break;
                    }
                if(face)
                    room->askForUseCard(player, "Jiefachang", "@jiefachang");
            }
        }
        break;
    }
    case AskForRetrial:{
        ServerPlayer *source = room->findPlayerWhohasCard("fuckgaolian");
        if(source && source == player){
            room->setPlayerFlag(player, "FuckLian");
            const Card *fuck = room->askForCard(player, "FuckGaolian", "@fuckl", data);
            if(fuck){
                player->playCardEffect("@fuckgaolian2");
                JudgeStar judge = data.value<JudgeStar>();
                source->obtainCard(judge->card);
                judge->card = fuck;
                room->moveCardTo(judge->card, NULL, Player::Special);
                LogMessage log;
                log.type = "$ChangedJudge";
                log.from = player;
                log.to << judge->who;
                log.card_str = QString::number(fuck->getId());
                room->sendLog(log);
                room->sendJudgeResult(player);
            }
            room->setPlayerFlag(player, "-FuckLian");
        }
        break;
    }
    case FinishJudge: {
        JudgeStar judge = data.value<JudgeStar>();
        if(judge->card->getSuit() == Card::Spade){
            ServerPlayer *source = room->findPlayerWhohasCard("fuckgaolian");
            if(source){
                room->setPlayerFlag(source, "FuckGao");
                room->askForUseCard(source, "FuckGaolian", "@fuckg");
                room->setPlayerFlag(source, "-FuckGao");
            }
        }
        if(judge->card->inherits("Analeptic") && room->getCardPlace(judge->card->getEffectiveId()) == Player::DiscardedPile){
            ServerPlayer *sour = room->findPlayerWhohasCard("jiangjieshi");
            if(sour && sour != room->getCurrent()){
                const Card *fight = room->askForCard(sour, "Jiangjieshi", "@jiangshi", data, CardDiscarded);
                if(fight){
                    sour->playCardEffect("@jiangjieshi2");
                    LogMessage log;
                    log.type = "#Jiangjs";
                    log.from = sour;
                    log.arg = "jiangjieshi";
                    log.arg2 = judge->card->objectName();
                    room->sendLog(log);
                    sour->obtainCard(judge->card);
                }
            }
        }
        break;
    }

    default:
        break;
    }

    return false;
}

ReincarnationRule::ReincarnationRule(QObject *parent)
    :GameRule(parent)
{
    setObjectName("reincarnation_rule");
}

int ReincarnationRule::getPriority(TriggerEvent event) const{
    if(event == RewardAndPunish)
        return 2;
    else
        return -1;
}

bool ReincarnationRule::trigger(TriggerEvent event, Room* room, ServerPlayer *player, QVariant &data) const{
    switch(event){
    case GameStart:{
        if(room->getMode() != "wheel_fight"){
            int count = Sanguosha->getPlayerCount(room->getMode());
            if(count > 3)
                room->attachSkillToPlayer(player, "sacrifice");
        }
        break;
    }
    case RewardAndPunish:{
        if(player->property("isDead").toBool())
            return true;
        break;
    }
    case PhaseChange:{
        if(player->getPhase() == Player::NotActive){
            int count = Sanguosha->getPlayerCount(room->getMode());
            if(count < 4)
                break;
            int max = count > 5 ? 4 : 3;
            ServerPlayer *next = player->getNext();
            while(next->isDead()){
                if(next->getHandcardNum() >= max){
                    LogMessage log;
                    log.type = "#ReincarnRevive";
                    log.from = next;
                    room->sendLog(log);

                    room->broadcastInvoke("playAudio", "mode/reincarnation");
                    room->revivePlayer(next);

                    if(!Config.value("ReincaPersist", false).toBool()){
                        QStringList names;
                        foreach(ServerPlayer *tmp, room->getAllPlayers()){
                            names << tmp->getGeneralName();
                            if(tmp->getGeneral2())
                                names << tmp->getGeneral2Name();
                        }
                        if(!names.isEmpty()){
                            QSet<QString> names_set = names.toSet();
                            QString newname = Sanguosha->getRandomGenerals(1, names_set).first();
                            room->transfigure(next, newname, false, true);
                        }
                    }
                    if(next->getMaxHp() == 0)
                        room->setPlayerProperty(next, "maxhp", 1);
                    room->setPlayerProperty(next, "hp", 1);

                    room->getThread()->delay(1500);
                    room->attachSkillToPlayer(next, "sacrifice");
                    room->setPlayerMark(next, "@skull", 1);
                    room->setPlayerProperty(next, "isDead", true);
                }
                next = next->getNext();
            }
        }
        break;
    }

    default:
        break;
    }

    return false;
}

ConjuringRule::ConjuringRule(QObject *parent)
    :GameRule(parent)
{
    setObjectName("conjuring_rule");
    events << DrawNCards
           << DamagedProceed << Damaged
           << PreConjuring << Conjured;
}

int ConjuringRule::getPriority(TriggerEvent e) const{
    switch(e){
    case DrawNCards:
    case CardUsed:
    case DamageDone:
    case AskForPeaches:
    case PreConjuring:
    case Conjured:
        return 1;
    case DamagedProceed:
        return 2;
    default:
        return -1;
    }
}

bool ConjuringRule::trigger(TriggerEvent event, Room* room, ServerPlayer *player, QVariant &data) const{
    switch(event){
    case PreConjuring:{
        QStringList dataa = data.toString().split("*");
        QString conjur = dataa.first();
        int percent = QString(dataa.last()).toInt();
        if(conjur.startsWith("sleep"))
            percent = 75;
        else if(conjur.startsWith("dizzy"))
            percent = 75;
        data = QString("%1*%2").arg(conjur).arg(percent); //sleep_jur*75
        break;
    }
    case Conjured:{
        QStringList dataa = data.toString().split("*");
        QString conjur = dataa.first();
        int percent = QString(dataa.last()).toInt();
        if(conjur.startsWith("lucky"))
            percent = 75;
        else if(conjur.startsWith("chaos"))
            percent = 75;
        else if(conjur.startsWith("reflex"))
            percent = 25;
        data = QString("%1*%2").arg(conjur).arg(percent); //reflex_jur*25
        break;
    }
    case DrawNCards:{
        if(player->hasJur("lucky_jur"))
            data = data.toInt() + 1;
        break;
    }
    case PhaseChange:{
        if(player->getPhase() == Player::RoundStart){
            if(player->hasJur("poison_jur")){
                LogMessage log;
                log.from = player;
                log.type = "#Poison";
                room->sendLog(log);
                if(player->getMark("poison_jur") <= 2){
                    if(player->getCards("hej").length() > 1 && room->askForChoice(player, "poison_jur", "hp+cd") == "cd"){
                        int index = -1;
                        if(!player->isAllNude()){
                            index = qrand() % player->getCards("hej").length();
                            const Card *card = player->getCards("hej").at(index);
                            room->throwCard(card, player);
                        }

                        if(!player->isAllNude()){
                            index = qrand() % player->getCards("hej").length();
                            const Card *card = player->getCards("hej").at(index);
                            room->throwCard(card, player);
                        }
                    }
                    else
                        room->loseHp(player);
                }
                else if(!player->isAllNude()){
                    int index = qrand() % player->getCards("hej").length();
                    const Card *card = player->getCards("hej").at(index);
                    room->throwCard(card, player);
                }
            }
            if(player->hasJur("sleep_jur"))
                player->setFlags("ShutUp");
        }
        else if(player->getPhase() == Player::NotActive){
            // shengsizhizhan
            foreach(ServerPlayer *tmp, room->getAllPlayers()){
                tmp->loseAllMarks("@death");
                tmp->loseAllMarks("@life");
            }

            QStringList jurs = player->getAllMarkName(3, "_jur");
            foreach(QString jur, jurs)
                player->loseMark(jur);
        }
        break;
    }
    case DamagedProceed:{
        if(player->hasJur("reflex_jur")){
            DamageStruct damage = data.value<DamageStruct>();
            damage.to = damage.from;
            room->damage(damage);
            return true;
        }
        break;
    }
    case DamageDone:{
        //shengsizhizhan
        DamageStruct damage = data.value<DamageStruct>();
        if(damage.to->hasMark("@death") && damage.damage > 0){
            PlayerStar life = damage.to->tag["DtoL"].value<PlayerStar>();
            if(life)
                room->setPlayerMark(life, "mind", damage.damage);
        }
        break;
    }
    case Damaged:{
        //shengsizhizhan
        foreach(ServerPlayer *t, room->getAlivePlayers()){
            if(t->hasMark("@life") && t->hasMark("mind")){
                LogMessage log;
                log.type = "#Mind";
                log.from = t;
                log.arg = "mastermind";
                room->sendLog(log);

                RecoverStruct recover;
                recover.who = player;
                recover.recover = t->getMark("mind");
                room->recover(t, recover);
                room->setPlayerMark(t, "mind", 0);
            }
        }

        if(player->hasJur("sleep_jur"))
            player->removeJur("sleep_jur");
        break;
    }
    case AskForPeaches:{
        if(player->hasJur("sleep_jur")){
            if(player->getPhase() == Player::NotActive){
                LogMessage log;
                log.from = player;
                log.type = "#Sleep";
                room->sendLog(log);
                return true;
            }
        }
        break;
    }
    case CardUsed: {
        if(data.canConvert<CardUseStruct>()){
            CardUseStruct card_use = data.value<CardUseStruct>();
            const Card *card = card_use.card;
            if(card->isKindOf("Slash") || card->isKindOf("Duel") || card->isKindOf("Assassinate")){
                if(player->hasJur("chaos_jur")){
                    QList<ServerPlayer *> use_to = card_use.to;
                    foreach(ServerPlayer *tmp, use_to){
                        QList<ServerPlayer *> others = room->getOtherPlayers(tmp);
                        qShuffle(others);
                        card_use.to.replace(use_to.indexOf(tmp), others.first());
                    }
                    data = QVariant::fromValue(card_use);
                }
            }
        }
        break;
    }
    case CardAsk:
    case CardUseAsk:{
        QString pattern = data.toString();
        if(pattern.startsWith("@"))
            break;
        if(player->hasJur("sleep_jur")){
            if(player->getPhase() == Player::NotActive){
                LogMessage log;
                log.from = player;
                log.type = "#Sleep";
                room->sendLog(log);
                return true;
            }
        }
        break;
    }
    case Dying:{
        if(player->hasJur("dizzy_jur")){
            room->setPlayerMark(player, "scarecrow", 0);
            player->removeJur("dizzy_jur");
        }
        break;
    }
    case Death:{
        if(player->getGeneralName().startsWith("sun_") ||
           player->getGeneralName().startsWith("moon_")){
            if(player->askForSkillInvoke("casket_death")){
                QStringList ban;
                foreach(ServerPlayer *tmp, room->getAllPlayers()){
                    if(!ban.contains(tmp->getGeneralName()))
                        ban << tmp->getGeneralName();
                    if(!ban.contains(tmp->getGeneral2Name()))
                        ban << tmp->getGeneral2Name();
                }
                QStringList genlist = Sanguosha->getRandomGenerals(qMin(5, Config.value("MaxChoice", 3).toInt()), ban.toSet());
                QString general = room->askForGeneral(player, genlist);
                room->setPlayerProperty(player, "general", general);
                room->revivePlayer(player);
                room->setPlayerProperty(player, "maxhp", 1);
                room->setPlayerProperty(player, "hp", 1);
                player->drawCards(3);
                return true;
            }
        }
        break;
    }

    default:
        break;
    }

    return false;
}
