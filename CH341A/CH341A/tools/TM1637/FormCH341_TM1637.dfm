object frmCH341Tm1637: TfrmCH341Tm1637
  Left = 0
  Top = 0
  Caption = 'TM1637 display'
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
    Top = 151
    Width = 75
    Height = 25
  end
  object lblConnectionInfo: TLabel
    Left = 8
    Top = 8
    Width = 381
    Height = 13
    Caption = 
      'Using SCL (-> display CLK) and SDA (<-> display DIO) CH341A pins' +
      ' for control.'
  end
  object lblInfo: TLabel
    Left = 8
    Top = 27
    Width = 437
    Height = 13
    Caption = 
      'Tested with display module with four 8-segment digits (with dots' +
      '), branded as "DIY MORE".'
  end
  object lblBrightness: TLabel
    Left = 8
    Top = 88
    Width = 50
    Height = 13
    Caption = 'Brightness'
  end
  object btnInit: TButton
    Left = 8
    Top = 49
    Width = 75
    Height = 25
    Caption = 'Init'
    TabOrder = 0
    OnClick = btnInitClick
  end
  object btnWrite: TButton
    Left = 8
    Top = 120
    Width = 75
    Height = 25
    Caption = 'Write'
    TabOrder = 1
    OnClick = btnWriteClick
  end
  object cbBrightness: TComboBox
    Left = 89
    Top = 85
    Width = 170
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 2
    OnChange = cbBrightnessChange
  end
  object edText: TEdit
    Left = 89
    Top = 120
    Width = 120
    Height = 25
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 3
    Text = '12.Ab'
  end
end
