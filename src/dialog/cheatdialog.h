#ifndef CHEATDIALOG_H
#define CHEATDIALOG_H

#include "photo.h"
#include "dashboard.h"
#include <QDialog>
#include <QSpinBox>
#include <QListWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QMenu>

class Window;

class ScriptExecutor: public QDialog{
    Q_OBJECT

public:
    ScriptExecutor(QWidget *parent);

public slots:
    void doScript();
};

class CheatDialog: public QDialog{
    Q_OBJECT

public:
    CheatDialog(QWidget *parent, ClientPlayer *Self);
    QTabWidget *tab_widget;
    QAbstractButton *kill, *unkill;

private:
    ClientPlayer *Self;
    QWidget *createDamageMakeTab();
    QWidget *createDeathNoteTab();
    QWidget *createCardMoveTab();
    QWidget *createSetStateTab();
    QPushButton *ok_button;
    QPushButton *cancel_button;
    QPushButton *apply_button;

    QComboBox *damage_source;
    QComboBox *damage_target;
    QButtonGroup *damage_nature;
    QAbstractButton *normal, *fire, *thunder, *rec, *lh, *lmh, *rmh;
    QSpinBox *damage_point, *damage_card;
    QComboBox *killer, *victim;
    QButtonGroup *killtype;
    QAbstractButton *revive1, *revive2;

    QListWidget *cards_list;
    QTabWidget *tab_state;
    QComboBox *source, *target;
    QLineEdit *general, *kingdom, *hpslot, *role, *sex;
    QCheckBox *turn, *chain, *ecst, *drank, *shutup;
    QLineEdit *skill_history;
    QPushButton *extra_button, *clear_button;
    QButtonGroup *conjur_group;
    QAbstractButton *poison, *sleep, *dizzy, *stealth;
    QLineEdit *conjur_text;
    QLineEdit *flags, *marks, *propty, *tag;
    QPushButton *flag_option;
    QPushButton *mark_option;
    QCheckBox *tag_room;

protected:
    virtual QString getPlayerString();
    virtual const Player *getPlayer();
    virtual const QString makeData();
    virtual const QStringList getExtraSkills();
    virtual void accept();

private slots:
    void doApply();
    void doApplyExpert();
    void doClearExpert();
    void disableSource(QAbstractButton* but);
    void loadCard(int index);
    void loadState(int index);
    void loadBase();
    void clearBase();
    void fillBase();
    void equipIt();
    void loseSkill();
    void clearHistory();
    void addSkill();
    void addHistory();
    void drawOne();
    void discardOne();
    void setGray(int);
};

#endif // CHEATDIALOG_H
