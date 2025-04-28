object frmCH341I2CDetect: TfrmCH341I2CDetect
  Left = 0
  Top = 0
  Caption = 'I2C detect'
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
  object lblOutputFormatting: TLabel
    Left = 97
    Top = 54
    Width = 87
    Height = 13
    Caption = 'Output formatting'
  end
  object btnDetectDevices: TButton
    Left = 16
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Detect'
    TabOrder = 0
    OnClick = btnDetectDevicesClick
  end
  object memoDetect: TMemo
    Left = 16
    Top = 78
    Width = 393
    Height = 213
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
    TabOrder = 3
  end
  object chbAutoDetect: TCheckBox
    Left = 97
    Top = 12
    Width = 264
    Height = 17
    Caption = 'auto detect (repeat detection every second)'
    TabOrder = 1
    OnClick = chbAutoDetectClick
    OnMouseDown = chbAutoDetectMouseDown
  end
  object chbBeep: TCheckBox
    Left = 97
    Top = 33
    Width = 264
    Height = 17
    Caption = 'beep when something is detected'
    TabOrder = 2
  end
  object cbOutputFormatting: TComboBox
    Left = 190
    Top = 51
    Width = 145
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 4
    Text = 'list'
    Items.Strings = (
      'list'
      'Linux i2cdetect style')
  end
  object tmrAutoDetect: TTimer
    Enabled = False
    OnTimer = tmrAutoDetectTimer
    Left = 384
    Top = 8
  end
end
