object CH341SpiMax7219Led8x8: TCH341SpiMax7219Led8x8
  Left = 0
  Top = 0
  Caption = 'MAX7219 SPI 8x8 LED'
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
  object shapeBackground: TShape
    Left = 112
    Top = 54
    Width = 160
    Height = 160
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
    Width = 318
    Height = 13
    Caption = '8x8 LED matrix display controlled by MAX7219 connected over SPI'
  end
  object lblIntensity: TLabel
    Left = 16
    Top = 241
    Width = 43
    Height = 13
    Caption = 'Intensity'
  end
  object lblClickInfo: TLabel
    Left = 278
    Top = 59
    Width = 134
    Height = 13
    Caption = '<- click to change LED state'
  end
  object btnInit: TButton
    Left = 8
    Top = 54
    Width = 89
    Height = 25
    Caption = 'Init + write'
    TabOrder = 0
    OnClick = btnInitClick
  end
  object trbarIntensity: TTrackBar
    Left = 65
    Top = 229
    Width = 353
    Height = 45
    Max = 15
    Position = 7
    TabOrder = 1
    TickMarks = tmBoth
    OnChange = trbarIntensityChange
  end
  object btnWrite: TButton
    Left = 8
    Top = 85
    Width = 89
    Height = 25
    Caption = 'Write'
    TabOrder = 2
    OnClick = btnWriteClick
  end
  object btnClear: TButton
    Left = 8
    Top = 116
    Width = 89
    Height = 25
    Caption = 'Clear'
    TabOrder = 3
    OnClick = btnClearClick
  end
end
