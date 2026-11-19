//---------------------------------------------------------------------------

#ifndef TabManagerH
#define TabManagerH
//---------------------------------------------------------------------------

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include "ToolGroups.h"

namespace Comctrls
{
	class DELPHICLASS TTreeView;
	class DELPHICLASS TPageControl;
	class DELPHICLASS TTabSheet;
}

class TabManager
{
private:
	TabManager(void);
	virtual ~TabManager(void) {};
	TabManager(const TabManager& source) {};
	TabManager& operator=(const TabManager&);
	static TabManager instance;
	Comctrls::TPageControl *pages;
	Comctrls::TTreeView *tree;
public:
	static TabManager& Instance() {
		return instance;
	}
	void Configure(Comctrls::TPageControl *pages, Comctrls::TTreeView *tree);
	int Register(TForm *frm, unsigned int groups);
	void SwitchToPage(Comctrls::TTabSheet *tab);
	/** Call after all forms are created: ends the batched tree update, writes startup_timing.txt (time per form) */
	void EndStartup(DWORD processStartTick);
	/** Startup diagnostics: remembers "label at N ms since process creation" for startup_timing.txt */
	static void MarkStartup(const char *label);
};

#endif
