#ifndef PACKAGINGEDITOR_H
#define PACKAGINGEDITOR_H

#include <QDialog>
#include <QListWidget>
#include <QGroupBox>
#include <QSettings>
#include <QTextEdit>

class MetaInfoWidget: public QGroupBox{
    Q_OBJECT

public:
    MetaInfoWidget(bool load_config);
    void saveToSettings(QSettings &settings);
    void showSettings(const QSettings *settings);

    void setName(const QString &name);
    void setDesigner(const QString &designer);
    void setCoder(const QString &coder);

private:
    QLineEdit *name_edit;
    QLineEdit *designer_edit;
    QLineEdit *programmer_edit;
    QLineEdit *version_edit;
    QTextEdit *description_edit;
};

class PackagingEditor : public QDialog
{
    Q_OBJECT
public:
    explicit PackagingEditor(QWidget *parent = 0);

private:
    QListWidget *package_list;
    MetaInfoWidget *package_list_meta;

    QListWidget *file_list;
    MetaInfoWidget *file_list_meta;
    QListWidget *lua_list;
    QTabWidget *tab_widget;
    QWidget *createManagerTab();
    QWidget *createPackagingTab();
    QWidget *createSniffTab();
    void loadPackageList();

private slots:
    void installPackage();
    void modifyPackage(QListWidgetItem* item);
    void modifyPackage();
    void uninstallPackage();
    void hideorshowPackage();
    void rescanPackage();
    void browseFiles();
    void removeFile(QListWidgetItem* item, bool mute = true);
    void removeFile();
    void editFile();
    void makePackage();
    void migrationPackage();
    void done7zProcess(int exit_code);
    void updateMetaInfo(QListWidgetItem *item);
    void sniffLua();
};

#endif // PACKAGINGEDITOR_H
