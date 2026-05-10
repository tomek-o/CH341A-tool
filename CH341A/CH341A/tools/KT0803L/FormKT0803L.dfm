object frmKT0803L: TfrmKT0803L
  Left = 0
  Top = 0
  Caption = 'KT0803L transmitter'
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
    Top = 144
    Width = 53
    Height = 13
  end
  object lblFrequency: TLabel
    Left = 8
    Top = 8
    Width = 51
    Height = 13
    Caption = 'Frequency'
  end
  object lblPreemphasis: TLabel
    Left = 8
    Top = 32
    Width = 60
    Height = 13
    Caption = 'Preemphasis'
  end
  object lblMhz: TLabel
    Left = 165
    Top = 8
    Width = 20
    Height = 13
    Caption = 'MHz'
  end
  object btnInit: TButton
    Left = 8
    Top = 105
    Width = 75
    Height = 25
    Caption = 'Init'
    TabOrder = 3
    OnClick = btnInitClick
  end
  object cbPreemphasis: TComboBox
    Left = 99
    Top = 29
    Width = 181
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 1
    TabOrder = 1
    Text = '50 us (Europe, Australia, Japan)'
    Items.Strings = (
      '75 us (USA)'
      '50 us (Europe, Australia, Japan)')
  end
  object edFrequency: TEdit
    Left = 99
    Top = 5
    Width = 56
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    Text = '96.0'
  end
  object chbHighPilotAmplitude: TCheckBox
    Left = 8
    Top = 56
    Width = 147
    Height = 17
    Caption = 'High pilot signal amplitude'
    TabOrder = 2
  end
end
