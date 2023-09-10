//---------------------------------------------------------------------------

#ifndef MruH
#define MruH
//---------------------------------------------------------------------------

#include <System.hpp>
#include <Menus.hpp>
#include <deque>

namespace Json
{
	class Value;
}

/** \brief Most Recently Used
*/
class Mru
{
private:
	enum { MRU_LIMIT = 20 };
	std::deque<AnsiString> MRU;
public:
	Mru(void);
	void Add(AnsiString item);
	void Clear(void);
	void LoadFrom(const Json::Value &jv);
	void SaveTo(Json::Value &jv) const;
	const std::deque<AnsiString>& Get(void) const
	{
		return MRU;
	}
	bool empty(void) const
	{
    	return MRU.empty();
	}
	unsigned int size(void) const
	{
    	return MRU.size();
	}
	void RemoveObsoleteFiles(void);
	AnsiString GetLastDir(void) const;
	AnsiString GetLastFile(void) const;
	typedef void __fastcall (__closure *OnMruClick)(TObject *Sender);
	void UpdateMenu(TMainMenu *menu, TMenuItem *mi, OnMruClick onClick);
private:
	TMainMenu *menu;
	TMenuItem *mi;
	OnMruClick onClick;
	void __fastcall RemoveObsoleteFilesClick(TObject *Sender);
	void __fastcall ClearMruClick(TObject *Sender);
	void RefreshMenu(void);	
};

#endif
