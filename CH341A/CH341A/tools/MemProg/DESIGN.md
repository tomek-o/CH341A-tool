# EEPROM / FLASH programmer - technical description

Companion to [README.md](README.md) (user view). This file describes how the code works: layers,
the exact bus sequences used for every chip family, data formats, error handling, and what is
still unverified. Everything here is derived from `MemProg.cpp` and `FormCH341MemProg.cpp`; if the
two disagree, the code wins.

## 1. Layers

```
FormCH341MemProg   (VCL form, file dialogs, progress bar, log)
        |  uses MemProg::Progress, MemProg::Status
MemProg.h/.cpp     (SpiFlash, At45, Microwire, I2cEeprom - no GUI code)
        |
CH341A class       (SpiTransfer, SpiTransferBitStream, I2CWriteRead, I2CCheckDev,
        |           SetD5D0, GetGpioInputs)
CH341DLL.DLL       (CH341StreamSPI4, CH341BitStreamSPI, CH341StreamI2C, ...)
```

`MemProg.cpp` only includes `CH341A.h` and `windows.h` (`Sleep`, `GetTickCount`), so the chip logic can
be reused from another front end. The form is the only user of the VCL.

Two additions were made to the existing `CH341A` class: `SetD5D0()` (wraps `CH341Set_D5_D0`, needed
for Microwire chip select handling) and the `ToolGroupMemory` tool group ("EEPROM / FLASH").

### 1.1 Common conventions

- Addresses and sizes are byte-based everywhere (`uint32_t addr`, `unsigned long count`), also for
  chips that are word- or page-organised. Chip specific rules are enforced in the class
  (see 1.3).
- All operations return `MemProg::Status`:

| Value | Meaning |
| --- | --- |
| `StatusOk` (0) | success |
| `StatusNotOpened` | CH341 not opened (`CH341A::IsOpened()` false, or the CH341A layer returned -1) |
| `StatusCommError` | any other CH341A layer failure |
| `StatusCancelled` | `Progress::IsCancelled()` returned true |
| `StatusTimeout` | chip stayed busy longer than the operation specific timeout |
| `StatusNoDevice` | no/invalid response (JEDEC ID all 0x00 or 0xFF, AT45 ID mismatch, I2C no ACK) |
| `StatusVerifyFailed` | verify mismatch, first differing address in `mismatchAddr` |
| `StatusInvalidArgs` | range outside the chip, wrong alignment, chip not configured |

- Long operations take `Progress *progress` (may be `NULL`). `Report(done, total)` is called after every
  chunk, `IsCancelled()` before every chunk and inside busy loops. Units are bytes, except
  `EraseSectors` (sectors).
- Chunks never cross the boundary that matters for the chip: a page for writes, a 256-byte block
  for I2C reads, a 16-byte block for Microwire reads.
- A failed status read is never treated as "ready" (`SpiFlash::WaitReady`, `At45::WaitReady`): the
  error is returned instead, so a dead link cannot turn into a silent "Done".

### 1.2 CH341 primitives used

| Primitive | CH341 API | Notes |
| --- | --- | --- |
| `SpiTransfer(buf, n)` | `CH341StreamSPI4(idx, 0x80, n, buf)` | full duplex, MSB first (`CH341SetStream` bit 7 set in `CH341A::Open`), CS0 low for the whole call, input overwrites the buffer |
| `SpiTransferBitStream(buf, nbits)` | `CH341BitStreamSPI` | one byte per clock, see 5.1 |
| `SetD5D0(dir, data)` | `CH341Set_D5_D0` | direction/level of D5..D0 |
| `GetGpioInputs(v)` | `CH341GetInput` | bit 7 = D7 (DO of Microwire chip) |
| `I2CWriteRead(w, wn, r, rn)` | `CH341StreamI2C` | first byte of `w` is the 8-bit device address (write); with `rn > 0` a restart + read follows |
| `I2CCheckDev(addr7)` | start, address byte, ACK check, stop | used for probing and write-cycle polling |

Every SPI command is one `SpiTransfer` call: header and data go out in one CS assertion, and read
data is taken from the same buffer after the header. Commands without data (WREN, chip erase...) are
1-byte calls.

### 1.3 Alignment rules

| Class | Rule |
| --- | --- |
| `SpiFlash` | none for read/verify; writes are cut at page boundaries internally; AAI word mode needs an even start address |
| `I2cEeprom` | none (chunking is internal) |
| `Microwire` | x16: address and count must be even; x8: none |
| `At45` | address and count must be multiples of the page size (264/528/1056 or 256/512/1024) |

