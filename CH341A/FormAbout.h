/** \brief "About program" window header file
*/

//----------------------------------------------------------------------------
#ifndef FormAboutH
#define FormAboutH
//----------------------------------------------------------------------------
#include <vcl\System.hpp>
#include <vcl\Windows.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Classes.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\ExtCtrls.hpp>
//----------------------------------------------------------------------------
class TfrmAbout : public TForm
{
__published:
	TPanel *Panel1;
	TImage *ProgramIcon;
	TLabel *ProductName;
	TButton *OKButton;
	TLabel *lblVersion;
	TLabel *lblUrl;
	TLabel *lblBuilt;
	TLabel *lblBuildTimestamp;
	TMemo *Memo;
	TLabel *lblBaresipLicense;
	TLabel *lblCodeguard;
	TLabel *lblCodeGuardState;
	TLabel *lblDebug;
	TLabel *lblDebugState;
	TLabel *lblExeLocation;
	TLabel *lblLocation;
	TLabel *lblBuildConfig;
	void __fastcall lblUrlClick(TObject *Sender);
	void __fastcall lblCodeGuardStateClick(TObject *Sender);
	void __fastcall lblExeLocationClick(TObject *Sender);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
private:
public:
	virtual __fastcall TfrmAbout(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TfrmAbout *frmAbout;
//----------------------------------------------------------------------------
#endif
