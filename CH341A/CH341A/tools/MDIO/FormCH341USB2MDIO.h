//---------------------------------------------------------------------------

#ifndef FormCH341USB2MDIOH
#define FormCH341USB2MDIOH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------

#include <vector>

class TfrmCH341USB2MDIO : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TListView *lvScript;
	TButton *btnLoadScript;
	TLabel *lblInfo;
	TLabel *lblPhyAddress;
	TComboBox *cbPhyAddress;
	TButton *btnRunScript;
	TLabel *lblStatus;
	TOpenDialog *openDialog;
	void __fastcall btnLoadScriptClick(TObject *Sender);
	void __fastcall lvScriptData(TObject *Sender, TListItem *Item);
	void __fastcall btnRunScriptClick(TObject *Sender);
private:	// User declarations
	struct ScriptLine {
		AnsiString script;
		AnsiString result;
	};
	std::vector<ScriptLine> scriptLines;
	void Load(AnsiString fileName);
	int RunLine(ScriptLine &scriptLine);
public:		// User declarations
	__fastcall TfrmCH341USB2MDIO(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341USB2MDIO *frmCH341USB2MDIO;
//---------------------------------------------------------------------------
#endif
