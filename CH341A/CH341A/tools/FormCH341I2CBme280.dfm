object frmCH341I2CBme280: TfrmCH341I2CBme280
  Left = 0
  Top = 0
  Caption = 'BME280/BMP280 sensor'
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
  object lblTemperature: TLabel
    Left = 8
    Top = 120
    Width = 62
    Height = 13
    Caption = 'Temperature'
  end
  object lblPressure: TLabel
    Left = 8
    Top = 152
    Width = 42
    Height = 13
    Caption = 'Pressure'
  end
  object lblHumidity: TLabel
    Left = 8
    Top = 184
    Width = 41
    Height = 13
    Caption = 'Humidity'
  end
  object lblI2CAddress: TLabel
    Left = 8
    Top = 8
    Width = 126
    Height = 13
    Caption = 'Device I2C address (7-bit)'
  end
  object lblHumidityInfo: TLabel
    Left = 200
    Top = 184
    Width = 111
    Height = 13
    Caption = '(valid only for BME280)'
  end
  object btnRead: TButton
    Left = 8
    Top = 40
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 0
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 44
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every second)'
    TabOrder = 1
  end
  object edTemperature: TEdit
    Left = 89
    Top = 117
    Width = 96
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 2
  end
  object edPressure: TEdit
    Left = 89
    Top = 149
    Width = 96
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 3
  end
  object edHumidity: TEdit
    Left = 89
    Top = 181
    Width = 96
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 4
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
    TabOrder = 5
  end
  object tmrAutoRead: TTimer
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 40
  end
end
