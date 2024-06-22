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
    Top = 152
    Width = 3
    Height = 13
  end
  object lblDistance: TLabel
    Left = 8
    Top = 184
    Width = 41
    Height = 13
    Caption = 'Distance'
  end
  object lblMm: TLabel
    Left = 200
    Top = 184
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
  object lblMeasurementTimingBudget: TLabel
    Left = 8
    Top = 65
    Width = 173
    Height = 13
    Caption = 'Measure timing budget (= precision)'
  end
  object lblUs: TLabel
    Left = 278
    Top = 65
    Width = 11
    Height = 13
    Caption = 'us'
  end
  object btnRead: TButton
    Left = 8
    Top = 104
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 0
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 108
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every second)'
    TabOrder = 1
  end
  object edDistance: TEdit
    Left = 89
    Top = 181
    Width = 96
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 2
  end
  object chbLongRange: TCheckBox
    Left = 8
    Top = 42
    Width = 293
    Height = 17
    Caption = 'Long range (lower required minimum reflected signal)'
    Checked = True
    State = cbChecked
    TabOrder = 3
  end
  object cbMeasurementTimingBudget: TComboBox
    Left = 197
    Top = 62
    Width = 76
    Height = 21
    ItemHeight = 13
    ItemIndex = 3
    TabOrder = 4
    Text = '200000'
    Items.Strings = (
      '20000'
      '33000'
      '100000'
      '200000')
  end
  object tmrAutoRead: TTimer
    OnTimer = tmrAutoReadTimer
    Left = 360
    Top = 104
  end
end
