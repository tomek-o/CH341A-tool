object frmCH341I2CWriteRead: TfrmCH341I2CWriteRead
  Left = 0
  Top = 0
  Caption = 'I2C write/read'
  ClientHeight = 435
  ClientWidth = 438
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    438
    435)
  PixelsPerInch = 96
  TextHeight = 13
  object lblBytes: TLabel
    Left = 127
    Top = 194
    Width = 97
    Height = 13
    Caption = 'byte(s) after writing'
  end
  object lblRead: TLabel
    Left = 17
    Top = 194
    Width = 25
    Height = 13
    Caption = 'Read'
  end
  object lblWrite: TLabel
    Left = 17
    Top = 93
    Width = 236
    Height = 13
    Caption = 'Optional hex data to write (after device address)'
  end
  object lblI2CAddress: TLabel
    Left = 17
    Top = 55
    Width = 126
    Height = 13
    Caption = 'Device I2C address (7-bit)'
  end
  object lblWriteBytesCount: TLabel
    Left = 288
    Top = 93
    Width = 57
    Height = 13
    AutoSize = False
    Caption = '0 byte(s)'
  end
  object btnWriteRead: TButton
    Left = 16
    Top = 16
    Width = 89
    Height = 25
    Caption = 'Write + read'
    TabOrder = 0
    OnClick = btnWriteReadClick
  end
  object cbReadBytesCount: TComboBox
    Left = 56
    Top = 191
    Width = 65
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 1
  end
  object memoRead: TMemo
    Left = 16
    Top = 213
    Width = 329
    Height = 214
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
    TabOrder = 2
  end
  object memoWrite: TMemo
    Left = 17
    Top = 112
    Width = 328
    Height = 57
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 3
    OnChange = memoWriteChange
  end
  object cbI2CAddress: TComboBox
    Left = 161
    Top = 52
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
    TabOrder = 4
  end
end
