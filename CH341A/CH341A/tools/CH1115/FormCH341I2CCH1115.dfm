object frmCH341I2CCH1115: TfrmCH341I2CCH1115
  Left = 0
  Top = 0
  Caption = 'CH1115 display'
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
    Top = 64
    Width = 126
    Height = 13
  end
  object lblAddress: TLabel
    Left = 8
    Top = 8
    Width = 135
    Height = 13
    Caption = 'I2C address (default: 0x3C)'
  end
  object lblContrast: TLabel
    Left = 8
    Top = 264
    Width = 42
    Height = 13
    Caption = 'Contrast'
  end
  object lblInfo: TLabel
    Left = 8
    Top = 32
    Width = 136
    Height = 13
    Caption = '0.50" 48x88px OLED display'
  end
  object cbI2CAddress: TComboBox
    Left = 152
    Top = 5
    Width = 65
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
    Text = '0x3C'
    Items.Strings = (
      '0x3C'
      '0x3D')
  end
  object btnInit: TButton
    Left = 8
    Top = 100
    Width = 105
    Height = 25
    Caption = 'Init and write'
    TabOrder = 1
    OnClick = btnInitClick
  end
  object btnInvertDisplay: TButton
    Left = 8
    Top = 189
    Width = 105
    Height = 25
    Caption = 'Invert display'
    TabOrder = 2
    OnClick = btnInvertDisplayClick
  end
  object btnTurnOn: TButton
    Left = 8
    Top = 221
    Width = 105
    Height = 25
    Caption = 'Turn ON'
    TabOrder = 3
    OnClick = btnTurnOnClick
  end
  object btnTurnOff: TButton
    Left = 119
    Top = 221
    Width = 105
    Height = 25
    Caption = 'Turn OFF'
    TabOrder = 4
    OnClick = btnTurnOffClick
  end
  object trbarContrast: TTrackBar
    Left = 56
    Top = 252
    Width = 353
    Height = 45
    Max = 255
    Frequency = 10
    Position = 128
    TabOrder = 5
    TickMarks = tmBoth
    OnChange = trbarContrastChange
  end
  object btnClearDisplay: TButton
    Left = 119
    Top = 190
    Width = 105
    Height = 25
    Caption = 'Clear display'
    TabOrder = 6
    OnClick = btnClearDisplayClick
  end
  object cbImage: TComboBox
    Left = 152
    Top = 102
    Width = 81
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 7
    Text = 'image #1'
    Items.Strings = (
      'image #1'
      'image #2')
  end
end
