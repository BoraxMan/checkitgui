#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_about.h"


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow), rows(0)
{
  ui->setupUi(this);


  //workerthread.moveToThread();
  connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
  connect(ui->actionQuit_2, SIGNAL(triggered()), this, SLOT(close()));
  connect(ui->actionAdd_Directory_2, SIGNAL(triggered()), this, SLOT(addDirectory()));
  connect(ui->actionAdd_File_2, SIGNAL(triggered()), this, SLOT(addFile()));
  connect(ui->actionCheck, SIGNAL(triggered()), this, SLOT(check()));
  connect(ui->actionStore, SIGNAL(triggered()), this, SLOT(store()));
  ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui->listWidget, SIGNAL(customContextMenuRequested(QPoint)), this,
          SLOT(customContextMenuRequested(QPoint)));

}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::addCheckitWidget(checkitFileData &data)
{

  ui->listWidget->addItem(data.file);

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

  for (auto x : files)
    {
      checkitFileData entry;
      entry.file = x;
      entry.status = Unchecked;
      entry.directory = false;
      addCheckitWidget(entry);
      processList.append(entry);

    }
  qDebug() << "Added file";
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

  for (auto x : files)
    {
      QDirIterator dirIterator(x, QDirIterator::Subdirectories);
      while (dirIterator.hasNext())
        {
          checkitFileData entry;

          QFileInfo info;
          entry.file = dirIterator.filePath();
          info = dirIterator.fileInfo();
          if (info.isFile() && (info.isHidden() == false))
            {

              entry.status = Unchecked;
              entry.directory = false;

              addCheckitWidget(entry);
              //   qDebug() << entry.file;
              processList.append(entry);

            }
          dirIterator.next();
        }


    }

}

void MainWindow::check()
{
  int count = 0;
  if (processList.size() == 0)
    {
      ui->statusbar->showMessage(tr("No files to check."));
      return;
    }

  int filesCompleted = 0, failed = 0;
  QStringList failedFileList;
  checkitFileData d;
  QString text;
  ui->progressBar->setMinimum(0);
  ui->progressBar->setMaximum(processList.size());

  //  d.statusIcon->setPixmap(amberlite);
  for (auto &i : processList)
    {
      t_crc64 result = getCRC(i.file.toStdString().c_str());

      if (!result) {

          ui->listWidget->item(count)->setForeground(Qt::yellow);
        } else { // Only bother calculating CRC if there is a value to
          // compare against.
          t_crc64 fileCRC = FileCRC64(i.file.toStdString().c_str());
          //qDebug() << result << fileCRC;

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
      ++filesCompleted;

      ui->progressBar->setValue(filesCompleted);
      ui->listWidget->repaint();
      ++count;
    }

  if (failed) {
      ui->statusbar->showMessage(tr("Some files have failed."));
    } else {
      ui->statusbar->showMessage(tr("All files passed."));
    }
}

void MainWindow::clearList()
{
  processList.clear();
  ui->listWidget->clear();
}

void MainWindow::store()
{
  QString text;

  int count = 0;
  checkitFileData d;
  if (processList.size() == 0)
    {
      ui->statusbar->showMessage(tr("No files to store CRC."));
      return;
    }
  int flags = 0;
  if (ui->overwriteCheckbox->isChecked())
    {
      qDebug() << "Overwrite";
      flags |= OVERWRITE;
    }

  for (auto &i : processList)
    {
      t_crc64 result = putCRC(i.file.toStdString().c_str(), flags);
      //qDebug() << result;
      if (!result) {

          ui->listWidget->item(count)->setForeground(Qt::green);
          text = processList[count].file;
          text += "  [SAVED]";
          ui->listWidget->item(count)->setText(text);
        } else {
          ui->listWidget->item(count)->setForeground(Qt::red);
          text = processList[count].file;
          text += "  [NOT SAVED]";
          ui->listWidget->item(count)->setText(text);
        }
      ++count;
    }
}

void MainWindow::about()
{
  QDialog *dialog = new QDialog(this);
  Ui::Dialog aboutDialog;
  aboutDialog.setupUi(dialog);
  dialog->exec();
  delete dialog;

}

void MainWindow::customContextMenuRequested(const QPoint &pos)
{
  int flags = 0;
  if (ui->overwriteCheckbox->isChecked())
    {
      qDebug() << "Overwrite";
      flags |= OVERWRITE;
    }

  QPoint position = ui->listWidget->mapToGlobal(pos);
  QModelIndex index = ui->listWidget->indexAt(pos);
  QMenu *menu = new QMenu(this);
  QString file;

  menu->addAction(new QAction("Export CRC",this));
  menu->addAction(new QAction("Import CRC",this));
  menu->addAction(new QAction("Remove CRC",this));
  auto x = menu->exec(position);

  if (x->text() == "Export CRC") {
     exportCRC(processList[ui->listWidget->currentRow()].file.toStdString().c_str(), flags);
    } else if (x->text() == "Import CRC") {
       importCRC(processList[ui->listWidget->currentRow()].file.toStdString().c_str(), flags);
    } else if (x->text() == "Remove CRC") {
       removeCRC(processList[ui->listWidget->currentRow()].file.toStdString().c_str());
    }

}
