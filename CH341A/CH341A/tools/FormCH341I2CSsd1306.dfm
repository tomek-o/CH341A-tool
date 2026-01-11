object frmCH341I2CSsd1306: TfrmCH341I2CSsd1306
  Left = 0
  Top = 0
  Caption = 'SSD1306 display'
  ClientHeight = 329
  ClientWidth = 631
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
    Top = 70
    Width = 3
    Height = 13
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
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
    Top = 264
    Width = 42
    Height = 13
    Caption = 'Contrast'
  end
  object lblAnimationInfo: TLabel
    Left = 230
    Top = 164
    Width = 216
    Height = 13
    Caption = 'Shows simple patterns with lines and squares'
  end
  object btnWrite: TButton
    Left = 8
    Top = 129
    Width = 105
    Height = 25
    Caption = 'Write text'
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
    ItemHeight = 0
    ParentFont = False
    TabOrder = 1
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
  object btnInit: TButton
    Left = 8
    Top = 100
    Width = 105
    Height = 25
    Caption = 'Init and write'
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
    Top = 131
    Width = 121
    Height = 21
    TabOrder = 5
    Text = 'TEST text'
  end
  object btnTurnOn: TButton
    Left = 8
    Top = 221
    Width = 105
    Height = 25
    Caption = 'Turn ON'
    TabOrder = 6
    OnClick = btnTurnOnClick
  end
  object btnTurnOff: TButton
    Left = 119
    Top = 221
    Width = 105
    Height = 25
    Caption = 'Turn OFF'
    TabOrder = 7
    OnClick = btnTurnOffClick
  end
  object trbarContrast: TTrackBar
    Left = 56
    Top = 252
    Width = 353
    Height = 45
    Max = 255
    Frequency = 10
    Position = 255
    TabOrder = 8
    TickMarks = tmBoth
    OnChange = trbarContrastChange
  end
  object btnAnimationStart: TButton
    Left = 8
    Top = 159
    Width = 105
    Height = 25
    Caption = 'Start animation'
    TabOrder = 9
    OnClick = btnAnimationStartClick
  end
  object btnAnimationStop: TButton
    Left = 119
    Top = 159
    Width = 105
    Height = 25
    Caption = 'Stop animation'
    Enabled = False
    TabOrder = 10
    OnClick = btnAnimationStopClick
  end
  object tmrAnimate: TTimer
    Enabled = False
    Interval = 2000
    OnTimer = tmrAnimateTimer
    Left = 464
    Top = 160
  end
end
