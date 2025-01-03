object frmCH341I2CMAX30102: TfrmCH341I2CMAX30102
  Left = 0
  Top = 0
  Caption = 'MAX30102 heart rate sensor'
  ClientHeight = 299
  ClientWidth = 635
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnResize = FormResize
  DesignSize = (
    635
    299)
  PixelsPerInch = 96
  TextHeight = 13
  object lblStatus: TLabel
    Left = 8
    Top = 82
    Width = 3
    Height = 13
  end
  object btnRead: TButton
    Left = 8
    Top = 40
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 0
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 44
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 1 s)'
    TabOrder = 1
  end
  object btnInit: TButton
    Left = 8
    Top = 9
    Width = 75
    Height = 25
    Caption = 'Init'
    TabOrder = 2
    OnClick = btnInitClick
  end
  object pnlPlot1: TPanel
    Left = 8
    Top = 107
    Width = 619
    Height = 86
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 3
  end
  object pnlPlot2: TPanel
    Left = 8
    Top = 205
    Width = 619
    Height = 86
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 4
  end
  object tmrAutoRead: TTimer
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 40
  end
end
