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
    Top = 50
    Width = 121
    Height = 13
  end
  object lblI2CSpeed: TLabel
    Left = 176
    Top = 27
    Width = 33
    Height = 13
  end
  object lblReadingState: TLabel
    Left = 176
    Top = 8
    Width = 40
    Height = 13
    Caption = 'Stopped'
  end
  object btnStart: TButton
    Left = 8
    Top = 9
    Width = 105
    Height = 25
    Caption = 'Init + start reading'
    TabOrder = 0
    OnClick = btnStartClick
  end
  object btnStop: TButton
    Left = 119
    Top = 9
    Width = 50
    Height = 25
    Caption = 'Stop'
    Enabled = False
    TabOrder = 1
    OnClick = btnStopClick
  end
  object pnlPlot1: TPanel
    Left = 8
    Top = 107
    Width = 619
    Height = 86
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 2
  end
  object pnlPlot2: TPanel
    Left = 8
    Top = 205
    Width = 619
    Height = 86
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 3
  end
  object tmrAutoRead: TTimer
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 336
    Top = 8
  end
end
