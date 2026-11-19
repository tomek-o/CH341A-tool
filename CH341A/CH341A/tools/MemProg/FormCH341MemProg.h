//---------------------------------------------------------------------------

#ifndef FormCH341MemProgH
#define FormCH341MemProgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include "MemProg.h"
#include "FrameHexEditor.h"
//---------------------------------------------------------------------------
class TfrmCH341MemProg : public TForm
{
__published:	// IDE-managed Components
	TPageControl *pcDevice;
	TTabSheet *tsSpiFlash;
	TLabel *lblSpiSize;
	TLabel *lblSpiPage;
	TComboBox *cbSpiSize;
	TComboBox *cbSpiPage;
	TButton *btnReadId;
	TButton *btnUnprotect;
	TLabel *lblEraseMode;
	TComboBox *cbEraseMode;
	TLabel *lblProgMode;
	TComboBox *cbProgMode;
	TTabSheet *tsSpiEeprom;
	TLabel *lblSpiEeType;
	TLabel *lblSpiEePage;
	TComboBox *cbSpiEeType;
	TComboBox *cbSpiEePage;
	TButton *btnSpiEeUnprotect;
	TTabSheet *tsAt45;
	TButton *btnAt45Detect;
	TButton *btnAt45Unprotect;
	TTabSheet *tsMicrowire;
	TLabel *lblMwType;
	TLabel *lblMwOrg;
	TComboBox *cbMwType;
	TComboBox *cbMwOrg;
	TTabSheet *tsI2cEeprom;
	TLabel *lblI2cType;
	TLabel *lblI2cAddr;
	TComboBox *cbI2cType;
	TComboBox *cbI2cAddr;
	TButton *btnProbe;
	TCheckBox *chkVerifyAfterWrite;
	TButton *btnRead;
	TButton *btnWrite;
	TButton *btnVerify;
	TButton *btnErase;
	TButton *btnCancel;
	TProgressBar *progressBar;
	TMemo *memoLog;
	TPanel *pnlHex;
	TCheckBox *chkWriteEdited;
	TLabel *lblAt45Info;
	void __fastcall pcDeviceChange(TObject *Sender);
	void __fastcall btnReadIdClick(TObject *Sender);
	void __fastcall btnUnprotectClick(TObject *Sender);
	void __fastcall btnProbeClick(TObject *Sender);
	void __fastcall btnAt45DetectClick(TObject *Sender);
	void __fastcall btnAt45UnprotectClick(TObject *Sender);
	void __fastcall cbSpiEeTypeChange(TObject *Sender);
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall btnWriteClick(TObject *Sender);
	void __fastcall btnVerifyClick(TObject *Sender);
	void __fastcall btnEraseClick(TObject *Sender);
	void __fastcall btnCancelClick(TObject *Sender);
private:	// User declarations
	TframeHexEditor *hexFrame;
	bool busy;
	int imageDevice;	// device the editor contents were read from / written to, -1 if none
	bool cancelRequested;
	MemProg::Progress *progress;

	MemProg::SpiFlash spi;	// SPI flash and SPI EEPROM
	MemProg::Microwire microwire;
	MemProg::At45 at45;
	MemProg::I2cEeprom i2cEeprom;

	friend class UiProgress;

	int GetDevice(void) const;
	bool CheckOpened(void);
	/** Checks if the selected device is present, if it can be told (I2C only) */
	bool CheckPresent(void);
	static AnsiString DescribeI2cScan(const MemProg::I2cScanResult &scan);
	/** Applies chip settings from UI to selected device object, returns chip size or 0 on error */
	unsigned long Configure(void);
	void SetBusy(bool state);
	void Log(const AnsiString &text);
	void LogResult(const char *operation, int status, DWORD startTick);

	/** Gets data to write/verify from the hex editor window and checks it against the chip */
	bool GetEditorData(unsigned long chipSize, std::vector<uint8_t> &data);

	int DoRead(uint8_t *data, unsigned long size);
	int DoWrite(uint32_t addr, const uint8_t *data, unsigned long size, bool skipBlank);
	/** Writes only what was edited in the editor since it was read from the chip */
	int WriteEdited(const std::vector<uint8_t> &data);
	int DoVerify(const uint8_t *data, unsigned long size, uint32_t &mismatchAddr);
	int DoErase(void);
public:		// User declarations
	__fastcall TfrmCH341MemProg(TComponent* Owner);
	__fastcall ~TfrmCH341MemProg(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341MemProg *frmCH341MemProg;
//---------------------------------------------------------------------------
#endif
