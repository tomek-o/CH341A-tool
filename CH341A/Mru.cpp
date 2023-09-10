//---------------------------------------------------------------------------


#pragma hdrstop

#include "Mru.h"
#include <SysUtils.hpp>
#include <json/json.h>
#include <assert.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

Mru::Mru(void):
	menu(NULL),
	mi(NULL),
	onClick(NULL)
{

}

void Mru::Add(AnsiString item)
{
	// check if item is already in MRU list
	std::deque<AnsiString>::iterator iter;
	for (iter=MRU.begin(); iter != MRU.end(); )
	{
		if (*iter == item)
		{
			iter = MRU.erase(iter);
		}
		else
		{
            ++iter;
        }
	}
	// push item on top
	MRU.push_back(item);
	while (MRU.size() > MRU_LIMIT)
	{
		MRU.pop_front();
	}
	RefreshMenu();
}

void Mru::Clear(void)
{
	MRU.clear();
}

void Mru::LoadFrom(const Json::Value &jv)
{
	MRU.clear();
	for (unsigned int i=0; i<std::min<unsigned>(jv.size(), MRU_LIMIT); i++)
	{
		MRU.push_back(jv[i].asString().c_str());
	}
}

void Mru::SaveTo(Json::Value &jv) const
{
	jv.resize(0);
	for (unsigned int i=0; i<MRU.size(); i++)
	{
		jv[i] = MRU[i].c_str();
	}
}

void Mru::RemoveObsoleteFiles(void)
{
	std::deque<AnsiString>::iterator iter;
	for (iter = MRU.begin(); iter != MRU.end(); )
	{
		AnsiString name = *iter;
		bool exist = FileExists(name);
		if (exist)
		{
			++iter;
		}
		else
		{
			iter = MRU.erase(iter);
        }
	}
}

AnsiString Mru::GetLastDir(void) const
{
	if (MRU.empty() == false)
	{
		AnsiString fileName = MRU.back();
		return ExtractFileDir(fileName);
	}
	return "";
}

AnsiString Mru::GetLastFile(void) const
{
	if (MRU.empty() == false)
	{
		AnsiString fileName = MRU.back();
		return fileName;
	}
	return "";
}

void Mru::UpdateMenu(TMainMenu *menu, TMenuItem *mi, OnMruClick onClick)
{
	assert(menu);
	this->menu = menu;
	assert(mi);
	this->mi = mi;
	assert(onClick);
	this->onClick = onClick;

	RefreshMenu();
}

void Mru::RefreshMenu(void)
{
	if (menu == NULL || mi == NULL)
		return;
		 
	mi->Clear();
	TMenuItem *item;
	mi->AutoHotkeys = maManual;
	if (MRU.empty())
	{
		item = new TMenuItem(menu);
		item->AutoHotkeys = maManual;
		item->Caption = "- empty -";
		item->Enabled = false;
		mi->Insert(0, item);
	}
	for (unsigned int i=0; i<MRU.size(); i++)
	{
		item = new TMenuItem(menu);
		item->AutoHotkeys = maManual;
		item->Caption = MRU[i];
		item->OnClick = onClick;
		mi->Insert(0, item);
	}
	item = new TMenuItem(menu);
	item->Caption = "-";	// separator
	mi->Add(item);
	item = new TMenuItem(menu);
	item->Caption = "Remove obsolete files";
	item->OnClick = RemoveObsoleteFilesClick;
	mi->Add(item);
	item = new TMenuItem(menu);
	item->Caption = "Clear list";
	item->OnClick = ClearMruClick;
	mi->Add(item);
}

void __fastcall Mru::RemoveObsoleteFilesClick(TObject *Sender)
{
	RemoveObsoleteFiles();
	UpdateMenu(menu, mi, onClick);
}

void __fastcall Mru::ClearMruClick(TObject *Sender)
{
	Clear();
	UpdateMenu(menu, mi, onClick);	
}


