//---------------------------------------------------------------------------

#ifndef FormCH341DS18B20H
#define FormCH341DS18B20H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <vector>
#include "OneWire.h"
//---------------------------------------------------------------------------
class TfrmCH341DS18B20 : public TForm
{
__published:	// IDE-managed Components
	TMemo *memInfo;
	TLabel *lblPin;
	TComboBox *cbPin;
	TButton *btnScan;
	TLabel *lblDevice;
	TComboBox *cbDevice;
	TCheckBox *chbParasite;
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TLabel *lblTemperature;
	TLabel *lblDetails;
	TLabel *lblResolution;
	TComboBox *cbResolution;
	TCheckBox *chbStore;
	TButton *btnSetResolution;
	TLabel *lblStatus;
	TTimer *tmrAutoRead;
	void __fastcall btnScanClick(TObject *Sender);
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall btnSetResolutionClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:	// User declarations
	CH341OneWire ow;
	std::vector<OneWireRom> devices;	///< temperature sensors found by Scan, item N + 1 of cbDevice
	bool Prepare(void);
	void ShowError(int status);
	/** Selected device: \a rom is NULL (Skip ROM) for the single-device item, family is read from the device then */
	bool GetTarget(const OneWireRom *&rom, OneWireRom &single, uint8_t &family);
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341DS18B20(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341DS18B20 *frmCH341DS18B20;
//---------------------------------------------------------------------------
#endif
