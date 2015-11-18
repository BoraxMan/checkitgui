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

#include "errorlog.h"
#include "ui_errorlog.h"
#include <QDebug>

ErrorLog::ErrorLog(QWidget *parent) :
  QDialog(parent),
  stale(true), ui(new Ui::ErrorLog)
{
  ui->setupUi(this);
}

ErrorLog::~ErrorLog()
{
  delete ui;
}

void ErrorLog::loadResults(const QVector<checkitFileData> &resultList)
{
  if (stale == false)
    return;

  if (resultList.isEmpty())
    { // don't process this at all!
      return;
    }
  QString passedFiles, failedFiles, failedStoreFiles, noCRCFiles, savedFiles;

  errorLogHTML.clear();
  ui->textBrowserErrorLog->clear();

  errorLogHTML+=QString("<!DOCTYPE html>\n<html>\n<body>\n");

  for (const auto &i : resultList)
    {
      if (i.status == checkitStatus::OK) {
          passedFiles+= i.file + "<br>\n";
        } else if (i.status == checkitStatus::Failed) {
          failedFiles += i.file + "<br>\n";
        } else if (i.status == checkitStatus::FailedSave) {
          failedStoreFiles += i.file + "<br>\n";
        } else if (i.status == checkitStatus::NOCRC) {
          noCRCFiles += i.file + "<br>\n";
        } else if (i.status == checkitStatus::Saved) {
          savedFiles += i.file + "<br>\n";
        }
    }

  if (!failedFiles.isEmpty()) {
      errorLogHTML+= QString("<h2>Failed</h2><br>\n");
      errorLogHTML+= failedFiles;
    }

  if (!failedStoreFiles.isEmpty()) {
      errorLogHTML+= QString("<h2>Failed to Store CRC</h2><br>\n");
      errorLogHTML+= failedStoreFiles;
    }

  if (!noCRCFiles.isEmpty()) {
      errorLogHTML+= QString("<h2>No CRC</h2><br>\n");
      errorLogHTML+= noCRCFiles;
    }

  if (!savedFiles.isEmpty()) {
      errorLogHTML+= QString("<h2>CRC Stored</h2><br>\n");
      errorLogHTML+= savedFiles;
    }

  if (!passedFiles.isEmpty()) {
      errorLogHTML+= QString("<h2>Passed checks</h2><br>\n");
      errorLogHTML+= passedFiles;
    }

  errorLogHTML += "<br>Checkit run on " + QDateTime::currentDateTime().toString();
  errorLogHTML+= QString("</body></html>\n");

  ui->textBrowserErrorLog->insertHtml(errorLogHTML);
  stale = false;

}

void ErrorLog::saveLog()
{

  QFileDialog fileDialog(this);
  fileDialog.setFileMode(QFileDialog::AnyFile);
  //fileDialog.setViewMode(QFileDialog::Detail);
  fileDialog.setDefaultSuffix(".html");
  fileDialog.setAcceptMode(QFileDialog::AcceptSave);

  QString filename = fileDialog.getOpenFileName();

  if ((filename.right(5).toLower() != QString(".html")) &&
      filename.right(4).toLower() != QString(".htm"))
    { // Auto add extension, if not specified by the user already.
      filename+=".html";
    }

  QFile logFile(filename);
  if (!logFile.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text)) {
      QMessageBox messagebox;
      messagebox.setText(tr("Could not create file ") + filename);
      messagebox.setWindowTitle(tr("File error"));
      messagebox.exec();
      return;
    }
  QTextStream out(&logFile);
  out << errorLogHTML;
  logFile.close();


}
