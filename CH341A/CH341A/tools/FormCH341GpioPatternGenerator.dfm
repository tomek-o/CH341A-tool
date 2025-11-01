object frmCH341GpioPatternGenerator: TfrmCH341GpioPatternGenerator
  Left = 0
  Top = 0
  Caption = 'GPIO pattern generator'
  ClientHeight = 327
  ClientWidth = 635
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object pnlTop: TPanel
    Left = 0
    Top = 0
    Width = 635
    Height = 126
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    object lblStepTime: TLabel
      Left = 8
      Top = 34
      Width = 59
      Height = 13
      Caption = 'Step period:'
    end
    object lblMs: TLabel
      Left = 139
      Top = 34
      Width = 13
      Height = 13
      Caption = 'ms'
    end
    object lblCurrentPosition: TLabel
      Left = 8
      Top = 85
      Width = 81
      Height = 13
      Caption = 'Current position:'
    end
    object lblCurrentPositionVal: TLabel
      Left = 135
      Top = 85
      Width = 10
      Height = 13
      Caption = '-1'
    end
    object lblRows: TLabel
      Left = 8
      Top = 11
      Width = 63
      Height = 13
      Caption = 'Rows (steps)'
    end
    object lblStatus: TLabel
      Left = 8
      Top = 105
      Width = 49
      Height = 13
    end
    object edStepPeriod: TEdit
      Left = 77
      Top = 31
      Width = 52
      Height = 21
      TabOrder = 0
      Text = '500'
    end
    object btnStart: TButton
      Left = 8
      Top = 55
      Width = 121
      Height = 25
      Caption = 'Start'
      TabOrder = 1
      OnClick = btnStartClick
    end
    object btnStop: TButton
      Left = 135
      Top = 55
      Width = 121
      Height = 25
      Caption = 'Stop'
      TabOrder = 2
      OnClick = btnStopClick
    end
    object edRowsCount: TEdit
      Left = 77
      Top = 8
      Width = 52
      Height = 21
      TabOrder = 3
      Text = '10'
    end
    object btnSetRowsCount: TButton
      Left = 135
      Top = 8
      Width = 75
      Height = 21
      Caption = 'Apply'
      TabOrder = 4
      OnClick = btnSetRowsCountClick
    end
  end
  object Grid: TStringGrid
    Left = 0
    Top = 126
    Width = 635
    Height = 164
    Align = alClient
    ColCount = 33
    DefaultColWidth = 20
    DefaultRowHeight = 20
    DefaultDrawing = False
    RowCount = 11
    TabOrder = 1
    OnDrawCell = GridDrawCell
    OnKeyPress = GridKeyPress
    OnMouseDown = GridMouseDown
    ExplicitTop = 133
    ExplicitHeight = 154
  end
  object pnlBottom: TPanel
    Left = 0
    Top = 290
    Width = 635
    Height = 37
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 2
    object btnSave: TButton
      Left = 8
      Top = 7
      Width = 121
      Height = 25
      Caption = 'Save to file'
      TabOrder = 0
      OnClick = btnSaveClick
    end
    object btnLoad: TButton
      Left = 135
      Top = 7
      Width = 121
      Height = 25
      Caption = 'Load from file'
      TabOrder = 1
      OnClick = btnLoadClick
    end
  end
  object tmrUpdatePosition: TTimer
    Enabled = False
    Interval = 50
    OnTimer = tmrUpdatePositionTimer
    Left = 240
    Top = 8
  end
  object openDialog: TOpenDialog
    DefaultExt = 'json'
    Filter = 'JSON files|*.json|All files|*.*'
    Left = 264
    Top = 296
  end
  object saveDialog: TSaveDialog
    DefaultExt = 'json'
    Filter = 'JSON files|*.json|All files|*.*'
    Left = 296
    Top = 296
  end
end
