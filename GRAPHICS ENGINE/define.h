/*-----------------------------------------------------------------------------
This source file is part of Daimonin (http://daimonin.sourceforge.net)
Copyright (c) 2004 The Daimonin Team
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/licenses/licenses.html
-----------------------------------------------------------------------------*/

#ifndef DEFINE_H
#define DEFINE_H

#include <string>

/** Only a single chunk is used. **/
//#define SINGLE_CHUNK 1
/** Write timing behaviour into logfile. **/
#define LOG_TIMING   1

const char PRG_NAME[]                = "TileEngine";
const char FILE_LOGGING[]            = "TileEngine_log.html";
const std::string FILE_HEIGHT_MAP    = "Hoehenkarte.png";
const std::string PATH_TILE_TEXTURES = "media/materials/textures/";

#endif
