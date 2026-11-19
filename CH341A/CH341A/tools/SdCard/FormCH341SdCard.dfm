object frmCH341SdCard: TfrmCH341SdCard
  Left = 0
  Top = 0
  Caption = 'SD / MMC card: commands'
  ClientHeight = 474
  ClientWidth = 640
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    640
    474)
  PixelsPerInch = 96
  TextHeight = 13
  object lblPresets: TLabel
    Left = 8
    Top = 108
    Width = 157
    Height = 13
    Caption = 'Commands (see hints for details)'
  end
  object lblCmd: TLabel
    Left = 8
    Top = 164
    Width = 22
    Height = 13
    Caption = 'CMD'
  end
  object lblArg: TLabel
    Left = 230
    Top = 164
    Width = 46
    Height = 13
    Caption = 'Arg (hex)'
  end
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
    Width = 90
    Height = 25
    Caption = 'Init card'
    TabOrder = 1
    OnClick = btnInitClick
  end
  object btnInfo: TButton
    Left = 104
    Top = 74
    Width = 150
    Height = 25
    Caption = 'Card info (CID/CSD/OCR)'
    TabOrder = 2
    OnClick = btnInfoClick
  end
  object btnStatus: TButton
    Left = 260
    Top = 74
    Width = 100
    Height = 25
    Caption = 'Status (CMD13)'
    TabOrder = 3
    OnClick = btnStatusClick
  end
  object btnCancel: TButton
    Left = 556
    Top = 74
    Width = 76
    Height = 25
    Anchors = [akTop, akRight]
    Caption = 'Cancel'
    Enabled = False
    TabOrder = 4
    OnClick = btnCancelClick
  end
  object btnPreset0: TButton
    Left = 8
    Top = 124
    Width = 72
    Height = 25
    Hint = 
      'CMD0 - GO_IDLE_STATE'#13#10'Resets the card. Sent with CS low it selec' +
      'ts SPI mode.'#13#10'Expected response: R1 = 01 (idle).'
    Caption = 'CMD0'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 5
    OnClick = btnPresetClick
  end
  object btnPreset1: TButton
    Tag = 1
    Left = 84
    Top = 124
    Width = 72
    Height = 25
    Hint = 
      'CMD8 - SEND_IF_COND, argument 000001AA'#13#10'Checks the card supports' +
      ' 2.7-3.6 V (the 1AA pattern is echoed).'#13#10'SD v2 / SDHC card: R7 =' +
      ' 00 00 01 AA after R1 = 01.'#13#10'SD v1 or MMC card: R1 = 05 (illegal' +
      ' command).'
    Caption = 'CMD8'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 6
    OnClick = btnPresetClick
  end
  object btnPreset2: TButton
    Tag = 2
    Left = 160
    Top = 124
    Width = 72
    Height = 25
    Hint = 
      'CMD9 - SEND_CSD'#13#10'Reads the 16-byte Card-Specific Data: capacity,' +
      ' block length, speed.'#13#10'This button reads and decodes both CID an' +
      'd CSD.'#13#10'Card must be initialized first.'
    Caption = 'CMD9 CSD'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 7
    OnClick = btnPresetClick
  end
  object btnPreset3: TButton
    Tag = 3
    Left = 236
    Top = 124
    Width = 72
    Height = 25
    Hint = 
      'CMD10 - SEND_CID'#13#10'Reads the 16-byte Card IDentification: manufac' +
      'turer, product name,'#13#10'revision, serial number, manufacturing dat' +
      'e.'#13#10'This button reads and decodes both CID and CSD.'#13#10'Card must b' +
      'e initialized first.'
    Caption = 'CMD10 CID'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 8
    OnClick = btnPresetClick
  end
  object btnPreset4: TButton
    Tag = 4
    Left = 312
    Top = 124
    Width = 72
    Height = 25
    Hint = 
      'CMD13 - SEND_STATUS'#13#10'Returns R2 = R1 + one status byte.'#13#10'Second ' +
      'byte flags: card locked, write protect violation, error, out of ' +
      'range ...'
    Caption = 'CMD13'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 9
    OnClick = btnPresetClick
  end
  object btnPreset5: TButton
    Tag = 5
    Left = 388
    Top = 124
    Width = 72
    Height = 25
    Hint = 
      'CMD16 - SET_BLOCKLEN, argument 00000200'#13#10'Sets the block length t' +
      'o 512 bytes. Needed by SDSC cards only,'#13#10'ignored (block size is ' +
      'fixed) by SDHC / SDXC.'
    Caption = 'CMD16'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 10
    OnClick = btnPresetClick
  end
  object btnPreset6: TButton
    Tag = 6
    Left = 464
    Top = 124
    Width = 72
    Height = 25
    Hint = 
      'CMD58 - READ_OCR'#13#10'Returns R3 = R1 + 4 bytes of the OCR register.' +
      #13#10'Bit 31: power-up finished, bit 30 (CCS): 1 = SDHC / SDXC (sect' +
      'or addressing),'#13#10'0 = SDSC (byte addressing), bits 23-15: support' +
      'ed voltages.'
    Caption = 'CMD58'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 11
    OnClick = btnPresetClick
  end
  object btnPreset7: TButton
    Tag = 7
    Left = 540
    Top = 124
    Width = 72
    Height = 25
    Hint = 
      'ACMD41 - SD_SEND_OP_COND (CMD55 first), argument 40000000'#13#10'Start' +
      's card initialization; HCS bit (30) tells the card that SDHC is ' +
      'supported.'#13#10'R1 = 01 while initializing, 00 when ready - repeat u' +
      'ntil 00.'#13#10'Init card does this automatically.'
    Caption = 'ACMD41'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 12
    OnClick = btnPresetClick
  end
  object edCmd: TEdit
    Left = 40
    Top = 160
    Width = 40
    Height = 21
    TabOrder = 13
    Text = '0'
  end
  object chkApp: TCheckBox
    Left = 90
    Top = 162
    Width = 130
    Height = 17
    Caption = 'ACMD (CMD55 first)'
    TabOrder = 14
  end
  object edArg: TEdit
    Left = 286
    Top = 160
    Width = 80
    Height = 21
    TabOrder = 15
    Text = '00000000'
  end
  object cbResponse: TComboBox
    Left = 376
    Top = 160
    Width = 130
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 16
  end
  object btnSend: TButton
    Left = 516
    Top = 158
    Width = 70
    Height = 25
    Caption = 'Send'
    TabOrder = 17
    OnClick = btnSendClick
  end
  object memoInfo: TMemo
    Left = 8
    Top = 190
    Width = 624
    Height = 190
    Anchors = [akLeft, akTop, akRight, akBottom]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssBoth
    TabOrder = 18
  end
  object progressBar: TProgressBar
    Left = 8
    Top = 386
    Width = 624
    Height = 14
    Anchors = [akLeft, akRight, akBottom]
    TabOrder = 19
  end
  object memoLog: TMemo
    Left = 8
    Top = 406
    Width = 624
    Height = 60
    Anchors = [akLeft, akRight, akBottom]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 20
  end
  object btnClearLog: TButton
    Left = 416
    Top = 74
    Width = 90
    Height = 25
    Caption = 'Clear log'
    TabOrder = 21
    OnClick = btnClearLogClick
  end
end
