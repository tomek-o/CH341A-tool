object frmCH341SpiNrf24L01: TfrmCH341SpiNrf24L01
  Left = 0
  Top = 0
  Caption = 'nRF24L01+'
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
    Top = 96
    Width = 3
    Height = 13
  end
  object lblRfSpeed: TLabel
    Left = 89
    Top = 30
    Width = 59
    Height = 13
    Caption = 'Radio speed'
  end
  object lblRfChannel: TLabel
    Left = 320
    Top = 30
    Width = 53
    Height = 13
    Caption = 'RF channel'
  end
  object lblNote: TLabel
    Left = 8
    Top = 6
    Width = 344
    Height = 13
    Caption = 
      'Note: requires SPI connection + CH341 TXD connected to nRF24L01 ' +
      'CE'
  end
  object btnRead: TButton
    Left = 8
    Top = 56
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 1
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 60
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 500 ms)'
    TabOrder = 2
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
  end
  object btnInit: TButton
    Left = 8
    Top = 25
    Width = 75
    Height = 25
    Caption = 'Init'
    TabOrder = 0
    OnClick = btnInitClick
  end
  object cbRfSpeed: TComboBox
    Left = 156
    Top = 27
    Width = 150
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 3
  end
  object cbRfChannel: TComboBox
    Left = 381
    Top = 27
    Width = 50
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 4
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 56
  end
end
