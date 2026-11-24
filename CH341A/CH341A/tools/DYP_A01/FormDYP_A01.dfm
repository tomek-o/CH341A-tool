object frmDYP_A01: TfrmDYP_A01
  Left = 0
  Top = 0
  Caption = 'DYP-A01 sonar'
  ClientHeight = 393
  ClientWidth = 752
  Color = clBtnFace
  Constraints.MinHeight = 250
  Constraints.MinWidth = 640
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object lblDescription: TLabel
    AlignWithMargins = True
    Left = 8
    Top = 0
    Width = 736
    Height = 17
    Margins.Left = 8
    Margins.Top = 2
    Margins.Right = 8
    Margins.Bottom = 0
    Align = alTop
    AutoSize = False
    Caption = 'DYP-A01: ultrasonic distance sensor (UART).'
    WordWrap = True
  end
  object pnlTop: TPanel
    Left = 0
    Top = 19
    Width = 752
    Height = 34
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    object btnScreenshot: TButton
      AlignWithMargins = True
      Left = 668
      Top = 6
      Width = 80
      Height = 22
      Margins.Top = 6
      Margins.Right = 4
      Margins.Bottom = 6
      Align = alRight
      Caption = 'Screenshot'
      TabOrder = 0
      OnClick = btnScreenshotClick
    end
    object pnlUart: TPanel
      Left = 0
      Top = 0
      Width = 664
      Height = 34
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 1
    end
  end
  object paintBox: TPaintBox
    AlignWithMargins = True
    Left = 5
    Top = 39
    Width = 742
    Height = 307
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    Align = alClient
    OnPaint = paintBoxPaint
  end
  object pnlBottom: TPanel
    Left = 0
    Top = 351
    Width = 752
    Height = 42
    Align = alBottom
    TabOrder = 1
    object lblMin: TLabel
      Left = 16
      Top = 13
      Width = 121
      Height = 22
      Caption = 'min: ------'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'Courier New'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lblMax: TLabel
      Left = 154
      Top = 13
      Width = 121
      Height = 22
      Caption = 'max: ------'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'Courier New'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lblAvg: TLabel
      Left = 299
      Top = 13
      Width = 121
      Height = 22
      Caption = 'avg: ------'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'Courier New'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lblLast: TLabel
      Left = 443
      Top = 13
      Width = 132
      Height = 22
      Caption = 'last: ------'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'Courier New'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object chkCsv: TCheckBox
      Left = 600
      Top = 6
      Width = 140
      Height = 30
      Anchors = [akTop, akRight]
      Caption = 'Save values to a csv file'
      Checked = True
      State = cbChecked
      TabOrder = 0
      WordWrap = True
    end
  end
  object tmrRefresh: TTimer
    Interval = 500
    OnTimer = tmrRefreshTimer
    Left = 616
    Top = 328
  end
end
