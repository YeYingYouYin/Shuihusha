#ifndef GENERALOVERVIEW_H
#define GENERALOVERVIEW_H

class General;
class Skill;
class QCommandLinkButton;

#include <QDialog>
#include <QTableWidgetItem>
#include <QButtonGroup>
#include <QVBoxLayout>

namespace Ui {
    class GeneralOverview;
}

class GeneralOverview : public QDialog {
    Q_OBJECT
public:
    GeneralOverview(QWidget *parent = 0);
    ~GeneralOverview();
    void fillGenerals(const QList<const General *> &generals);
    bool isInvisibleSkill(const QString &skill_name, int index);
    bool singleSkillFineTuning(const QString &general_name, const QString &skill_name, int index);

private:
    Ui::GeneralOverview *ui;
    QVBoxLayout *button_layout;

    void resetButtons();
    void addLines(const Skill *skill, int wake_index = 0);
    void addWakeLines(const QString &general_name);
    void addCopyAction(QCommandLinkButton *button);
    void addChangeAction(QPushButton *button);

private slots:
    void playEffect();
    void copyLines();
    void askChange();
    void on_tableWidget_itemSelectionChanged();
    void on_tableWidget_itemDoubleClicked(QTableWidgetItem* item);
};

#endif // GENERALOVERVIEW_H
