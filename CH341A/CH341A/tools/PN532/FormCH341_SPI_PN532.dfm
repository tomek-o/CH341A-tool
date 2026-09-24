object frmCH341_SPI_PN532: TfrmCH341_SPI_PN532
  Left = 0
  Top = 0
  Caption = 'PN532 RFID reader (SPI)'
  ClientHeight = 299
  ClientWidth = 635
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object lblStatus: TLabel
    Left = 8
    Top = 88
    Width = 3
    Height = 13
  end
  object lblReadingState: TLabel
    Left = 170
    Top = 15
    Width = 40
    Height = 13
    Caption = 'Stopped'
  end
  object btnStart: TButton
    Left = 8
    Top = 9
    Width = 145
    Height = 25
    Caption = 'Init + start reading'
    TabOrder = 0
    OnClick = btnStartClick
  end
  object btnStop: TButton
    Left = 8
    Top = 40
    Width = 75
    Height = 25
    Caption = 'Stop'
    Enabled = False
    TabOrder = 1
    OnClick = btnStopClick
  end
  object memo: TMemo
    Left = 8
    Top = 107
    Width = 393
    Height = 150
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 2
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 40
  end
end
