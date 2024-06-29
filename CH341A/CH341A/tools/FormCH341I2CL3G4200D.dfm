object frmCH341I2CL3G4200D: TfrmCH341I2CL3G4200D
  Left = 0
  Top = 0
  Caption = 'L3G4200D gyro'
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
    Top = 144
    Width = 3
    Height = 13
  end
  object lblAddress: TLabel
    Left = 8
    Top = 8
    Width = 199
    Height = 13
    Caption = 'I2C address, depending on SDO pin state'
  end
  object lblX: TLabel
    Left = 8
    Top = 167
    Width = 26
    Height = 13
    Caption = 'X dps'
  end
  object lblY: TLabel
    Left = 8
    Top = 192
    Width = 26
    Height = 13
    Caption = 'Y dps'
  end
  object lblZ: TLabel
    Left = 8
    Top = 217
    Width = 26
    Height = 13
    Caption = 'Z dps'
  end
  object lblRange: TLabel
    Left = 8
    Top = 38
    Width = 31
    Height = 13
    Caption = 'Range'
  end
  object lblDps: TLabel
    Left = 136
    Top = 38
    Width = 123
    Height = 13
    Caption = 'dps (degrees per second)'
  end
  object btnRead: TButton
    Left = 8
    Top = 96
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 0
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 100
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 500 ms)'
    TabOrder = 1
  end
  object cbAddress: TComboBox
    Left = 224
    Top = 5
    Width = 65
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
    Text = '0x69'
    Items.Strings = (
      '0x68'
      '0x69')
  end
  object btnInit: TButton
    Left = 8
    Top = 65
    Width = 75
    Height = 25
    Caption = 'Init'
    TabOrder = 3
    OnClick = btnInitClick
  end
  object edX: TEdit
    Left = 40
    Top = 162
    Width = 81
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
  object edY: TEdit
    Left = 40
    Top = 189
    Width = 81
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
  object edZ: TEdit
    Left = 40
    Top = 214
    Width = 81
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
  object cbRange: TComboBox
    Left = 56
    Top = 35
    Width = 65
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    ItemIndex = 0
    ParentFont = False
    TabOrder = 7
    Text = '250'
    Items.Strings = (
      '250'
      '500'
      '2000')
  end
  object tmrAutoRead: TTimer
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 96
  end
end
