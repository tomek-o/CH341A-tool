object frmCH341MdioLan8742Cbln: TfrmCH341MdioLan8742Cbln
  Left = 0
  Top = 0
  Caption = 'LAN8742 CBLN'
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
    Width = 391
    Height = 13
    Caption = 
      'Cable length estimation for active 100 Mbps link, up to 120 m, w' +
      'ith +/- 20 m error'
  end
  object lblCbln: TLabel
    Left = 8
    Top = 168
    Width = 94
    Height = 13
    Caption = 'CBLN register value'
  end
  object lblEstLen: TLabel
    Left = 8
    Top = 200
    Width = 108
    Height = 13
    Caption = 'Estimated cable length'
  end
  object lblM: TLabel
    Left = 199
    Top = 200
    Width = 8
    Height = 13
    Caption = 'm'
  end
  object lblPhyAddress: TLabel
    Left = 8
    Top = 93
    Width = 61
    Height = 13
    Caption = 'PHY Address'
  end
  object btnRead: TButton
    Left = 8
    Top = 120
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 0
    OnClick = btnReadClick
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
  object edCbln: TEdit
    Left = 136
    Top = 165
    Width = 57
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 2
  end
  object edLength: TEdit
    Left = 136
    Top = 197
    Width = 57
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 3
  end
  object cbPhyAddress: TComboBox
    Left = 104
    Top = 90
    Width = 46
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 4
  end
end
