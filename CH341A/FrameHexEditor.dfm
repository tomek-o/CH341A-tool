object frameHexEditor: TframeHexEditor
  Left = 0
  Top = 0
  Width = 544
  Height = 244
  TabOrder = 0
  object pnlTop: TPanel
    Left = 0
    Top = 0
    Width = 544
    Height = 34
    Align = alTop
    BevelOuter = bvNone
    Caption = ''
    TabOrder = 0
    object lblGoto: TLabel
      Left = 200
      Top = 11
      Width = 61
      Height = 13
      Caption = 'Go to (hex)'
    end
    object btnOpen: TButton
      Left = 8
      Top = 5
      Width = 90
      Height = 25
      Caption = 'Open file...'
      TabOrder = 0
      OnClick = btnOpenClick
    end
    object btnSave: TButton
      Left = 104
      Top = 5
      Width = 90
      Height = 25
      Caption = 'Save file...'
      TabOrder = 1
      OnClick = btnSaveClick
    end
    object edGoto: TEdit
      Left = 267
      Top = 7
      Width = 80
      Height = 21
      TabOrder = 2
      OnKeyPress = edGotoKeyPress
    end
    object btnGoto: TButton
      Left = 353
      Top = 5
      Width = 45
      Height = 25
      Caption = 'Go'
      TabOrder = 3
      OnClick = btnGotoClick
    end
    object chkReadOnly: TCheckBox
      Left = 420
      Top = 9
      Width = 80
      Height = 17
      Caption = 'Read only'
      TabOrder = 4
      OnClick = chkReadOnlyClick
    end
  end
  object statusBar: TStatusBar
    Left = 0
    Top = 401
    Width = 640
    Height = 19
    Panels = <
      item
        Width = 160
      end
      item
        Width = 200
      end
      item
        Width = 250
      end
      item
        Width = 150
      end>
  end
  object openDialog: TOpenDialog
    Filter = 'Binary files|*.bin;*.rom;*.eep|All files|*.*'
    Left = 40
    Top = 80
  end
  object saveDialog: TSaveDialog
    DefaultExt = 'bin'
    Filter = 'Binary files|*.bin|All files|*.*'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
    Left = 80
    Top = 80
  end
end
