object frmCH341I2CSsd1306: TfrmCH341I2CSsd1306
  Left = 0
  Top = 0
  Caption = 'SSD1306 display'
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
    Top = 72
    Width = 3
    Height = 13
  end
  object lblI2CAddress: TLabel
    Left = 8
    Top = 8
    Width = 126
    Height = 13
    Caption = 'Device I2C address (7-bit)'
  end
  object lblAddressNote: TLabel
    Left = 264
    Top = 8
    Width = 122
    Height = 13
    Caption = 'Note: likely 0x3C or 0x3D'
  end
  object lblDisplayResolution: TLabel
    Left = 8
    Top = 36
    Width = 84
    Height = 13
    Caption = 'Display resolution'
  end
  object lblContrast: TLabel
    Left = 8
    Top = 232
    Width = 42
    Height = 13
    Caption = 'Contrast'
  end
  object btnWrite: TButton
    Left = 8
    Top = 131
    Width = 105
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
  object btnInvertDisplay: TButton
    Left = 8
    Top = 162
    Width = 105
    Height = 25
    Caption = 'Invert display'
    TabOrder = 2
    OnClick = btnInvertDisplayClick
  end
  object btnInit: TButton
    Left = 8
    Top = 100
    Width = 105
    Height = 25
    Caption = 'Init'
    TabOrder = 3
    OnClick = btnInitClick
  end
  object cbDisplayResolution: TComboBox
    Left = 152
    Top = 33
    Width = 96
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
    TabOrder = 4
    Text = '128 x 64'
    Items.Strings = (
      '128 x 64'
      '128 x 32')
  end
  object edText: TEdit
    Left = 152
    Top = 133
    Width = 121
    Height = 21
    TabOrder = 5
    Text = 'TEST text'
  end
  object btnTurnOn: TButton
    Left = 8
    Top = 193
    Width = 105
    Height = 25
    Caption = 'Turn ON'
    TabOrder = 6
    OnClick = btnTurnOnClick
  end
  object btnTurnOff: TButton
    Left = 119
    Top = 193
    Width = 105
    Height = 25
    Caption = 'Turn OFF'
    TabOrder = 7
    OnClick = btnTurnOffClick
  end
  object trbarContrast: TTrackBar
    Left = 56
    Top = 220
    Width = 353
    Height = 45
    Max = 255
    Frequency = 10
    Position = 255
    TabOrder = 8
    TickMarks = tmBoth
    OnChange = trbarContrastChange
  end
end