## 2. SPI flash and SPI EEPROM (`SpiFlash`)

The same class serves both. `Configure(size, pageSize, addrBytes)`; `addrBytes == 0` means automatic:
3 bytes, or 4 bytes if `size > 16 MB`. SPI EEPROMs pass 1, 2 or 3.

### 2.1 Command header

`BuildHeader(buf, opcode3, opcode4, addr)` produces opcode + big-endian address:

| Address bytes | Layout |
| --- | --- |
| 1 | `op, A7..A0`; for a 512-byte chip `op |= A8 << 3` (0x03 -> 0x0B for read, 0x02 -> 0x0A for write) |
| 2 | `op, A15..A8, A7..A0` |
| 3 | `op, A23..A16, A15..A8, A7..A0` |
| 4 | `op4, A31..A24, ...` where `op4` is the 4-byte variant (0x13 read, 0x12 page program, 0x21 sector erase) |

The 4-byte opcodes are used whenever the effective address length is 4. The chip is **not** switched
into 4-byte mode (EN4B) - dedicated 4-byte opcodes are used instead.

### 2.2 Operations

| Operation | Sequence | Timeout |
| --- | --- | --- |
| Read ID | `9F FF FF FF` -> manufacturer, type, capacity. All `00` or all `FF` -> `StatusNoDevice` | - |
| Read status | `05 FF` | - |
| Read | per chunk of up to 2048 bytes: `[03/13, addr, FF x n]`, data from offset `header length` | - |
| Page program | `06` (WREN), then `[02/12, addr, data]` (data never crosses a page boundary), then poll status bit 0 (WIP) | 1000 ms |
| Chip erase | `06`, `C7`, poll WIP | 5 min |
| Sector erase | for each 4 KB sector touched by the range: `06`, `[20/21, addr]`, poll WIP | 3000 ms per sector |
| Write changed | read the range (whole sectors), then per sector: equal -> skip; target only clears bits (`(cur & tgt) == tgt` for all bytes) -> program differing pages; else sector erase + program non-blank pages. The part of a partly covered sector outside the range is kept (taken from the chip). | as page program / sector erase |
| Fill blank (EEPROMs) | page program of 0xFF over the whole chip, no skipping | as page program |
| Unprotect | `50`; `01 00`; poll; `06`; `01 00`; poll | 1000 ms |
| Verify | read in 2048-byte chunks and compare | - |

`skipBlank` (used after an erase): pages consisting only of 0xFF are not programmed.

Polling loops sleep 1 ms between status reads and check the cancel callback each round.

### 2.3 SST auto address increment

`ProgramMode` selects the method; the form sets it before every operation (SPI EEPROM always uses
`ProgramPage`, since the object is shared).

| Mode | Opcode | Bytes per command |
| --- | --- | --- |
| `ProgramAaiWord` | `AD` | 2 |
| `ProgramAaiByte` | `AF` | 1 |

Algorithm (`WriteAai`):

1. First non-blank unit: `WREN`, then `[op, addr(3), data]`, poll WIP.
2. Following units while contiguous: `[op, data]`, poll WIP.
3. A blank unit (only with `skipBlank`) ends the AAI sequence: `WRDI` (`04`) + poll. The next
   non-blank unit starts a new sequence at its own address (step 1).
4. At the end, or after an error or cancel, `WRDI` is always sent to leave AAI mode.

A trailing odd byte in word mode is padded with 0xFF. Polling uses status bit 0, which is valid
during AAI. Timeout per unit: 100 ms.

### 2.4 Unprotect rationale

Block protection bits (BP0..) make erase/program silently do nothing. The status register is cleared
two ways because chips differ: old SST parts need "enable write status register" (`50`) in place of
WREN, while newer parts ignore `50` and need WREN. Chips whose status register is hardware
protected (SRP + /WP low) will not accept either; that has to be fixed by wiring.

## 3. I2C EEPROM (`I2cEeprom`)

### 3.1 Type table

| Type | Size | Page | Address bytes | Address bits in device address |
| --- | --- | --- | --- | --- |
| 24C01 | 128 | 8 | 1 | 0 |
| 24C02 | 256 | 8 | 1 | 0 |
| 24C04 | 512 | 16 | 1 | 1 |
| 24C08 | 1024 | 16 | 1 | 2 |
| 24C16 | 2048 | 16 | 1 | 3 |
| 24C32 | 4096 | 32 | 2 | 0 |
| 24C64 | 8192 | 32 | 2 | 0 |
| 24C128 | 16384 | 64 | 2 | 0 |
| 24C256 | 32768 | 64 | 2 | 0 |
| 24C512 | 65536 | 128 | 2 | 0 |
| 24C1024 | 131072 | 256 | 2 | 1 |

