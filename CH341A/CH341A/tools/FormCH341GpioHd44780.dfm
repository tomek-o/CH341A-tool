object frmCH341GpioHd44780: TfrmCH341GpioHd44780
  Left = 0
  Top = 0
  Caption = 'HD44780 display'
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
  object Memo1: TMemo
    Left = 0
    Top = 0
    Width = 635
    Height = 240
    Align = alClient
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    Lines.Strings = (
      'Connections:'
      'CH341A      Display'
      ''
      'GND         VSS (pin 1)'
      '+5V         VCC (pin 2) - or 3.3V, depending on display model'
      
        'RX          Register Select (pin 4); low -> command, high -> dat' +
        'a'
      'TX          ENABLE (pin 6)'
      'SPI CLK     DATA4 (pin 11)'
      'SPI CS      DATA5 (pin 12)'
      'SPI MOSI    DATA6 (pin 13)'
      'SPI MISO    DATA7 (pin 14)'
      ''
      'Display contrast input might need external potentiometer.'
      'Display R/W (pin 5) should be connected to GND.')
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 0
    ExplicitLeft = 8
    ExplicitTop = 8
    ExplicitWidth = 619
    ExplicitHeight = 114
  end
  object pnlBottom: TPanel
    Left = 0
    Top = 240
    Width = 635
    Height = 59
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object lblStatus: TLabel
      Left = 7
      Top = 40
      Width = 3
      Height = 13
    end
    object btnWrite: TButton
      Left = 7
      Top = 6
      Width = 75
      Height = 25
      Caption = 'Write'
      TabOrder = 0
      OnClick = btnWriteClick
    end
  end
end
