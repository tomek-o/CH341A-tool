# EEPROM / FLASH programmer

Tool tab "EEPROM / FLASH programmer" (tool group "EEPROM / FLASH"). It reads, writes,
verifies and erases memory chips connected to the CH341A, using files as the data source/target.

Implementation details (bus sequences, formats, assumptions): [DESIGN.md](DESIGN.md).

Status: written against the AsProgrammer (dregmod) sources as reference and compiled with BDS 2006.
It has **not been tested on real chips yet** - always run verify after a write, and try a small,
unimportant chip first.

## Supported chips

| Device type | Chips | Bus |
| --- | --- | --- |
| SPI flash (25xx) | any JEDEC-style NOR flash, 64 KB .. 32 MB (4-byte addressing above 16 MB) | SPI, CS0 |
| SPI EEPROM (25xxx/95xxx) | 25x010 .. 25x1024 (128 B .. 128 KB) | SPI, CS0 |
| I2C EEPROM (24xx) | 24C01 .. 24C1024 | I2C |
| Microwire EEPROM (93Cxx) | 93C46/56/66/76/86, x8 and x16 organisation | CH341 bit stream, CS0 |
| SPI DataFlash | AT45DB011 .. AT45DB642 | SPI, CS0 |

Not supported: SPI/parallel NAND, parallel flash, eMMC, MCU internal flash, AT45DB128 and bigger
(they need 4 address bytes), sector/block erase other than 4 KB.

## Wiring

CH341A programmer boards put these signals on the programming socket / header
(check the pinout of your board, it is the same as for any SPI/I2C use of CH341):

| Signal | CH341 pin | Used by |
| --- | --- | --- |
| CS | CS0 (D0) | SPI flash, SPI EEPROM, Microwire, AT45 |
| SCK / SK | D3 | same |
| MOSI / DI | D5 | same |
| MISO / DO | D7 | same |
| SCL, SDA | SCL, SDA | I2C EEPROM |

Things to watch:
- Check the supply voltage of the programmer (many boards supply 5 V to the socket) against the chip.
  1.8 V chips need a level shifter/adapter.
- In-circuit programming usually works only if the rest of the board is not powered and does not
  load the bus.
- /WP and /HOLD of SPI chips must be high (or wired as required by the chip).
- Microwire: the ORG pin selects x8/x16 organisation - the tool setting has to match the wiring.
- I2C EEPROM: A0..A2 select the device address (0x50 .. 0x57); the tool's address setting defaults to *Auto* (uses the device found).

## Using the tool

1. Open the CH341 (as for any other tool), select the device type with the tabs at the top, choose the chip and set parameters.
2. Use the buttons:

| Button | Action |
| --- | --- |
| Read chip | reads the whole chip into the hex editor (the large area of the tab) |
| Write chip | writes the contents of the hex editor from address 0 (must not be bigger than the chip), then verifies if "Verify" is checked. With "Write only edited" checked it writes just the parts changed in the editor since the chip was read (see below) |
| Verify chip | compares the chip with the contents of the hex editor |
| Erase chip / Fill with 0xFF | chip erase for flash/AT45/Microwire, writes 0xFF over the whole chip for 24xx and 25xxx EEPROMs |
| Cancel | stops the current operation (a chip erase or a page write in progress is finished by the chip itself) |
| Read ID / Detect | JEDEC ID (flash) or ID + geometry (AT45) |
| Detect | I2C: scans the addresses and suggests the chip type |
| Unprotect | clears the block protection bits of the status register |

Data is not handled as files directly: it lives in the hex editor that fills the tab (frame in `FrameHexEditor.cpp`
and control in `HexView.cpp`, in the main source folder - reusable by other tools). *Read chip* fills it, *Write chip* /
*Verify chip* use it. Use *Open file...* / *Save file...* above the editor to load a file to write or save a dump.
The editor has an offset column, 16 bytes per row in hex and an ASCII column; bytes changed since loading are red.
Hex digits (or characters in the ASCII pane, switched with Tab) overwrite data, Shift+arrows / mouse select,
Ctrl+C / Ctrl+V copy and paste hex. The log below is only a few lines high.

**Write only edited:** with this box checked, *Write chip* writes only what you changed in the editor (red bytes) since
*Read chip*, without reading the chip again. It assumes the chip still holds what was read. It does not work after loading
a file into the editor (the editor then holds no chip image; the tool says so). For SPI flash it uses the same logic as
*Write only changed data* (sector erase only where bits must be set, the "Before write" list is ignored); for EEPROMs and
AT45 it writes the runs of edited bytes, widened to a whole page (AT45) or 16-bit word (Microwire x16). After a
successful write the editor's reference is updated, so a next edit/write cycle works without reading again.

Operations ask for confirmation before writing/erasing. Every result is logged with the time it took.

### SPI flash

- Size and page size are selected manually. "Read ID" reads the JEDEC ID and sets size from the
  capacity byte (2^n bytes); this guess can be wrong for some vendors, check the log.
