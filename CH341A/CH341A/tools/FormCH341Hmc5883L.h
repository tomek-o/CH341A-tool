//---------------------------------------------------------------------------

#ifndef FormCH341Hmc5883LH
#define FormCH341Hmc5883LH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include <stdint.h>
#include <vector>

class TfrmCH341Hmc5883L : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TButton *btnInit;
	TLabel *lblLegend;
	TLabel *lblX;
	TEdit *edRawX;
	TEdit *edMagneticInductionX;
	TLabel *lblY;
	TEdit *edRawY;
	TEdit *edMagneticInductionY;
	TLabel *lblZ;
	TEdit *edRawZ;
	TEdit *edMagneticInductionZ;
	TLabel *lblAveraging;
	TComboBox *cbAvgSel;
	TLabel *lblOutputRate;
	TComboBox *cbOutputRateSel;
	TLabel *lblRange;
	TComboBox *cbGainSel;
	TLabel *lblBias;
	TComboBox *cbMeasModeSel;
	TLabel *lblNote;
	TLabel *lblCalibration;
	TCheckBox *chbCalibrationCollect;
	TButton *btnCopyData;
	TButton *btnResetData;
	TLabel *lblPointsCount;
	TLabel *lblMiddleX;
	TEdit *edMiddleX;
	TLabel *lblMiddleY;
	TEdit *edMiddleY;
	TEdit *edOffsetX;
	TEdit *edOffsetY;
	TEdit *edOffsetZ;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall btnResetDataClick(TObject *Sender);
	void __fastcall btnCopyDataClick(TObject *Sender);
private:	// User declarations
	void Read(void);
	struct DataPoint
	{
		int16_t X, Y, Z;
		DataPoint(void):
			X(0), Y(0), Z(0)
		{}
	};
	std::vector<DataPoint> dataPoints;
	void UpdateDataStats(void);
	double GetGaussPerLsb(void);
public:		// User declarations
	__fastcall TfrmCH341Hmc5883L(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341Hmc5883L *frmCH341Hmc5883L;
//---------------------------------------------------------------------------
#endif
