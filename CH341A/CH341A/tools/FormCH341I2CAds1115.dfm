object frmCH341I2CAds1115: TfrmCH341I2CAds1115
  Left = 0
  Top = 0
  Caption = 'ADS1115 ADC'
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
    Top = 200
    Width = 5
    Height = 19
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object lblAddress: TLabel
    Left = 8
    Top = 8
    Width = 58
    Height = 13
    Caption = 'I2C address'
  end
  object lblMux: TLabel
    Left = 8
    Top = 36
    Width = 21
    Height = 13
    Caption = 'MUX'
  end
  object lblRange: TLabel
    Left = 8
    Top = 64
    Width = 31
    Height = 13
    Caption = 'Range'
  end
  object lblRate: TLabel
    Left = 8
    Top = 92
    Width = 23
    Height = 13
    Caption = 'Rate'
  end
  object lblNote: TLabel
    Left = 8
    Top = 125
    Width = 341
    Height = 13
    Caption = 
      'Do not apply more than VDD + 0.3V to the analog inputs of the de' +
      'vice.'
  end
  object btnRead: TButton
    Left = 8
    Top = 152
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 1
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 156
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 500 ms)'
    TabOrder = 2
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
  end
  object cbAddress: TComboBox
    Left = 89
    Top = 5
    Width = 264
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 0
    ParentFont = False
    TabOrder = 0
  end
  object cbMux: TComboBox
    Left = 89
    Top = 33
    Width = 305
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 0
    ParentFont = False
    TabOrder = 3
  end
  object cbRange: TComboBox
    Left = 89
    Top = 61
    Width = 155
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 0
    ParentFont = False
    TabOrder = 4
  end
  object cbRate: TComboBox
    Left = 89
    Top = 89
    Width = 83
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 0
    ParentFont = False
    TabOrder = 5
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 152
  end
end
