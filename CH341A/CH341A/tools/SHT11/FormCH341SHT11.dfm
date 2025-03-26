object frmCH341SHT11: TfrmCH341SHT11
  Left = 0
  Top = 0
  Caption = 'SHT11 sensor'
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
    Top = 97
    Width = 5
    Height = 19
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object lblAddress: TLabel
    Left = 8
    Top = 8
    Width = 434
    Height = 13
    Caption = 
      'This is not I2C sensor, but it is using SCL (-> SCK) and SDA (<-' +
      '>DATA) CH341A pins here.'
  end
  object lblSupplyVoltage: TLabel
    Left = 8
    Top = 35
    Width = 298
    Height = 13
    Caption = 'Sensor supply voltage (for temperature calculation correction)'
  end
  object btnRead: TButton
    Left = 8
    Top = 66
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 0
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 70
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 2 s)'
    TabOrder = 1
    OnClick = chbAutoReadClick
    OnKeyDown = chbAutoReadKeyDown
  end
  object cbSupplyVoltage: TComboBox
    Left = 312
    Top = 32
    Width = 65
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 2
    TabOrder = 2
    Text = '3.5V'
    Items.Strings = (
      '5V'
      '4V'
      '3.5V'
      '3V'
      '2.5V')
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 2000
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 66
  end
end
