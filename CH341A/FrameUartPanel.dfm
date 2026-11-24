object frameUartPanel: TframeUartPanel
  Left = 0
  Top = 0
  Width = 640
  Height = 34
  TabOrder = 0
  object lblPort: TLabel
    Left = 8
    Top = 11
    Width = 25
    Height = 13
    Caption = 'UART'
  end
  object lblBaud: TLabel
    Left = 312
    Top = 11
    Width = 21
    Height = 13
    Caption = 'Baud'
  end
  object lblState: TLabel
    Left = 556
    Top = 11
    Width = 52
    Height = 13
    Caption = 'Port closed'
  end
  object cbPort: TComboBox
    Left = 40
    Top = 7
    Width = 170
    Height = 21
    ItemHeight = 13
    TabOrder = 0
    OnDropDown = cbPortDropDown
  end
  object btnRefresh: TButton
    Left = 216
    Top = 6
    Width = 62
    Height = 22
    Caption = 'Refresh'
    TabOrder = 1
    OnClick = btnRefreshClick
  end
  object cbBaud: TComboBox
    Left = 340
    Top = 7
    Width = 68
    Height = 21
    ItemHeight = 13
    TabOrder = 2
  end
  object btnOpen: TButton
    Left = 418
    Top = 6
    Width = 62
    Height = 22
    Caption = 'Open'
    TabOrder = 3
    OnClick = btnOpenClick
  end
  object btnClose: TButton
    Left = 484
    Top = 6
    Width = 62
    Height = 22
    Caption = 'Close'
    TabOrder = 4
    OnClick = btnCloseClick
  end
  object tmrRead: TTimer
    Enabled = False
    Interval = 100
    OnTimer = tmrReadTimer
    Left = 600
    Top = 2
  end
end
