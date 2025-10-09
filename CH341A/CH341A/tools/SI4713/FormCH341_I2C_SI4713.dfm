object frmCH341I2CSi4713: TfrmCH341I2CSi4713
  Left = 0
  Top = 0
  Caption = 'Si4713 FM transmitter'
  ClientHeight = 357
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
    Left = 110
    Top = 203
    Width = 21
    Height = 13
  end
  object lblAddress: TLabel
    Left = 8
    Top = 32
    Width = 280
    Height = 13
    Caption = 'I2C address, depending on /SEN pin state, default = 0x63'
  end
  object lblRadioStatus: TLabel
    Left = 8
    Top = 259
    Width = 8
    Height = 16
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
  end
  object lblFrequency: TLabel
    Left = 8
    Top = 56
    Width = 51
    Height = 13
    Caption = 'Frequency'
  end
  object lblMhz: TLabel
    Left = 165
    Top = 56
    Width = 20
    Height = 13
    Caption = 'MHz'
  end
  object lblInfo: TLabel
    Left = 8
    Top = 8
    Width = 427
    Height = 13
    Caption = 
      'Requires I2C + CH341 TXD connected to Si4713 RST, device is not ' +
      'detected without RST'
  end
  object lblRdsStation: TLabel
    Left = 8
    Top = 127
    Width = 79
    Height = 13
    Caption = 'RDS station text'
  end
  object lblRdsBuffer: TLabel
    Left = 8
    Top = 151
    Width = 76
    Height = 13
    Caption = 'RDS buffer text'
  end
  object lblRdsProgramId: TLabel
    Left = 8
    Top = 103
    Width = 337
    Height = 13
    Caption = 
      'RDS program ID                                      (16 bit hex ' +
      'unique in the area)'
  end
  object lblPreemphasis: TLabel
    Left = 8
    Top = 80
    Width = 60
    Height = 13
    Caption = 'Preemphasis'
  end
  object btnRead: TButton
    Left = 8
    Top = 227
    Width = 75
    Height = 25
    Caption = 'Read status'
    TabOrder = 10
    OnClick = btnReadClick
  end
  object chbAutoRead: TCheckBox
    Left = 99
    Top = 231
    Width = 264
    Height = 17
    Caption = 'auto refresh (repeat reading every 500 ms)'
    TabOrder = 11
    OnClick = chbAutoReadClick
    OnMouseDown = chbAutoReadMouseDown
  end
  object cbAddress: TComboBox
    Left = 298
    Top = 29
    Width = 65
    Height = 22
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ItemHeight = 14
    ItemIndex = 1
    ParentFont = False
    TabOrder = 0
    Text = '0x63'
    Items.Strings = (
      '0x11'#10
      '0x63')
  end
  object btnInit: TButton
    Left = 8
    Top = 198
    Width = 75
    Height = 25
    Caption = 'Init + tune'
    TabOrder = 9
    OnClick = btnInitClick
  end
  object edFrequency: TEdit
    Left = 99
    Top = 53
    Width = 56
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    Text = '100.50'
  end
  object edRdsStation: TEdit
    Left = 99
    Top = 124
    Width = 82
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
    Text = 'Si4713'
  end
  object edRdsBuffer: TEdit
    Left = 99
    Top = 148
    Width = 274
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 5
    Text = 'CH341 + Si4713 test'
  end
  object edRdsProgramId: TEdit
    Left = 99
    Top = 100
    Width = 82
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 3
    Text = '0xC341'
  end
  object cbPreemphasis: TComboBox
    Left = 99
    Top = 77
    Width = 181
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 1
    TabOrder = 2
    Text = '50 us (Europe, Australia, Japan)'
    Items.Strings = (
      '75 us (USA)'
      '50 us (Europe, Australia, Japan)'
      'disabled')
  end
  object btnUpdateRdsBufferText: TButton
    Left = 380
    Top = 148
    Width = 50
    Height = 22
    Caption = 'Update'
    TabOrder = 6
    OnClick = btnUpdateRdsBufferTextClick
  end
  object btnGetWinampRds: TButton
    Left = 99
    Top = 173
    Width = 168
    Height = 21
    Caption = 'Update RDS from Winamp 2.x'
    TabOrder = 7
    OnClick = btnGetWinampRdsClick
  end
  object chbRdsUpdateFromWinamp: TCheckBox
    Left = 273
    Top = 175
    Width = 157
    Height = 17
    Caption = 'Update automatically'
    TabOrder = 8
    OnClick = chbRdsUpdateFromWinampClick
    OnMouseDown = chbRdsUpdateFromWinampMouseDown
  end
  object btnScanRnl: TButton
    Left = 8
    Top = 281
    Width = 365
    Height = 25
    Caption = 
      'Scan received noise levels (shown in log, would take ~20 seconds' +
      ')'
    TabOrder = 12
    OnClick = btnScanRnlClick
  end
  object tmrAutoRead: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrAutoReadTimer
    Left = 376
    Top = 229
  end
  object tmrUpdateRds: TTimer
    Enabled = False
    OnTimer = tmrUpdateRdsTimer
    Left = 448
    Top = 168
  end
end
