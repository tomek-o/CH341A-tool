object frmCH341SpiST7567: TfrmCH341SpiST7567
  Left = 0
  Top = 0
  Caption = 'ST7567 display'
  ClientHeight = 299
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
    299)
  PixelsPerInch = 96
  TextHeight = 13
  object lblStatus: TLabel
    Left = 8
    Top = 232
    Width = 38
    Height = 13
  end
  object btnWrite: TButton
    Left = 8
    Top = 96
    Width = 75
    Height = 25
    Caption = 'Write'
    TabOrder = 1
    OnClick = btnWriteClick
  end
  object btnInit: TButton
    Left = 8
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Init'
    TabOrder = 0
    OnClick = btnInitClick
  end
  object memo: TMemo
    Left = 89
    Top = 8
    Width = 538
    Height = 65
    TabStop = False
    Anchors = [akLeft, akTop, akRight]
    Color = clBtnFace
    Lines.Strings = (
      'Tested with OPEN-SMART 2.4", 128x64 LCD display.'
      'Connect CH341A SPI SCK, MOSI and CS0 to ST7567.'
      'Connect CH341A UART TX to ST7567 DC (data = H, command = L).'
      'Connect CH341A UART RX to ST7567 RESET.')
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 2
  end
  object memoText: TMemo
    Left = 89
    Top = 98
    Width = 160
    Height = 119
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    Lines.Strings = (
      'LINE 1 ABCDEFGHI'
      'LINE 2 JKLMNOPQR'
      'LINE 3 STUVWXYZa'
      'LINE 4 bcdefghij'
      'LINE 5 klmnopqrs'
      'LINE 6 tuvwxyz'
      'LINE 7'
      'LINE 8')
    ParentFont = False
    TabOrder = 3
  end
end
