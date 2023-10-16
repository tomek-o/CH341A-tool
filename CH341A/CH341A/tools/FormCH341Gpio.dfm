object frmCH341Gpio: TfrmCH341Gpio
  Left = 0
  Top = 0
  Caption = 'GPIO'
  ClientHeight = 361
  ClientWidth = 648
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    648
    361)
  PixelsPerInch = 96
  TextHeight = 13
  object lblWarning: TLabel
    Left = 8
    Top = 4
    Width = 300
    Height = 13
    Caption = 'Warning: setting GPIO as output might be dangerous!'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblLegend: TLabel
    Left = 8
    Top = 25
    Width = 372
    Height = 13
    Caption = 
      '      Pin                             Direction = OUT        Out' +
      'put state            Input state'
  end
  object scrollBox: TScrollBox
    Left = 8
    Top = 44
    Width = 412
    Height = 275
    Anchors = [akLeft, akTop, akBottom]
    TabOrder = 0
    object panel: TFlowPanel
      Left = 0
      Top = 0
      Width = 408
      Height = 217
      Align = alTop
      AutoSize = True
      AutoWrap = False
      BevelOuter = bvNone
      FlowStyle = fsTopBottomLeftRight
      TabOrder = 0
    end
  end
  object pnlBottom: TPanel
    Left = 0
    Top = 325
    Width = 648
    Height = 36
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object btnWrite: TButton
      Left = 8
      Top = 5
      Width = 145
      Height = 25
      Caption = 'Write direction + outputs'
      TabOrder = 0
      OnClick = btnWriteClick
    end
    object btnRead: TButton
      Left = 172
      Top = 5
      Width = 145
      Height = 25
      Caption = 'Read inputs'
      TabOrder = 1
      OnClick = btnReadClick
    end
    object chbReadPeriodically: TCheckBox
      Left = 323
      Top = 10
      Width = 97
      Height = 17
      Caption = 'read periodically'
      TabOrder = 2
    end
  end
  object timerRead: TTimer
    OnTimer = timerReadTimer
    Left = 432
    Top = 48
  end
end
