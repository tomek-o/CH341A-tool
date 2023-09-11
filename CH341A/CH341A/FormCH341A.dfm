object frmCH341A: TfrmCH341A
  Left = 0
  Top = 0
  Align = alClient
  BorderStyle = bsNone
  Caption = 'CH341A'
  ClientHeight = 338
  ClientWidth = 651
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object pnlTop: TPanel
    Left = 0
    Top = 0
    Width = 651
    Height = 41
    Align = alTop
    TabOrder = 0
    ExplicitLeft = 400
    ExplicitTop = 40
    ExplicitWidth = 185
    object btnClose: TButton
      Left = 118
      Top = 9
      Width = 100
      Height = 25
      Caption = 'Close CH341A'
      Enabled = False
      TabOrder = 0
      OnClick = btnCloseClick
    end
    object btnOpen: TButton
      Left = 10
      Top = 9
      Width = 100
      Height = 25
      Caption = 'Open CH341A'
      TabOrder = 1
      OnClick = btnOpenClick
    end
  end
  object tvTools: TTreeView
    Left = 0
    Top = 41
    Width = 137
    Height = 297
    Align = alLeft
    HideSelection = False
    Indent = 19
    ReadOnly = True
    RowSelect = True
    ShowRoot = False
    TabOrder = 1
    OnChange = tvToolsChange
  end
  object pages: TPageControl
    Left = 137
    Top = 41
    Width = 514
    Height = 297
    Align = alClient
    TabOrder = 2
    ExplicitLeft = 272
    ExplicitTop = 128
    ExplicitWidth = 289
    ExplicitHeight = 193
  end
  object tmrStartup: TTimer
    Interval = 200
    OnTimer = tmrStartupTimer
    Left = 264
    Top = 8
  end
end
