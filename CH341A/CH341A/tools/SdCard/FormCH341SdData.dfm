object frmCH341SdData: TfrmCH341SdData
  Left = 0
  Top = 0
  Caption = 'SD / MMC card: sector data'
  ClientHeight = 504
  ClientWidth = 640
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object memoConnection: TMemo
    Left = 8
    Top = 8
    Width = 624
    Height = 58
    Anchors = [akLeft, akTop, akRight]
    Color = clBtnFace
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object btnInit: TButton
    Left = 8
    Top = 74
    Width = 80
    Height = 25
    Caption = 'Init card'
    TabOrder = 1
    OnClick = btnInitClick
  end
  object lblSector: TLabel
    Left = 104
    Top = 80
    Width = 30
    Height = 13
    Caption = 'Sector'
  end
  object edSector: TEdit
    Left = 144
    Top = 76
    Width = 80
    Height = 21
    TabOrder = 2
    Text = '0'
  end
  object lblCount: TLabel
    Left = 236
    Top = 80
    Width = 30
    Height = 13
    Caption = 'Count'
  end
  object edCount: TEdit
    Left = 272
    Top = 76
    Width = 50
    Height = 21
    TabOrder = 3
    Text = '1'
  end
  object btnReadSectors: TButton
    Left = 336
    Top = 74
    Width = 90
    Height = 25
    Caption = 'Read sectors'
    TabOrder = 4
    OnClick = btnReadSectorsClick
  end
  object btnWriteSectors: TButton
    Left = 432
    Top = 74
    Width = 90
    Height = 25
    Caption = 'Write sectors'
    TabOrder = 5
    OnClick = btnWriteSectorsClick
  end
  object chkAllowWrite: TCheckBox
    Left = 534
    Top = 78
    Width = 98
    Height = 17
    Caption = 'Allow writing'
    TabOrder = 6
  end
  object pnlHex: TPanel
    Left = 8
    Top = 106
    Width = 624
    Height = 300
    Anchors = [akLeft, akTop, akRight, akBottom]
    BevelOuter = bvNone
    Caption = ''
    TabOrder = 7
  end
  object progressBar: TProgressBar
    Left = 8
    Top = 414
    Width = 540
    Height = 14
    Anchors = [akLeft, akRight, akBottom]
    TabOrder = 8
  end
  object btnCancel: TButton
    Left = 556
    Top = 410
    Width = 76
    Height = 22
    Caption = 'Cancel'
    Anchors = [akRight, akBottom]
    Enabled = False
    TabOrder = 9
    OnClick = btnCancelClick
  end
  object memoLog: TMemo
    Left = 8
    Top = 436
    Width = 624
    Height = 60
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssVertical
    Anchors = [akLeft, akRight, akBottom]
    TabOrder = 10
  end
end
