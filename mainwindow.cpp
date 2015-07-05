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

  for (const auto &x : files)
    {
      checkitFileData entry;
      entry.file = x;
      entry.status = checkitStatus::Unchecked;
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
              entry.status = checkitStatus::Unchecked;
              ui->listWidget->addItem(entry.file);
              processList.append(entry);
            }
        } // end of while
    } // end of range for loop.
} // end of addDirectory()

void MainWindow::check()
{

  if (processList.size() == 0)
    {
      ui->statusbar->showMessage(tr("No files to check."));
      return;
    }

  ui->statusbar->showMessage("");

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
          processList[count].status = checkitStatus::NOCRC;
          ++nocrc;
          ui->listWidget->item(count)->setText(text);
        } else { // Only bother calculating CRC if there is a value to
          // compare against.
          t_crc64 fileCRC = FileCRC64(i.file.toStdString().c_str());

          if (result == fileCRC) {

              ui->listWidget->item(count)->setForeground(Qt::green);
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
          ui->listWidget->item(count)->setForeground(Qt::red);
          text = processList[count].file;
          text += "  [NOT SAVED] : " + QString(errorMessage(result));
          processList[count].status = checkitStatus::FailedSave;
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

void MainWindow::displayErrorLog()
{
  ErrorLog errorlog;
  errorlog.loadResults(processList);
  errorlog.exec();
}

void MainWindow::customContextMenuRequested(const QPoint &pos)
{
  int flags = 0;
  int error;

  auto rightClickStatusError = [&] (QString message) { ui->statusbar->showMessage((message) + processList[ui->listWidget->currentRow()].file + " : " + errorMessage(error)); };

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
        } else { checkitFlags = UPDATEABLE; }

      for (const auto i : selectedListIndex)
        {

          error = setCheckitOptions(processList[i].file.toStdString().c_str(), checkitFlags);

          if (error) {
              rightClickStatusError(tr("Could not set checkit attributefor "));

            }
        }
    }
}

