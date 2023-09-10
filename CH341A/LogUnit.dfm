object frmLog: TfrmLog
  Left = 0
  Top = 0
  Caption = 'Log'
  ClientHeight = 294
  ClientWidth = 635
  Color = clBtnFace
  Constraints.MinHeight = 80
  Constraints.MinWidth = 200
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  ScreenSnap = True
  OnClose = FormClose
  OnKeyPress = FormKeyPress
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object redMain: TRichEdit
    Left = 0
    Top = 0
    Width = 635
    Height = 264
    Align = alClient
    PopupMenu = pmenuRichEdit
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object pnlBottom: TPanel
    Left = 0
    Top = 264
    Width = 635
    Height = 30
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object btnClear: TButton
      Left = 8
      Top = 6
      Width = 73
      Height = 20
      Caption = 'Clear'
      TabOrder = 0
      OnClick = btnClearClick
    end
    object chbAutoScroll: TCheckBox
      Left = 103
      Top = 6
      Width = 97
      Height = 17
      Caption = 'Auto Scroll'
      Checked = True
      State = cbChecked
      TabOrder = 1
    end
    object chbLogToFile: TCheckBox
      Left = 206
      Top = 6
      Width = 251
      Height = 17
      Caption = 'Log to file'
      TabOrder = 2
      OnClick = chbLogToFileClick
    end
  end
  object tmrUpdate: TTimer
    Interval = 500
    OnTimer = tmrUpdateTimer
    Left = 8
    Top = 8
  end
  object pmenuRichEdit: TPopupMenu
    Left = 8
    Top = 40
    object miCopy: TMenuItem
      Caption = '&Copy'
      OnClick = miCopyClick
    end
    object miSaveToFile: TMenuItem
      Caption = 'Save to file'
      OnClick = miSaveToFileClick
    end
  end
  object saveDialog: TSaveDialog
    DefaultExt = 'log'
    Filter = 'Log files (*.log)|*.log|Text files (*.txt)|*.txt|Any files|*.*'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
    Left = 8
    Top = 72
  end
end
