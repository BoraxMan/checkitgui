#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QFileDialog>
#include <QDebug>
#include <QPixmap>
#include <QDirIterator>
#include <QTime>
#include <QList>
#include <QListWidgetItem>
#include <QScopedPointer>
#include <algorithm>
#include "checkitgui.h"
#include "errorlog.h"

extern "C" {
#include "checkit.h"
#include "checkit_attr.h"
}


namespace Ui {
  class MainWindow;
}


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
  void displayErrorLog();
  void customContextMenuRequested ( const QPoint & pos );


};

#endif // MAINWINDOW_H
