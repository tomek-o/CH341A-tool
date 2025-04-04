object frmCH341I2CCH455: TfrmCH341I2CCH455
  Left = 0
  Top = 0
  Caption = 'CH455 display + keyboard'
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
    Top = 269
    Width = 4
    Height = 16
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object lblInfo: TLabel
    Left = 113
    Top = 14
    Width = 257
    Height = 13
    Caption = 'LED display and keyboard driver using I2C connection'
  end
  object lblIntensity: TLabel
    Left = 113
    Top = 38
    Width = 62
    Height = 13
    Caption = 'LED intensity'
  end
  object lblMode: TLabel
    Left = 113
    Top = 63
    Width = 26
    Height = 13
    Caption = 'Mode'
  end
  object lblSegmentCodes: TLabel
    Left = 17
    Top = 161
    Width = 72
    Height = 13
    Caption = 'segment codes'
  end
  object lblDigit0: TLabel
    Left = 113
    Top = 135
    Width = 175
    Height = 14
    Caption = 'DIGIT #0   HEX        BIN'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
  end
  object lblDigit1: TLabel
    Left = 113
    Top = 160
    Width = 175
    Height = 14
    Caption = 'DIGIT #1   HEX        BIN'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
  end
  object lblDigit2: TLabel
    Left = 113
    Top = 185
    Width = 175
    Height = 14
    Caption = 'DIGIT #2   HEX        BIN'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
  end
  object lblDigit3: TLabel
    Left = 113
    Top = 211
    Width = 175
    Height = 14
    Caption = 'DIGIT #3   HEX        BIN'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
  end
  object lblBits: TLabel
    Left = 297
    Top = 116
    Width = 105
    Height = 14
    Caption = '7 6 5 4 3 2 1 0'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
  end
  object btnRead: TButton
    Left = 8
    Top = 233
    Width = 91
    Height = 25
    Caption = 'Read key code'
    TabOrder = 6
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 113
    Top = 237
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 500 ms)'
    TabOrder = 7
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
  end
  object btnInit: TButton
    Left = 8
    Top = 9
    Width = 91
    Height = 25
    Caption = 'Init'
    TabOrder = 0
    OnClick = btnInitClick
  end
  object cbIntensity: TComboBox
    Left = 181
    Top = 35
    Width = 92
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    ParentFont = False
    TabOrder = 1
    Items.Strings = (
      '1/8'
      '2/8'
      '3/8'
      '4/8'
      '5/8'
      '6/8'
      '7/8'
      '8/8')
  end
  object cbMode: TComboBox
    Left = 181
    Top = 60
    Width = 92
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    ItemIndex = 1
    ParentFont = False
    TabOrder = 2
    Text = '8-segment'
    Items.Strings = (
      '7-segment'
      '8-segment')
  end
  object chbSleep: TCheckBox
    Left = 113
    Top = 88
    Width = 97
    Height = 17
    Caption = 'Sleep'
    TabOrder = 3
  end
  object chbEnable: TCheckBox
    Left = 220
    Top = 88
    Width = 216
    Height = 17
    Caption = 'Enable display and keyboard scanning'
    Checked = True
    State = cbChecked
    TabOrder = 4
  end
  object btnWriteDigits: TButton
    Left = 8
    Top = 130
    Width = 91
    Height = 25
    Caption = 'Write digits'
    TabOrder = 5
    OnClick = btnWriteDigitsClick
  end
  object edDigit0: TEdit
    Left = 215
    Top = 132
    Width = 36
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 8
    Text = '0x01'
    OnChange = edDigitHexChange
  end
  object edDigit1: TEdit
    Left = 215
    Top = 157
    Width = 36
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 9
    Text = '0x03'
    OnChange = edDigitHexChange
  end
  object edDigit2: TEdit
    Left = 215
    Top = 182
    Width = 36
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 10
    Text = '0x07'
    OnChange = edDigitHexChange
  end
  object edDigit3: TEdit
    Left = 215
    Top = 208
    Width = 36
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 11
    Text = '0x0F'
    OnChange = edDigitHexChange
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 384
    Top = 233
  end
end
