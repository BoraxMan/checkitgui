#ifndef ERRORLOG_H
#define ERRORLOG_H

#include <QDialog>
#include <QVector>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QDateTime>
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

private:
  Ui::ErrorLog *ui;
  QString errorLogHTML;

private slots:
  void saveLog();
};

#endif // ERRORLOG_H
