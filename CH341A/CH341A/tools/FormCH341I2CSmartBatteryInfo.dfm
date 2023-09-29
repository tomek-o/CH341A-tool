object frmCH341I2CSmartBatteryInfo: TfrmCH341I2CSmartBatteryInfo
  Left = 0
  Top = 0
  Caption = 'Smart Battery info'
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
  object memoInfo: TMemo
    Left = 0
    Top = 41
    Width = 635
    Height = 258
    Align = alClient
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 0
    ExplicitTop = 39
  end
  object pnlTop: TPanel
    Left = 0
    Top = 0
    Width = 635
    Height = 41
    Align = alTop
    TabOrder = 1
    ExplicitLeft = 296
    ExplicitTop = -7
    ExplicitWidth = 185
    object lblI2CAddress: TLabel
      Left = 193
      Top = 13
      Width = 126
      Height = 13
      Caption = 'Device I2C address (7-bit)'
    end
    object btnReadInfo: TButton
      Left = 16
      Top = 8
      Width = 153
      Height = 25
      Caption = 'Read Smart Battery info'
      TabOrder = 0
      OnClick = btnReadInfoClick
    end
    object cbI2CAddress: TComboBox
      Left = 337
      Top = 10
      Width = 96
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
    end
  end
end
