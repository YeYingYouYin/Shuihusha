#include "settings.h"
#include "photo.h"
#include "card.h"
#include "engine.h"

#include <QFontDatabase>
#include <QStringList>
#include <QFile>
#include <QMessageBox>
#include <QApplication>
#include <QNetworkInterface>
#include <QDateTime>

Settings Config;

static const qreal ViewWidth = 1280 * 0.8;
static const qreal ViewHeight = 800 * 0.8;

//consts
const int Settings::S_CHOOSE_GENERAL_TIMEOUT = 15;
const int Settings::S_GUANXING_TIMEOUT = 20;
const int Settings::S_SURRNDER_REQUEST_MIN_INTERVAL = 60;

Settings::Settings()

#ifdef Q_OS_WIN32
    :QSettings("config.ini", QSettings::IniFormat)
#else
    :QSettings("QSanguosha.org", "QSanguosha")
#endif

     ,Rect(-ViewWidth/2, -ViewHeight/2, ViewWidth, ViewHeight)
{
}

void Settings::init(){
    if(!qApp->arguments().contains("-server")){
        QString font_path = value("DefaultFontPath", "font/font.ttf").toString();
        int font_id = QFontDatabase::addApplicationFont(font_path);
        if(font_id!=-1){
            QString font_family = QFontDatabase::applicationFontFamilies(font_id).first();
            BigFont.setFamily(font_family);
            SmallFont.setFamily(font_family);
            TinyFont.setFamily(font_family);
        }else
            QMessageBox::warning(NULL, tr("Warning"), tr("Font file %1 could not be loaded!").arg(font_path));

        BigFont.setPixelSize(56);
        SmallFont.setPixelSize(27);
        TinyFont.setPixelSize(18);

        SmallFont.setWeight(QFont::Bold);

        AppFont = value("AppFont", QApplication::font("QMainWindow")).value<QFont>();
        UIFont = value("UIFont", QApplication::font("QTextEdit")).value<QFont>();
        TextEditColor = QColor(value("TextEditColor", "white").toString());
    }

    CountDownSeconds = value("CountDownSeconds", 3).toInt();
    GameMode = value("GameMode", "08p").toString();

    if(!contains("BanPackages")){
        QStringList banlist;
        banlist << "test" << "god" << "sp" << "gift"
                << "customcards"
                //<< "joy" << "kuso" << "joyer"
                ;

        setValue("BanPackages", banlist);
    }
    BanPackages = value("BanPackages").toStringList();

    ContestMode = value("ContestMode", false).toBool();
    Statistic = value("Statistic", false).toBool();
    ForbidSIMC = value("ForbidSIMC", false).toBool();
    DisableChat = value("DisableChat", false).toBool();
    Enable2ndGeneral = value("Enable2ndGeneral", false).toBool();
    EnableReincarnation = value("EnableReincarnation", false).toBool();
    EnableScene = value("EnableScene", false).toBool();	//changjing
    EnableSame = value("EnableSame", false).toBool();
    EnableEndless = value("EnableEndless", false).toBool();
    EnableAnzhan = value("EnableAnzhan", false).toBool();
    EnableBasara = value("EnableBasara", false).toBool();
    EnableHegemony = value("EnableHegemony", false).toBool();
    MaxHpScheme = value("MaxHpScheme", 0).toInt();
    AnnounceIP = value("AnnounceIP", false).toBool();
    Address = value("Address", QString()).toString();
    FreeChooseGenerals = value("FreeChooseGenerals", false).toBool();
    FreeChooseCards = value("FreeChooseCards", false).toBool();
    FreeAssignSelf = value("FrstoryIPs").toStringList();
    DetectorPort = value("DetectorPort", 9526u).toUInt();
    MaxCards = value("MaxCards", 15).toInt();

    CircularView = value("CircularView", true).toBool();
    FitInView = value("FitInView", false).toBool();
    EnableHotKey = value("EnableHotKey", true).toBool();
    NeverNullifyMyTrick = value("NeverNullifyMyTrick", true).toBool();
    EnableMinimizeDialog = value("EnableMinimizeDialog", false).toBool();
    EnableAutoTarget = valeeAssignSelf", false).toBool();
    EnableAI = value("EnableAI", true).toBool();
    AIDelay = value("AIDelay", 1500).toInt();
    ServerPort = value("ServerPort", 9527u).toUInt();

#ifdef Q_OS_WIN32
    UserName = value("UserName", qgetenv("USERNAME")).toString();
#else
    UserName = value("USERNAME", qgetenv("USER")).toString();
#endif

    if(UserName == "Admin" || UserName == "Administrator")
        UserName = tr("Shuihusha-fans");
    ServerName = value("ServerName", tr("%1's server").arg(UserName)).toString();

    HostAddress = value("HostAddress", "127.0.0.1").toString();
    UserAvatar = value("UserAvatar", "anjiang").toString();
    HistoryIPs = value("Hiue("EnableAutoTarget", false).toBool();
    NullificationCountDown = value("NullificationCountDown", 8).toInt();
    ShowAllName = value("ShowAllName", true).toBool();
    SPOpen = value("SPOpen", false).toBool();
    OperationTimeout = value("OperationTimeout", 15).toInt();
    OperationNoLimit = value("OperationNoLimit", false).toBool();
    EnableCardEffects = value("EnableCardEffects", true).toBool();
    EnableEquipEffects = value("EnableEquipEffects", true).toBool();
    EnableSkillEffects = value("EnableSkillEffects", true).toBool();
    EnableLastWord = value("EnableLastWord", true).toBool();
    EnableCheatRing = value("EnableCheatRing", true).toBool();
    EnableBgMusic = value("EnableBgMusic", true).toBool();
    BGMVolume = value("BGMVolume", 0.75f).toFloat();
    EffectVolume = value("EffectVolume", 1.0f).toFloat();
    EnableLua = value("EnableLua", false).toBool();

    BackgroundBrush = value("BackgroundBrush", "backdrop/shuihu.jpg").toString();

    QStringList roles_ban, kof_ban, basara_ban, hegemony_ban, pairs_ban;

    roles_ban << "ubuntenkei";

    kof_ban << "andaoquan" << "shixiu"
            /* << "shenwuyong" << "wangdingliu" << "zhaoji"*/;

    //basara_ban << "dingdesun" << "houjian" << "shenwusong" << "shenwuyong" << "shenzhangqing" << "lili";

    hegemony_ban.append(basara_ban);
    hegemony_ban << "gongsunsheng";
    foreach(QString general, Sanguosha->getLimitedGeneralNames()){
        if(Sanguosha->getGeneral(general)->getKingdom() == "god" && !hegemony_ban.contains(general))
            hegemony_ban << general;
    }

    pairs_ban << "tongguan" << "tongguanf" << "caijing"
              << "gaoqiu+luozhenren" << "wangying+zhangqing" << "wangying+qiongying"
              << "tianhu+yanshun" << "zhangheng+shixiu"
              //<< "shenwuyong"
              //<< "liruilan+shijin" << "lujunyi+shenzhangqing" << "luozhenren+yuehe"
              //<< "likui+luozhenren" << "husanniang+jiashi" << "shijin+yanshun"
              //<< "oupeng+wangqing" << "jiashi+shenzhangqing"
              //<< "husanniang+zhaoji" << "dingdesun+wangqing"
              ;

    QStringList banlist = value("Banlist/Roles").toStringList();
    foreach(QString ban_general, roles_ban){
        if(!banlist.contains(ban_general))
            banlist << ban_general;
    }
    setValue("Banlist/Roles", banlist);

    banlist = value("Banlist/1v1").toStringList();
    foreach(QString ban_general, kof_ban){
        if(!banlist.contains(ban_general))
            banlist << ban_general;
    }
    setValue("Banlist/1v1", banlist);

    banlist = value("Banlist/Basara").toStringList();
    foreach(QString ban_general, basara_ban){
        if(!banlist.contains(ban_general))
                banlist << ban_general;
    }
    setValue("Banlist/Basara", banlist);

    banlist = value("Banlist/Hegemony").toStringList();
    foreach(QString ban_general, hegemony_ban){
        if(!banlist.contains(ban_general))
                banlist << ban_general;
    }
    setValue("Banlist/Hegemony", banlist);

    banlist = value("Banlist/Pairs").toStringList();
    foreach(QString ban_general, pairs_ban){
        if(!banlist.contains(ban_general))
                banlist << ban_general;
    }
    setValue("Banlist/Pairs", banlist);

    QStringList forbid_packages;
    forbid_packages << "test";
    setValue("ForbidPackages", forbid_packages.join("+"));

//ui
    setValue("UI/ExpandDashboard", value("UI/ExpandDashboard", true).toBool());
}
