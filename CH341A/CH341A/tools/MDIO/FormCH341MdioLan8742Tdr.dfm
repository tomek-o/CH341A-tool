object frmCH341MdioLan8742Tdr: TfrmCH341MdioLan8742Tdr
  Left = 0
  Top = 0
  Caption = 'LAN8742 TDR with MDIO'
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
    Top = 63
    Width = 3
    Height = 13
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblInfo: TLabel
    Left = 104
    Top = 37
    Width = 326
    Height = 13
    Caption = 
      'Sets CH341A SCL pin as MDC (clock), SDA pin as MDIO (data in/out' +
      ')'
  end
  object lblDesc: TLabel
    Left = 8
    Top = 8
    Width = 405
    Height = 13
    Caption = 
      'Time domain reflectometry, detecting open/short TX/RX pair, esti' +
      'mating cable length'
  end
  object lblPhyAddress: TLabel
    Left = 8
    Top = 93
    Width = 61
    Height = 13
    Caption = 'PHY Address'
  end
  object lblCableType: TLabel
    Left = 152
    Top = 93
    Width = 155
    Height = 13
    Caption = 'Cable type for length calculation'
  end
  object btnTest: TButton
    Left = 8
    Top = 128
    Width = 75
    Height = 25
    Caption = 'Test'
    TabOrder = 0
    OnClick = btnTestClick
  end
  object btnInit: TButton
    Left = 8
    Top = 32
    Width = 75
    Height = 25
    Caption = 'Init'
    TabOrder = 1
    OnClick = btnInitClick
  end
  object cbPhyAddress: TComboBox
    Left = 75
    Top = 90
    Width = 46
    Height = 21
    Style = csDropDownList
    ItemHeight = 0
    TabOrder = 2
  end
  object cbPair: TComboBox
    Left = 96
    Top = 130
    Width = 113
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 3
    Text = 'both cable pairs'
    Items.Strings = (
      'both cable pairs'
      'TX cable pair'
      'RX cable pair')
  end
  object cbCableType: TComboBox
    Left = 313
    Top = 90
    Width = 112
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 4
    Text = 'unknown / mixed'
    OnChange = cbCableTypeChange
    Items.Strings = (
      'unknown / mixed'
      'CAT 6'
      'CAT 5E'
      'CAT 5')
  end
  object grboxTx: TGroupBox
    Left = 8
    Top = 159
    Width = 417
    Height = 50
    Caption = 'TX cable pair'
    TabOrder = 5
    object lblCableTxState: TLabel
      Left = 8
      Top = 24
      Width = 53
      Height = 13
      Caption = 'Impedance'
    end
    object lblCableTxLengthRaw: TLabel
      Left = 135
      Top = 24
      Width = 73
      Height = 13
      Caption = 'Length register'
    end
    object lblCableTxLengthMeters: TLabel
      Left = 258
      Top = 24
      Width = 80
      Height = 13
      Caption = 'Length in meters'
    end
    object edCableTxState: TEdit
      Left = 65
      Top = 21
      Width = 59
      Height = 21
      Color = clBtnFace
      TabOrder = 0
    end
    object edCableTxLengthReg: TEdit
      Left = 210
      Top = 21
      Width = 35
      Height = 21
      Color = clBtnFace
      TabOrder = 1
    end
    object edCableTxLengthMeters: TEdit
      Left = 343
      Top = 21
      Width = 65
      Height = 21
      Color = clBtnFace
      TabOrder = 2
    end
  end
  object grboxRx: TGroupBox
    Left = 8
    Top = 220
    Width = 417
    Height = 50
    Caption = 'RX cable pair'
    TabOrder = 6
    object lblCableRxState: TLabel
      Left = 8
      Top = 24
      Width = 53
      Height = 13
      Caption = 'Impedance'
    end
    object lblCableRxLengthRaw: TLabel
      Left = 135
      Top = 24
      Width = 73
      Height = 13
      Caption = 'Length register'
    end
    object lblCableRxLengthMeters: TLabel
      Left = 258
      Top = 24
      Width = 80
      Height = 13
      Caption = 'Length in meters'
    end
    object edCableRxState: TEdit
      Left = 65
      Top = 21
      Width = 59
      Height = 21
      Color = clBtnFace
      TabOrder = 0
    end
    object edCableRxLengthReg: TEdit
      Left = 210
      Top = 21
      Width = 35
      Height = 21
      Color = clBtnFace
      TabOrder = 1
    end
    object edCableRxLengthMeters: TEdit
      Left = 343
      Top = 21
      Width = 65
      Height = 21
      Color = clBtnFace
      TabOrder = 2
    end
  end
end