### 3.2 Addressing

Base 7-bit address `devAddr` is 0x50..0x57 (A2..A0 pins, selected in the UI). Address bits that do not
fit into the address byte(s) replace the lowest bits of the 7-bit device address:

```
shift = addrBytes * 8            // 8 for 1-byte types, 16 for 2-byte types
mask  = (1 << addrBitsInDev) - 1
dev7  = (devAddr & ~mask) | ((addr >> shift) & mask)
header = [ dev7 << 1, (addr >> 8 if 2 address bytes), addr & 0xFF ]
```

Consequently the device address pins that are "used up" (A0 for 24C04, A0..A1 for 24C08,
A0..A2 for 24C16) must be treated as don't care; the UI base address low bits are overwritten.
For 24C1024 the single bit is A16, in 7-bit address bit 0 (control byte bit 1), as on 24LC1026 / M24M01.

### 3.3 Operations

| Operation | Sequence |
| --- | --- |
| Scan / Probe | `I2CCheckDev` on 0x50..0x57. The ACKing addresses must form one aligned run of 1, 2, 4 or 8 (`I2cScanResult`); `Probe` additionally requires the run length to equal `1 << addrBitsInDev` of the selected type (else `StatusWrongDevice`) and, with *Auto* address, takes the base address from the scan. The UI *Detect* button maps run length 2/4/8 to 24C04(24C1024) / 24C08 / 24C16; a run of 1 is ambiguous (24C01/02 or 24C32+) |
| Read | per chunk: `I2CWriteRead(header, n_header, buf, n)`; chunks end at 256-byte boundaries so one transfer never crosses a device-address change or a 1-byte address wrap |
| Write | per chunk (up to the next page boundary): `I2CWriteRead([header, data], n, NULL, 0)` then poll `I2CCheckDev(dev7)` until ACK (max 100 ms, 1 ms sleeps) |
| Erase | write 0xFF over the whole chip |
| Verify | read in 1024-byte chunks, compare |

The CH341 DLL reports success for transfers even when no device ACKs (see the todo in
`CH341A::I2CWriteRead`). Hence: probe before every operation, ACK polling after every page (a
write-protected chip ACKs but does not store data, ACK polling then ends at once), and verification
as the only real proof of a good write.

## 4. SPI DataFlash AT45 (`At45`)

### 4.1 Detection

`9F FF FF FF` -> manufacturer `1F`, device ID 1, device ID 2. Accepted when manufacturer is `0x1F`,
`(id1 & 0xE0) == 0x20` (AT45 family) and density `d = id1 & 0x1F` is in 2..8.

```
base      = d <= 5 ? 256 : (d <= 7 ? 512 : 1024)
status    = D7h reply; bit 0 = 1 -> binary page size
pageSize  = bit0 ? base : base + base/32          // 256->264, 512->528, 1024->1056
pageCount = 2^(15 + d) / base
```

| Density code | Chip | Pages | Standard page | Size (standard) |
| --- | --- | --- | --- | --- |
| 2 | AT45DB011 | 512 | 264 | 135168 |
| 3 | AT45DB021 | 1024 | 264 | 270336 |
| 4 | AT45DB041 | 2048 | 264 | 540672 |
| 5 | AT45DB081 | 4096 | 264 | 1081344 |
| 6 | AT45DB161 | 4096 | 528 | 2162688 |
| 7 | AT45DB321 | 8192 | 528 | 4325376 |
| 8 | AT45DB642 | 8192 | 1056 | 8650752 |

(Density 9 and above would need 4 address bytes and are rejected as unsupported.)

### 4.2 Addressing

24-bit address `= page << offsetBits`, `offsetBits` being the smallest `n` with `2^n >= pageSize`
(8 for 256/264, 9 for 512/528, 10 for 1024/1056). The file/buffer offset of page `p` is simply
`p * pageSize`; the extra bytes of standard pages are part of the page data and are read and written.

### 4.3 Operations

| Operation | Sequence |
| --- | --- |
| Read | `[E8, a2, a1, a0, 00 00 00 00, FF x n]` per chunk of `max(1, 2048/pageSize)` pages; data after the 8 header bytes (continuous array read, legacy opcode) |
| Write | per page `[82, a2, a1, a0, pageSize bytes]` (main memory page program through buffer 1 *with* built-in erase), then poll status bit 7 (RDY, 1 = ready), 1000 ms |
| Chip erase | `C7 94 80 9A`, poll RDY, 5 min |
| Disable sector protection | `3D 2A 7F 9A`, poll RDY |
| Verify | read + compare |

