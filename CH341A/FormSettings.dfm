object frmSettings: TfrmSettings
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Settings'
  ClientHeight = 251
  ClientWidth = 357
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnKeyPress = FormKeyPress
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object pnlBottom: TPanel
    Left = 0
    Top = 214
    Width = 357
    Height = 37
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object btnCancel: TButton
      Left = 274
      Top = 6
      Width = 75
      Height = 25
      Caption = 'Cancel'
      TabOrder = 1
      OnClick = btnCancelClick
    end
    object btnApply: TButton
      Left = 193
      Top = 6
      Width = 75
      Height = 25
      Caption = 'Apply'
      TabOrder = 0
      OnClick = btnApplyClick
    end
  end
  object pages: TPageControl
    Left = 0
    Top = 0
    Width = 357
    Height = 214
    ActivePage = tsGeneral
    Align = alClient
    TabOrder = 0
    object tsCH341A: TTabSheet
      Caption = 'CH341A'
      ImageIndex = 2
      object lblI2CSpeed: TLabel
        Left = 3
        Top = 30
        Width = 49
        Height = 13
        Caption = 'I2C speed'
      end
      object cbI2CSpeed: TComboBox
        Left = 72
        Top = 27
        Width = 145
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 0
      end
      object chbCH341AOpenAtStartup: TCheckBox
        Left = 3
        Top = 3
        Width = 325
        Height = 17
        Caption = 'Open at startup'
        TabOrder = 1
      end
    end
    object tsGeneral: TTabSheet
      Caption = 'General'
      object chbAlwaysOnTop: TCheckBox
        Left = 3
        Top = 3
        Width = 325
        Height = 17
        Caption = 'Window always on top'
        TabOrder = 0
        OnClick = chbAlwaysOnTopClick
      end
      object chbSortToolsAlphabetically: TCheckBox
        Left = 3
        Top = 26
        Width = 325
        Height = 17
        Caption = 'Sort tool alphabetically on the tree (applied after restart)'
        TabOrder = 1
        OnClick = chbAlwaysOnTopClick
      end
    end
    object tsLogging: TTabSheet
      Caption = 'Logging'
      ImageIndex = 1
      object lblUiCapacity: TLabel
        Left = 5
        Top = 26
        Width = 194
        Height = 13
        Caption = 'Visible (buffered) log lines in log window:'
      end
      object cmbMaxUiLogLines: TComboBox
        Left = 216
        Top = 23
        Width = 111
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 0
        OnChange = cmbMaxUiLogLinesChange
        Items.Strings = (
          '100'
          '200'
          '500'
          '1000'
          '2000'
          '5000'
          '10000')
      end
      object chbLogToFile: TCheckBox
        Left = 5
        Top = 3
        Width = 325
        Height = 17
        Caption = 'Log to file'
        TabOrder = 1
        OnClick = chbAlwaysOnTopClick
      end
      object cbhLogAddTimestamps: TCheckBox
        Left = 5
        Top = 48
        Width = 325
        Height = 17
        Caption = 'Add timestamps'
        TabOrder = 2
        OnClick = chbAlwaysOnTopClick
      end
    end
  end
end
