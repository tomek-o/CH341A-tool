object frmCH341I2CAs5600: TfrmCH341I2CAs5600
  Left = 0
  Top = 0
  Caption = 'AS5600 sensor'
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
    Top = 127
    Width = 3
    Height = 13
  end
  object lblRawAngle: TLabel
    Left = 8
    Top = 176
    Width = 50
    Height = 13
    Caption = 'Raw angle'
  end
  object lblI2CAddress: TLabel
    Left = 8
    Top = 40
    Width = 126
    Height = 13
    Caption = 'Device I2C address (7-bit)'
  end
  object lblAddressNote: TLabel
    Left = 8
    Top = 65
    Width = 365
    Height = 13
    Caption = 
      'Note: AS5600 uses address = 0x36, AS5600L: configurable, default' +
      ' = 0x40'
  end
  object lblInfo: TLabel
    Left = 8
    Top = 8
    Width = 193
    Height = 13
    Caption = 'AS5600/AS5600L magnetic angle sensor'
  end
  object lblSensorStatus: TLabel
    Left = 8
    Top = 203
    Width = 31
    Height = 13
    Caption = 'Status'
  end
  object btnRead: TButton
    Left = 8
    Top = 96
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 0
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 97
    Top = 100
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every second)'
    TabOrder = 1
    OnClick = chbAutoReadClick
    OnKeyPress = chbAutoReadKeyPress
  end
  object edRawAngle: TEdit
    Left = 97
    Top = 173
    Width = 288
    Height = 22
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 2
  end
  object cbI2CAddress: TComboBox
    Left = 152
    Top = 37
    Width = 96
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    ParentFont = False
    TabOrder = 3
  end
  object edSensorStatus: TEdit
    Left = 97
    Top = 200
    Width = 288
    Height = 22
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 4
  end
  object tmrAutoRead: TTimer
    Enabled = False
    OnTimer = tmrAutoReadTimer
    Left = 384
    Top = 96
  end
end
