#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "cfg.h"
#include "resultparse.h"
#include "kclog.h"


Config::Config(const char* filename)
{
    root = NULL;
    //полный путь к конфигу
    if (filename == NULL)
	fullname = NULL;
    else
    {
	//хоум каталог
	const char* homepath = getenv("HOME");
	//полный путь
	fullname = (char*)malloc(strlen(homepath)+strlen(filename)+2);
	sprintf(fullname,"%s/%s",homepath,filename);
	kLogPrintf("FULLCFGPATH=%s\n",fullname); fflush(stdout);
    }
    //загружаем если файл уже есть или генерируем дефолтный
    load();
}


Config::~Config()
{
    save();
    if (root != NULL)
	delete root;
    if (fullname == NULL)
	free(fullname);
}


void Config::load()
{
    if (fullname == NULL)
    {
	generatedefault();
	return;
    }

    struct stat st;
    int retcode = lstat(fullname, &st);
    if (retcode != 0) //файла нет?
    {
	//делаем дефолтный
	generatedefault();
	return;
    }
    //читаем файл
    FILE* pfile;
    pfile = fopen(fullname,"r");
    if (pfile!=NULL)
    {
	kLogPrintf("SIZE=%ld\n",st.st_size);
	char* buf = (char*)malloc(st.st_size + 1);
	size_t n = fread (buf,1,st.st_size,pfile);
	buf[n]=0;
	kLogPrintf("%s\n",buf);
	root = xmlparse(buf, st.st_size);
	fclose (pfile);
    }
}


void Config::save()
{
    if (fullname == NULL)
	return;
    if (root == NULL)
	return;
    kLogPrintf("Save Config to FULLCFGPATH=%s\n",fullname);
    Item* cfg = root->findItem("boinctui_cfg");
    if (cfg == NULL)
	return;
    std::string configxml = cfg->toxmlstring();
    kLogPrintf("\n%s\n",cfg->toxmlstring().c_str());
    FILE* pfile = fopen(fullname,"w");
    if (pfile!=NULL)
    {
	fwrite(cfg->toxmlstring().c_str(),1, strlen(cfg->toxmlstring().c_str()),pfile);
	fclose (pfile);
    }
}


void Config::addhost(const char* shost, const char* sport, const char* spwd)
{
    if ( (strlen(shost) == 0)||(strlen(sport) == 0) )
	return; //пустые не заносим

    Item* boinctui_cfg = getcfgptr();
    if (boinctui_cfg == NULL)
	return;

    Item* srv  = new Item("server");

    Item* ip   = new Item("ip");
    ip->appendvalue(shost);
    srv->addsubitem(ip);

    Item* port = new Item("port");
    port->appendvalue(sport);
    srv->addsubitem(port);

    if (strlen(spwd) > 0)
    {
	Item* pwd = new Item("pwd");
	pwd->appendvalue(spwd);
	srv->addsubitem(pwd);
    }

    boinctui_cfg->addsubitem(srv);
}


void Config::generatedefault()
{
    kLogPrintf("generatedafault()\n");
    root       = new Item(""); //корневой
    Item* cfg  = new Item("boinctui_cfg");
    root->addsubitem(cfg);
    addhost("127.0.0.1","31416","");
}
