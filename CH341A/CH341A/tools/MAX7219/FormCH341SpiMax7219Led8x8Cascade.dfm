object frmCH341SpiMax7219Led8x8Cascade: TfrmCH341SpiMax7219Led8x8Cascade
  Left = 0
  Top = 0
  Caption = 'MAX7219 SPI 8x8 LED cascade'
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
  object shapeBackground1: TShape
    Left = 8
    Top = 102
    Width = 104
    Height = 104
    Brush.Color = clBlack
  end
  object lblStatus: TLabel
    Left = 8
    Top = 25
    Width = 3
    Height = 13
  end
  object lblNote: TLabel
    Left = 8
    Top = 5
    Width = 380
    Height = 13
    Caption = 
      'Cascaded 4x8x8 LED matrix display controlled by MAX7219 connecte' +
      'd over SPI'
  end
  object lblIntensity: TLabel
    Left = 8
    Top = 249
    Width = 43
    Height = 13
    Caption = 'Intensity'
  end
  object lblClickInfo: TLabel
    Left = 8
    Top = 83
    Width = 152
    Height = 13
    Caption = 'Click below to change LED state'
  end
  object shapeBackground2: TShape
    Left = 114
    Top = 102
    Width = 104
    Height = 104
    Brush.Color = clBlack
  end
  object shapeBackground3: TShape
    Left = 220
    Top = 102
    Width = 104
    Height = 104
    Brush.Color = clBlack
  end
  object shapeBackground4: TShape
    Left = 326
    Top = 102
    Width = 104
    Height = 104
    Brush.Color = clBlack
  end
  object lblCascadeCount: TLabel
    Left = 8
    Top = 57
    Width = 92
    Height = 13
    Caption = 'Number of modules'
  end
  object btnInit: TButton
    Left = 151
    Top = 52
    Width = 89
    Height = 25
    Caption = 'Init + write'
    TabOrder = 0
    OnClick = btnInitClick
  end
  object trbarIntensity: TTrackBar
    Left = 57
    Top = 237
    Width = 353
    Height = 36
    Max = 15
    Position = 5
    TabOrder = 1
    TickMarks = tmBoth
    OnChange = trbarIntensityChange
  end
  object btnWrite: TButton
    Left = 246
    Top = 52
    Width = 89
    Height = 25
    Caption = 'Write'
    TabOrder = 2
    OnClick = btnWriteClick
  end
  object btnClear: TButton
    Left = 341
    Top = 52
    Width = 89
    Height = 25
    Caption = 'Clear'
    TabOrder = 3
    OnClick = btnClearClick
  end
  object cbCascadeCount: TComboBox
    Left = 105
    Top = 54
    Width = 39
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 3
    TabOrder = 4
    Text = '4'
    OnChange = cbCascadeCountChange
    Items.Strings = (
      '1'
      '2'
      '3'
      '4')
  end
end
