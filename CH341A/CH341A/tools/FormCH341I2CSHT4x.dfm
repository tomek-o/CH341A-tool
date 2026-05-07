object frmCH341I2CSHT4x: TfrmCH341I2CSHT4x
  Left = 0
  Top = 0
  Caption = 'SHT4x sensors'
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
    Top = 184
    Width = 3
    Height = 13
  end
  object lblTemperature: TLabel
    Left = 8
    Top = 216
    Width = 62
    Height = 13
    Caption = 'Temperature'
  end
  object lblHumidity: TLabel
    Left = 8
    Top = 248
    Width = 41
    Height = 13
    Caption = 'Humidity'
  end
  object lblInfo: TLabel
    Left = 8
    Top = 8
    Width = 345
    Height = 13
    Caption = 
      'SHT40, SHT41, SHT43, SHT45 -A/-B/-C humidity + temperature senso' +
      'rs'
  end
  object lblAddress: TLabel
    Left = 8
    Top = 31
    Width = 58
    Height = 13
    Caption = 'I2C address'
  end
  object lblPrecision: TLabel
    Left = 8
    Top = 56
    Width = 174
    Height = 13
    Caption = 'Measurement precision/repeatability'
  end
  object btnRead: TButton
    Left = 6
    Top = 80
    Width = 123
    Height = 25
    Caption = 'Read'
    TabOrder = 2
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 137
    Top = 84
    Width = 232
    Height = 17
    Caption = 'auto refresh (repeat reading every second)'
    TabOrder = 3
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
  end
  object edTemperature: TEdit
    Left = 89
    Top = 213
    Width = 96
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 8
  end
  object edHumidity: TEdit
    Left = 89
    Top = 245
    Width = 96
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 9
  end
  object cbAddress: TComboBox
    Left = 188
    Top = 28
    Width = 200
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    ItemIndex = 0
    ParentFont = False
    TabOrder = 0
    Text = '0x44 ("-A" chip variant)'
    Items.Strings = (
      '0x44 ("-A" chip variant)'
      '0x45 ("-B" chip variant)'
      '0x46 ("-C" chip variant)')
  end
  object cbPrecision: TComboBox
    Left = 188
    Top = 53
    Width = 200
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    ItemIndex = 0
    ParentFont = False
    TabOrder = 1
    Text = 'high (slowest)'
    Items.Strings = (
      'high (slowest)'
      'medium'
      'low (fastest)')
  end
  object btnReadWithHeater: TButton
    Left = 6
    Top = 111
    Width = 123
    Height = 25
    Caption = 'Read with heater ON'
    TabOrder = 4
    OnClick = btnReadWithHeaterClick
  end
  object cbHeater: TComboBox
    Left = 188
    Top = 113
    Width = 200
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    ItemIndex = 3
    ParentFont = False
    TabOrder = 5
    Text = 'medium power heater, 0.1s'
    Items.Strings = (
      'high power heater, 1 s'
      'high power heater, 0.1 s'
      'medium power heater, 1 s'
      'medium power heater, 0.1s'
      'low power heater, 1 s'
      'low power heater, 0.1 s')
  end
  object btnReadSerial: TButton
    Left = 6
    Top = 142
    Width = 123
    Height = 25
    Caption = 'Read serial number'
    TabOrder = 6
    OnClick = btnReadSerialClick
  end
  object edSerial: TEdit
    Left = 188
    Top = 144
    Width = 200
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 7
  end
  object tmrAutoRead: TTimer
    Enabled = False
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 80
  end
end
