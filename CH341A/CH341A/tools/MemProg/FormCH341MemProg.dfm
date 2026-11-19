object frmCH341MemProg: TfrmCH341MemProg
  Left = 0
  Top = 0
  Caption = 'EEPROM / FLASH programmer'
  ClientHeight = 480
  ClientWidth = 560
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object pcDevice: TPageControl
    Left = 8
    Top = 8
    Width = 544
    Height = 104
    ActivePage = tsAt45
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
    OnChange = pcDeviceChange
    object tsI2cEeprom: TTabSheet
      Caption = 'I2C EEPROM'
      object lblI2cType: TLabel
        Left = 3
        Top = 11
        Width = 21
        Height = 13
        Caption = 'Chip'
      end
      object lblI2cAddr: TLabel
        Left = 250
        Top = 12
        Width = 39
        Height = 13
        Caption = 'Address'
      end
      object cbI2cType: TComboBox
        Left = 35
        Top = 8
        Width = 200
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 0
      end
      object cbI2cAddr: TComboBox
        Left = 298
        Top = 8
        Width = 60
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 1
      end
      object btnProbe: TButton
        Left = 376
        Top = 6
        Width = 80
        Height = 25
        Caption = 'Detect'
        TabOrder = 2
        OnClick = btnProbeClick
      end
    end
    object tsSpiFlash: TTabSheet
      Caption = 'SPI flash'
      object lblSpiSize: TLabel
        Left = 6
        Top = 12
        Width = 19
        Height = 13
        Caption = 'Size'
      end
      object lblSpiPage: TLabel
        Left = 235
        Top = 12
        Width = 24
        Height = 13
        Caption = 'Page'
      end
      object lblEraseMode: TLabel
        Left = 6
        Top = 46
        Width = 59
        Height = 13
        Caption = 'Before write'
      end
      object lblProgMode: TLabel
        Left = 290
        Top = 46
        Width = 40
        Height = 13
        Caption = 'Program'
      end
      object cbSpiSize: TComboBox
        Left = 35
        Top = 8
        Width = 190
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 0
      end
      object cbSpiPage: TComboBox
        Left = 270
        Top = 8
        Width = 55
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 1
      end
      object btnReadId: TButton
        Left = 340
        Top = 6
        Width = 80
        Height = 25
        Caption = 'Detect'
        Hint = 'Reads the JEDEC ID and the SFDP table'#13#10'and selects the chip size and page size'
        ShowHint = True
        TabOrder = 2
        OnClick = btnReadIdClick
      end
      object btnUnprotect: TButton
        Left = 426
        Top = 6
        Width = 80
        Height = 25
        Caption = 'Unprotect'
        TabOrder = 3
        OnClick = btnUnprotectClick
      end
      object cbEraseMode: TComboBox
        Left = 76
        Top = 42
        Width = 200
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 4
      end
      object cbProgMode: TComboBox
        Left = 335
        Top = 42
        Width = 150
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 5
      end
    end
    object tsSpiEeprom: TTabSheet
      Caption = 'SPI EEPROM'
      object lblSpiEeType: TLabel
        Left = 3
        Top = 12
        Width = 21
        Height = 13
        Caption = 'Chip'
      end
      object lblSpiEePage: TLabel
        Left = 235
        Top = 12
        Width = 24
        Height = 13
        Caption = 'Page'
      end
      object cbSpiEeType: TComboBox
        Left = 35
        Top = 8
        Width = 190
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 0
        OnChange = cbSpiEeTypeChange
      end
      object cbSpiEePage: TComboBox
        Left = 270
        Top = 8
        Width = 55
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 1
      end
      object btnSpiEeUnprotect: TButton
        Left = 340
        Top = 6
        Width = 80
        Height = 25
        Caption = 'Unprotect'
        TabOrder = 2
        OnClick = btnUnprotectClick
      end
    end
    object tsMicrowire: TTabSheet
      Caption = 'Microwire'
      object lblMwType: TLabel
        Left = 3
        Top = 12
        Width = 21
        Height = 13
        Caption = 'Chip'
      end
      object lblMwOrg: TLabel
        Left = 190
        Top = 12
        Width = 61
        Height = 13
        Caption = 'Organisation'
      end
      object cbMwType: TComboBox
        Left = 35
        Top = 8
        Width = 140
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 0
      end
      object cbMwOrg: TComboBox
        Left = 258
        Top = 8
        Width = 190
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 1
      end
    end
    object tsAt45: TTabSheet
      Caption = 'AT45 DataFlash'
      object lblAt45Info: TLabel
        Left = 188
        Top = 8
        Width = 149
        Height = 13
      end
      object btnAt45Detect: TButton
        Left = 6
        Top = 3
        Width = 80
        Height = 25
        Caption = 'Detect'
        TabOrder = 0
        OnClick = btnAt45DetectClick
      end
      object btnAt45Unprotect: TButton
        Left = 92
        Top = 3
        Width = 80
        Height = 25
        Caption = 'Unprotect'
        TabOrder = 1
        OnClick = btnAt45UnprotectClick
      end
    end
  end
  object chkWriteEdited: TCheckBox
    Left = 325
    Top = 126
    Width = 108
    Height = 17
    Hint = 'Write only what was edited in the editor since the chip was read'
    Caption = 'Write only edited'
    Anchors = [akTop, akRight]
    ParentShowHint = False
    ShowHint = True
    TabOrder = 18
  end
  object chkVerifyAfterWrite: TCheckBox
    Left = 437
    Top = 126
    Width = 55
    Height = 17
    Caption = 'Verify'
    Checked = True
    State = cbChecked
    Anchors = [akTop, akRight]
    TabOrder = 1
  end
  object btnRead: TButton
    Left = 17
    Top = 122
    Width = 72
    Height = 25
    Caption = 'Read chip'
    TabOrder = 2
    OnClick = btnReadClick
  end
  object btnWrite: TButton
    Left = 93
    Top = 122
    Width = 72
    Height = 25
    Caption = 'Write chip'
    TabOrder = 4
    OnClick = btnWriteClick
  end
  object btnVerify: TButton
    Left = 169
    Top = 122
    Width = 72
    Height = 25
    Caption = 'Verify chip'
    TabOrder = 6
    OnClick = btnVerifyClick
  end
  object btnErase: TButton
    Left = 245
    Top = 122
    Width = 72
    Height = 25
    Caption = 'Erase chip'
    TabOrder = 8
    OnClick = btnEraseClick
  end
  object btnCancel: TButton
    Left = 495
    Top = 122
    Width = 48
    Height = 25
    Caption = 'Cancel'
    Enabled = False
    Anchors = [akTop, akRight]
    TabOrder = 3
    OnClick = btnCancelClick
  end
  object pnlHex: TPanel
    Left = 8
    Top = 154
    Width = 544
    Height = 246
    Anchors = [akLeft, akTop, akRight, akBottom]
    BevelOuter = bvNone
    Caption = ''
    TabOrder = 17
  end
  object progressBar: TProgressBar
    Left = 8
    Top = 406
    Width = 544
    Height = 16
    Anchors = [akLeft, akRight, akBottom]
    TabOrder = 5
  end
  object memoLog: TMemo
    Left = 8
    Top = 426
    Width = 544
    Height = 46
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssVertical
    Anchors = [akLeft, akRight, akBottom]
    TabOrder = 7
  end
end
