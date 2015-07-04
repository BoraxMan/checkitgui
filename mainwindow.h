#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QFileDialog>
#include <QDebug>
#include <QPixmap>
#include <QDirIterator>
#include <QTime>
#include <QListWidgetItem>
#include <QScopedPointer>

extern "C" {
#include "checkit.h"
#include "checkit_attr.h"
}

#define XVERSION "0.1.0"

enum checkitstatus {
  OK,
  Failed,
  Saved,
  FailedSave,
  Unchecked
} typedef checkitStatus;

/*
enum function {
  getcrc,
  storecrc
} typedef Function;*/


namespace Ui {
  class MainWindow;
}

struct checkitFileData {
  QString file;
  checkitStatus status;
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

private slots:
  void addFile();
  void addDirectory();
  void check();
  void clearList();
  void store();
  void about();
  void help();
  void quit();
  void customContextMenuRequested ( const QPoint & pos );


};

#endif // MAINWINDOW_H
