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

#ifndef CHECKITGUI_H
#define CHECKITGUI_H

#include <QString>

#define XVERSION "0.1.1"

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

