object frmCH341DS18B20: TfrmCH341DS18B20
  Left = 0
  Top = 0
  Caption = 'DS18B20 sensor'
  ClientHeight = 361
  ClientWidth = 641
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    641
    361)
  PixelsPerInch = 96
  TextHeight = 13
  object lblPin: TLabel
    Left = 8
    Top = 112
    Width = 32
    Height = 13
    Caption = 'DQ pin'
  end
  object lblDevice: TLabel
    Left = 8
    Top = 147
    Width = 33
    Height = 13
    Caption = 'Sensor'
  end
  object lblTemperature: TLabel
    Left = 8
    Top = 282
    Width = 90
    Height = 29
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -24
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object lblDetails: TLabel
    Left = 8
    Top = 231
    Width = 619
    Height = 45
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    WordWrap = True
  end
  object lblResolution: TLabel
    Left = 8
    Top = 326
    Width = 50
    Height = 13
    Caption = 'Resolution'
  end
  object lblStatus: TLabel
    Left = 8
    Top = 349
    Width = 90
    Height = 13
  end
  object memInfo: TMemo
    Left = 8
    Top = 8
    Width = 619
    Height = 89
    TabStop = False
    Anchors = [akLeft, akTop, akRight]
    BorderStyle = bsNone
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 0
  end
  object cbPin: TComboBox
    Left = 60
    Top = 108
    Width = 260
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 1
    Text = 'D0 - SPI CS0 (socket pin 1)'
    Items.Strings = (
      'D0 - SPI CS0 (socket pin 1)'
      'D3 - SPI CLK (socket pin 6)'
      'D5 - SPI MOSI (socket pin 5)')
  end
  object btnScan: TButton
    Left = 340
    Top = 106
    Width = 90
    Height = 25
    Caption = 'Scan bus'
    TabOrder = 2
    OnClick = btnScanClick
  end
  object cbDevice: TComboBox
    Left = 60
    Top = 143
    Width = 567
    Height = 22
    Style = csDropDownList
    Anchors = [akLeft, akTop, akRight]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    ItemIndex = 0
    ParentFont = False
    TabOrder = 3
    Text = 'Single device on the bus (Skip ROM)'
    Items.Strings = (
      'Single device on the bus (Skip ROM)')
  end
  object chbParasite: TCheckBox
    Left = 8
    Top = 177
    Width = 600
    Height = 17
    Caption = 
      'Parasite power (sensor VDD connected to GND): drive DQ high duri' +
      'ng the conversion'
    TabOrder = 4
  end
  object btnRead: TButton
    Left = 8
    Top = 200
    Width = 90
    Height = 25
    Caption = 'Read'
    TabOrder = 5
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 110
    Top = 204
    Width = 300
    Height = 17
    Caption = 'auto refresh (repeat reading every 2 s)'
    TabOrder = 6
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
  end
  object cbResolution: TComboBox
    Left = 70
    Top = 322
    Width = 70
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 3
    TabOrder = 7
    Text = '12 bit'
    Items.Strings = (
      '9 bit'
      '10 bit'
      '11 bit'
      '12 bit')
  end
  object chbStore: TCheckBox
    Left = 150
    Top = 324
    Width = 200
    Height = 17
    Caption = 'also store in sensor EEPROM'
    TabOrder = 8
  end
  object btnSetResolution: TButton
    Left = 360
    Top = 319
    Width = 110
    Height = 25
    Caption = 'Set resolution'
    TabOrder = 9
    OnClick = btnSetResolutionClick
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 2000
    OnTimer = tmrAutoReadTimer
    Left = 420
    Top = 200
  end
end
