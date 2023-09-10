object frmAbout: TfrmAbout
  Left = 200
  Top = 108
  BorderStyle = bsDialog
  Caption = 'About'
  ClientHeight = 213
  ClientWidth = 298
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = True
  Position = poScreenCenter
  OnKeyPress = FormKeyPress
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 8
    Top = 8
    Width = 281
    Height = 161
    BevelInner = bvRaised
    BevelOuter = bvLowered
    ParentColor = True
    TabOrder = 0
    object ProgramIcon: TImage
      Left = 8
      Top = 8
      Width = 65
      Height = 57
      Stretch = True
      Transparent = True
      IsControl = True
    end
    object ProductName: TLabel
      Left = 88
      Top = 8
      Width = 77
      Height = 13
      Caption = 'ProductName'
      FocusControl = OKButton
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      IsControl = True
    end
    object Version: TLabel
      Left = 88
      Top = 27
      Width = 35
      Height = 13
      Caption = 'Version'
      IsControl = True
    end
    object Copyright: TLabel
      Left = 8
      Top = 80
      Width = 172
      Height = 13
      Caption = 'Copyright '#169' Tomasz Ostrowski 2021'
      IsControl = True
    end
    object lblVersion: TLabel
      Left = 146
      Top = 27
      Width = 45
      Height = 13
      Caption = 'lblVersion'
    end
    object lblInfo1: TLabel
      Left = 8
      Top = 136
      Width = 19
      Height = 13
      Caption = 'Visit'
    end
    object lblInfo2: TLabel
      Left = 33
      Top = 136
      Width = 84
      Height = 13
      Cursor = crHandPoint
      Caption = 'http://tomeko.net'
      Color = clBtnFace
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlue
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsUnderline]
      ParentColor = False
      ParentFont = False
      OnClick = lblInfo2Click
    end
    object lblInfo3: TLabel
      Left = 123
      Top = 136
      Width = 150
      Height = 13
      Caption = 'for updates and documentation.'
    end
    object lblBuilt: TLabel
      Left = 88
      Top = 46
      Width = 20
      Height = 13
      Caption = 'Built'
      IsControl = True
    end
    object lblBuildTimestamp: TLabel
      Left = 146
      Top = 46
      Width = 84
      Height = 13
      Caption = 'lblBuildTimestamp'
      IsControl = True
    end
  end
  object OKButton: TButton
    Left = 111
    Top = 180
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 1
  end
end
