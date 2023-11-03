object frmCH341I2CRda5807m: TfrmCH341I2CRda5807m
  Left = 0
  Top = 0
  Caption = 'RDA5807M FM tuner'
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
    Top = 8
    Width = 3
    Height = 13
  end
  object lblVolume: TLabel
    Left = 8
    Top = 176
    Width = 34
    Height = 13
    Caption = 'Volume'
  end
  object lblRdaStatus: TLabel
    Left = 8
    Top = 95
    Width = 144
    Height = 17
    Caption = 'Status not known'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Courier New'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object btnRead: TButton
    Left = 8
    Top = 58
    Width = 91
    Height = 25
    Caption = 'Read status'
    TabOrder = 0
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 105
    Top = 62
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every second)'
    TabOrder = 1
  end
  object btnInit: TButton
    Left = 8
    Top = 27
    Width = 91
    Height = 25
    Caption = 'Init'
    TabOrder = 2
    OnClick = btnInitClick
  end
  object btnSearchDown: TButton
    Left = 8
    Top = 132
    Width = 91
    Height = 25
    Caption = 'Search down'
    TabOrder = 3
    OnClick = btnSearchDownClick
  end
  object btnSearchUp: TButton
    Left = 105
    Top = 132
    Width = 91
    Height = 25
    Caption = 'Search up'
    TabOrder = 4
    OnClick = btnSearchUpClick
  end
  object trbarVolume: TTrackBar
    Left = 48
    Top = 163
    Width = 148
    Height = 41
    Max = 15
    Position = 15
    TabOrder = 5
    TickMarks = tmBoth
    OnChange = trbarVolumeChange
  end
  object tmrAutoRead: TTimer
    OnTimer = tmrAutoReadTimer
    Left = 384
    Top = 50
  end
end
