object frmCH341I2CRda5807m: TfrmCH341I2CRda5807m
  Left = 0
  Top = 0
  Caption = 'RDA5807M FM tuner'
  ClientHeight = 369
  ClientWidth = 646
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    646
    369)
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
    Top = 144
    Width = 34
    Height = 13
    Caption = 'Volume'
  end
  object lblRdaStatus: TLabel
    Left = 8
    Top = 63
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
  object lblRdsPi: TLabel
    Left = 8
    Top = 217
    Width = 141
    Height = 13
    Caption = 'PI (Programme Identification)'
  end
  object lblRdsPs: TLabel
    Left = 222
    Top = 217
    Width = 144
    Height = 13
    Caption = 'PS (Programme Service name)'
  end
  object lblRdsFlags: TLabel
    Left = 8
    Top = 245
    Width = 25
    Height = 13
    Caption = 'Flags'
  end
  object lblRdsRt: TLabel
    Left = 8
    Top = 267
    Width = 73
    Height = 13
    Caption = 'RT (RadioText)'
  end
  object btnRead: TButton
    Left = 104
    Top = 27
    Width = 91
    Height = 25
    Caption = 'Read status'
    TabOrder = 0
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 201
    Top = 31
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading periodically)'
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
    Top = 100
    Width = 91
    Height = 25
    Caption = 'Search down'
    TabOrder = 3
    OnClick = btnSearchDownClick
  end
  object btnSearchUp: TButton
    Left = 105
    Top = 100
    Width = 91
    Height = 25
    Caption = 'Search up'
    TabOrder = 4
    OnClick = btnSearchUpClick
  end
  object trbarVolume: TTrackBar
    Left = 48
    Top = 131
    Width = 148
    Height = 41
    Max = 15
    Position = 15
    TabOrder = 5
    TickMarks = tmBoth
    OnChange = trbarVolumeChange
  end
  object chbRdsEnabled: TCheckBox
    Left = 8
    Top = 195
    Width = 97
    Height = 17
    Caption = 'RDS enabled'
    Checked = True
    State = cbChecked
    TabOrder = 6
    OnClick = chbRdsEnabledClick
  end
  object edRdsPi: TEdit
    Left = 155
    Top = 214
    Width = 41
    Height = 22
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 7
  end
  object edRdsPs: TEdit
    Left = 372
    Top = 214
    Width = 67
    Height = 22
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 8
  end
  object edRdsFlags: TEdit
    Left = 155
    Top = 239
    Width = 200
    Height = 22
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 9
  end
  object edRdsRt: TEdit
    Left = 155
    Top = 264
    Width = 483
    Height = 22
    Anchors = [akLeft, akTop, akRight]
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 10
  end
  object tmrAutoRead: TTimer
    Interval = 30
    OnTimer = tmrAutoReadTimer
    Left = 408
    Top = 26
  end
end
