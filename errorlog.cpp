#include "errorlog.h"
#include "ui_errorlog.h"
#include <QDebug>

ErrorLog::ErrorLog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ErrorLog)
{
  ui->setupUi(this);
}

ErrorLog::~ErrorLog()
{
  delete ui;
}

void ErrorLog::loadResults(const QVector<checkitFileData> &resultList)
{
  QString passedFiles, failedFiles, failedStoreFiles, noCRCFiles, savedFiles;

  errorLogHTML.clear();
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
    { // Auto add extension, if not specified.
      filename+=".html";
    }

  qDebug() << filename;

  QFile logFile(filename);
  if (!logFile.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text)) {
      return;
    }
  QTextStream out(&logFile);
  out << errorLogHTML;
  logFile.close();


}
