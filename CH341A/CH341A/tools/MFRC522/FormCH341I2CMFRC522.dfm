object frmCH341I2CMFRC522: TfrmCH341I2CMFRC522
  Left = 0
  Top = 0
  Caption = 'MFRC522 RFID I2C'
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
    Top = 105
    Width = 3
    Height = 13
  end
  object lblAddress: TLabel
    Left = 8
    Top = 8
    Width = 58
    Height = 13
    Caption = 'I2C address'
  end
  object btnRead: TButton
    Left = 8
    Top = 64
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 0
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 68
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 500 ms)'
    TabOrder = 1
  end
  object cbAddress: TComboBox
    Left = 72
    Top = 5
    Width = 97
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    ParentFont = False
    TabOrder = 2
  end
  object btnInit: TButton
    Left = 8
    Top = 33
    Width = 75
    Height = 25
    Caption = 'Init'
    TabOrder = 3
    OnClick = btnInitClick
  end
  object memo: TMemo
    Left = 8
    Top = 131
    Width = 619
    Height = 160
    Anchors = [akLeft, akTop, akRight, akBottom]
    Color = clBtnFace
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 4
  end
  object tmrAutoRead: TTimer
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 64
  end
end
