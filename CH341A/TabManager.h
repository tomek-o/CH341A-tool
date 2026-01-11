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
};

#endif
