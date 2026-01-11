object frmCH341I2CSi5351: TfrmCH341I2CSi5351
  Left = 0
  Top = 0
  Caption = 'Si5351A clock generator'
  ClientHeight = 275
  ClientWidth = 488
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    488
    275)
  PixelsPerInch = 96
  TextHeight = 13
  object lblStatus: TLabel
    Left = 8
    Top = 232
    Width = 3
    Height = 13
  end
  object lblFrequencyCorrection: TLabel
    Left = 8
    Top = 157
    Width = 233
    Height = 13
    Caption = 'Frequency error to correct, in relation to 10 MHz'
  end
  object Label2: TLabel
    Left = 318
    Top = 157
    Width = 12
    Height = 13
    Caption = 'Hz'
  end
  object grboxCLK0: TGroupBox
    Left = 8
    Top = 1
    Width = 472
    Height = 73
    Anchors = [akLeft, akTop, akRight]
    Caption = 'CLK0 output, PLL A'
    TabOrder = 0
    object lblFrequencyA: TLabel
      Left = 10
      Top = 22
      Width = 51
      Height = 13
      Caption = 'Frequency'
    end
    object lblHzA: TLabel
      Left = 208
      Top = 22
      Width = 12
      Height = 13
      Caption = 'Hz'
    end
    object lblDriveStrength0: TLabel
      Left = 10
      Top = 51
      Width = 69
      Height = 13
      Caption = 'Drive strength'
    end
    object edFrequencyA: TEdit
      Left = 104
      Top = 19
      Width = 97
      Height = 22
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      Text = '10000'
    end
    object cbDriveStrength0: TComboBox
      Left = 104
      Top = 48
      Width = 97
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      ItemIndex = 0
      TabOrder = 1
      Text = '2 mA'
      Items.Strings = (
        '2 mA'
        '4 mA'
        '6 mA'
        '8 mA')
    end
  end
  object grboxCLK2: TGroupBox
    Left = 8
    Top = 78
    Width = 472
    Height = 73
    Anchors = [akLeft, akTop, akRight]
    Caption = 'CLK2 output, PLL B'
    TabOrder = 1
    ExplicitWidth = 619
    object lblFrequencyB: TLabel
      Left = 10
      Top = 22
      Width = 51
      Height = 13
      Caption = 'Frequency'
    end
    object lblDriveStrength2: TLabel
      Left = 10
      Top = 51
      Width = 69
      Height = 13
      Caption = 'Drive strength'
    end
    object lblHzB: TLabel
      Left = 208
      Top = 22
      Width = 12
      Height = 13
      Caption = 'Hz'
    end
    object cbDriveStrength2: TComboBox
      Left = 104
      Top = 48
      Width = 97
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      ItemIndex = 0
      TabOrder = 0
      Text = '2 mA'
      Items.Strings = (
        '2 mA'
        '4 mA'
        '6 mA'
        '8 mA')
    end
    object edFrequencyB: TEdit
      Left = 104
      Top = 19
      Width = 97
      Height = 22
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      TabOrder = 1
      Text = '20000'
    end
  end
  object btnWrite: TButton
    Left = 8
    Top = 189
    Width = 113
    Height = 25
    Caption = 'Write to Si5351'
    TabOrder = 2
    OnClick = btnWriteClick
  end
  object edCorrection: TEdit
    Left = 256
    Top = 154
    Width = 57
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 3
    Text = '0'
  end
end
