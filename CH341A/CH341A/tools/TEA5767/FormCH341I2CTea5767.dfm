object frmCH341I2CTea5767: TfrmCH341I2CTea5767
  Left = 0
  Top = 0
  Caption = 'TEA5767 FM tuner'
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
    Top = 8
    Width = 3
    Height = 13
  end
  object lblTeaStatus: TLabel
    Left = 8
    Top = 64
    Width = 144
    Height = 17
    Caption = 'Status not known'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Courier New'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblTeaStatus2: TLabel
    Left = 79
    Top = 87
    Width = 9
    Height = 17
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Courier New'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblManualTuning: TLabel
    Left = 8
    Top = 131
    Width = 24
    Height = 13
    Caption = 'Tune'
  end
  object btnInit: TButton
    Left = 8
    Top = 27
    Width = 113
    Height = 25
    Caption = 'Init + start handling'
    TabOrder = 0
    OnClick = btnInitClick
  end
  object btnSearchDown: TButton
    Left = 8
    Top = 227
    Width = 113
    Height = 25
    Caption = 'Search down'
    TabOrder = 1
    Visible = False
    OnClick = btnSearchDownClick
  end
  object btnSearchUp: TButton
    Left = 138
    Top = 227
    Width = 113
    Height = 25
    Caption = 'Search up'
    TabOrder = 2
    Visible = False
    OnClick = btnSearchUpClick
  end
  object btnTuneM50: TButton
    Tag = -50
    Left = 8
    Top = 196
    Width = 50
    Height = 25
    Caption = '- 50 kHz'
    TabOrder = 3
    OnClick = btnTuneByClick
  end
  object btnTuneM10: TButton
    Tag = -10
    Left = 71
    Top = 196
    Width = 50
    Height = 25
    Caption = '- 10 kHz'
    TabOrder = 4
    OnClick = btnTuneByClick
  end
  object btnTuneP10: TButton
    Tag = 10
    Left = 138
    Top = 195
    Width = 50
    Height = 25
    Caption = '+ 10 kHz'
    TabOrder = 5
    OnClick = btnTuneByClick
  end
  object btnTuneP50: TButton
    Tag = 50
    Left = 201
    Top = 195
    Width = 50
    Height = 25
    Caption = '+ 50 kHz'
    TabOrder = 6
    OnClick = btnTuneByClick
  end
  object btnStopHandling: TButton
    Left = 138
    Top = 27
    Width = 113
    Height = 25
    Caption = 'Stop handling'
    TabOrder = 7
    OnClick = btnStopHandlingClick
  end
  object trbar: TTrackBar
    Left = 2
    Top = 150
    Width = 256
    Height = 45
    Max = 10800
    Min = 8750
    ParentShowHint = False
    Frequency = 100
    Position = 9990
    ShowHint = True
    TabOrder = 8
    TickMarks = tmBoth
    OnChange = trbarChange
  end
  object tmrAutoRead: TTimer
    Enabled = False
    OnTimer = tmrAutoReadTimer
    Left = 384
    Top = 50
  end
end
