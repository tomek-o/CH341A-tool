object frmCH341I2CTSL2591: TfrmCH341I2CTSL2591
  Left = 0
  Top = 0
  Caption = 'TSL2591 light sensor'
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
    Top = 139
    Width = 3
    Height = 13
  end
  object lblCh0: TLabel
    Left = 8
    Top = 171
    Width = 116
    Height = 13
    Caption = 'Channel 0, full spectrum'
  end
  object lblVisible: TLabel
    Left = 8
    Top = 227
    Width = 142
    Height = 13
    Caption = 'Visible (channel 0 - channel 1)'
  end
  object lblCh1: TLabel
    Left = 8
    Top = 199
    Width = 93
    Height = 13
    Caption = 'Channel 1, infrared'
  end
  object lblLux: TLabel
    Left = 8
    Top = 255
    Width = 17
    Height = 13
    Caption = 'Lux'
  end
  object lblGain: TLabel
    Left = 8
    Top = 8
    Width = 21
    Height = 13
    Caption = 'Gain'
  end
  object lblIntegrationTime: TLabel
    Left = 8
    Top = 34
    Width = 77
    Height = 13
    Caption = 'Integration time'
  end
  object btnRead: TButton
    Left = 8
    Top = 91
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 3
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 95
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading with pause = 1000 ms)'
    TabOrder = 4
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
  end
  object btnInit: TButton
    Left = 8
    Top = 60
    Width = 75
    Height = 25
    Caption = 'Init'
    TabOrder = 2
    OnClick = btnInitClick
  end
  object edChannel0: TEdit
    Left = 159
    Top = 168
    Width = 121
    Height = 22
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 5
  end
  object edChannel1: TEdit
    Left = 159
    Top = 196
    Width = 121
    Height = 22
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 6
  end
  object edVisible: TEdit
    Left = 159
    Top = 224
    Width = 121
    Height = 22
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 7
  end
  object edLux: TEdit
    Left = 159
    Top = 252
    Width = 121
    Height = 22
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 8
  end
  object cbGain: TComboBox
    Left = 159
    Top = 5
    Width = 121
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    ItemIndex = 1
    ParentFont = False
    TabOrder = 0
    Text = 'medium (25x)'
    Items.Strings = (
      'low (1x)'
      'medium (25x)'
      'high (428x)')
  end
  object cbIntegrationTime: TComboBox
    Left = 159
    Top = 31
    Width = 121
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    ItemIndex = 2
    ParentFont = False
    TabOrder = 1
    Text = '300 ms'
    Items.Strings = (
      '100 ms'
      '200 ms'
      '300 ms'
      '400 ms'
      '500 ms'
      '600 ms')
  end
  object tmrAutoRead: TTimer
    Enabled = False
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 91
  end
end
