# SD / MMC card (SPI mode)

Two windows in the *SD / MMC* tool group, sharing one card (init once, use in either):

- **SD / MMC card: commands** - *Init card* (80 clocks, CMD0, CMD8, ACMD41 loop, CMD58,
  CMD16), *Card info* (OCR, CID, CSD: manufacturer, product, serial, date, capacity), *Status* (CMD13),
  one button per common command (CMD0, CMD8, CMD9, CMD10, CMD13, CMD16, CMD58, ACMD41) and a raw command box
  (command, ACMD prefix, hex argument, response format R1 / R2 / R3-R7). Every command and response is logged.
- **SD / MMC card: sector data** - *Read sectors* (CMD17 per sector) into the hex editor, *Write sectors*
  (CMD24, editor data must be a multiple of 512 bytes; locked by the *Allow writing* checkbox and a confirmation).
  Sector number and count accept decimal or `0x` hex. Editor offsets are relative to the first sector read.

Connections are described in the memo at the top of both windows and in `SD_WIRING.txt` (3.3 V only!).

## Files
- `SdCard.h/.cpp` - protocol (namespace `SdMmc`, no GUI); one SPI exchange per command, so CS stays low
  from the command through the response and the data block. Init clocks use `CH341BitStreamSPI` (CS high).
- `FormCH341SdCard.*`, `FormCH341SdData.*` - the windows.

## Not verified on hardware
- CH341A SPI clock vs. the 400 kHz limit during card init (cards usually tolerate it).
- `CH341StreamSPI4` with ~800 byte buffers (one CMD17 exchange).
- CMD24: CS is released between the command and the data block, and busy is polled by re-selecting the card.
- SD v1 / MMC paths (only the SDHC flow is straightforward).
