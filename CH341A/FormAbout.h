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
	TLabel *Version;
	TLabel *Copyright;
	TButton *OKButton;
	TLabel *lblVersion;
	TLabel *lblInfo1;
	TLabel *lblInfo2;
	TLabel *lblInfo3;
	TLabel *lblBuilt;
	TLabel *lblBuildTimestamp;
	void __fastcall lblInfo2Click(TObject *Sender);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
private:
public:
	virtual __fastcall TfrmAbout(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TfrmAbout *frmAbout;
//----------------------------------------------------------------------------
#endif
