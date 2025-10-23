object frmCH341_I2C_PN532: TfrmCH341_I2C_PN532
  Left = 0
  Top = 0
  Caption = 'PN532 RFID reader'
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
  object btnRead: TButton
    Left = 8
    Top = 40
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 1
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 44
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 500 ms)'
    TabOrder = 2
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
  end
  object btnInit: TButton
    Left = 8
    Top = 9
    Width = 75
    Height = 25
    Caption = 'Init'
    TabOrder = 0
    OnClick = btnInitClick
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
    TabOrder = 3
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 40
  end
end
