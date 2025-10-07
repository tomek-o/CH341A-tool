//---------------------------------------------------------------------------


#pragma hdrstop

#include "TabManager.h"
#include "FormMain.h"
#include "Log.h"

#include <ComCtrls.hpp>
#include <vector>
#include <assert.h>
//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace
{
	std::vector<TTabSheet*> tabSheets;
	TTabSheet *prevTabSheet = NULL;	
}

TabManager TabManager::instance;

TabManager::TabManager(void):
	pages(NULL),
	tree(NULL)
{

}


int TabManager::Register(TForm *frm)
{
	assert(pages);
	assert(tree);
	TTabSheet *pTabSheet = new TTabSheet(pages);
	pTabSheet->PageControl = pages;
	pTabSheet->Caption = frm->Caption;
	//pTabSheet->Align = alClient;

	TTreeNode *node = tree->Items->Add(NULL, pTabSheet->Caption);
	(void)node;

	pTabSheet->TabVisible = false;
	pTabSheet->Visible = false;

	frm->BorderStyle = bsNone;
	frm->Align = alClient;
	frm->Parent = pTabSheet;
	frm->Visible = true;
	frm->DoubleBuffered = true;

	tabSheets.push_back(pTabSheet);
	return 0;
}

void TabManager::SwitchToPage(unsigned int id)
{
    assert(id < tabSheets.size());
	if (prevTabSheet)
		prevTabSheet->Visible = false;

	TTabSheet *pTabSheet = tabSheets[id];
	pTabSheet->Visible = true;
	if (prevTabSheet != pTabSheet)
	{
		LOG("Switched to [%s] tab\n", pTabSheet->Caption.c_str());
		prevTabSheet = pTabSheet;
    }
}

