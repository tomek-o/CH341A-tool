object frmCH341DS1302: TfrmCH341DS1302
  Left = 0
  Top = 0
  Caption = 'DS1302 RTC'
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
    Left = 89
    Top = 115
    Width = 3
    Height = 13
  end
  object lblDateTime: TLabel
    Left = 8
    Top = 139
    Width = 11
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'Courier New'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object btnRead: TButton
    Left = 8
    Top = 88
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 3
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 92
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 500 ms)'
    TabOrder = 4
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
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
  object btnSetTrickleCharge: TButton
    Left = 8
    Top = 171
    Width = 146
    Height = 25
    Caption = 'Set trickle charging mode'
    TabOrder = 5
    OnClick = btnSetTrickleChargeClick
  end
  object cbTrickleChargeMode: TComboBox
    Left = 160
    Top = 173
    Width = 217
    Height = 21
    Style = csDropDownList
    ItemHeight = 0
    TabOrder = 6
  end
  object btnSetCurrentTime: TButton
    Left = 8
    Top = 57
    Width = 146
    Height = 25
    Caption = 'Set current time'
    TabOrder = 2
    OnClick = btnSetCurrentTimeClick
  end
  object memo: TMemo
    Left = 89
    Top = 8
    Width = 538
    Height = 41
    TabStop = False
    Anchors = [akLeft, akTop, akRight]
    Color = clBtnFace
    Lines.Strings = (
      'Connect CH341A SCL to DS1302 SCLK, SDA to DS1302 I/O. '
      'Connect CH341A SPI CS to DS1302 CE (= RST) pin.')
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 1
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 88
  end
end
