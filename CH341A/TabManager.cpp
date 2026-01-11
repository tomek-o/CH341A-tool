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

void TabManager::Configure(Comctrls::TPageControl *pages, Comctrls::TTreeView *tree) {
	this->pages = pages;
	this->tree = tree;
	for (int i=0; i<ToolGroup_Limiter; i++)
	{
		TTreeNode *node = tree->Items->Add(NULL, GetToolGroupName(static_cast<ToolGroup>(i)));
		(void)node;
	}
}

int TabManager::Register(TForm *frm, unsigned int groups)
{
	assert(pages);
	assert(tree);
	TTabSheet *pTabSheet = new TTabSheet(pages);
	pTabSheet->PageControl = pages;
	pTabSheet->Caption = frm->Caption;
	//pTabSheet->Align = alClient;

	groups += (1<<ToolGroupAll);

	TTreeNode *parentNode = tree->Items->Item[0];
	for (unsigned int i=0; i<32; i++)
	{
		if (i >= ToolGroup_Limiter)
			break;
		if (groups & (1u<<i))
		{
			TTreeNode *node = tree->Items->AddChild(parentNode, pTabSheet->Caption);
			node->Data = pTabSheet;
		}
		parentNode = parentNode->getNextSibling();
	}

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

void TabManager::SwitchToPage(Comctrls::TTabSheet *tab)
{
	if (prevTabSheet)
		prevTabSheet->Visible = false;

	tab->Visible = true;
	if (prevTabSheet != tab)
	{
		LOG("Switched to [%s] tab\n", tab->Caption.c_str());
		prevTabSheet = tab;
    }
}

