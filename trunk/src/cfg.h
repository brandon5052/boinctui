// =============================================================================
// This file is part of boinctui.
// http://boinctui.googlecode.com
// Copyright (C) 2012,2013 Sergey Suslov
//
// boinctui is free software; you can redistribute it and/or modify it  under
// the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// boinctui is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details
// <http://www.gnu.org/licenses/>.
// =============================================================================

#ifndef CFG_H
#define CFG_H

#include "resultdom.h"

class Config
{
  public:
    Config(const char* filename);
    ~Config();
    void  load();
    void  save();
    void  generatedefault();
    Item* getcfgptr() { if ( root!= NULL) return root->findItem("boinctui_cfg"); else return NULL; };
    void  addhost(const char* host, const char* port, const char* pwd);
  protected:
    char* fullname;	//полное имя файла
    Item* root;  	//корень дерева конфига
};


#endif //CFG_H