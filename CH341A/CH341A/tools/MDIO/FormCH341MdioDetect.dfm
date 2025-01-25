object frmCH341MdioDetect: TfrmCH341MdioDetect
  Left = 0
  Top = 0
  Caption = 'MDIO detect'
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
  object lblInfo: TLabel
    Left = 16
    Top = 40
    Width = 376
    Height = 13
    Caption = 
      'Note: this sets CH341A TXD pin as MDC (clock), RXD pin as MDIO (' +
      'data in/out)'
  end
  object lblStatus: TLabel
    Left = 112
    Top = 13
    Width = 3
    Height = 13
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
    Top = 64
    Width = 401
    Height = 227
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
    TabOrder = 1
  end
end
