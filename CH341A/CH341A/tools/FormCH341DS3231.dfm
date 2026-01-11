object frmCH341DS3231: TfrmCH341DS3231
  Left = 0
  Top = 0
  Caption = 'DS3231 RTC'
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
    Top = 79
    Width = 3
    Height = 13
  end
  object lblDateTime: TLabel
    Left = 8
    Top = 98
    Width = 11
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'Courier New'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblOutputControl: TLabel
    Left = 8
    Top = 144
    Width = 80
    Height = 13
    Caption = 'Control registers'
  end
  object btnRead: TButton
    Left = 8
    Top = 48
    Width = 161
    Height = 25
    Caption = 'Read time and temperature'
    TabOrder = 1
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 175
    Top = 52
    Width = 242
    Height = 17
    Caption = 'auto refresh (repeat reading every 500 ms)'
    TabOrder = 2
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
  end
  object btnSetCurrentTime: TButton
    Left = 8
    Top = 7
    Width = 233
    Height = 25
    Caption = 'Write current time from PC and start clock'
    TabOrder = 0
    OnClick = btnSetCurrentTimeClick
  end
  object btnSetControl: TButton
    Left = 8
    Top = 243
    Width = 161
    Height = 25
    Caption = 'Write CONTROL registers'
    TabOrder = 8
    OnClick = btnSetControlClick
  end
  object cbControl: TComboBox
    Left = 263
    Top = 161
    Width = 138
    Height = 21
    Style = csDropDownList
    ItemHeight = 0
    TabOrder = 4
  end
  object chbSqwOutput: TCheckBox
    Left = 8
    Top = 163
    Width = 249
    Height = 17
    Caption = 'Enable SQW (square wave, open drain) output'
    TabOrder = 3
  end
  object chbEnable32k: TCheckBox
    Left = 8
    Top = 204
    Width = 201
    Height = 17
    Caption = 'Enable 32 kHz output (open-drain)'
    TabOrder = 6
  end
  object chbBbsqw: TCheckBox
    Left = 24
    Top = 183
    Width = 281
    Height = 17
    Caption = 'Enable SQW also when powered by backup battery'
    TabOrder = 5
  end
  object chbTemperatureConv: TCheckBox
    Left = 8
    Top = 223
    Width = 329
    Height = 17
    Caption = 'Force immediate temperature reading and TCXO compensation'
    TabOrder = 7
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 328
    Top = 16
  end
end
