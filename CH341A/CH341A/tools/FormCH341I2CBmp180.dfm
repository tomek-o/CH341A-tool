object frmCH341I2CBmp180: TfrmCH341I2CBmp180
  Left = 0
  Top = 0
  Caption = 'BMP180 sensor'
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
    Left = 16
    Top = 56
    Width = 3
    Height = 13
  end
  object lblTemperature: TLabel
    Left = 16
    Top = 88
    Width = 62
    Height = 13
    Caption = 'Temperature'
  end
  object lblPressure: TLabel
    Left = 16
    Top = 120
    Width = 42
    Height = 13
    Caption = 'Pressure'
  end
  object btnRead: TButton
    Left = 16
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 0
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 97
    Top = 12
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every second)'
    TabOrder = 1
  end
  object edTemperature: TEdit
    Left = 97
    Top = 85
    Width = 96
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 2
  end
  object edPressure: TEdit
    Left = 97
    Top = 117
    Width = 96
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 3
  end
  object tmrAutoRead: TTimer
    OnTimer = tmrAutoReadTimer
    Left = 384
    Top = 8
  end
end
