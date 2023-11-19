object frmCH341I2CHd44780: TfrmCH341I2CHd44780
  Left = 0
  Top = 0
  Caption = 'HD44780 + PCF8574 display '
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
    Top = 36
    Width = 29
    Height = 13
  end
  object lblI2CAddress: TLabel
    Left = 8
    Top = 8
    Width = 137
    Height = 13
    Caption = 'PCF8574 I2C address (7-bit)'
  end
  object lblBacklight: TLabel
    Left = 8
    Top = 171
    Width = 42
    Height = 13
    Caption = 'Backlight'
  end
  object lblLine1: TLabel
    Left = 8
    Top = 67
    Width = 39
    Height = 13
    Caption = 'LINE #1'
  end
  object lblLine2: TLabel
    Left = 8
    Top = 92
    Width = 39
    Height = 13
    Caption = 'LINE #2'
  end
  object btnWrite: TButton
    Left = 8
    Top = 115
    Width = 75
    Height = 25
    Caption = 'Write'
    TabOrder = 0
    OnClick = btnWriteClick
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
    ItemHeight = 14
    ParentFont = False
    TabOrder = 1
  end
  object btnBacklightOn: TButton
    Left = 70
    Top = 166
    Width = 75
    Height = 25
    Caption = 'ON'
    TabOrder = 2
    OnClick = btnBacklightOnClick
  end
  object btnBacklightOff: TButton
    Left = 151
    Top = 166
    Width = 75
    Height = 25
    Caption = 'OFF'
    TabOrder = 3
    OnClick = btnBacklightOffClick
  end
  object edLine1: TEdit
    Left = 70
    Top = 64
    Width = 195
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
    Text = 'ABCDEFGHIJKLMNOP'
  end
  object edLine2: TEdit
    Left = 70
    Top = 89
    Width = 195
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 5
    Text = 'abcdefghijklmnop'
  end
end
