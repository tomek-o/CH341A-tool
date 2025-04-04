object frmCH341MdioWriteRead: TfrmCH341MdioWriteRead
  Left = 0
  Top = 0
  Caption = 'MDIO write/read'
  ClientHeight = 420
  ClientWidth = 574
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
    Top = 44
    Width = 3
    Height = 13
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblPhyAddress: TLabel
    Left = 8
    Top = 69
    Width = 61
    Height = 13
    Caption = 'PHY Address'
  end
  object lblInfo: TLabel
    Left = 104
    Top = 13
    Width = 327
    Height = 13
    Caption = 
      'Sets CH341A TXD pin as MDC (clock), RXD pin as MDIO (data in/out' +
      ')'
  end
  object lblRegister: TLabel
    Left = 8
    Top = 96
    Width = 40
    Height = 13
    Caption = 'Register'
  end
  object lblHex: TLabel
    Left = 8
    Top = 162
    Width = 48
    Height = 13
    Caption = 'HEX value'
  end
  object lblDec: TLabel
    Left = 232
    Top = 162
    Width = 49
    Height = 13
    Caption = 'DEC value'
  end
  object lblBin: TLabel
    Left = 10
    Top = 186
    Width = 17
    Height = 13
    Caption = 'BIN'
  end
  object btnInit: TButton
    Left = 8
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Init'
    TabOrder = 0
    OnClick = btnInitClick
  end
  object btnReadRegister: TButton
    Left = 76
    Top = 121
    Width = 100
    Height = 25
    Caption = 'Read'
    TabOrder = 1
    OnClick = btnReadRegisterClick
  end
  object cbPhyAddress: TComboBox
    Left = 76
    Top = 66
    Width = 61
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 2
  end
  object cbRegId: TComboBox
    Left = 76
    Top = 92
    Width = 345
    Height = 24
    Style = csDropDownList
    DropDownCount = 12
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Courier New'
    Font.Style = [fsBold]
    ItemHeight = 16
    ParentFont = False
    TabOrder = 3
    OnChange = cbRegIdChange
  end
  object edValueHex: TEdit
    Left = 76
    Top = 157
    Width = 121
    Height = 24
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
    OnChange = edValueHexChange
  end
  object edValueDec: TEdit
    Left = 300
    Top = 157
    Width = 121
    Height = 24
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 5
    OnChange = edValueDecChange
  end
  object chlbox: TCheckListBox
    Left = 8
    Top = 186
    Width = 560
    Height = 229
    OnClickCheck = chlboxClickCheck
    Align = alCustom
    Anchors = [akLeft, akTop, akRight, akBottom]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    Items.Strings = (
      'bit 15'
      'bit 14'
      'bit 13'
      'bit 12'
      'bit 11'
      'bit 10'
      'bit 9'
      'bit 8'
      'bit 7'
      'bit 6'
      'bit 5'
      'bit 4'
      'bit 3'
      'bit 2'
      'bit 1'
      'bit 0')
    ParentFont = False
    TabOrder = 6
  end
  object btnWriteRegister: TButton
    Left = 203
    Top = 121
    Width = 100
    Height = 25
    Caption = 'Write'
    TabOrder = 7
    OnClick = btnWriteRegisterClick
  end
end
