//---------------------------------------------------------------------------

#ifndef FormCH341Ina226H
#define FormCH341Ina226H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include <stdint.h>

class TfrmCH341Ina226 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblAddress;
	TComboBox *cbAddress;
	TButton *btnInit;
	TLabel *lblAveraging;
	TComboBox *cbAvgSel;
	TLabel *lblVbusConversion;
	TComboBox *cbVbusConversionSel;
	TLabel *lblShuntConversion;
	TComboBox *cbShuntConversionSel;
	TLabel *lblVbus;
	TEdit *edVbusVoltage;
	TLabel *lblShunt;
	TEdit *edShuntVoltage;
	TEdit *edVbusReg;
	TEdit *edShuntReg;
	TEdit *edShuntCurrent;
	TLabel *lblCalibration;
	TEdit *edShuntResistance;
	TEdit *edShuntMaxExpectedCurrent;
	TLabel *lblPower;
	TEdit *edPower;
	TEdit *edPowerReg;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:	// User declarations
	void Read(void);
	uint8_t GetAddress(void);
	int WriteReg(uint8_t reg, uint16_t val);
	int16_t ReadReg(uint8_t reg);
	double ReadVbusVoltage(void);
	double ReadShuntVoltage(void);
	double currentLsb;
public:		// User declarations
	__fastcall TfrmCH341Ina226(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341Ina226 *frmCH341Ina226;
//---------------------------------------------------------------------------
#endif
