object frmAT58MP1T1RS32A: TfrmAT58MP1T1RS32A
  Left = 0
  Top = 0
  Caption = 'AT58MP1T1RS32A radar'
  ClientHeight = 393
  ClientWidth = 752
  Color = clBtnFace
  Constraints.MinHeight = 250
  Constraints.MinWidth = 640
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object lblDescription: TLabel
    AlignWithMargins = True
    Left = 8
    Top = 2
    Width = 736
    Height = 17
    Margins.Left = 8
    Margins.Top = 2
    Margins.Right = 8
    Margins.Bottom = 0
    Align = alTop
    AutoSize = False
    Caption = 'AT58MP1T1RS32A: 5.8 GHz microwave motion sensor (UART).'
    WordWrap = True
    ExplicitTop = 0
  end
  object paintBox: TPaintBox
    AlignWithMargins = True
    Left = 5
    Top = 58
    Width = 742
    Height = 217
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    Align = alClient
    OnPaint = paintBoxPaint
    ExplicitTop = 39
    ExplicitHeight = 236
  end
  object pnlTop: TPanel
    Left = 0
    Top = 19
    Width = 752
    Height = 34
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    object btnScreenshot: TButton
      AlignWithMargins = True
      Left = 668
      Top = 6
      Width = 80
      Height = 22
      Margins.Top = 6
      Margins.Right = 4
      Margins.Bottom = 6
      Align = alRight
      Caption = 'Screenshot'
      TabOrder = 0
      OnClick = btnScreenshotClick
    end
    object pnlUart: TPanel
      Left = 0
      Top = 0
      Width = 665
      Height = 34
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 1
    end
  end
  object pnlBottom: TPanel
    Left = 0
    Top = 280
    Width = 752
    Height = 113
    Align = alBottom
    TabOrder = 1
    object lblCommands: TLabel
      Left = 5
      Top = 3
      Width = 52
      Height = 13
      Caption = 'Commands'
    end
    object lblSetRange: TLabel
      Left = 61
      Top = 22
      Width = 46
      Height = 13
      Caption = 'setRange'
    end
    object lblRangeM: TLabel
      Left = 183
      Top = 22
      Width = 8
      Height = 13
      Caption = 'm'
    end
    object lblSetSensitivity: TLabel
      Left = 61
      Top = 46
      Width = 64
      Height = 13
      Caption = 'setSensitivity'
    end
    object lblSetLatency: TLabel
      Left = 61
      Top = 70
      Width = 89
      Height = 13
      Caption = 'setLatency    L->H'
    end
    object lblLatencyHL: TLabel
      Left = 210
      Top = 70
      Width = 24
      Height = 13
      Caption = 'H->L'
    end
    object btnWriteSetRange: TButton
      Left = 207
      Top = 19
      Width = 57
      Height = 21
      Caption = 'Write'
      TabOrder = 0
      OnClick = btnWriteSetRangeClick
    end
    object cbSetRange: TComboBox
      Left = 129
      Top = 19
      Width = 48
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      ItemIndex = 6
      TabOrder = 1
      Text = '6'
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11')
    end
    object cbSetSensitivity: TComboBox
      Left = 129
      Top = 43
      Width = 48
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      ItemIndex = 7
      TabOrder = 2
      Text = '7'
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9')
    end
    object btnSetSensitivity: TButton
      Left = 207
      Top = 43
      Width = 57
      Height = 21
      Caption = 'Write'
      TabOrder = 3
      OnClick = btnSetSensitivityClick
    end
    object cbLatencyLH: TComboBox
      Left = 156
      Top = 67
      Width = 48
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      ItemIndex = 1
      TabOrder = 4
      Text = '1'
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10')
    end
    object cbLatencyHL: TComboBox
      Left = 240
      Top = 67
      Width = 48
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      ItemIndex = 0
      TabOrder = 5
      Text = '1'
      Items.Strings = (
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15')
    end
    object btnSetLatency: TButton
      Left = 294
      Top = 67
      Width = 57
      Height = 21
      Caption = 'Write'
      TabOrder = 6
      OnClick = btnSetLatencyClick
    end
    object btnResetCfg: TButton
      Left = 61
      Top = 89
      Width = 76
      Height = 21
      Caption = 'resetCfg'
      TabOrder = 7
      OnClick = btnResetCfgClick
    end
    object btnSensorStart: TButton
      Left = 143
      Top = 89
      Width = 76
      Height = 21
      Caption = 'sensorStart'
      TabOrder = 8
      OnClick = btnSensorStartClick
    end
    object chkCsv: TCheckBox
      Left = 400
      Top = 21
      Width = 200
      Height = 17
      Caption = 'Save values to a csv file'
      Checked = True
      State = cbChecked
      TabOrder = 9
    end
  end
  object tmrRefresh: TTimer
    Interval = 500
    OnTimer = tmrRefreshTimer
    Left = 688
    Top = 328
  end
end
