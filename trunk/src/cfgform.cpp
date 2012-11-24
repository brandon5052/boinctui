#include "cfgform.h"
#include "mbstring.h"
#include "kclog.h"


char* rtrim(char* s) //удалить завершающие пробелы в строке
{
    if (s == NULL)
	return NULL;
    if (strlen(s) == 0)
	return s;
    char* p = s + strlen(s) - 1;
    while ( p >= s)
    {
        if (*p == ' ')
	    *p = 0;
	else
	    break;
	p--;
    }
    return s;
}


CfgForm::CfgForm(int rows, int cols, Config* cfg) : NForm(rows,cols)
{
    this->cfg = cfg;
    fields = NULL;
//    text1 = new NStaticText(NRect(1, 44, rect.begrow + 2, rect.begcol + 5));
//    text1->setstring(A_BOLD,"host             port   pwd");
//    insert(text1);
    genfields(false);
    set_form_fields(frm, fields);
    post_form(frm);
}


CfgForm::~CfgForm()
{
    delfields();
}


void CfgForm::delfields()
{
    if (fields != NULL)
    {
	int n = field_count(frm);
	for (int i = 0; i < n; i++)
	    free_field(fields[i]);
	free(fields);
	set_form_fields(frm, NULL);
    }
}
/*
void CfgForm::refresh()
{
    NForm::refresh();
//    mvwprintw(win,0,0,"AAAAAA");
}
*/

void CfgForm::genfields(bool extfields) //создаст массив полей (extfields если нужно добавить хост)
{
    this->extfields = extfields;
    //читаем из конфига
    Item* boinctui_cfg = cfg->getcfgptr();
    if (boinctui_cfg == NULL)
	return;
    std::vector<Item*> slist = boinctui_cfg->getItems("server");
    if (slist.empty())
	extfields = true;
    //поля ввода для серверов
    nhost = slist.size(); //число хостов
    if (extfields)
	nhost++; //новый добавочный хост
    int fcount = nhost*3+1+1+1; //число серверов по 3 поля на каждый +1 для стат текста заголовка +1 для клавиш +1 для NULL
    fields = (FIELD**)malloc((fcount)*sizeof(FIELD*)); //выделяем память под массив полей
    std::vector<Item*>::iterator it;
    int i  = 0; //номер хоста
    int nf = 0; //номер поля
    int nl = 2; //номер экранной строки
    //статический заголовок полей хостов
    fields[nf]   = new_field(1, 44, nl, 5, 0, 0);
    field_opts_off(fields[nf], O_ACTIVE); //статический текст
    set_field_buffer(fields[nf], 0, "host             port   pwd");
    nf++; nl = nl + 1;
    //поля для хостов
    //for (it = slist.begin(); it != slist.end(); it++, i++) //цикл по хостам
    for (i = 0; i < nhost; i++) //цикл по хостам
    {
	//поле для хоста
	fields[nf] = new_field(1, 15, nl,   5, 0, 0);
	set_field_back(fields[nf],COLOR_PAIR(8) | A_BOLD);
        field_opts_off(fields[nf], O_AUTOSKIP);
	//set_field_type(field[nf], TYPE_ALNUM, 0);
	if (i < slist.size())
	{
	    Item* ip = slist[i]->findItem("ip");
	    if (ip != NULL)
		set_field_buffer(fields[nf], 0, ip->getsvalue());
	}
	if (i == 0)
	    set_current_field(frm, fields[nf]); //фокус на поле
	nf++;
	//поле для порта
	fields[nf] = new_field(1, 5, nl, 17+5, 0, 0);
	set_field_back(fields[nf],COLOR_PAIR(8) | A_BOLD);
	set_field_type(fields[nf], TYPE_INTEGER, 0, 0, 65535);
        field_opts_off(fields[nf], O_AUTOSKIP);
	if (i < slist.size())
	{
	    Item* port = slist[i]->findItem("port");
	    if (port != NULL)
		set_field_buffer(fields[nf], 0, port->getsvalue());
	}
	nf++;
	//поле для пароля
	fields[nf] = new_field(1, 20, nl, 29, 0, 0);
	set_field_back(fields[nf],COLOR_PAIR(8) | A_BOLD);
	field_opts_off(fields[nf], O_AUTOSKIP);
	if (i < slist.size())
	{
	    Item* pwd = slist[i]->findItem("pwd");
	    if (pwd != NULL)
		set_field_buffer(fields[nf], 0, pwd->getsvalue());
	}
	nf++;
	nl = nl + 2;
    }
    //клавиши упр-я
    nl++;
    fields[nf] = new_field(1, 44, nl, 5, 0, 0);
    field_opts_off(fields[nf], O_ACTIVE); //статический текст
    if (extfields)
	set_field_buffer(fields[nf], 0, "Esc-Cancel   Enter-Accept");
    else
	set_field_buffer(fields[nf], 0, "Esc-Cancel   Enter-Accept   Ins-Add host");
    nf++; nl = nl + 2;
    //финализация списка полей
    fields[nf] = NULL;
    //пересчитываем высоту формы, чтобы влезли все поля и центрируем
    resize(nl + 1,getwidth());
    move(getmaxy(stdscr)/2-getheight()/2,getmaxx(stdscr)/2-getwidth()/2);
}