Page size is determined from the chip on every `Detect()`. The form calls `Detect()` as part of
`Configure()`, i.e. before every operation, so there is nothing to select manually.

## 5. Microwire 93Cxx (`Microwire`)

### 5.1 Bit stream format

`CH341BitStreamSPI(nbits, buf)` clocks one bit per buffer byte. Bit usage per byte (from the DLL
documentation):

| Bit | Pin | Use |
| --- | --- | --- |
| 0 | D0 | CS (kept 1 for the whole stream) |
| 3 | D3 | clock, generated by the CH341 |
| 5 | D5 | DOUT -> chip DI |
| 7 | D7 | DIN <- chip DO (returned) |

Pin directions are set with `SetD5D0(0x29, ...)` (outputs: D0, D3, D5). Output data changes while the
clock is low, rising edge latches in the chip; DIN is sampled in the high period of the same clock
cycle. A transaction (`Transfer`) is:

1. `SetD5D0(0x29, 0)` - CS low
2. `SetD5D0(0x29, 1)` - CS high
3. `SpiTransferBitStream(bits, n)`
4. `SetD5D0(0x29, 0)` - CS low

`PinsGuard` releases D5..D0 to inputs (`SetD5D0(0, 0)`) when a public operation returns.
Streams stay below the 256-bit recommendation of the DLL (a read chunk is at most 14 + 128 bits).

### 5.2 Types and address length

| Type | Size | Address bits x16 | Address bits x8 |
| --- | --- | --- | --- |
| 93C46 | 128 B | 6 | 7 |
| 93C56 | 256 B | 8 | 9 |
| 93C66 | 512 B | 8 | 9 |
| 93C76 | 1 KB | 10 | 11 |
| 93C86 | 2 KB | 10 | 11 |

