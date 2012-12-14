#ifndef NMENU_H
#define NMENU_H

#include <menu.h>
#include <stdlib.h>
#include "ngroup.h"

class NMenu : public NGroup
{
  public:
    NMenu(NRect rect);
    virtual ~NMenu();
    virtual void refresh();
    void 	 eventhandle(NEvent* ev); 	//обработчик событий
    void	 setbackground(int attr) { set_menu_back(menu, attr); wattrset(win, attr); bgattr = attr; };
    virtual bool createsubmenu() { return false; }; //открыть субменю
    virtual void destroysubmenu(); //закрыть субменю
    virtual bool action() { return false; }; //вызывается при нажатии Enter
  protected:
    MENU*	menu;
    ITEM** 	mitems; //масив элементов
    int		bgattr; //цвет фона
};

#endif //NMENU_H