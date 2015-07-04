#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_about.h"
#include "ui_manual.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow), rows(0)
{
  ui->setupUi(this);

  connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
  connect(ui->actionQuit_2, SIGNAL(triggered()), this, SLOT(close()));
  connect(ui->actionAdd_Directory_2, SIGNAL(triggered()), this, SLOT(addDirectory()));
  connect(ui->actionAdd_File_2, SIGNAL(triggered()), this, SLOT(addFile()));
  connect(ui->actionCheck, SIGNAL(triggered()), this, SLOT(check()));
  connect(ui->actionStore, SIGNAL(triggered()), this, SLOT(store()));
  connect(ui->actionManual, SIGNAL(triggered()), this, SLOT(help()));
  connect(ui->actionClear, SIGNAL(triggered()), this, SLOT(clearList()));
  ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui->listWidget, SIGNAL(customContextMenuRequested(QPoint)), this,
          SLOT(customContextMenuRequested(QPoint)));

  this->setAttribute(Qt::WA_QuitOnClose);


}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::addFile()
{

  QFileDialog fileDialog(this);
  fileDialog.setFileMode(QFileDialog::ExistingFiles);
  fileDialog.setViewMode(QFileDialog::Detail);

  QStringList files;
  if (fileDialog.exec()) {
      files = fileDialog.selectedFiles();
    }

  for (auto &x : files)
    {
      checkitFileData entry;
      entry.file = x;
      entry.status = Unchecked;
      ui->listWidget->addItem(entry.file);
      processList.append(entry);
    }
}

void MainWindow::addDirectory()
{

  QFileDialog fileDialog(this);
  fileDialog.setFileMode(QFileDialog::Directory);
  fileDialog.setViewMode(QFileDialog::Detail);
  fileDialog.setOption(QFileDialog::ShowDirsOnly);

  //  ui->listWidget->setUpdatesEnabled(false);
  QStringList files;
  if (fileDialog.exec()) {
      files = fileDialog.selectedFiles();
    }

  for (auto &x : files)
    {
      QDirIterator dirIterator(x, QDirIterator::Subdirectories);
      while (dirIterator.hasNext())
        {
          checkitFileData entry;
          QFileInfo info;
          dirIterator.next();
          entry.file = dirIterator.filePath();
          info = dirIterator.fileInfo();

          if (info.isFile() && (info.isHidden() == false))
            {
              entry.status = Unchecked;
              ui->listWidget->addItem(entry.file);
              processList.append(entry);
            }

        }
    }
}

void MainWindow::check()
{

  if (processList.size() == 0)
    {
      ui->statusbar->showMessage(tr("No files to check."));
      return;
    }

  int count = 0, failed = 0, nocrc = 0;
  QStringList failedFileList;
  QString text;

  ui->progressBar->setMinimum(0);
  ui->progressBar->setMaximum(processList.size());

  for (const auto &i : processList)
    {
      t_crc64 result = getCRC(i.file.toStdString().c_str());

      if (!result) {
          ui->listWidget->item(count)->setForeground(Qt::darkYellow);
          text = processList[count].file;
          text += "  [NO CRC]";
          ++nocrc;
          ui->listWidget->item(count)->setText(text);
        } else { // Only bother calculating CRC if there is a value to
          // compare against.
          t_crc64 fileCRC = FileCRC64(i.file.toStdString().c_str());

          if (result == fileCRC) {

              ui->listWidget->item(count)->setForeground(Qt::green);
              text = processList[count].file;
              text += "  [PASSED]";
              ui->listWidget->item(count)->setText(text);
            } else  if (result != 0 && result != fileCRC) {
              ++failed;
              ui->listWidget->item(count)->setForeground(Qt::red);
              text = processList[count].file;
              text += "  [FAILED]";
              ui->listWidget->item(count)->setText(text);
              failedFileList << i.file;
            }
        }

      ++count;
      ui->progressBar->setValue(count);

      QApplication::processEvents();

    }

  QString statusMessage;
  if (failed) {
      statusMessage += QString::number(failed) + " file(s) failed. ";
    } else if (!nocrc) {
      statusMessage += "All files have passed. ";
    } else {
      statusMessage += QString::number(count - failed - nocrc) + " file(s) passed. ";
    }
  if (nocrc) {
      statusMessage += QString::number(nocrc) + " file(s) with no CRC's stored. ";
    }
  ui->statusbar->showMessage(statusMessage);
}

void MainWindow::clearList()
{
  processList.clear();
  ui->listWidget->clear();
  ui->progressBar->setValue(0);
}

