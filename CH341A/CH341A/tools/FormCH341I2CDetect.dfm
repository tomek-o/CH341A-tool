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
    Left = 104
    Top = 10
    Width = 305
    Height = 281
    Anchors = [akLeft, akTop, akBottom]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 1
  end
end
