//---------------------------------------------------------------------------


#pragma hdrstop

#include "TabManager.h"
#include "FormMain.h"
#include "Log.h"

#include <ComCtrls.hpp>
#include <vector>
#include <string>
#include <assert.h>
//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace
{
	struct ToolEntry
	{
		std::string className;
		std::string caption;
		unsigned int rank;	// position in the order list
		unsigned int groups;
		ToolFormFactory factory;
		TTabSheet *sheet;
		TForm *form;
	};

	/** Function-local, since tools register before main() */
	std::vector<ToolEntry>& Tools(void)
	{
		static std::vector<ToolEntry> tools;
		return tools;
	}

	TTabSheet *prevTabSheet = NULL;
	const char *const *toolOrder = NULL;

	bool ToolBefore(const ToolEntry &a, const ToolEntry &b)
	{
		if (a.rank != b.rank)
			return a.rank < b.rank;
		if (a.caption != b.caption)
			return a.caption < b.caption;
		return a.className < b.className;
	}

	/** Caption of a form read from its DFM resource, without creating the form (class name if not found) */
	AnsiString ReadFormCaption(const std::string &className)
	{
		AnsiString caption = className.c_str();
		TResourceStream *stream = NULL;
		TStringStream *text = NULL;
		try
		{
			stream = new TResourceStream(reinterpret_cast<int>(HInstance), className.c_str(), RT_RCDATA);
			text = new TStringStream("");
			ObjectBinaryToText(stream, text);
			// the form's own properties are indented by 2 spaces, those of its components by more
			const AnsiString tag = "\n  Caption = '";
			const AnsiString dfm = text->DataString;
			const int pos = dfm.Pos(tag);
			if (pos > 0)
			{
				AnsiString value;
				for (int i = pos + tag.Length(); i <= dfm.Length(); i++)
				{
					if (dfm[i] == '\'')
					{
						if (i < dfm.Length() && dfm[i + 1] == '\'')	// doubled quote
							i++;
						else
							break;
					}
					value += dfm[i];
				}
				caption = value;
			}
		}
		catch (Exception &)
		{
		}
		delete text;
		delete stream;
		return caption;
	}

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

TabManager TabManager::instance;

TabManager::TabManager(void):
	pages(NULL),
	tree(NULL)
{

}

void TabManager::SetToolOrder(const char *const *classNames)
{
	toolOrder = classNames;
}

void TabManager::AddTool(const char *className, unsigned int groups, ToolFormFactory factory)
{
	ToolEntry entry;
	entry.className = className;
	entry.groups = groups;
	entry.rank = 0xFFFFFFFFu;
	entry.factory = factory;
	entry.sheet = NULL;
	entry.form = NULL;
	Tools().push_back(entry);
}

void TabManager::Configure(Comctrls::TPageControl *pages, Comctrls::TTreeView *tree) {
	this->pages = pages;
	this->tree = tree;

	tree->Items->BeginUpdate();	// no repainting while ~100 nodes are added
	for (int i=0; i<ToolGroup_Limiter; i++)
	{
		TTreeNode *node = tree->Items->Add(NULL, GetToolGroupName(static_cast<ToolGroup>(i)));
		(void)node;
	}

	std::vector<ToolEntry> &tools = Tools();
	for (size_t t=0; t<tools.size(); t++)
	{
		ToolEntry &tool = tools[t];
		tool.caption = ReadFormCaption(tool.className).c_str();
		tool.rank = 0xFFFFFFFFu;	// not listed: after the listed ones
		for (unsigned int i=0; toolOrder && toolOrder[i]; i++)
		{
			if (tool.className == toolOrder[i])
			{
				tool.rank = i;
				break;
			}
		}
	}
	// insertion sort (a few dozen entries)
	for (size_t i=1; i<tools.size(); i++)
	{
		const ToolEntry entry = tools[i];
		size_t j = i;
		while (j > 0 && ToolBefore(entry, tools[j - 1]))
		{
			tools[j] = tools[j - 1];
			j--;
		}
		tools[j] = entry;
	}

	for (size_t t=0; t<tools.size(); t++)
	{
		ToolEntry &tool = tools[t];

		TTabSheet *pTabSheet = new TTabSheet(pages);
		pTabSheet->PageControl = pages;
		pTabSheet->Caption = tool.caption.c_str();
		pTabSheet->Tag = static_cast<int>(t);	// index in Tools()

		unsigned int groups = tool.groups + (1u << ToolGroupAll);

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
		tool.sheet = pTabSheet;
	}
	tree->Items->EndUpdate();
}

void TabManager::SwitchToPage(Comctrls::TTabSheet *tab)
{
	if (prevTabSheet)
		prevTabSheet->Visible = false;

	std::vector<ToolEntry> &tools = Tools();
	assert(tab->Tag >= 0 && static_cast<size_t>(tab->Tag) < tools.size());
	ToolEntry &tool = tools[tab->Tag];
	if (!tool.form)
	{
		const DWORD start = GetTickCount();
		TForm *frm = tool.factory();
		frm->BorderStyle = bsNone;
		frm->Align = alClient;
		frm->Parent = tab;
		frm->DoubleBuffered = true;
		tool.form = frm;
		LOG("Created tool form [%s] in %lu ms\n", tool.caption.c_str(), static_cast<unsigned long>(GetTickCount() - start));
	}

	tab->Visible = true;
	if (!tool.form->Visible)
		tool.form->Visible = true;
	if (prevTabSheet != tab)
	{
		LOG("Switched to [%s] tab\n", tab->Caption.c_str());
		prevTabSheet = tab;
	}
}

void TabManager::LogStartupTime(void)
{
	LOG("Startup took %lu ms\n", SinceProcessCreation());
}
