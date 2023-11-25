object frmCH341SpiWriteRead: TfrmCH341SpiWriteRead
  Left = 0
  Top = 0
  Caption = 'SPI write/read'
  ClientHeight = 303
  ClientWidth = 438
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object lblWrite: TLabel
    Left = 17
    Top = 45
    Width = 133
    Height = 13
    Caption = 'Hex data to write, MSB first'
  end
  object lblWriteBytesCount: TLabel
    Left = 288
    Top = 45
    Width = 57
    Height = 13
    AutoSize = False
    Caption = '0 byte(s)'
  end
  object lblDataRead: TLabel
    Left = 17
    Top = 151
    Width = 48
    Height = 13
    Caption = 'Data read'
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
  object memoRead: TMemo
    Left = 17
    Top = 168
    Width = 329
    Height = 85
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 1
  end
  object memoWrite: TMemo
    Left = 17
    Top = 62
    Width = 328
    Height = 85
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 2
    OnChange = memoWriteChange
  end
end
