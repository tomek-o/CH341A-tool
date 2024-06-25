object frmCH341I2CApds9960: TfrmCH341I2CApds9960
  Left = 0
  Top = 0
  Caption = 'APDS-9960 light sensor'
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
    Top = 85
    Width = 3
    Height = 13
  end
  object lblLight: TLabel
    Left = 8
    Top = 117
    Width = 23
    Height = 13
    Caption = 'Light'
  end
  object lblProximity: TLabel
    Left = 8
    Top = 147
    Width = 44
    Height = 13
    Caption = 'Proximity'
  end
  object lblGesture: TLabel
    Left = 8
    Top = 176
    Width = 38
    Height = 13
    Caption = 'Gesture'
  end
  object btnRead: TButton
    Left = 8
    Top = 38
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 0
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 42
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 500 ms)'
    TabOrder = 1
  end
  object edProximity: TEdit
    Left = 89
    Top = 144
    Width = 81
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 2
  end
  object btnInit: TButton
    Left = 8
    Top = 7
    Width = 75
    Height = 25
    Caption = 'Init'
    TabOrder = 3
    OnClick = btnInitClick
  end
  object edLight: TEdit
    Left = 89
    Top = 114
    Width = 296
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 4
  end
  object memoGestures: TMemo
    Left = 89
    Top = 173
    Width = 296
    Height = 118
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
    TabOrder = 5
  end
  object chbReadGestures: TCheckBox
    Left = 89
    Top = 11
    Width = 264
    Height = 17
    Caption = 'read gestures'
    TabOrder = 6
  end
  object tmrAutoRead: TTimer
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 368
    Top = 37
  end
end
