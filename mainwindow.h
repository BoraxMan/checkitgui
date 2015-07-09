/*
 * This file is part of Checkitgui: A graphical file integrity checksum tool
 * Copyright (C) 2014  Dennis Katsonis dennisk@netspace.net.au
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QFileDialog>
#include <QDirIterator>
#include <QTime>
#include <QList>
#include <QFileInfo>
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
  ErrorLog errorlog;
  void addFile(const QString filename);
  void addDirectory(const QString dir);

private slots:
  void addFileSlot();
  void addDirectorySlot();
  void check();
  void clearList();
  void store();
  void about();
  void help();
  void aboutQT();
  void quit();
  void displayErrorLog();
  void customContextMenuRequested ( const QPoint & pos );
};

#endif // MAINWINDOW_H
