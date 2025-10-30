//---------------------------------------------------------------------------

#ifndef GridUtilsH
#define GridUtilsH
//---------------------------------------------------------------------------

#include <Classes.hpp>
#include <Grids.hpp>

bool __fastcall GetCheckState(TStringGrid& AGrid, int ACol,int ARow);

void __fastcall SetCheckState(TStringGrid& AGrid, int ACol, int ARow, bool AChecked);

void __fastcall InvalidateCell(TStringGrid& AGrid, int ACol, int ARow);

bool __fastcall GetCheckBox(TStringGrid& AGrid, int ACol, int ARow);

void __fastcall SetCheckBox(TStringGrid& AGrid, int ACol, int ARow,	bool AShow, bool AChecked);

bool __fastcall PtInCheckBox(TStringGrid& AGrid, int AX, int AY, int &ACol, int &ARow);

#endif
