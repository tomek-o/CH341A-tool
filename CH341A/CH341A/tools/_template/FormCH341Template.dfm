object frmCH341Template: TfrmCH341Template
  Left = 0
  Top = 0
  Caption = 'Template'
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
    Top = 144
    Width = 3
    Height = 13
  end
  object lblAddress: TLabel
    Left = 8
    Top = 8
    Width = 199
    Height = 13
    Caption = 'I2C address, depending on SDO pin state'
  end
  object btnRead: TButton
    Left = 8
    Top = 96
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 2
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 100
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 500 ms)'
    TabOrder = 3
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
  end
  object cbAddress: TComboBox
    Left = 224
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
    ItemIndex = 1
    ParentFont = False
    TabOrder = 0
    Text = '0x69'
    Items.Strings = (
      '0x68'
      '0x69')
  end
  object btnInit: TButton
    Left = 8
    Top = 65
    Width = 75
    Height = 25
    Caption = 'Init'
    TabOrder = 1
    OnClick = btnInitClick
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 96
  end
end
