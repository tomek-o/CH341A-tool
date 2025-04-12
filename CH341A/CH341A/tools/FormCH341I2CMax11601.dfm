object frmCH341I2CMax11601: TfrmCH341I2CMax11601
  Left = 0
  Top = 0
  Caption = 'MAX11601 ADC'
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
    Top = 80
    Width = 5
    Height = 19
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object lblReferenceVoltage: TLabel
    Left = 8
    Top = 8
    Width = 89
    Height = 13
    Caption = 'Reference voltage'
  end
  object lblAin0: TLabel
    Left = 8
    Top = 120
    Width = 24
    Height = 13
    Caption = 'AIN0'
  end
  object lblAin1: TLabel
    Left = 8
    Top = 152
    Width = 24
    Height = 13
    Caption = 'AIN1'
  end
  object lblAin2: TLabel
    Left = 8
    Top = 184
    Width = 24
    Height = 13
    Caption = 'AIN2'
  end
  object lblAin3: TLabel
    Left = 8
    Top = 216
    Width = 53
    Height = 13
    Caption = 'AIN3 / REF'
  end
  object btnRead: TButton
    Left = 8
    Top = 32
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 0
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 36
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 500 ms)'
    TabOrder = 1
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
  end
  object cbReferenceVoltage: TComboBox
    Left = 103
    Top = 5
    Width = 290
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 2
    OnChange = cbReferenceVoltageChange
    Items.Strings = (
      '')
  end
  object edAin0: TEdit
    Left = 103
    Top = 117
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
    TabOrder = 3
  end
  object edAin1: TEdit
    Left = 103
    Top = 149
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
    TabOrder = 4
  end
  object edAin2: TEdit
    Left = 103
    Top = 181
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
    TabOrder = 5
  end
  object edAin3: TEdit
    Left = 103
    Top = 213
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
    TabOrder = 6
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 32
  end
end
