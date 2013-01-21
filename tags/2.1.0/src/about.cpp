#include <string.h>
#include "about.h"

//стрингификатор
#define XSTR(S)		STR(S)
#define STR(S)		#S


AboutWin::AboutWin(int rows, int cols) : NGroup(NRect(rows, cols, getmaxy(stdscr)/2-rows/2,getmaxx(stdscr)/2-cols/2))
{
    caption = strdup(" BOINCTUI ");
    resize(10,getwidth());
    wattrset(win,getcolorpair(COLOR_WHITE, COLOR_BLACK) | A_BOLD);
    box(win,0,0);
    mvwprintw(win,0,getwidth()/2-(strlen(caption)/2),caption);
    char buf[1024];
    snprintf(buf,sizeof(buf),"%s ver %s","BOINC Client manager", XSTR(VERSION));
    mvwprintw(win,3,getwidth()/2-(strlen(buf)/2),buf);
    mvwprintw(win,5,getwidth()/2-(strlen("(c) 2012 Sergey Suslov")/2),"(c) 2012 Sergey Suslov");
    mvwprintw(win,6,getwidth()/2-(strlen("suleman1971@gmail.com")/2),"suleman1971@gmail.com");
}


void AboutWin::eventhandle(NEvent* ev) 	//обработчик событий
{
    NGroup::eventhandle(ev); //предок
    if ( ev->done )
	return;
    if ( ev->type == NEvent::evKB )
    {
	ev->done = true;
	switch(ev->cmdcode)
	{
	    case 27:
	    case KEY_ENTER:
	    case ' ':
	    case '\n':
		putevent(new NEvent(NEvent::evPROG, 3)); //создаем событие с кодом 3 "окно About"
		break;
	} //switch
	if (ev->done) //если обработали, то нужно перерисоваться
	    refresh();
    }
}