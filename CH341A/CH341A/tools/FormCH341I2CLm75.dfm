object frmCH341I2CLm75: TfrmCH341I2CLm75
  Left = 0
  Top = 0
  Caption = 'LM75 sensor'
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
    Top = 95
    Width = 3
    Height = 13
  end
  object lblTemperature: TLabel
    Left = 8
    Top = 144
    Width = 62
    Height = 13
    Caption = 'Temperature'
  end
  object lblI2CAddress: TLabel
    Left = 8
    Top = 8
    Width = 126
    Height = 13
    Caption = 'Device I2C address (7-bit)'
  end
  object lblAddressNote: TLabel
    Left = 8
    Top = 33
    Width = 365
    Height = 13
    Caption = 
      'Note: address can be selected with LM75 A2...A0 pins in 0x48...0' +
      'x4F range'
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
    Left = 97
    Top = 68
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every second)'
    TabOrder = 1
  end
  object edTemperature: TEdit
    Left = 97
    Top = 141
    Width = 96
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 2
  end
  object cbI2CAddress: TComboBox
    Left = 152
    Top = 5
    Width = 96
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 0
    ParentFont = False
    TabOrder = 3
  end
  object tmrAutoRead: TTimer
    OnTimer = tmrAutoReadTimer
    Left = 384
    Top = 64
  end
end
