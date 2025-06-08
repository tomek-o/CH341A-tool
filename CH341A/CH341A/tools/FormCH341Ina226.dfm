object frmCH341Ina226: TfrmCH341Ina226
  Left = 0
  Top = 0
  Caption = 'INA226 power monitor'
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
    Top = 167
    Width = 81
    Height = 13
  end
  object lblAddress: TLabel
    Left = 10
    Top = 8
    Width = 260
    Height = 13
    Caption = 'I2C address, depending on A0 and A1 pins connection'
  end
  object lblAveraging: TLabel
    Left = 89
    Top = 36
    Width = 49
    Height = 13
    Caption = 'Averaging'
  end
  object lblVbusConversion: TLabel
    Left = 89
    Top = 60
    Width = 103
    Height = 13
    Caption = 'VBUS conversion time'
  end
  object lblShuntConversion: TLabel
    Left = 89
    Top = 84
    Width = 111
    Height = 13
    Caption = 'SHUNT conversion time'
  end
  object lblVbus: TLabel
    Left = 8
    Top = 200
    Width = 272
    Height = 13
    Caption = 
      'VBUS          raw register                      voltage         ' +
      '        V'
  end
  object lblShunt: TLabel
    Left = 8
    Top = 232
    Width = 405
    Height = 13
    Caption = 
      'SHUNT       raw register                      voltage           ' +
      '      mV       current                     A'
  end
  object lblCalibration: TLabel
    Left = 89
    Top = 107
    Width = 316
    Height = 13
    Caption = 
      'SHUNT resistance             Ohm      max expected current      ' +
      '       A'
  end
  object lblPower: TLabel
    Left = 8
    Top = 264
    Width = 275
    Height = 13
    Caption = 
      'Power        raw register                                       ' +
      '            W'
  end
  object btnRead: TButton
    Left = 8
    Top = 136
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 2
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 140
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 500 ms)'
    TabOrder = 3
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
  end
  object cbAddress: TComboBox
    Left = 276
    Top = 5
    Width = 65
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    ParentFont = False
    TabOrder = 0
  end
  object btnInit: TButton
    Left = 8
    Top = 31
    Width = 75
    Height = 25
    Caption = 'Init'
    TabOrder = 1
    OnClick = btnInitClick
  end
  object cbAvgSel: TComboBox
    Left = 144
    Top = 33
    Width = 89
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 4
  end
  object cbVbusConversionSel: TComboBox
    Left = 206
    Top = 57
    Width = 89
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 5
  end
  object cbShuntConversionSel: TComboBox
    Left = 276
    Top = 5
    Width = 89
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 6
  end
  object edVbusVoltage: TEdit
    Left = 225
    Top = 197
    Width = 45
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 7
  end
  object edShuntVoltage: TEdit
    Left = 225
    Top = 229
    Width = 45
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 8
  end
  object edVbusReg: TEdit
    Left = 124
    Top = 197
    Width = 45
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 9
  end
  object edShuntReg: TEdit
    Left = 124
    Top = 229
    Width = 45
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 10
  end
  object edShuntCurrent: TEdit
    Left = 345
    Top = 229
    Width = 56
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 11
  end
  object edShuntResistance: TEdit
    Left = 176
    Top = 104
    Width = 34
    Height = 21
    TabOrder = 12
    Text = '0.1'
  end
  object edShuntMaxExpectedCurrent: TEdit
    Left = 361
    Top = 104
    Width = 34
    Height = 21
    TabOrder = 13
    Text = '0.8'
  end
  object edPower: TEdit
    Left = 225
    Top = 261
    Width = 45
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 14
  end
  object edPowerReg: TEdit
    Left = 124
    Top = 261
    Width = 45
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 15
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 136
  end
end
