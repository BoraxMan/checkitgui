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

#ifndef ERRORLOG_H
#define ERRORLOG_H

#include <QDialog>
#include <QVector>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QDateTime>
#include <QMessageBox>
#include "checkitgui.h"

namespace Ui {
  class ErrorLog;
}

class ErrorLog : public QDialog
{
  Q_OBJECT

public:
  explicit ErrorLog(QWidget *parent = 0);
  ~ErrorLog();
  void loadResults(const QVector<checkitFileData> &resultList);
  bool stale; // This is can be set to true, to indicate
  // that the results must be reprocessed.  If false, calls to loadResults don't do anything.
  // Slightly more efficient.

private:
  Ui::ErrorLog *ui;
  QString errorLogHTML;

private slots:
  void saveLog();
};

#endif // ERRORLOG_H
