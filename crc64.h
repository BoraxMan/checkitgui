#ifndef CRC64_H
#define CRC64_H
/*  CHECKIT
    A file checksummer and integrity tester
    Copyright (C) 2014 Dennis Katsonis

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

static const uint64_t crc64_tab[256];
uint64_t crc64(uint64_t crc, const unsigned char *s, uint64_t l);

#endif // CRC64_H
