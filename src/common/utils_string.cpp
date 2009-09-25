/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of EVEmu: EVE Online Server Emulator
    Copyright 2006 - 2008 The EVEmu Team
    For the latest information visit http://evemu.mmoforge.org
    ------------------------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any later
    version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
    this program; if not, write to the Free Software Foundation, Inc., 59 Temple
    Place - Suite 330, Boston, MA 02111-1307, USA, or go to
    http://www.gnu.org/copyleft/lesser.txt.
    ------------------------------------------------------------------------------------
    Author:     Captnoord
*/

#include "common.h"

#include "PyRep.h"
#include "utils_string.h"

size_t py_mbstowcs( uint16 * wcstr, const char * mbstr, size_t max )
{
    size_t i;
    for (i = 0; i < max; i++, wcstr++, mbstr++) {
        *wcstr = *mbstr;
        if (*wcstr == '\0') {
            break;
        }
    }
    return i;
}

/************************************************************************/
/* ContainsNonPrintables                                                */
/************************************************************************/
bool ContainsNonPrintables( const char *c, uint32 length ) {
    for(; length > 0; c++, length--) {
        if( !isgraph( *c ) && !isspace( *c ) )
            return true;
    }
    return false;
}

bool ContainsNonPrintables( const std::string& str )
{
    return ContainsNonPrintables( str.c_str(), str.size() );
}

bool ContainsNonPrintables( const PyString* str )
{
    return ContainsNonPrintables( str->content() );
}

