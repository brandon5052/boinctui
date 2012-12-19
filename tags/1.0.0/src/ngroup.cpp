#include "ngroup.h"


NGroup::~NGroup()
{
    std::list<NView*>::iterator it;
    for(it = items.begin(); it != items.end(); it++)
    {
	delete (*it);
    }
}


void NGroup::setneedrefresh()
{
    NView::setneedrefresh();
    std::list<NView*>::iterator it;
    for(it = items.begin(); it != items.end(); it++) //перерисовать все подэлементы
    {
	(*it)->setneedrefresh();
    }
};


void NGroup::refresh()
{
    NView::refresh(); //предок
    std::list<NView*>::iterator it;
    for(it = items.begin(); it != items.end(); it++) //перерисовать все подэлементы
    {
	(*it)->refresh();
    }
}


void NGroup::eventhandle(NEvent* ev)	//обработчик событий
{
    //если событие уже кем-то обработано, то просто выходим
    if (ev->done)
	return;
    //посылаем событие всем своим подэлементам (последние вставленные
    //получат первыми. Если один из подэл-тов обработал, то выходим
    std::list<NView*>::reverse_iterator rit;
    for(rit = items.rbegin(); rit != items.rend(); rit++) //отправить всем подэлементам
    {
	(*rit)->eventhandle(ev);
	if (ev->done)
	    return; //прекращаем если событие обработано
    }
    //раз подэл-ты не обработали пытаемся обработать самостоятельно
    NView::eventhandle(ev); //предок
}