#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QFileDialog>
#include <QDebug>
#include <QPixmap>
#include <QDirIterator>
#include <QListWidgetItem>

extern "C" {
#include "checkit.h"
}

enum checkitstatus {
  OK,
  Failed,
  Saved,
  FailedSave,
  Unchecked
} typedef checkitStatus;



namespace Ui {
  class MainWindow;
}

struct checkitFileData {
  QString file;
  checkitStatus status;
  bool directory;

};

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private:
  Ui::MainWindow *ui;
  QVector<checkitFileData> processList;
  QStringList fileList;

  int rows;
  void addCheckitWidget(checkitFileData &data);


private slots:
  void addFile();
  void addDirectory();
  void check();
  void clearList();
  void store();
  void about();
  void customContextMenuRequested ( const QPoint & pos );


};

#endif // MAINWINDOW_H
