object frmCH341SpiNrf24L01Sniffer: TfrmCH341SpiNrf24L01Sniffer
  Left = 0
  Top = 0
  Caption = 'nRF24L01+ sniffer'
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
    Left = 8
    Top = 52
    Width = 59
    Height = 13
    Caption = 'Radio speed'
  end
  object lblRfChannel: TLabel
    Left = 239
    Top = 52
    Width = 53
    Height = 13
    Caption = 'RF channel'
  end
  object lblNote: TLabel
    Left = 8
    Top = 30
    Width = 344
    Height = 13
    Caption = 
      'Note: requires SPI connection + CH341 TXD connected to nRF24L01 ' +
      'CE'
  end
  object lblAddressBytes: TLabel
    Left = 8
    Top = 76
    Width = 154
    Height = 13
    Caption = 'Number of address bytes to use'
  end
  object lblAddress: TLabel
    Left = 239
    Top = 76
    Width = 125
    Height = 13
    Caption = 'Address (2...5 hex bytes)'
  end
  object lblInfo: TLabel
    Left = 8
    Top = 176
    Width = 184
    Height = 13
    Caption = 'Received data is shown in log window.'
  end
  object lblInfo1: TLabel
    Left = 8
    Top = 8
    Width = 452
    Height = 13
    Caption = 
      'EXPERIMENTAL AND LIMITED - nRF24L01+ does not have real promiscu' +
      'ous mode'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object btnRead: TButton
    Left = 89
    Top = 124
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 1
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 170
    Top = 128
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading)'
    TabOrder = 2
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
  end
  object btnInit: TButton
    Left = 8
    Top = 124
    Width = 75
    Height = 25
    Caption = 'Init'
    TabOrder = 0
    OnClick = btnInitClick
  end
  object cbRfSpeed: TComboBox
    Left = 75
    Top = 49
    Width = 150
    Height = 21
    Style = csDropDownList
    ItemHeight = 0
    TabOrder = 3
  end
  object cbRfChannel: TComboBox
    Left = 314
    Top = 49
    Width = 54
    Height = 21
    Style = csDropDownList
    ItemHeight = 0
    TabOrder = 4
  end
  object cbAddressBytes: TComboBox
    Left = 176
    Top = 73
    Width = 49
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 1
    TabOrder = 5
    Text = '3'
    Items.Strings = (
      '2'
      '3'
      '4'
      '5')
  end
  object edAddress: TEdit
    Left = 367
    Top = 73
    Width = 79
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 6
    Text = '00AAAA'
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 50
    OnTimer = tmrAutoReadTimer
    Left = 457
    Top = 124
  end
end
