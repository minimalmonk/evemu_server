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
    Author:     Zhur
*/

#include "EVECommonPCH.h"

#include "python/PyRep.h"
#include "python/PyVisitor.h"
#include "python/PyLookupDump.h"
#include "utils/EVEUtils.h"

static const uint32 MIN_RESOLVABLE_INT = 100;

PyLookupDump::PyLookupDump(PyLookupResolver *res, LogType type, bool full_hex, bool full_lists)
: PyLogsysDump(type, full_hex, full_lists),
  m_resolver(res)
{
}

PyLookupDump::PyLookupDump(PyLookupResolver *res, LogType type, LogType hex_type, bool full_hex, bool full_lists)
: PyLogsysDump(type, hex_type, full_hex, full_lists),
  m_resolver(res)
{
}

PyLookupDump::~PyLookupDump() {
}

void PyLookupDump::VisitInteger(const PyInt *rep) {
    const char *look = m_resolver->LookupInt(rep->value());
    if(look != NULL)
        _print("Integer field: "I64u" (%s)", rep->value(), look);
    else
        _print("Integer field: "I64u, rep->value());
}

void PyLookupDump::VisitString(const PyString *rep) {
    if(ContainsNonPrintables( rep )) {
        _print("String%s: '<binary, len=%d>'", rep->isType1()?" (Type1)":"", rep->content().size());
    } else {
        const char *look = m_resolver->LookupString(rep->content().c_str());
        if(look != NULL)
            _print("String%s: '%s' (%s)", rep->isType1()?" (Type1)":"", rep->content().c_str(), look);
        else
            _print("String%s: '%s'", rep->isType1()?" (Type1)":"", rep->content().c_str());
    }
}

PyLookupResolver::PyLookupResolver() {
}

bool PyLookupResolver::LoadIntFile(const char *file) {
    FILE *f = fopen(file, "r");
    if(f == NULL)
        return false;
    char *buf = new char[10240];
    while(fgets(buf, 10240, f)) {
        //kill empty lines
        if(*buf == '\n' || *buf == '\r' || *buf == '\0' || *buf == '|')
            continue;
        //find the first delimiter
        char *p = buf;
        while(*p != '|' && *p != '\0' && *p != '\n' && *p != '\r') {
            p++;
        }
        if(*p == '\0' || *p == '\n' || *p == '\r')
            continue;   //didnt find it
        *p = '\0';
        p++;
        if(*p == '\n' || *p == '\r' || *p == '\0')
            continue;   //skip empty values
        //strip newline
        char *e = p;
        while(*e != '\0' && *e != '\n' && *e != '\r') {
            e++;
        }
        *e = '\0';
        uint32 v = strtoul(buf, NULL, 10);
        if(v < MIN_RESOLVABLE_INT)
            continue;
        m_intRecords[v] = p;
    }
    delete[] buf;
    fclose(f);
    return true;
}

bool PyLookupResolver::LoadStringFile(const char *file) {
    FILE *f = fopen(file, "r");
    if(f == NULL)
        return false;
    char *buf = new char[10240];
    while(fgets(buf, 10240, f)) {
        //kill empty lines
        if(*buf == '\n' || *buf == '\r' || *buf == '\0' || *buf == '|')
            continue;
        //find the first delimiter
        char *p = buf;
        while(*p != '|' && *p != '\0' && *p != '\n' && *p != '\r') {
            p++;
        }
        if(*p == '\0' || *p == '\n' || *p == '\r')
            continue;   //didnt find it
        *p = '\0';
        p++;
        if(*p == '\n' || *p == '\r' || *p == '\0')
            continue;   //skip empty values
        //strip newline
        char *e = p;
        while(*e != '\0' && *e != '\n' && *e != '\r') {
            e++;
        }
        *e = '\0';

        m_strRecords[buf] = p;
    }
    delete[] buf;
    fclose(f);
    return true;
}

const char *PyLookupResolver::LookupInt(uint64 value) const {
    //hackish check for win32 time looking things...
    if(value > 127900000000000000LL && value < 130000000000000000LL) {
        //this is not thread safe or reentrant..
        m_dateBuffer = Win32TimeToString(value);
        return(m_dateBuffer.c_str());
    }

    if(value > 0xFFFFFFFFLL || value < MIN_RESOLVABLE_INT)
        return NULL;
    std::map<uint32, std::string>::const_iterator res;
    res = m_intRecords.find(uint32(value));
    if(res == m_intRecords.end())
        return NULL;
    return(res->second.c_str());
}

const char *PyLookupResolver::LookupString(const char *value) const {
    if(*value == '\0')
        return NULL;
    std::map<std::string, std::string>::const_iterator res;
    res = m_strRecords.find(value);
    if(res == m_strRecords.end())
        return NULL;
    return(res->second.c_str());
}









































