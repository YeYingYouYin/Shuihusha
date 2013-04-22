#ifndef GENERAL_H
#define GENERAL_H

class Skill;
class TriggerSkill;
class Package;
class QSize;

#include <QObject>
#include <QSet>
#include <QMap>
#include <QStringList>

class General : public QObject
{
    Q_OBJECT
    Q_ENUMS(Gender)
    Q_PROPERTY(QString kingdom READ getKingdom CONSTANT)
    Q_PROPERTY(int maxhp READ getMaxHp CONSTANT)
    Q_PROPERTY(bool male READ isMale STORED false CONSTANT)
    Q_PROPERTY(bool female READ isFemale STORED false CONSTANT)
    Q_PROPERTY(Gender gender READ getGender WRITE setGender)
    Q_PROPERTY(bool lord READ isLord CONSTANT)
    Q_PROPERTY(bool hidden READ isHidden CONSTANT)

public:
    enum Gender {Male = 0, Female = 1, Neuter = 2};
    explicit General(Package *package, const QString &name, const QString &kingdom, int max_hp = 4, bool male = true, bool hidden = false, bool never_shown = false);
    explicit General(Package *package, const QString &name, const QString &kingdom, const QString &hp_mxhp, Gender gender = Male, bool hidden = false, bool never_shown = false);

    // property getters/setters
    int getMaxHp() const;
    QString getKingdom(bool unmap = false) const;
    bool isMale() const {return gender == Male;}
    bool isFemale() const {return gender == Female;}
    bool isNeuter() const {return gender == Neuter;}
    bool isLord() const {return lord;}
    bool isHidden() const {return hidden;}
    bool isTotallyHidden() const {return never_shown;}
    bool isLuaGeneral() const;

    Gender getGender() const {return gender;}
    void setGender(Gender gender);
    void setGenderString(const QString &sex);
    QString getGenderString() const;
    int getLoseHp() const;

    void addSkill(Skill* skill);
    void addSkill(const QString &skill_name);
    bool hasSkill(const QString &skill_name) const;
    QList<const Skill *> getVisibleSkillList() const;
    QSet<const Skill *> getVisibleSkills() const;
    QSet<const TriggerSkill *> getTriggerSkills() const;

    void addRelateSkill(const QString &skill_name);
    QStringList getRelatedSkillNames() const;

    QString getPixmapPath(const QString &category) const;
    QString getPackage() const;
    QString getSkillDescription() const;
    QString getLastword() const;
    QString getWinword() const;
    QString getId() const;
    QString getNickname(bool full = false) const;
    QString getShowHp() const;

    static QSize BigIconSize;
    static QSize SmallIconSize;
    static QSize TinyIconSize;

public slots:
    void lastWord() const;
    void winWord() const;

private:
    void init(const QString &name);
    QString kingdom;
    int max_hp, losehp;
    Gender gender;
    bool lord;
    QSet<QString> skill_set;
    QSet<QString> extra_set;
    QStringList related_skills;
    bool hidden;
    bool never_shown;
    QMap<QString, QString> kmap;
};

#endif // GENERAL_H
