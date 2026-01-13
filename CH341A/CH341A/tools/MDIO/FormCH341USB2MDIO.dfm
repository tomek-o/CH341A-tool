object frmCH341USB2MDIO: TfrmCH341USB2MDIO
  Left = 0
  Top = 0
  Caption = 'TI USB-2-MDIO'
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
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 635
    Height = 77
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    object lblInfo: TLabel
      Left = 8
      Top = 7
      Width = 468
      Height = 13
      Caption = 
        'Run Texas Instruments USB-2-MDIO script. Sets SCL pin as MDC (cl' +
        'ock), SDA pin as MDIO (data).'
    end
    object lblPhyAddress: TLabel
      Left = 128
      Top = 31
      Width = 61
      Height = 13
      Caption = 'PHY Address'
    end
    object lblStatus: TLabel
      Left = 8
      Top = 58
      Width = 69
      Height = 13
    end
    object btnLoadScript: TButton
      Left = 8
      Top = 26
      Width = 75
      Height = 25
      Caption = 'Load script'
      TabOrder = 0
      OnClick = btnLoadScriptClick
    end
    object cbPhyAddress: TComboBox
      Left = 197
      Top = 28
      Width = 61
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 1
    end
    object btnRunScript: TButton
      Left = 264
      Top = 26
      Width = 75
      Height = 25
      Caption = 'Run script'
      TabOrder = 2
      OnClick = btnRunScriptClick
    end
  end
  object lvScript: TListView
    Left = 0
    Top = 77
    Width = 635
    Height = 222
    Align = alClient
    Columns = <
      item
        Caption = 'Line'
        Width = 40
      end
      item
        AutoSize = True
        Caption = 'Script text'
      end
      item
        Caption = 'Result'
        Width = 190
      end>
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    HideSelection = False
    OwnerData = True
    ReadOnly = True
    RowSelect = True
    ParentFont = False
    TabOrder = 1
    ViewStyle = vsReport
    OnData = lvScriptData
  end
  object openDialog: TOpenDialog
    DefaultExt = '*.txt'
    Filter = 'Text files|*.txt|All files|*'
    Left = 88
    Top = 24
  end
end
