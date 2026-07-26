object frmCH341I2CBH1750: TfrmCH341I2CBH1750
  Left = 0
  Top = 0
  Caption = 'BH1750 light sensor'
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
    Top = 91
    Width = 3
    Height = 13
  end
  object lblRawData: TLabel
    Left = 8
    Top = 227
    Width = 47
    Height = 13
    Caption = 'Raw Data'
  end
  object lblLux: TLabel
    Left = 8
    Top = 255
    Width = 17
    Height = 13
    Caption = 'Lux'
  end
  object lblMode: TLabel
    Left = 8
    Top = 50
    Width = 94
    Height = 13
    Caption = 'Measurement Mode'
  end
  object btnRead: TButton
    Left = 170
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Read'
    Enabled = False
    TabOrder = 2
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 257
    Top = 12
    Width = 128
    Height = 17
    Caption = 'auto refresh (500ms)'
    TabOrder = 3
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
  end
  object btnOpen: TButton
    Left = 8
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Open'
    TabOrder = 1
    OnClick = btnOpenClick
  end
  object edRawData: TEdit
    Left = 71
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
    TabOrder = 4
  end
  object edLux: TEdit
    Left = 71
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
    TabOrder = 5
  end
  object cbMode: TComboBox
    Left = 119
    Top = 47
    Width = 202
    Height = 22
    Style = csDropDownList
    Enabled = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    ItemIndex = 0
    ParentFont = False
    TabOrder = 0
    Text = 'High 1 (0.5 lux - 120 ms)'
    Items.Strings = (
      'High 1 (0.5 lux - 120 ms)'
      'High 2 (1 lux - 120 ms)'
      'Low (4 lux - 16 ms)')
  end
  object btnClose: TButton
    Left = 89
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Close'
    Enabled = False
    TabOrder = 6
    OnClick = btnCloseClick
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 344
    Top = 43
  end
end
