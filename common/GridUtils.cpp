//---------------------------------------------------------------------------


#pragma hdrstop

#include "GridUtils.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

bool __fastcall GetCheckState(
    TStringGrid& AGrid, int ACol,int ARow)
{
    return HIWORD(reinterpret_cast<long>(
                      AGrid.Objects[ACol][ARow]));
}

void __fastcall SetCheckState(
    TStringGrid& AGrid,
    int ACol, int ARow, bool AChecked)
{
    long data = reinterpret_cast<long>(AGrid.Objects[ACol][ARow]);
    AGrid.Objects[ACol][ARow] = reinterpret_cast<TObject*>(MAKELONG(LOWORD(data), AChecked));
}

void __fastcall InvalidateCell(
    TStringGrid& AGrid, int ACol, int ARow)
{
    AGrid.Objects[ACol][ARow] =
        AGrid.Objects[ACol][ARow];
}

bool __fastcall GetCheckBox(
    TStringGrid& AGrid, int ACol, int ARow)
{
    return LOWORD(reinterpret_cast<long>(
                      AGrid.Objects[ACol][ARow]));
}

void __fastcall SetCheckBox(
    TStringGrid& AGrid, int ACol, int ARow,
    bool AShow, bool AChecked)
{
    AGrid.Objects[ACol][ARow] = reinterpret_cast<TObject*>(MAKELONG(AShow, false));
    SetCheckState(AGrid, ACol, ARow, AChecked);
}

bool __fastcall PtInCheckBox(
    TStringGrid& AGrid, int AX, int AY,
    int &ACol, int &ARow)
{
    AGrid.MouseToCell(AX, AY, ACol, ARow);
    RECT RCell = static_cast<RECT>(
                     AGrid.CellRect(ACol, ARow));

    OffsetRect(&RCell, 2, (int)(0.5 * (RCell.bottom - RCell.top)));
    RCell.right = RCell.left +
                  GetSystemMetrics(SM_CXMENUCHECK);
    RCell.bottom = RCell.top +
                   GetSystemMetrics(SM_CYMENUCHECK);
    RCell.top -= (int)(0.5 * (RCell.bottom - RCell.top)) + 2;

    return
        PtInRect(&RCell, Point(AX, AY));
}
