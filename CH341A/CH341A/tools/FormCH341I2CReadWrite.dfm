object frmCH341I2CReadWrite: TfrmCH341I2CReadWrite
  Left = 0
  Top = 0
  Caption = 'I2C read/write'
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
  object lblBytes: TLabel
    Left = 175
    Top = 21
    Width = 35
    Height = 13
    Caption = 'byte(s)'
  end
  object btnRead: TButton
    Left = 16
    Top = 16
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 0
    OnClick = btnReadClick
  end
  object cbReadBytesCount: TComboBox
    Left = 104
    Top = 18
    Width = 65
    Height = 21
    ItemHeight = 13
    ItemIndex = 1
    TabOrder = 1
    Text = '2'
    Items.Strings = (
      '1'
      '2'
      '3'
      '4'
      '8'
      '16'
      '32'
      '64'
      '128'
      '256'
      '512'
      '1024'
      '2048'
      '4096')
  end
  object memoRead: TMemo
    Left = 104
    Top = 45
    Width = 289
    Height = 76
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 2
  end
end
