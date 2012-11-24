#include <string.h>
#include "msgwin.h"
#include "net.h"
#include "resultparse.h"


void MsgWin::updatedata() //обновить данные с сервера
{
    if (srv == NULL)
	return;
    srv->updatemsgs(); //обновить данные в хранилище
    if (srv->msgdom == NULL)
    {
	clearcontent();
	return;
    }
    if (lastmsgno == srv->lastmsgno) //в srv есть новые еще не отрисованные сообщения?
	return;
    // === дополняем массив визуальных строк ===
    if (lastmsgno == 0)
        clearcontent(); //очищаем если отображение идет с начала
    Item* msgs = srv->msgdom->findItem("msgs");
    if (msgs != NULL)
    {
	std::vector<Item*> mlist = msgs->getItems("msg");
	std::vector<Item*>::iterator it;
	for (it = mlist.begin(); it != mlist.end(); it++) //цикл по сообщениям
	{
	    Item* number = (*it)->findItem("seqno"); 	//номер текущего сообщения
	    if (number->getivalue()<=lastmsgno)
		continue; //пропускаем старые (уже добавленные ранее) сообщения
	    Item* body = (*it)->findItem("body"); 	//текст сообщения
	    Item* time = (*it)->findItem("time"); 	//время сообщения
	    time_t t = time->getivalue(); 		//время в секундах от 1.1.1970
	    //wattron(win,A_BOLD);
	    addstring(COLOR_PAIR(5),"---> #%d %s", number->getivalue(),ctime(&t));
	    //wattroff(win,A_BOLD);
	    addstring(0, "%s", body->getsvalue());
	}
    }
    lastmsgno = srv->lastmsgno;
    //поскольку есть новые сообщения, то делаем автоскроллинг в конец
    setautoscroll(true);
}


void MsgWin::eventhandle(NEvent* ev) 	//обработчик событий
{
    NScrollView::eventhandle(ev); //предок
    if ( ev->done )
	return;
    if ( ev->type == NEvent::evKB )
    {
	ev->done = true;
        switch(ev->keycode)
	{
	    case KEY_PPAGE:
		//wprintw(win,"PgUp");
		scrollto(-getheight()/2); //вверх на полокна
		setautoscroll(false);
		break;
	    case KEY_NPAGE:
		//wprintw(win,"PgDn");
		if (!getautoscroll())
		{
		    int oldpos = startindex;
		    scrollto(getheight()/2); 	//вниз на пол окна
		    if ( oldpos == startindex) 	//позиция не изменилась (уже достигли конца)
			setautoscroll(true);	//включаем автоскроллинг
		}
		break;
	    default:
		ev->done = false; //нет реакции на этот код
	} //switch
	if (ev->done) //если обработали, то нужно перерисоваться
	    refresh();
    }
}
