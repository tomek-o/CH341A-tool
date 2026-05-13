object frmMPR121: TfrmMPR121
  Left = 0
  Top = 0
  Caption = 'MPR121 keypad'
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
    Top = 192
    Width = 3
    Height = 13
  end
  object lblAddress: TLabel
    Left = 8
    Top = 8
    Width = 233
    Height = 13
    Caption = 'I2C address, depending on ADDR pin connection'
  end
  object lblTouchThreshold: TLabel
    Left = 8
    Top = 54
    Width = 77
    Height = 13
    Caption = 'Touch threshold'
  end
  object lblReleaseThreshold: TLabel
    Left = 8
    Top = 78
    Width = 86
    Height = 13
    Caption = 'Release threshold'
  end
  object btnRead: TButton
    Left = 8
    Top = 144
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 2
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 148
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 100 ms)'
    TabOrder = 3
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
  end
  object cbAddress: TComboBox
    Left = 247
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
    Text = '0x5A'
    Items.Strings = (
      '0x5A'
      '0x5B'
      '0x5C'
      '0x5D')
  end
  object btnInit: TButton
    Left = 8
    Top = 113
    Width = 75
    Height = 25
    Caption = 'Init'
    TabOrder = 1
    OnClick = btnInitClick
  end
  object chbProximityChannel: TCheckBox
    Left = 8
    Top = 31
    Width = 369
    Height = 17
    Caption = 
      'Enable proximity channel (using all 12 electrodes for proximity ' +
      'sensing)'
    TabOrder = 4
  end
  object cbTouchThreshold: TComboBox
    Left = 112
    Top = 51
    Width = 57
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 5
  end
  object cbReleaseThreshold: TComboBox
    Left = 112
    Top = 75
    Width = 57
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 6
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 100
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 144
  end
end
