//---------------------------------------------------------------------------


#pragma hdrstop

#include "TabManager.h"
#include "FormMain.h"
#include "Log.h"

#include <ComCtrls.hpp>
#include <vector>
#include <string>
#include <stdio.h>
#include <assert.h>
//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace
{
	std::vector<TTabSheet*> tabSheets;
	TTabSheet *prevTabSheet = NULL;	
	// startup timing: time between consecutive Register() calls is the creation time of a form
	std::vector<std::string> timings;
	const char *const NL = "\r\n";
	DWORD lastTick = 0;
}

TabManager TabManager::instance;

namespace
{
	/** Milliseconds since this process was created (includes loading of DLLs and static initialization) */
	unsigned long SinceProcessCreation(void)
	{
		FILETIME created, exited, kernel, user, now;
		if (!GetProcessTimes(GetCurrentProcess(), &created, &exited, &kernel, &user))
			return 0;
		GetSystemTimeAsFileTime(&now);
		const unsigned __int64 c = (static_cast<unsigned __int64>(created.dwHighDateTime) << 32) | created.dwLowDateTime;
		const unsigned __int64 n = (static_cast<unsigned __int64>(now.dwHighDateTime) << 32) | now.dwLowDateTime;
		return static_cast<unsigned long>((n - c) / 10000);
	}
}

void TabManager::MarkStartup(const char *label)
{
	char line[200];
	_snprintf(line, sizeof(line), "%5lu ms since process start: %s", SinceProcessCreation(), label);
	line[sizeof(line) - 1] = 0;
	timings.push_back(line);
}

TabManager::TabManager(void):
	pages(NULL),
	tree(NULL)
{

}

void TabManager::Configure(Comctrls::TPageControl *pages, Comctrls::TTreeView *tree) {
	this->pages = pages;
	this->tree = tree;
	MarkStartup("TabManager::Configure");
	tree->Items->BeginUpdate();	// no repainting while ~100 nodes are added, see EndStartup()
	lastTick = GetTickCount();
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
	{
		const DWORD now = GetTickCount();
		char line[200];
		_snprintf(line, sizeof(line), "%5lu ms  before registering %s", static_cast<unsigned long>(now - lastTick), AnsiString(frm->Caption).c_str());
		line[sizeof(line) - 1] = 0;
		timings.push_back(line);
	}
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
	lastTick = GetTickCount();
	if (tabSheets.size() == 60)
		MarkStartup("last Register finished");
	return 0;
}

void TabManager::EndStartup(DWORD processStartTick)
{
	MarkStartup("all forms created");
	if (tree)
		tree->Items->EndUpdate();
	MarkStartup("tree update finished");
	FILE *f = fopen(ChangeFileExt(Application->ExeName, "_startup.txt").c_str(), "w");
	if (!f)
		return;
	fprintf(f, "form creation + constructor time (ms, up to the Register call of the form named):%s", NL);
	for (size_t i=0; i<timings.size(); i++)
		fprintf(f, "%s%s", timings[i].c_str(), NL);
	fprintf(f, "total until Application->Run: %lu ms%s", static_cast<unsigned long>(GetTickCount() - processStartTick), NL);
	fclose(f);
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