void MainWindow::store()
{

  if (processList.size() == 0)
    {
      ui->statusbar->showMessage(tr("No files to store CRC."));
      return;
    }

  QString text;
  int flags = 0, count = 0;

  ui->statusbar->showMessage("");
  checkitFileData d;

  if (ui->overwriteCheckbox->isChecked())
    {
      flags |= OVERWRITE;
    }

  for (const auto &i : processList)
    {
      char checkitOpts = getCheckitOptions(i.file.toStdString().c_str());

      if (checkitOpts & STATIC) {
          flags =  flags  & ~OVERWRITE; // disallow overwrite in this case.
        } else if (checkitOpts & UPDATEABLE) {
          flags |= OVERWRITE;
        } // This will overwrite the checkbox in the GUI.

      t_crc64 result = putCRC(i.file.toStdString().c_str(), flags);

      if (!result) {
          ui->listWidget->item(count)->setForeground(Qt::green);
          text = processList[count].file;
          text += "  [SAVED]";
          ui->listWidget->item(count)->setText(text);
        } else {
          ui->listWidget->item(count)->setForeground(Qt::red);
          text = processList[count].file;
          text += "  [NOT SAVED] : " + QString(errorMessage(result));
          ui->listWidget->item(count)->setText(text);
        }
      ++count;
    }
}

void MainWindow::about()
{
  //QDialog *dialog = new QDialog();
  QDialog dialog;
  Ui::Dialog aboutDialog;
  aboutDialog.setupUi(&dialog);
  aboutDialog.versionLabel->setText(XVERSION);
  dialog.exec();
  //delete dialog;
}

void MainWindow::help()
{
  QDialog dialog;
  Ui::manualDialog mandialog;
  mandialog.setupUi(&dialog);
  dialog.exec();
}

void MainWindow::quit()
{
  QApplication::quit();
}

void MainWindow::customContextMenuRequested(const QPoint &pos)
{
  int flags = 0;
  int error;
  if (ui->overwriteCheckbox->isChecked())
    {
      flags |= OVERWRITE;
    }

  QVector<int> selectedListIndex;

  QPoint position = ui->listWidget->mapToGlobal(pos);
  QScopedPointer<QMenu> menu(new QMenu(this));

  QAction *exportAction = new QAction("Export CRC", menu.data());
  QAction *importAction = new QAction("Import CRC", menu.data());
  QAction *removeAction = new QAction("Remove CRC", menu.data());
  QAction *allowUpdateAction = new QAction("Allow CRC Updates", menu.data());
  allowUpdateAction->setCheckable(true);

  menu->addAction(exportAction);
  menu->addAction(importAction);
  menu->addAction(removeAction);
  menu->addAction(allowUpdateAction);

  QList<QListWidgetItem*> selectedItems = ui->listWidget->selectedItems();

  for (auto &i : selectedItems)
    { // Build list of row numbers selected.
      selectedListIndex.push_back(ui->listWidget->row(i));
    }

  auto checkitFlags = getCheckitOptions(processList[ui->listWidget->currentRow()].file.toStdString().c_str());
  if (checkitFlags == UPDATEABLE) {
      allowUpdateAction->setChecked(true);
    }

  auto x = menu->exec(position);

  if (x == exportAction) {
      for (auto i : selectedListIndex)
        {
          error = exportCRC(processList[i].file.toStdString().c_str(), flags);
          if (error) {
              ui->statusbar->showMessage(tr("Could not export CRC for ") + processList[ui->listWidget->currentRow()].file
                  + " : " + errorMessage(error));
            }
        }
    } else if (x == importAction) {
      for (auto i : selectedListIndex)
        {
          error = importCRC(processList[i].file.toStdString().c_str(), flags);
          if (error) {
              ui->statusbar->showMessage(tr("Could not import CRC for ") + processList[ui->listWidget->currentRow()].file
                  + " : " + errorMessage(error));
            }
        }
    } else if (x == removeAction) {
      for (auto i : selectedListIndex)
        {
          error = removeCRC(processList[i].file.toStdString().c_str());
          if (error) {
              ui->statusbar->showMessage(tr("Could not remove CRC for ") + processList[ui->listWidget->currentRow()].file
                  + " : " + errorMessage(error));
            }
        }
    } else if (x == allowUpdateAction) {
      allowUpdateAction->toggle();

      if (checkitFlags == UPDATEABLE)
        {
          checkitFlags = STATIC;
        } else { checkitFlags = UPDATEABLE; }

      for (auto i : selectedListIndex)
        {

          error = setCheckitOptions(processList[i].file.toStdString().c_str(), checkitFlags);

          if (error) {
              ui->statusbar->showMessage(tr("Could not set checkit attributefor ") + processList[ui->listWidget->currentRow()].file
                  + " : " + errorMessage(error));
            }
        }
    }
}

