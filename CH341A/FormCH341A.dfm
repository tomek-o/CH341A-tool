object frmCH341A: TfrmCH341A
  Left = 0
  Top = 0
  Align = alClient
  BorderStyle = bsNone
  Caption = 'CH341A'
  ClientHeight = 338
  ClientWidth = 651
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object lblBytes: TLabel
    Left = 175
    Top = 61
    Width = 35
    Height = 13
    Caption = 'byte(s)'
  end
  object btnOpen: TButton
    Left = 16
    Top = 16
    Width = 75
    Height = 25
    Caption = 'Open'
    TabOrder = 0
    OnClick = btnOpenClick
  end
  object btnClose: TButton
    Left = 104
    Top = 16
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 1
    OnClick = btnCloseClick
  end
  object btnRead: TButton
    Left = 16
    Top = 56
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 2
    OnClick = btnReadClick
  end
  object cbReadBytesCount: TComboBox
    Left = 104
    Top = 58
    Width = 65
    Height = 21
    ItemHeight = 13
    ItemIndex = 1
    TabOrder = 3
    Text = '2'
    Items.Strings = (
      '1'
      '2'
      '3'
      '4'
      '8'
      '16'
      '32'
      '64'
      '128'
      '256'
      '512'
      '1024'
      '2048'
      '4096')
  end
  object memoRead: TMemo
    Left = 104
    Top = 85
    Width = 289
    Height = 76
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
  end
  object btnDetectDevices: TButton
    Left = 16
    Top = 200
    Width = 75
    Height = 25
    Caption = 'Detect'
    TabOrder = 5
    OnClick = btnDetectDevicesClick
  end
  object memoDetect: TMemo
    Left = 104
    Top = 202
    Width = 289
    Height = 76
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 6
  end
end
