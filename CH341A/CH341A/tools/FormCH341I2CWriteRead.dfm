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
    Top = 178
    Width = 97
    Height = 13
    Caption = 'byte(s) after writing'
  end
  object lblRead: TLabel
    Left = 17
    Top = 178
    Width = 25
    Height = 13
    Caption = 'Read'
  end
  object lblWrite: TLabel
    Left = 17
    Top = 77
    Width = 236
    Height = 13
    Caption = 'Optional hex data to write (after device address)'
  end
  object lblI2CAddress: TLabel
    Left = 17
    Top = 39
    Width = 126
    Height = 13
    Caption = 'Device I2C address (7-bit)'
  end
  object lblWriteBytesCount: TLabel
    Left = 288
    Top = 77
    Width = 57
    Height = 13
    AutoSize = False
    Caption = '0 byte(s)'
  end
  object btnWriteRead: TButton
    Left = 17
    Top = 8
    Width = 89
    Height = 25
    Caption = 'Write + read'
    TabOrder = 0
    OnClick = btnWriteReadClick
  end
  object cbReadBytesCount: TComboBox
    Left = 56
    Top = 175
    Width = 65
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 1
  end
  object memoRead: TMemo
    Left = 16
    Top = 197
    Width = 329
    Height = 230
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
    Top = 96
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
    Top = 36
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
