object frmCH341OneWire: TfrmCH341OneWire
  Left = 0
  Top = 0
  Caption = '1-Wire'
  ClientHeight = 452
  ClientWidth = 635
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    635
    452)
  PixelsPerInch = 96
  TextHeight = 13
  object lblPin: TLabel
    Left = 8
    Top = 111
    Width = 32
    Height = 13
    Caption = 'DQ pin'
  end
  object lblReadTiming: TLabel
    Left = 276
    Top = 111
    Width = 169
    Height = 13
    Caption = 'Read slot: low / wait before sample'
  end
  object lblWrite: TLabel
    Left = 8
    Top = 182
    Width = 85
    Height = 13
    Caption = 'Write bytes (hex)'
  end
  object lblReadCount: TLabel
    Left = 360
    Top = 182
    Width = 65
    Height = 13
    Caption = 'Bytes to read'
  end
  object lblStatus: TLabel
    Left = 8
    Top = 448
    Width = 3
    Height = 13
  end
  object memInfo: TMemo
    Left = 8
    Top = 8
    Width = 619
    Height = 89
    TabStop = False
    Anchors = [akLeft, akTop, akRight]
    BorderStyle = bsNone
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 0
  end
  object cbPin: TComboBox
    Left = 60
    Top = 108
    Width = 189
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 1
    Text = 'D0 - SPI CS0 (socket pin 1)'
    Items.Strings = (
      'D0 - SPI CS0 (socket pin 1)'
      'D3 - SPI CLK (socket pin 6)'
      'D5 - SPI MOSI (socket pin 5)')
  end
  object edReadLow: TEdit
    Left = 456
    Top = 107
    Width = 35
    Height = 21
    TabOrder = 2
    Text = '2'
  end
  object edReadWait: TEdit
    Left = 501
    Top = 107
    Width = 35
    Height = 21
    TabOrder = 3
    Text = '6'
  end
  object btnCheck: TButton
    Left = 8
    Top = 142
    Width = 90
    Height = 25
    Caption = 'Check bus'
    TabOrder = 4
    OnClick = btnCheckClick
  end
  object btnReset: TButton
    Left = 104
    Top = 142
    Width = 110
    Height = 25
    Caption = 'Reset / presence'
    TabOrder = 5
    OnClick = btnResetClick
  end
  object btnReadRom: TButton
    Left = 220
    Top = 142
    Width = 90
    Height = 25
    Caption = 'Read ROM'
    TabOrder = 6
    OnClick = btnReadRomClick
  end
  object btnSearch: TButton
    Left = 316
    Top = 142
    Width = 90
    Height = 25
    Caption = 'Search ROMs'
    TabOrder = 7
    OnClick = btnSearchClick
  end
  object edWrite: TEdit
    Left = 110
    Top = 179
    Width = 240
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 8
    Text = 'CC'
  end
  object edReadCount: TEdit
    Left = 440
    Top = 179
    Width = 45
    Height = 21
    TabOrder = 9
    Text = '0'
  end
  object chbResetFirst: TCheckBox
    Left = 8
    Top = 210
    Width = 90
    Height = 17
    Caption = 'Reset first'
    Checked = True
    State = cbChecked
    TabOrder = 10
  end
  object btnWriteRead: TButton
    Left = 110
    Top = 206
    Width = 110
    Height = 25
    Caption = 'Write / read'
    TabOrder = 11
    OnClick = btnWriteReadClick
  end
  object memLog: TMemo
    Left = 8
    Top = 242
    Width = 619
    Height = 202
    Anchors = [akLeft, akTop, akRight, akBottom]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 12
  end
end
