object frmCH341I2CVeml7700: TfrmCH341I2CVeml7700
  Left = 0
  Top = 0
  Caption = 'VEML7700 light sensor'
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
    Top = 56
    Width = 4
    Height = 16
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object btnRead: TButton
    Left = 8
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 0
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 12
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 1000 ms)'
    TabOrder = 1
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
  end
  object tmrAutoRead: TTimer
    Enabled = False
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 8
  end
end
