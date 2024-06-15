# stm32-si5351

HAL-based Si5351 driver for STM32. For ESP32 platform there is a fork [osmanovv/esp32-si5351](https://github.com/osmanovv/esp32-si5351).

Si5351 is a I2C-programmable 8 kHz - 160 MHz clock generator made by Silicon Labs. It has 3 ports (or more depending on modification) with 50 Ohm output impedance. The signal level can be changed in ~2-11 dBm range and the phase shift between channels is configurable.

Basic interface:

```
const int32_t correction = 978;
si5351_Init(correction);

// 28 MHz @ ~7 dBm
si5351_SetupCLK0(28000000, SI5351_DRIVE_STRENGTH_4MA);

// 144 MHz @ ~7 dBm
si5351_SetupCLK2(144000000, SI5351_DRIVE_STRENGTH_4MA);

// Enable CLK0 and CLK2
si5351_EnableOutputs((1<<0) | (1<<2));
```

Advanced interface, setting up regular mode:

```
const int32_t correction = 978;
si5351_Init(correction);

si5351PLLConfig_t pll_conf;
si5351OutputConfig_t out_conf;
int32_t Fclk = 7000000; // 7 MHz

si5351_Calc(Fclk, &pll_conf, &out_conf);
si5351_SetupPLL(SI5351_PLL_A, &pll_conf);

si5351_SetupOutput(0, SI5351_PLL_A, SI5351_DRIVE_STRENGTH_4MA, &out_conf, 0);
si5351_EnableOutputs(1<<0);
```

Advanced interface, setting up I/Q-mode:

```
const int32_t correction = 978;
si5351_Init(correction);

si5351PLLConfig_t pll_conf;
si5351OutputConfig_t out_conf;
int32_t Fclk = 7000000; // 7 MHz

si5351_CalcIQ(Fclk, &pll_conf, &out_conf);

/*
 * `phaseOffset` is a 7bit value, calculated from Fpll, Fclk and desired phase shift.
 * To get N° phase shift the value should be round( (N/360)*(4*Fpll/Fclk) )
 * Two channels should use the same PLL to make it work. There are other restrictions.
 * Please see AN619 for more details.
 *
 * si5351_CalcIQ() chooses PLL and MS parameters so that:
 *   Fclk in [1.4..100] MHz
 *   out_conf.div in [9..127]
 *   out_conf.num = 0
 *   out_conf.denum = 1
 *   Fpll = out_conf.div * Fclk.
 * This automatically gives 90° phase shift between two channels if you pass
 * 0 and out_conf.div as a phaseOffset for these channels.
 */
uint8_t phaseOffset = (uint8_t)out_conf.div;
si5351_SetupOutput(0, SI5351_PLL_A, SI5351_DRIVE_STRENGTH_4MA, &out_conf, 0);
si5351_SetupOutput(2, SI5351_PLL_A, SI5351_DRIVE_STRENGTH_4MA, &out_conf, phaseOffset);

/*
 * The order is important! Setup the channels first, then setup the PLL.
 * Alternatively you could reset the PLL after setting up PLL and channels.
 * However since _SetupPLL() always resets the PLL this would only cause
 * sending extra I2C commands.
 */
si5351_SetupPLL(SI5351_PLL_A, &pll_conf);
si5351_EnableOutputs((1<<0) | (1<<2));
```

More comments are in the code. See also examples/ directory.

This library was forked from [ProjectsByJRP/si5351-stm32](https://github.com/ProjectsByJRP/si5351-stm32) which in it's turn is a port of [adafruit/Adafruit_Si5351_Library](https://github.com/adafruit/Adafruit_Si5351_Library). Both libraries are licensed under BSD.
