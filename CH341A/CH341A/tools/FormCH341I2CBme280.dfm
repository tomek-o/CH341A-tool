object frmCH341I2CBme280: TfrmCH341I2CBme280
  Left = 0
  Top = 0
  Caption = 'BME280 sensor'
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
  object btnRead: TButton
    Left = 16
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 0
    OnClick = btnReadClick
  end
  object memo: TMemo
    Left = 16
    Top = 40
    Width = 393
    Height = 251
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
    TabOrder = 1
  end
  object chbAutoRead: TCheckBox
    Left = 97
    Top = 12
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every second)'
    TabOrder = 2
  end
  object tmrAutoRead: TTimer
    OnTimer = tmrAutoReadTimer
    Left = 384
    Top = 8
  end
end
