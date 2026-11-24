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

//---------------------------------------------------------------------------

/** Creates a tool form (the form is owned by Application) */
typedef TForm* (*ToolFormFactory)(void);

/** Manages the tool tree and tool tabs.

	Tool forms register themselves with REGISTER_TOOL (in the form's .cpp file); the tree node and
	the empty tab are created at startup (the title is the Caption read from the form's DFM resource),
	but the form itself is created the first time its tab is opened.
	Tool forms therefore must not be in the project's auto-create list.
*/
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
	/** Called before main() by REGISTER_TOOL; \a className is the form class name (the name of its DFM resource) */
	static void AddTool(const char *className, unsigned int groups, ToolFormFactory factory);

	/** Optional order of the tools: NULL-terminated array of form class names. Tools that are not listed
		follow the listed ones in alphabetical order. Call before Configure(). */
	static void SetToolOrder(const char *const *classNames);

	/** Creates the tree nodes and tabs of all registered tools */
	void Configure(Comctrls::TPageControl *pages, Comctrls::TTreeView *tree);
	/** Shows the tab, creating its form if this is the first time */
	void SwitchToPage(Comctrls::TTabSheet *tab);

	/** Writes the startup time to the log */
	void LogStartupTime(void);
};

class ToolRegistrar
{
public:
	ToolRegistrar(const char *className, unsigned int groups, ToolFormFactory factory)
	{
		TabManager::AddTool(className, groups, factory);
	}
};

/** Registers a tool form; place after the definition of the form's global pointer:
		REGISTER_TOOL(TfrmMyTool, frmMyTool, 1u << ToolGroupMisc)
	\a groups is a mask of ToolGroup values. The tab / tree title is the form's Caption.
*/
#define REGISTER_TOOL(FormClass, formVar, groups) \
	static TForm* CreateTool##FormClass(void) \
	{ \
		Application->CreateForm(__classid(FormClass), &formVar); \
		return formVar; \
	} \
	static ToolRegistrar toolRegistrar##FormClass(#FormClass, (groups), CreateTool##FormClass);

#endif
