//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "FormAbout.h"
//--------------------------------------------------------------------- 
#pragma resource "*.dfm"
TfrmAbout *frmAbout;

//i.e. GetFileVer(Application->ExeName)
AnsiString GetFileVer(AnsiString FileName)
{
    AnsiString asVer="";
    VS_FIXEDFILEINFO *pVsInfo;
    unsigned int iFileInfoSize = sizeof( VS_FIXEDFILEINFO );
    
    int iVerInfoSize = GetFileVersionInfoSize(FileName.c_str(), NULL);
    if (iVerInfoSize!= 0)
    {
        char *pBuf = new char[iVerInfoSize];
        if (GetFileVersionInfo(FileName.c_str(),0, iVerInfoSize, pBuf ) )
        {
            if (VerQueryValue(pBuf, "\\",(void **)&pVsInfo,&iFileInfoSize))
            {
                asVer  = IntToStr( HIWORD(pVsInfo->dwFileVersionMS) )+".";
                asVer += IntToStr( LOWORD(pVsInfo->dwFileVersionMS) )+".";
                asVer += IntToStr( HIWORD(pVsInfo->dwFileVersionLS) )+".";
                asVer += IntToStr( LOWORD(pVsInfo->dwFileVersionLS) );
            }
        }
        delete [] pBuf;
    }
    return asVer;
}

//--------------------------------------------------------------------- 
__fastcall TfrmAbout::TfrmAbout(TComponent* AOwner)
	: TForm(AOwner)
{
	lblVersion->Caption = GetFileVer(Application->ExeName);
	lblBuildTimestamp->Caption = (AnsiString)__DATE__ + ", " __TIME__;
	ProductName->Caption = Application->Title;
}
//---------------------------------------------------------------------
void __fastcall TfrmAbout::lblInfo2Click(TObject *Sender)
{
	ShellExecute(NULL, "open", lblInfo2->Caption.c_str(), NULL, NULL, SW_SHOWNORMAL);	
}
//---------------------------------------------------------------------------

void __fastcall TfrmAbout::FormKeyPress(TObject *Sender, char &Key)
{
	if (Key == VK_ESCAPE)
	{
		Close();
	}
}
//---------------------------------------------------------------------------

