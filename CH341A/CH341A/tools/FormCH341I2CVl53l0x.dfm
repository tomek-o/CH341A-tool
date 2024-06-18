object frmCH341I2CVl53l0x: TfrmCH341I2CVl53l0x
  Left = 0
  Top = 0
  Caption = 'VL53L0X ToF sensor'
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
    Top = 80
    Width = 3
    Height = 13
  end
  object lblDistance: TLabel
    Left = 8
    Top = 112
    Width = 41
    Height = 13
    Caption = 'Distance'
  end
  object lblMm: TLabel
    Left = 200
    Top = 112
    Width = 16
    Height = 13
    Caption = 'mm'
  end
  object lblInfo: TLabel
    Left = 8
    Top = 8
    Width = 293
    Height = 13
    Caption = 'Laser Time-of-Flight distance sensor with I2C, up to 2 meters'
  end
  object btnRead: TButton
    Left = 8
    Top = 32
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 0
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 36
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every second)'
    TabOrder = 1
  end
  object edDistance: TEdit
    Left = 89
    Top = 109
    Width = 96
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 2
  end
  object tmrAutoRead: TTimer
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 32
  end
end
