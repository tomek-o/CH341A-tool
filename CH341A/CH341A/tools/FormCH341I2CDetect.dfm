object frmCH341I2CDetect: TfrmCH341I2CDetect
  Left = 0
  Top = 0
  Caption = 'I2C detect'
  ClientHeight = 299
  ClientWidth = 452
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    452
    299)
  PixelsPerInch = 96
  TextHeight = 13
  object lblOutputFormatting: TLabel
    Left = 16
    Top = 62
    Width = 87
    Height = 13
    Caption = 'Output formatting'
  end
  object lblI2CAddressRange: TLabel
    Left = 16
    Top = 86
    Width = 293
    Height = 13
    Caption = 
      'Address range to check (7-bit): from                            ' +
      '        to'
  end
  object lblMs: TLabel
    Left = 283
    Top = 13
    Width = 13
    Height = 13
    Caption = 'ms'
  end
  object btnDetectDevices: TButton
    Left = 16
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Detect'
    TabOrder = 0
    OnClick = btnDetectDevicesClick
  end
  object memoDetect: TMemo
    Left = 16
    Top = 111
    Width = 393
    Height = 180
    Anchors = [akLeft, akTop, akBottom]
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 3
  end
  object chbAutoDetect: TCheckBox
    Left = 109
    Top = 12
    Width = 108
    Height = 17
    Caption = 'auto detect every'
    TabOrder = 1
    OnClick = chbAutoDetectClick
    OnMouseDown = chbAutoDetectMouseDown
  end
  object chbBeep: TCheckBox
    Left = 16
    Top = 40
    Width = 264
    Height = 17
    Caption = 'beep when something is detected'
    TabOrder = 2
  end
  object cbOutputFormatting: TComboBox
    Left = 109
    Top = 59
    Width = 145
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 4
    Text = 'list'
    Items.Strings = (
      'list'
      'Linux i2cdetect style')
  end
  object cbI2CAddressStart: TComboBox
    Left = 193
    Top = 83
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
    TabOrder = 5
  end
  object cbI2CAddressEnd: TComboBox
    Left = 313
    Top = 83
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
    TabOrder = 6
  end
  object cbAutoDetectPeriod: TComboBox
    Left = 218
    Top = 10
    Width = 59
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 3
    TabOrder = 7
    Text = '1000'
    OnChange = cbAutoDetectPeriodChange
    Items.Strings = (
      '100'
      '250'
      '500'
      '1000'
      '2000')
  end
  object tmrAutoDetect: TTimer
    Enabled = False
    OnTimer = tmrAutoDetectTimer
    Left = 384
    Top = 8
  end
end