(93C56/66 and 93C76/86 use the longer address fields from the manufacturer's command tables even
though the topmost bit is a don't care.) Address in the command is the word address in x16 mode
(`byteAddr >> 1`), the byte address in x8 mode.

### 5.3 Commands

Command = start bit `1`, 2-bit opcode, address field (MSB first).

| Command | Opcode | Address field | Data |
| --- | --- | --- | --- |
| READ | `10` | address | clocked out sequentially, address auto-increments |
| WRITE | `01` | address | 16 (x16) or 8 (x8) bits, then busy/ready wait |
| EWEN | `00` | `11` + zeros | - |
| EWDS | `00` | `00` + zeros | - |
| ERAL | `00` | `10` + zeros | busy/ready wait |

- **Read:** one transaction per 16-byte aligned chunk: header, then `n*8` clocks with DOUT = 0. The
  chip's dummy zero bit appears with the last address clock, so data bit 7 of the first byte is the
  sample belonging to clock number `3 + addrBits` (counting from 0). Bytes are assembled MSB first.
  x16 data is therefore stored in the file high byte first (big endian).
- **Write:** `EWEN`, then for every word/byte one transaction (header + data bits), then ready wait,
  and finally `EWDS` (also after errors/cancel). There is no skipping and no prior erase (the chip
  erases the cell itself).
- **Ready wait:** after the transaction CS is dropped low and raised high again (CS low -> high with
  no clock); the chip then drives DO low while busy and high when done. `GetGpioInputs` bit 7 is polled
  (1 ms sleeps) with a 200 ms timeout; CS is returned low afterwards.
- **ERAL:** `EWEN`, `ERAL` + ready wait, `EWDS`.

## 6. The form (`TfrmCH341MemProg`)

### 6.1 Structure

- One tab, registered in `ToolGroupMemory` (and, like every tool, in "All tools").
- The page control `pcDevice` holds one tab sheet per device type (`tsSpiFlash`, `tsSpiEeprom`,
  `tsI2cEeprom`, `tsMicrowire`, `tsAt45`); the active tab is the selected device (`GetDevice()` maps the
  active sheet to `DeviceIndex`, so tabs can be reordered in the designer). Chip settings live directly on the sheets, so they can be moved
  in the IDE form editor. 
- Chip objects (`spi`, `microwire`, `i2cEeprom`, `at45`) are members; `spi` serves both SPI flash and
  SPI EEPROM, which is why `Configure()` always sets the program mode.

### 6.2 Operation flow

Every action handler does the same:

```
if (busy || !CheckOpened()) return;
size = Configure();          // UI -> chip object; AT45 detects here; 0 = error (already logged)
if (!CheckPresent()) return; // I2C probe
[ file dialog, load file, size checks, confirmation ]
SetBusy(true);  ... operations ...  SetBusy(false);
LogResult(...)               // "Read: OK (1.2 s)" or the status text
```

Write sequence: optional erase (chip or sectors; `skipBlank = true` after it) -> write -> optional
verify. A failure stops the sequence; the log shows which step failed.

### 6.3 Progress and cancel

`UiProgress` (in the .cpp) implements `MemProg::Progress`: `Report()` is throttled to one UI update per 50 ms
(and always updated at completion); `IsCancelled()` calls `Application->ProcessMessages()` and returns
the `cancelRequested` flag set by the Cancel button. While busy, all other controls are
disabled, and the handlers also refuse reentry (`busy`).

Operations run in the UI thread; the UI stays responsive only through `ProcessMessages()`, so a very
long single CH341 call (large reads) blocks it for the duration of that call (chunks are 2 KB,
a fraction of a second at most).

### 6.3a "Write only edited" write

`THexView` keeps the data as given to `SetData()` (`Original()`), so the edits are `data != original`. The
frame also remembers whether it holds a chip image (`SetData(..., isChipImage)`: true after *Read chip*, false after
opening a file). With "Write only edited" the programmer refuses to continue unless it is a chip image; then `WriteEdited`
either calls `SpiFlash::WriteChanged(..., assumedCurrent = original)` (no chip read except for partly covered sectors
around the range) or writes runs of changed bytes with `DoWrite(addr, ...)`, widened to the chip's write unit. After
any successful write `MarkClean(true)` makes the editor contents the new reference.

### 6.4 Data and files

The data buffer is owned by the hex editor frame (`TframeHexEditor`, created in the programmer's constructor
into the `pnlHex` placeholder panel, `Align = alClient`, so it resizes with the tab): `SetData()` after a read,
`GetData()` before write/verify (`GetEditorData` also checks that it fits the chip), so edits made in the editor
are what gets written. Files are plain binary, no header; opening and saving is done with the frame's buttons.
Write/verify accept data up to the chip size (AT45 and x16 Microwire need page/word multiples).

`THexView` (control) and `TframeHexEditor` (frame) live next to `Mru.cpp` and know nothing about the
programmer; any tool can use them the same way. The tab's layout is anchored: the device page control and the
button row stay at the top, the editor takes the free space, and the progress bar and the 3 line log stay at the bottom.

## 7. Extending

- **New I2C EEPROM / SPI EEPROM / Microwire variant:** add a row to the type table in `MemProg.cpp`
  (`i2cEepromTypes`, `spiEepromTypes`, `microwireTypes`); the UI fills its lists from those tables.
  An EEPROM with a different address scheme needs a new field and a change in `BuildHeader`.
- **New SPI flash size:** add to `spiSizes[]` in `FormCH341MemProg.cpp`.
- **New chip family:** new class in `MemProg.h/.cpp` with `Configure/Read/Write/Verify`, a panel and a
  `DeviceIndex` entry in the form, and cases in `Configure/DoRead/DoWrite/DoVerify/DoErase`.
- **Another front end / script:** use the classes directly after `ch341a.Open(...)`.

## 8. Not verified / assumptions

Nothing has been run against real chips yet. Known assumptions to check first:

1. `CH341StreamSPI4` and `CH341StreamI2C` split buffers longer than one USB packet by themselves
   (the AsProgrammer reference does the same; chunk sizes here are at most the ones it uses, except I2C
   writes of up to 256 bytes + header for 24C1024 pages).
2. Microwire read data alignment (data starts directly after the last address clock, no extra bit
   skipped). If dumps are shifted by one bit, this is the place to look (`Microwire::Read`, `cmdBits`).
3. Microwire ready detection after WRITE/ERAL via D7 with CS low -> high (as `MWIsBusy` in the reference).
4. 4-byte address opcodes (0x13, 0x12, 0x21) on chips over 16 MB.
5. AAI timing, and that status bit 0 is usable for polling in AAI mode on the target SST parts.
6. AT45: legacy opcodes E8 / 82 / D7 / C7 94 80 9A are accepted by the "E" series as well as the older "D" series.
7. Sector erase opcode 0x20 exists on the flash being used.

Reading the same chip twice and comparing, writing + verifying a small test chip, and checking that a
verify of a modified file fails at the expected address is a sufficient first test for each family.
