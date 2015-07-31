/*r
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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_about.h"
#include "ui_manual.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
  connect(ui->actionQuit_2, SIGNAL(triggered()), this, SLOT(close()));
  connect(ui->actionAdd_Directory_2, SIGNAL(triggered()), this, SLOT(addDirectorySlot()));
  connect(ui->actionAdd_File_2, SIGNAL(triggered()), this, SLOT(addFileSlot()));
  connect(ui->actionCheck, SIGNAL(triggered()), this, SLOT(check()));
  connect(ui->actionStore, SIGNAL(triggered()), this, SLOT(store()));
  connect(ui->actionManual, SIGNAL(triggered()), this, SLOT(help()));
  connect(ui->actionAbout_Qt, SIGNAL(triggered()), this, SLOT(aboutQT()));
  connect(ui->actionClear, SIGNAL(triggered()), this, SLOT(clearList()));
  ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui->listWidget, SIGNAL(customContextMenuRequested(QPoint)), this,
          SLOT(customContextMenuRequested(QPoint)));

  this->setAttribute(Qt::WA_QuitOnClose);

  // See if there are any command line arguments.  Add this.  Recurse through directories
  // if directories are specified.

  char **c = QApplication::argv();
  for (auto x = 1 ; x < QApplication::argc(); ++x)
    {
      QFileInfo fileinfo(c[x]);
      if (fileinfo.isFile()) {
          addFile(c[x]);
        } else if (fileinfo.isDir()) {
          addDirectory(QString(c[x]));
        }
    } // end range for.
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::addFile(const QString filename)
{
  checkitFileData entry;
  entry.file = filename;
  entry.status = checkitStatus::Unchecked;
  ui->listWidget->addItem(entry.file);
  processList.append(entry);
}

void MainWindow::addFileSlot()
{

  QFileDialog fileDialog(this);
  fileDialog.setFileMode(QFileDialog::ExistingFiles);
  fileDialog.setViewMode(QFileDialog::Detail);

  QStringList files;
  if (fileDialog.exec()) {
      files = fileDialog.selectedFiles();
    }

  for (const auto &x : files)
    {
      addFile(x);
    }
}
void MainWindow::addDirectory(const QString dir)
{
  QDirIterator dirIterator(dir, QDirIterator::Subdirectories);
  while (dirIterator.hasNext())
    {
      checkitFileData entry;
      QFileInfo info;
      dirIterator.next();
      entry.file = dirIterator.filePath();
      info = dirIterator.fileInfo();

      if (info.isFile() && (info.isHidden() == false))
        {
          entry.status = checkitStatus::Unchecked;
          ui->listWidget->addItem(entry.file);
          processList.append(entry);
        }
    } // end of while
}

void MainWindow::addDirectorySlot()
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
      addDirectory(x);
    } // end of range for loop.
} // end of addDirectory()

void MainWindow::check()
{
  errorlog.stale = true;

  if (processList.size() == 0)
    {
      ui->statusbar->showMessage(tr("No files to check."));
      return;
    }
  ui->errorStatusLabel->clear();
  ui->statusbar->showMessage("");

  int count{}, failed{}, nocrc{};
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
          processList[count].status = checkitStatus::NOCRC;
          ++nocrc;
          ui->listWidget->item(count)->setText(text);
        } else { // Only bother calculating CRC if there is a value to
          // compare against.
          t_crc64 fileCRC = FileCRC64(i.file.toStdString().c_str());

          if (result == fileCRC) {

              ui->listWidget->item(count)->setForeground(Qt::darkGreen);
              text = processList[count].file;
              text += "  [PASSED]";
              processList[count].status = checkitStatus::OK;
              ui->listWidget->item(count)->setText(text);
            } else if (result != 0 && result != fileCRC) {
              ++failed;
              ui->listWidget->item(count)->setForeground(Qt::red);
              text = processList[count].file;
              text += "  [FAILED]";
              processList[count].status = checkitStatus::Failed;
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
      ui->errorStatusLabel->setText(tr("Errors reported!"));
      statusMessage += QString::number(failed) + " file(s) failed. ";
    } else if (!nocrc) {
      statusMessage += tr("All files have passed. ");
    } else {
      statusMessage += QString::number(count - failed - nocrc) + tr(" file(s) passed. ");
    }
  if (nocrc) {
      ui->errorStatusLabel->setText(tr("Errors reported!"));
      statusMessage += QString::number(nocrc) + tr(" file(s) with no CRC's stored. ");
    }
  ui->statusbar->showMessage(statusMessage);

}

void MainWindow::clearList()
{
  processList.clear();
  ui->listWidget->clear();
  ui->progressBar->setValue(0);
  ui->errorStatusLabel->clear();
  ui->statusbar->clearMessage();
}

void MainWindow::store()
{
  errorlog.stale = true;
  ui->errorStatusLabel->clear();
  int failed{};
  if (processList.size() == 0)
    {
      ui->statusbar->showMessage(tr("No files to store CRC."));
      return;
    }

  QString text;
  int flags{}, count{};

  ui->statusbar->showMessage("");

  for (const auto &i : processList)
    {
      char checkitOpts = getCheckitOptions(i.file.toStdString().c_str());

      if ((checkitOpts & STATIC || (!checkitOpts && !ui->overwriteCheckbox->isChecked()))) {
          flags =  flags  & ~OVERWRITE; // disallow overwrite in this case it is static, or no options present AND the overwrite checkbox is not selected
        } else if ((checkitOpts & UPDATEABLE) || (ui->overwriteCheckbox->isChecked())) {
          flags |= OVERWRITE; // If there  are options to update OR the checkbox is checked, overwrite.
          // Note that if the STATIC option was found earlier, this is ignored.  We want this behiaviour, as we want the checkit
          // attribute to overwrite the checkbox.
        } else if (checkitOpts & OPT_ERROR) {
          flags =  flags  & ~OVERWRITE; // disallow overwrite in this case.  If error, assume the safest.  No overwrite.
        }

      // This will overrule the checkbox in the GUI.

      t_crc64 result = putCRC(i.file.toStdString().c_str(), flags);

      if (!result) {
          ui->listWidget->item(count)->setForeground(Qt::green);
          text = processList[count].file;
          text += "  [SAVED]";
          processList[count].status = checkitStatus::Saved;
          ui->listWidget->item(count)->setText(text);
        } else {
          ++failed;
          ui->listWidget->item(count)->setForeground(Qt::red);
          text = processList[count].file;
          text += "  [NOT SAVED] : " + QString(errorMessage(result));
          processList[count].status = checkitStatus::FailedSave;
          ui->listWidget->item(count)->setText(text);
        }
      ++count;
    }
  if (failed)
    {
      ui->errorStatusLabel->setText(tr("Errors reported!"));
    }
}

void MainWindow::about()
{
  QDialog dialog;
  Ui::Dialog aboutDialog;
  aboutDialog.setupUi(&dialog);
  aboutDialog.versionLabel->setText(XVERSION);
  dialog.exec();
}

void MainWindow::help()
{
  QDialog dialog;
  Ui::manualDialog mandialog;
  mandialog.setupUi(&dialog);
  dialog.exec();
}

void MainWindow::aboutQT()
{
  QApplication::aboutQt();
}

void MainWindow::quit()
{
  QApplication::quit();
}

void MainWindow::displayErrorLog()
{
  errorlog.loadResults(processList);
  errorlog.show();
}

void MainWindow::customContextMenuRequested(const QPoint &pos)
{
  int flags{};
  int error;

  auto rightClickStatusError = [&] (QString message) { ui->statusbar->showMessage((message) + processList[ui->listWidget->currentRow()].file + " : " + errorMessage(error)); };

  QList<QListWidgetItem*> selectedItems = ui->listWidget->selectedItems();

  if (selectedItems.isEmpty())
    return; // No menu if nothing selected.

  if (ui->overwriteCheckbox->isChecked())
    {
      flags |= OVERWRITE;
    }

  QVector<int> selectedListIndex;

  QPoint position = ui->listWidget->mapToGlobal(pos);
  QScopedPointer<QMenu> menu(new QMenu(this));

  QAction *exportAction = new QAction(tr("Export CRC"), menu.data());
  QAction *importAction = new QAction(tr("Import CRC"), menu.data());
  QAction *removeAction = new QAction(tr("Remove CRC"), menu.data());
  QAction *allowUpdateAction = new QAction(tr("Allow CRC Updates"), menu.data());
  QAction *removeItemAction = new QAction(tr("Remove from list"), menu.data());
  allowUpdateAction->setCheckable(true);

  menu->addAction(exportAction);
  menu->addAction(importAction);
  menu->addAction(removeAction);
  menu->addAction(allowUpdateAction);
  menu->addAction(removeItemAction);

  for (const auto &i : selectedItems)
    { // Build list of row numbers selected.  This way we know which items were selected in the ListWidget.
      selectedListIndex.push_back(ui->listWidget->row(i));
    }

  auto checkitFlags = getCheckitOptions(processList[ui->listWidget->currentRow()].file.toStdString().c_str());
  // The checkbox on the context menu will only reflect the single file at the position of the mouse pointer.
  // The change to this one file will be reflected in all others.
  // This isn't ideal, but it works.  Maybe in the future it will show a greyed box if the checkitflags options
  // differ between files.

  if (checkitFlags == UPDATEABLE) {
      allowUpdateAction->setChecked(true);
    } // If not updatable, default is STATIC.  Leave the checkbox as it is (default is unchecked).

  auto x = menu->exec(position);

  if (x == exportAction) {
      for (const auto i : selectedListIndex)
        { // Work through all selected files.
          error = exportCRC(processList[i].file.toStdString().c_str(), flags);
          if (error) {
              rightClickStatusError(tr("Could not export CRC for "));
            }
        }
    } else if (x == importAction) {
      for (const auto i : selectedListIndex)
        {
          error = importCRC(processList[i].file.toStdString().c_str(), flags);
          if (error) {
              rightClickStatusError(tr("Could not import CRC for "));
            }
        }
    } else if (x == removeAction) {
      for (const auto i : selectedListIndex)
        {
          error = removeCRC(processList[i].file.toStdString().c_str());
          if (error) {
              rightClickStatusError(tr("Could not remove CRC for "));
            }
        }
    } else if (x == allowUpdateAction) {
      allowUpdateAction->toggle();

      if (checkitFlags == UPDATEABLE)
        {
          checkitFlags = STATIC;
        } else if (checkitFlags != UPDATEABLE) { checkitFlags = UPDATEABLE; }

      for (const auto i : selectedListIndex)
        {
          error = setCheckitOptions(processList[i].file.toStdString().c_str(), checkitFlags);
          if (error) {
              rightClickStatusError(tr("Could not set checkit attribute for "));
            }
        }
    } else if (x == removeItemAction) {
      for (const auto i : selectedListIndex)
        {
          ui->listWidget->takeItem(i);
          processList.remove(i);
        }
    }
}

