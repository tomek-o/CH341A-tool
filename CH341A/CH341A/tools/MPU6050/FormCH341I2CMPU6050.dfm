object frmCH341I2CMPU6050: TfrmCH341I2CMPU6050
  Left = 0
  Top = 0
  Caption = 'MPU6050 IMU'
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
    Top = 161
    Width = 3
    Height = 13
  end
  object lblAddress: TLabel
    Left = 8
    Top = 8
    Width = 199
    Height = 13
    Caption = 'I2C address, depending on SDO pin state'
  end
  object lblAcceleration: TLabel
    Left = 8
    Top = 187
    Width = 233
    Height = 13
    Caption = 
      'Acceleration      X                      Y                      ' +
      '  Z'
  end
  object lblGyro: TLabel
    Left = 8
    Top = 212
    Width = 233
    Height = 13
    Caption = 
      'Gyro                  X                      Y                  ' +
      '      Z'
  end
  object lblRange: TLabel
    Left = 8
    Top = 58
    Width = 54
    Height = 13
    Caption = 'Gyro range'
  end
  object lblDps: TLabel
    Left = 170
    Top = 58
    Width = 123
    Height = 13
    Caption = 'dps (degrees per second)'
  end
  object lblAccelRange: TLabel
    Left = 8
    Top = 33
    Width = 169
    Height = 13
    Caption = 'Acceleration range                        G'
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
  object chbAutoRead: TCheckBox
    Left = 89
    Top = 124
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 500 ms)'
    TabOrder = 1
    OnClick = chbAutoReadClick
    OnEnter = chbAutoReadEnter
  end
  object cbAddress: TComboBox
    Left = 224
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
    ItemIndex = 0
    ParentFont = False
    TabOrder = 2
    Text = '0x68'
    Items.Strings = (
      '0x68'
      '0x69')
  end
  object btnInit: TButton
    Left = 8
    Top = 89
    Width = 75
    Height = 25
    Caption = 'Init'
    TabOrder = 3
    OnClick = btnInitClick
  end
  object edAccelerationX: TEdit
    Left = 96
    Top = 184
    Width = 49
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
  end
  object edAccelerationY: TEdit
    Left = 168
    Top = 184
    Width = 49
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 5
  end
  object edAccelerationZ: TEdit
    Left = 247
    Top = 184
    Width = 49
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 6
  end
  object edGyroX: TEdit
    Left = 96
    Top = 209
    Width = 49
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 7
  end
  object edGyroY: TEdit
    Left = 168
    Top = 209
    Width = 49
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 8
  end
  object edGyroZ: TEdit
    Left = 247
    Top = 209
    Width = 49
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 9
  end
  object cbGyroRange: TComboBox
    Left = 110
    Top = 55
    Width = 51
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    ItemIndex = 1
    ParentFont = False
    TabOrder = 10
    Text = '500'
    Items.Strings = (
      '250'
      '500'
      '1000'
      '2000')
  end
  object cbAccelerationRange: TComboBox
    Left = 110
    Top = 30
    Width = 51
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
    TabOrder = 11
    Text = '2'
    Items.Strings = (
      '2'
      '4'
      '8'
      '16')
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 120
  end
end