- "Before write" selects:
  - *None (chip is blank)* - writes everything, does not erase. Use for blank chips.
  - *Erase whole chip* - chip erase (0xC7), then programs only non-blank pages.
  - *Erase 4 KB sectors* - erases only the sectors covered by the file (0x20, 0x21 for 4-byte
    address chips), then programs only non-blank pages. Chips without the 0x20 command need chip erase.
  - *Write only changed data* - reads the chip first, then per 4 KB sector: identical sectors are skipped,
    sectors where the data only clears bits get just the differing pages programmed (no erase), and sectors
    where bits have to be set are erased and rewritten. Much faster and gentler to the chip when changing a
    few bytes of a big image. The log tells how many sectors were erased and pages programmed.
- "Program" selects the programming method: *Page program* (0x02/0x12, default) or the SST
  auto address increment modes: *AAI word* (0xAD, e.g. SST25VF016B) and *AAI byte* (0xAF, e.g.
  SST25VF512/010). In AAI modes blank words are skipped, and an odd last byte is padded with 0xFF.
- Block protection: if the status register shows protection bits, use "Unprotect" first.
  The sequence covers old SST parts too (0x50 + WRSR, then WREN + WRSR).

### SPI EEPROM

- Pick the chip type; page size is set from the type but can be changed (it differs between
  manufacturers and suffixes, e.g. 25LC080 A/B vs C/D).
- Address length is derived from the type: 1 byte (up to 256 B), 1 byte + A8 in the opcode (512 B),
  2 bytes, 3 bytes (128 KB).

### I2C EEPROM

- Address bits that do not fit in the address bytes are sent in the device address
  (24C04/08/16, and A16 of 24C1024 - the latter matches 24LC1026/M24M01, other 1 Mbit parts such as
  24LC1025 use a different scheme and are not handled).
- **Detect** scans 0x50..0x57. A chip answers on 1 (24C01/02, 24C32 and bigger), 2 (24C04, 24C1024),
  4 (24C08) or 8 (24C16) consecutive addresses, so for 24C04/08/16 the type is selected automatically.
  With a single address the size can not be read from the chip - select the type manually.
- Before every operation the number of addresses the chip answers on is compared with the selected
  type; e.g. a 24C04 with "24C02" selected is refused ("does not match the selected chip type").
- Writes are done page by page with ACK polling; reads in 256-byte blocks.
- The CH341 reports success even when nothing ACKs the data, so the tool probes the device first
  and "Verify" is what really confirms a write.

### Microwire (93Cxx)

- Wiring as for SPI (see above), clock is generated by the CH341 bit stream mode, so it is slow (several KB/s).
- File format: in x16 mode every word is stored high byte first (big endian), as in AsProgrammer.
  Sizes and addresses must be even. In x8 mode data is plain bytes.
- Writes use EWEN / WRITE / EWDS and wait for the chip's ready signal (DO) after every word.
  "Erase chip" uses ERAL.

### AT45 DataFlash

- "Detect" reads the ID and the status register; size and page size are detected automatically
  (also done before every operation). Pages are 264/528/1056 bytes ("standard") or 256/512/1024
  ("binary", set in the chip).
- The file contains whole pages including the extra 8/16/32 bytes of standard pages, so its size
  is a multiple of the page size.
- Pages are written with the built-in erase (0x82), so no separate erase is needed.
- "Unprotect" disables sector protection (3D 2A 7F 9A).

## Source layout

All code is in `CH341A/tools/MemProg/`:

| File | Content |
| --- | --- |
| `MemProg.h/.cpp` | chip access without any GUI: `SpiFlash` (also used for SPI EEPROM), `At45`, `Microwire`, `I2cEeprom`, chip type tables, `Progress` callback interface |
| `FormCH341MemProg.h/.cpp/.dfm` | the tool tab |

Every class has the same shape: `Configure(...)`, `Read`, `Write`, `Verify` (and erase where the chip has it),
all taking a start address, buffer and byte count and returning `MemProg::Status` (`StatusText()`
turns it into a message). Long operations take an optional `Progress*`: it gets `Report(done, total)`
calls and is polled with `IsCancelled()`; pass `NULL` to run without. The form implements it
with a progress bar and `Application->ProcessMessages()`.

Low level access uses `ch341a.SpiTransfer()` (CS0 held for the whole call), `SpiTransferBitStream()` and
`SetD5D0()` for Microwire, and `I2CWriteRead()` / `I2CCheckDev()` for I2C.

## Known limitations

- The transfers rely on `CH341StreamSPI4` / `CH341StreamI2C` splitting long buffers into USB packets
  themselves (AsProgrammer does the same); chunk sizes are kept at the sizes it uses (2 KB for SPI reads).
- No chip database: flash size must be chosen by the user (the JEDEC ID only suggests it).
- Erase granularity is chip or 4 KB sector; no 32/64 KB block erase.
- Speed is limited by the CH341A (SPI is typically tens of KB/s, Microwire a few KB/s).
