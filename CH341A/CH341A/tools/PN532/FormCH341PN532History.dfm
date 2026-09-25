object frmCH341PN532History: TfrmCH341PN532History
  Left = 0
  Top = 0
  Caption = 'PN532 tag history (SPI)'
  ClientHeight = 340
  ClientWidth = 700
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    700
    340)
  PixelsPerInch = 96
  TextHeight = 13
  object lblStatus: TLabel
    Left = 8
    Top = 74
    Width = 3
    Height = 13
  end
  object lblState: TLabel
    Left = 226
    Top = 14
    Width = 40
    Height = 13
    Caption = 'Stopped'
  end
  object lblLog: TLabel
    Left = 8
    Top = 95
    Width = 72
    Height = 13
    Caption = 'Detected tags:'
  end
  object btnStart: TButton
    Left = 8
    Top = 9
    Width = 121
    Height = 25
    Caption = 'Init + start scanning'
    TabOrder = 0
    OnClick = btnStartClick
  end
  object btnStop: TButton
    Left = 135
    Top = 9
    Width = 75
    Height = 25
    Caption = 'Stop'
    Enabled = False
    TabOrder = 1
    OnClick = btnStopClick
  end
  object grdLog: TStringGrid
    Left = 8
    Top = 40
    Width = 684
    Height = 261
    Anchors = [akLeft, akTop, akRight, akBottom]
    DefaultRowHeight = 18
    FixedCols = 0
    RowCount = 2
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
    TabOrder = 2
    OnSelectCell = grdLogSelectCell
    OnSetEditText = grdLogSetEditText
    ColWidths = (
      120
      110
      210
      40
      160)
  end
  object btnClearLog: TButton
    Left = 8
    Top = 307
    Width = 90
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Clear log'
    TabOrder = 3
    OnClick = btnClearLogClick
  end
  object btnExportCsv: TButton
    Left = 104
    Top = 307
    Width = 110
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Export CSV...'
    TabOrder = 4
    OnClick = btnExportCsvClick
  end
  object btnSaveHistory: TButton
    Left = 264
    Top = 307
    Width = 113
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Save history'
    TabOrder = 5
    OnClick = btnSaveHistoryClick
  end
  object tmrPoll: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrPollTimer
    Left = 384
    Top = 8
  end
  object dlgExport: TSaveDialog
    DefaultExt = 'csv'
    Filter = 'CSV files (*.csv)|*.csv|All files (*.*)|*.*'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
    Title = 'Export tag history to CSV'
    Left = 344
    Top = 8
  end
end
