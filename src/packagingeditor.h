#ifndef PACKAGINGEDITOR_H
#define PACKAGINGEDITOR_H

#include <QDialog>
#include <QListWidget>

class PackagingEditor : public QDialog
{
    Q_OBJECT
public:
    explicit PackagingEditor(QWidget *parent = 0);

private:
    QListWidget *file_list;
    QListWidget *package_list;

    QWidget *createManagerTab();
    QWidget *createPackagingTab();
    void loadPackageList();

private slots:
    void installPackage();
    void uninstallPackage();
    void rescanPackage();
    void browseFiles();
    void makePackage();
    void done7zProcess(int exit_code);
};

#endif // PACKAGINGEDITOR_H
