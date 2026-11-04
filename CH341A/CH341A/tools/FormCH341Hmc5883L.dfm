object frmCH341Hmc5883L: TfrmCH341Hmc5883L
  Left = 0
  Top = 0
  Caption = 'HMC5883L magnetometer'
  ClientHeight = 317
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
    Top = 108
    Width = 121
    Height = 13
  end
  object lblLegend: TLabel
    Left = 8
    Top = 133
    Width = 435
    Height = 13
    Caption = 
      'Axis            Raw walue                     Offset to compensa' +
      'te          Magnetic induction (Gauss)'
  end
  object lblX: TLabel
    Left = 8
    Top = 155
    Width = 6
    Height = 13
    Caption = 'X'
  end
  object lblY: TLabel
    Left = 8
    Top = 178
    Width = 6
    Height = 13
    Caption = 'Y'
  end
  object lblZ: TLabel
    Left = 8
    Top = 201
    Width = 6
    Height = 13
    Caption = 'Z'
  end
  object lblAveraging: TLabel
    Left = 98
    Top = 32
    Width = 49
    Height = 13
    Caption = 'Averaging'
  end
  object lblOutputRate: TLabel
    Left = 250
    Top = 32
    Width = 57
    Height = 13
    Caption = 'Output rate'
  end
  object lblRange: TLabel
    Left = 98
    Top = 54
    Width = 31
    Height = 13
    Caption = 'Range'
  end
  object lblBias: TLabel
    Left = 250
    Top = 54
    Width = 48
    Height = 13
    Caption = 'Bias / test'
  end
  object lblNote: TLabel
    Left = 8
    Top = 220
    Width = 338
    Height = 13
    Caption = 
      'Note: my (fake?) sensor constantly returns -4096 / overflow for ' +
      'Z axis'
  end
  object lblCalibration: TLabel
    Left = 8
    Top = 267
    Width = 51
    Height = 13
    Caption = 'Calibration'
  end
  object lblPointsCount: TLabel
    Left = 64
    Top = 292
    Width = 68
    Height = 13
    Caption = 'Data points: 0'
  end
  object lblMiddleX: TLabel
    Left = 160
    Top = 292
    Width = 39
    Height = 13
    Caption = 'Middle X'
  end
  object lblMiddleY: TLabel
    Left = 288
    Top = 292
    Width = 39
    Height = 13
    Caption = 'Middle Y'
  end
  object lblHeading: TLabel
    Left = 8
    Top = 239
    Width = 146
    Height = 13
    Caption = 'Heading degrees for X-Y plane'
  end
  object lblI2CAddress: TLabel
    Left = 98
    Top = 7
    Width = 126
    Height = 13
    Caption = 'Device I2C address (7-bit)'
  end
  object btnRead: TButton
    Left = 8
    Top = 77
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 1
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 98
    Top = 81
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 500 ms)'
    TabOrder = 2
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
  end
  object btnInit: TButton
    Left = 8
    Top = 9
    Width = 75
    Height = 25
    Caption = 'Init'
    TabOrder = 0
    OnClick = btnInitClick
  end
  object edRawX: TEdit
    Left = 64
    Top = 152
    Width = 65
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 3
  end
  object edMagneticInductionX: TEdit
    Left = 313
    Top = 152
    Width = 65
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 4
  end
  object edRawY: TEdit
    Left = 64
    Top = 175
    Width = 65
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 5
  end
  object edMagneticInductionY: TEdit
    Left = 313
    Top = 175
    Width = 65
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 6
  end
  object edRawZ: TEdit
    Left = 64
    Top = 198
    Width = 65
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 7
  end
  object edMagneticInductionZ: TEdit
    Left = 313
    Top = 198
    Width = 65
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 8
  end
  object cbAvgSel: TComboBox
    Left = 155
    Top = 29
    Width = 78
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 9
  end
  object cbOutputRateSel: TComboBox
    Left = 313
    Top = 29
    Width = 65
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 10
  end
  object cbGainSel: TComboBox
    Left = 155
    Top = 51
    Width = 78
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 11
  end
  object cbMeasModeSel: TComboBox
    Left = 313
    Top = 51
    Width = 104
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 12
  end
  object chbCalibrationCollect: TCheckBox
    Left = 88
    Top = 266
    Width = 97
    Height = 17
    Caption = 'collect data'
    TabOrder = 13
  end
  object btnCopyData: TButton
    Left = 191
    Top = 262
    Width = 171
    Height = 25
    Caption = 'Copy data to clipboard as CSV'
    TabOrder = 14
    OnClick = btnCopyDataClick
  end
  object btnResetData: TButton
    Left = 368
    Top = 262
    Width = 75
    Height = 25
    Caption = 'Reset data'
    TabOrder = 15
    OnClick = btnResetDataClick
  end
  object edMiddleX: TEdit
    Left = 205
    Top = 289
    Width = 65
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 16
  end
  object edMiddleY: TEdit
    Left = 333
    Top = 289
    Width = 65
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 17
  end
  object edOffsetX: TEdit
    Left = 177
    Top = 152
    Width = 65
    Height = 21
    TabOrder = 18
    Text = '0'
  end
  object edOffsetY: TEdit
    Left = 177
    Top = 175
    Width = 65
    Height = 21
    TabOrder = 19
    Text = '0'
  end
  object edOffsetZ: TEdit
    Left = 177
    Top = 198
    Width = 65
    Height = 21
    TabOrder = 20
    Text = '0'
  end
  object edHeadingDegrees: TEdit
    Left = 177
    Top = 236
    Width = 65
    Height = 21
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 21
  end
  object cbI2CAddress: TComboBox
    Left = 241
    Top = 4
    Width = 96
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    ParentFont = False
    TabOrder = 22
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 77
  end
end
