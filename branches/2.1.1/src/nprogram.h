// =============================================================================
// This file is part of boinctui.
// http://boinctui.googlecode.com
// Copyright (C) 2012,2013 Sergey Suslov
//
// boinctui is free software; you can redistribute it and/or modify it  under
// the terms of the GNU General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// boinctui is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details
// <http://www.gnu.org/licenses/>.
// =============================================================================

#ifndef NPROGRAM_H
#define NPROGRAM_H

#include <queue>
#include "ngroup.h"


class NProgram : public NGroup
{
  public:
    static bool	needresize; 	//true если произошло изменение размеров терминала
    NProgram();
    virtual void putevent(NEvent* ev); //отправить событие по цепочке владельцев в очередь
    static void sig_winch(int signo); //вызывается при изменении размеров терминала
  protected:
    std::queue<NEvent*> evqueue; //очередь событий
};

#endif //NPROGRAM_H