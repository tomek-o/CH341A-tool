object frmCH341I2CMCP4725: TfrmCH341I2CMCP4725
  Left = 0
  Top = 0
  Caption = 'MCP4725 DAC'
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
  object lblStatus: TLabel
    Left = 8
    Top = 248
    Width = 3
    Height = 13
  end
  object lblAddress: TLabel
    Left = 8
    Top = 64
    Width = 315
    Height = 13
    Caption = 'I2C address, depending on A0 pin state and factory programming'
  end
  object lblValue: TLabel
    Left = 8
    Top = 104
    Width = 85
    Height = 13
    Caption = 'Value (0 ... 4095)'
  end
  object lblVal: TLabel
    Left = 177
    Top = 104
    Width = 3
    Height = 13
  end
  object lblRead: TLabel
    Left = 200
    Top = 192
    Width = 3
    Height = 13
  end
  object btnRead: TButton
    Left = 8
    Top = 187
    Width = 169
    Height = 25
    Caption = 'Read DAC and EEPROM'
    TabOrder = 0
    OnClick = btnReadClick
  end
  object cbAddress: TComboBox
    Left = 334
    Top = 61
    Width = 65
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    ItemIndex = 0
    ParentFont = False
    TabOrder = 1
    Text = '0x60'
    Items.Strings = (
      '0x60'
      '0x61'
      '0x62'
      '0x63'
      '0x64'
      '0x65'
      '0x66'
      '0x67')
  end
  object memo: TMemo
    Left = 8
    Top = 4
    Width = 619
    Height = 37
    Anchors = [akLeft, akTop, akRight]
    Color = clBtnFace
    Lines.Strings = (
      '12-bit DAC, Vdd = 2.7 ... 5.5 V, using Vdd as reference voltage.'
      
        'Includes EEPROM for storing default value to be used after resta' +
        'rt.')
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 2
  end
  object edValue: TEdit
    Left = 112
    Top = 101
    Width = 49
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 3
    Text = '2048'
    OnChange = edValueChange
  end
  object btnWriteDacFastMode: TButton
    Left = 8
    Top = 129
    Width = 169
    Height = 25
    Caption = 'Write DAC using fast mode'
    TabOrder = 4
    OnClick = btnWriteDacFastModeClick
  end
  object btnWriteDacAndEeprom: TButton
    Left = 200
    Top = 129
    Width = 169
    Height = 25
    Caption = 'Write DAC and EEPROM'
    TabOrder = 5
    OnClick = btnWriteDacAndEepromClick
  end
end
