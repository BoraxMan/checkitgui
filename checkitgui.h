#ifndef CHECKITGUI_H
#define CHECKITGUI_H

#include <QString>

#define XVERSION "0.1.0"

enum class checkitStatus {
  OK,
  Failed,
  Saved,
  FailedSave,
  NOCRC,
  Unchecked
};

/*
enum function {
  getcrc,
  storecrc
} typedef Function;*/



struct checkitFileData {
  QString file;
  checkitStatus status;
};

#endif // CHECKITGUI_H

