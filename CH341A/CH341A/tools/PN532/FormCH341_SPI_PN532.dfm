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
  DesignSize = (
    635
    299)
  PixelsPerInch = 96
  TextHeight = 13
  object lblStatus: TLabel
    Left = 8
    Top = 88
    Width = 3
    Height = 13
  end
  object lblReadingState: TLabel
    Left = 226
    Top = 14
    Width = 40
    Height = 13
    Caption = 'Stopped'
  end
  object btnStart: TButton
    Left = 8
    Top = 9
    Width = 121
    Height = 25
    Caption = 'Init + start reading'
    TabOrder = 0
    OnClick = btnStartClick
  end
  object btnStop: TButton
    Left = 135
    Top = 9
    Width = 75
    Height = 25
    Caption = 'Stop'
    Enabled = False
    TabOrder = 1
    OnClick = btnStopClick
  end
  object btnReadNdef: TButton
    Left = 391
    Top = 8
    Width = 110
    Height = 25
    Caption = 'Read NDEF'
    TabOrder = 3
    Visible = False
    OnClick = btnReadNdefClick
  end
  object memo: TMemo
    Left = 8
    Top = 44
    Width = 619
    Height = 247
    Anchors = [akLeft, akTop, akRight, akBottom]
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
    Left = 512
    Top = 8
  end
end
