//---------------------------------------------------------------------------

#ifndef FormCH341SpiNrf24L01TxRxH
#define FormCH341SpiNrf24L01TxRxH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include <stdint.h>

class TfrmCH341SpiNrf24L01TxRx : public TForm
{
__published:	// IDE-managed Components
	TLabel *lblNote;
	TLabel *lblRfSpeed;
	TComboBox *cbRfSpeed;
	TLabel *lblRfChannel;
	TComboBox *cbRfChannel;
	TLabel *lblAddressBytes;
	TComboBox *cbAddressBytes;
	TLabel *lblPayloadLength;
	TComboBox *cbPayloadLength;
	TCheckBox *chbAutoAck;
	TRadioGroup *rgMode;
	TButton *btnInit;
	TLabel *lblStatus;
	TGroupBox *grpTx;
	TLabel *lblTxAddress;
	TEdit *edTxAddress;
	TLabel *lblTxData;
	TEdit *edTxData;
	TButton *btnSend;
	TLabel *lblTxStatus;
	TGroupBox *grpRx;
	TLabel *lblRxAddress;
	TEdit *edRxAddress;
	TButton *btnRxRead;
	TCheckBox *chbRxAutoRead;
	TMemo *memoRxLog;
	TTimer *tmrRxAutoRead;
	TButton *btnDumpRegisters;
	TLabel *lblTxAddressHint;
	TLabel *lblRxAddressHint;

	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall btnSendClick(TObject *Sender);
	void __fastcall btnRxReadClick(TObject *Sender);
	void __fastcall tmrRxAutoReadTimer(TObject *Sender);
	void __fastcall chbRxAutoReadClick(TObject *Sender);
	void __fastcall rgModeClick(TObject *Sender);
	void __fastcall btnDumpRegistersClick(TObject *Sender);
private:	// User declarations
	bool initialized;
	/** Re-entrancy guard: SPI/GPIO access from Init/Send/Read/timer must not overlap */
	bool busy;
	/** Payload length configured into the chip at Init (RX_PW_P0 / TX length) */
	uint8_t activePayloadLength;
	void RxRead(void);
	void UpdateModeControlsEnabled(void);
	bool ParseAddress(TEdit *edit, uint8_t addressBytes, uint8_t *addr);
public:		// User declarations
	__fastcall TfrmCH341SpiNrf24L01TxRx(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341SpiNrf24L01TxRx *frmCH341SpiNrf24L01TxRx;
//---------------------------------------------------------------------------
#endif
