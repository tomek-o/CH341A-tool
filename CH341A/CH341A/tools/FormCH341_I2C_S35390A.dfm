object frmCH341_I2C_S35390A: TfrmCH341_I2C_S35390A
  Left = 0
  Top = 0
  Caption = 'S-35390A RTC'
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
    Top = 104
    Width = 3
    Height = 13
  end
  object lblDateTime: TLabel
    Left = 8
    Top = 139
    Width = 11
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'Courier New'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblInt: TLabel
    Left = 8
    Top = 192
    Width = 191
    Height = 13
    Caption = 'INT1 + INT2 open-drain outputs control'
  end
  object lblInt1: TLabel
    Left = 8
    Top = 219
    Width = 23
    Height = 13
    Caption = 'INT1'
  end
  object lblInt2: TLabel
    Left = 8
    Top = 246
    Width = 23
    Height = 13
    Caption = 'INT2'
  end
  object btnRead: TButton
    Left = 8
    Top = 56
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 0
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 60
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 500 ms)'
    TabOrder = 1
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
  end
  object btnSetCurrentTime: TButton
    Left = 8
    Top = 15
    Width = 233
    Height = 25
    Caption = 'Write current time from PC and start clock'
    TabOrder = 2
    OnClick = btnSetCurrentTimeClick
  end
  object cbInt1Frequency: TComboBox
    Left = 251
    Top = 216
    Width = 65
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 3
    Text = '1 Hz'
    Items.Strings = (
      '1 Hz'
      '2 Hz'
      '4 Hz'
      '8 Hz'
      '16 Hz')
  end
  object cbInt2Frequency: TComboBox
    Left = 251
    Top = 243
    Width = 65
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 4
    Text = '1 Hz'
    Items.Strings = (
      '1 Hz'
      '2 Hz'
      '4 Hz'
      '8 Hz'
      '16 Hz')
  end
  object btnSetOutputsConfig: TButton
    Left = 322
    Top = 216
    Width = 111
    Height = 48
    Caption = 'Set outputs config'
    TabOrder = 5
    OnClick = btnSetOutputsConfigClick
  end
  object cbInt1Sel: TComboBox
    Left = 37
    Top = 216
    Width = 208
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 6
    OnChange = cbInt1SelChange
    Items.Strings = (
      '')
  end
  object cbInt2Sel: TComboBox
    Left = 37
    Top = 243
    Width = 208
    Height = 21
    Style = csDropDownList
    ItemHeight = 0
    TabOrder = 7
    OnChange = cbInt2SelChange
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 56
  end
end
