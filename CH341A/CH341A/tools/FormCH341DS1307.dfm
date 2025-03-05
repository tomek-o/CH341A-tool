object frmCH341DS1307: TfrmCH341DS1307
  Left = 0
  Top = 0
  Caption = 'DS1307 RTC'
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
    Top = 104
    Width = 52
    Height = 13
  end
  object lblDateTime: TLabel
    Left = 8
    Top = 131
    Width = 11
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'Courier New'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblNote: TLabel
    Left = 8
    Top = 208
    Width = 382
    Height = 13
    Caption = 
      'Note: this RTC requires 5V supply voltage. SQW/OUT requires pull' +
      '-up if is used.'
  end
  object btnRead: TButton
    Left = 8
    Top = 64
    Width = 75
    Height = 25
    Caption = 'Read time'
    TabOrder = 0
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 68
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 500 ms)'
    TabOrder = 1
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
  end
  object btnSetCurrentTime: TButton
    Left = 8
    Top = 15
    Width = 233
    Height = 25
    Caption = 'Write current time from PC and start clock'
    TabOrder = 2
    OnClick = btnSetCurrentTimeClick
  end
  object btnSetTrickleCharge: TButton
    Left = 8
    Top = 171
    Width = 161
    Height = 25
    Caption = 'Set SQW/OUT open drain pin'
    TabOrder = 3
    OnClick = btnSetTrickleChargeClick
  end
  object cbControl: TComboBox
    Left = 192
    Top = 173
    Width = 217
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 4
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 64
  end
end