void CfgForm::eventhandle(NEvent* ev) 	//обработчик событий
{
    if ( ev->done )
	return;
    if ( ev->type == NEvent::evKB )
    {
	ev->done = true;
        switch(ev->keycode)
	{
	    case KEY_IC: //INSERT
		if (!extfields)
		{
		    unpost_form(frm);
		    delfields();
		    genfields(true);
		    set_form_fields(frm, fields);
		    post_form(frm);
		    refresh();
		    kLogPrintf("INSERT NEW HOST\n");
		}
		break;
	    case '\n': //ENTER
	    {
		form_driver(frm, REQ_NEXT_FIELD); //костыль чтобы текущее поле не потеряло значение
		kLogPrintf("ENTER\n");
		updatecfg(); //обновить данные в cfg
		cfg->save(); //сохранить на диск
		//gsrvlist->refreshcfg();
		//ev->keycode = 27; //костыль чтобы осн программа сдестркутила форму конфига
		NEvent* event = new NEvent(NEvent::evPROG, 1); //создаем програмное событие
		putevent(event);
		break;
	    }
	    case 27:
		kLogPrintf("ESC\n");
		ev->done = false; //нет реакции на этот код (пусть получает владелец)
		break;
	    default:
		kLogPrintf("CfgForm::KEYCODE=%d\n", ev->keycode);
		ev->done = false;
		NForm::eventhandle(ev); //предок
		break;
	} //switch
    }
}


void	CfgForm::updatecfg() //сохраняет данные из формы в cfg
{
    Item* boinctui_cfg = cfg->getcfgptr();
    if (boinctui_cfg == NULL)
	return;
    if (fields == NULL)
	return;
    //удаляем все старые записи "server" из конфига
    std::vector<Item*> slist = boinctui_cfg->getItems("server");
    std::vector<Item*>::iterator it;
    for (it = slist.begin(); it != slist.end(); it++)
	boinctui_cfg->delsubitem(*it);
    //создаем новые записи
    //int n = field_count(frm);
    for (int i = 0; i < nhost; i++) //хосты из формы
    {
	int nf = 1 + i*3; //номер поля для имени хоста
	char* shost = rtrim(field_buffer(fields[nf],0));
	char* sport = rtrim(field_buffer(fields[nf+1],0));
	char* spwd  = rtrim(field_buffer(fields[nf+2],0));
	kLogPrintf("SERVER %d [%s:%s <%s>]\n", i, shost, sport, spwd);
	cfg->addhost(shost, sport, spwd);
    }
}
