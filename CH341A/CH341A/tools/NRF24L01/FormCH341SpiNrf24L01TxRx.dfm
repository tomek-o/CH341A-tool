object frmCH341SpiNrf24L01TxRx: TfrmCH341SpiNrf24L01TxRx
  Left = 0
  Top = 0
  Caption = 'nRF24L01+ transmitter/receiver'
  ClientHeight = 424
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
    424)
  PixelsPerInch = 96
  TextHeight = 13
  object lblNote: TLabel
    Left = 8
    Top = 8
    Width = 344
    Height = 13
    Caption = 
      'Note: requires SPI connection + CH341 TXD connected to nRF24L01 ' +
      'CE'
  end
  object lblRfSpeed: TLabel
    Left = 8
    Top = 34
    Width = 59
    Height = 13
    Caption = 'Radio speed'
  end
  object lblRfChannel: TLabel
    Left = 239
    Top = 34
    Width = 53
    Height = 13
    Caption = 'RF channel'
  end
  object lblAddressBytes: TLabel
    Left = 8
    Top = 58
    Width = 154
    Height = 13
    Caption = 'Number of address bytes to use'
  end
  object lblPayloadLength: TLabel
    Left = 8
    Top = 82
    Width = 71
    Height = 13
    Caption = 'Payload length'
  end
  object lblStatus: TLabel
    Left = 360
    Top = 124
    Width = 3
    Height = 13
  end
  object cbRfSpeed: TComboBox
    Left = 75
    Top = 31
    Width = 150
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 0
  end
  object cbRfChannel: TComboBox
    Left = 314
    Top = 31
    Width = 54
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 1
  end
  object cbAddressBytes: TComboBox
    Left = 176
    Top = 55
    Width = 49
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 2
    TabOrder = 2
    Text = '5'
    Items.Strings = (
      '3'
      '4'
      '5')
  end
  object cbPayloadLength: TComboBox
    Left = 176
    Top = 79
    Width = 49
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 3
  end
  object chbAutoAck: TCheckBox
    Left = 239
    Top = 83
    Width = 220
    Height = 17
    Caption = 'Enable Auto-ACK (recommended)'
    Checked = True
    State = cbChecked
    TabOrder = 4
  end
  object rgMode: TRadioGroup
    Left = 8
    Top = 104
    Width = 150
    Height = 55
    Caption = 'Mode'
    ItemIndex = 0
    Items.Strings = (
      'Transmitter'
      'Receiver')
    TabOrder = 5
    OnClick = rgModeClick
  end
  object btnInit: TButton
    Left = 170
    Top = 118
    Width = 75
    Height = 25
    Caption = 'Init'
    TabOrder = 7
    OnClick = btnInitClick
  end
  object btnDumpRegisters: TButton
    Left = 251
    Top = 118
    Width = 100
    Height = 25
    Hint = 'Read all registers and write them, decoded, to the log window'
    Caption = 'Dump registers'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 6
    OnClick = btnDumpRegistersClick
  end
  object grpTx: TGroupBox
    Left = 8
    Top = 168
    Width = 619
    Height = 100
    Caption = 'Transmit'
    TabOrder = 8
    object lblTxAddress: TLabel
      Left = 8
      Top = 24
      Width = 102
      Height = 13
      Caption = 'Target address (hex)'
    end
    object lblTxData: TLabel
      Left = 8
      Top = 52
      Width = 91
      Height = 13
      Caption = 'Data to send (hex)'
    end
    object lblTxStatus: TLabel
      Left = 8
      Top = 79
      Width = 3
      Height = 13
    end
    object lblTxAddressHint: TLabel
      Left = 270
      Top = 25
      Width = 248
      Height = 13
      Caption = 'LSByte first: RF24 0xF0F0F0F0E1 = E1 F0 F0 F0 F0'
    end
    object edTxAddress: TEdit
      Left = 140
      Top = 21
      Width = 120
      Height = 22
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      Text = '1122334455'
    end
    object edTxData: TEdit
      Left = 112
      Top = 49
      Width = 350
      Height = 22
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      TabOrder = 1
      Text = '74657374'
    end
    object btnSend: TButton
      Left = 470
      Top = 48
      Width = 75
      Height = 25
      Caption = 'Send'
      TabOrder = 2
      OnClick = btnSendClick
    end
  end
  object grpRx: TGroupBox
    Left = 8
    Top = 168
    Width = 619
    Height = 248
    Anchors = [akLeft, akTop, akRight, akBottom]
    Caption = 'Receive'
    TabOrder = 9
    DesignSize = (
      619
      248)
    object lblRxAddress: TLabel
      Left = 8
      Top = 24
      Width = 98
      Height = 13
      Caption = 'Listen address (hex)'
    end
    object lblRxAddressHint: TLabel
      Left = 270
      Top = 25
      Width = 248
      Height = 13
      Caption = 'LSByte first: RF24 0xF0F0F0F0E1 = E1 F0 F0 F0 F0'
    end
    object btnRxRead: TButton
      Left = 8
      Top = 48
      Width = 75
      Height = 25
      Caption = 'Read now'
      TabOrder = 1
      OnClick = btnRxReadClick
    end
    object chbRxAutoRead: TCheckBox
      Left = 95
      Top = 52
      Width = 200
      Height = 17
      Caption = 'auto receive (poll)'
      TabOrder = 2
      OnClick = chbRxAutoReadClick
    end
    object edRxAddress: TEdit
      Left = 140
      Top = 21
      Width = 120
      Height = 22
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      Text = '1122334455'
    end
    object memoRxLog: TMemo
      Left = 8
      Top = 80
      Width = 600
      Height = 157
      Anchors = [akLeft, akTop, akRight, akBottom]
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
  end
  object tmrRxAutoRead: TTimer
    Enabled = False
    Interval = 100
    OnTimer = tmrRxAutoReadTimer
    Left = 560
    Top = 16
  end
end
