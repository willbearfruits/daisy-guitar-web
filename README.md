# DP // Dual Processing Guitar FX

**Version 2.1** - A professional dual-channel, real-time guitar effects processor for the [Electro-Smith Daisy Seed](https://www.electro-smith.com/daisy/daisy-seed) with comprehensive web-based control and browser-based firmware flashing.

## 🎸 Features

### Dual Channel Architecture
- **2 Independent Guitar Inputs** with full stereo processing
- **2 Outputs** with pristine audio quality
- Separate effect chains per channel
- Cross-channel modulation and routing

### Effects Per Channel
- **Input Gain** - 0-2x adjustable gain staging
- **Overdrive** - Musical tube-style saturation
- **State Variable Filter** - Lowpass, Bandpass, or Highpass modes
  - Frequency: 20Hz - 20kHz
  - Resonance: 0-100%
- **Delay** - Up to 1 second with feedback control
- **Chorus** - Rich modulation with adjustable depth and rate

### Cross-Channel Features
- **Cross Modulation** - Channel 1 modulates Channel 2 filter (and vice versa)
- **Channel Bleed** - Mix signals between channels for creative routing
- **Stereo Width** - Mid-side processing for stereo field control
- **Master Reverb** - Lush stereo reverb with time and mix controls
- **Master Gain** - Final output level control

### Web Interface
- **Browser-Based Firmware Flasher** - Upload firmware via WebUSB DFU (no CLI tools needed!)
- **Real-time Parameter Control** - USB Serial communication for instant tweaking
- **Dual-Panel Interface** - Color-coded channels (teal/pink) with Tailwind CSS
- **24+ Controllable Parameters** - All effects adjustable from browser
- **No Installation Required** - Works entirely in Chrome, Edge, or Opera
- **Activity Logs** - Real-time feedback during flashing and connection

## 📂 Project Structure

```
dp/
├── firmware/
│   ├── DaisyGuitar.cpp    # Main Daisy Seed firmware
│   ├── Makefile           # Build configuration
│   └── build/             # Compiled binaries (.bin, .elf, .hex)
├── docs/
│   ├── index.html         # Web interface (Tailwind CSS)
│   ├── daisy-bridge.js    # Web Serial API for parameter control
│   ├── dfu.js             # WebUSB DFU flashing protocol
│   ├── flasher.js         # Firmware flasher logic
│   └── firmware/
│       └── DaisyGuitar.bin # Pre-compiled firmware (v2.1)
├── RELEASE_NOTES.md
├── LICENSE
└── README.md
```

## 🛠️ Getting Started

### Prerequisites

1. **Hardware:**
   - Daisy Seed development board
   - USB cable (data capable)
   - Two guitar inputs (1/4" jacks with appropriate buffer circuits)
   - Audio interface or amp for outputs

2. **Software:**
   - libDaisy and DaisySP libraries
   - ARM GCC toolchain
   - `dfu-util` for flashing
   - Modern web browser (Chrome, Edge, or Opera)

### Building the Firmware

1. **Clone and setup dependencies:**
   ```bash
   cd firmware
   # Ensure libDaisy and DaisySP are installed at ../../libDaisy and ../../DaisySP
   ```

2. **Build the firmware:**
   ```bash
   make clean
   make
   ```

3. **Flash to Daisy Seed:**
   - Hold BOOT button on Daisy
   - Press and release RESET button
   - Release BOOT button (Daisy enters DFU mode)
   - Run:
   ```bash
   make program-dfu
   ```

### Using the Web Interface

#### Quick Start (No Installation!)

**Option 1: Use Hosted Version**
1. Visit **https://willbearfruits.github.io/daisy-guitar-web/**
2. Go to **Firmware** tab
3. Put Daisy in DFU mode (Hold BOOT, press RESET, release both)
4. Click **"Connect to Daisy Seed"** → Select device from browser
5. Click **"Flash Firmware"** → Wait ~10 seconds
6. Press **RESET** on Daisy Seed
7. Switch to **Dashboard** tab
8. Click **"CONNECT"** → Select serial port
9. Tweak all 24+ parameters in real-time!

**Option 2: Local Development**
```bash
cd docs
python3 -m http.server 8000
# Open http://localhost:8000 in Chrome/Edge/Opera
```

#### Features Overview
- **Dashboard Tab:** Real-time parameter control via Web Serial
- **Firmware Tab:** Browser-based firmware flashing via WebUSB DFU
- **No Drivers Needed:** Works on Windows 10+, macOS, Linux
- **Activity Logs:** See all operations in real-time

## 🎛️ Parameter Reference

### Channel 1 & 2 Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| `ch1_gain` / `ch2_gain` | 0.0 - 2.0 | 1.0 | Input gain level |
| `ch1_drive` / `ch2_drive` | 0.0 - 1.0 | 0.0 | Overdrive amount |
| `ch1_filter_mode` / `ch2_filter_mode` | 0, 1, 2 | 0 | 0=LP, 1=BP, 2=HP |
| `ch1_filter_freq` / `ch2_filter_freq` | 20 - 20000 | 10000 | Filter cutoff (Hz) |
| `ch1_filter_res` / `ch2_filter_res` | 0.0 - 1.0 | 0.1 | Filter resonance |
| `ch1_delay_time` / `ch2_delay_time` | 0.0 - 1.0 | 0.0 | Delay time (seconds) |
| `ch1_delay_fb` / `ch2_delay_fb` | 0.0 - 0.95 | 0.0 | Delay feedback |
| `ch1_delay_mix` / `ch2_delay_mix` | 0.0 - 1.0 | 0.0 | Delay wet/dry mix |
| `ch1_chorus_depth` / `ch2_chorus_depth` | 0.0 - 1.0 | 0.0 | Chorus depth |
| `ch1_chorus_rate` / `ch2_chorus_rate` | 0.01 - 10.0 | 0.5 | Chorus LFO rate (Hz) |

### Master Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| `cross_mod` | 0.0 - 1.0 | 0.0 | Cross-channel filter modulation |
| `cross_bleed` | 0.0 - 1.0 | 0.0 | Channel mixing amount |
| `stereo_width` | 0.0 - 2.0 | 1.0 | Stereo field width |
| `reverb_time` | 0.0 - 1.0 | 0.5 | Reverb decay time |
| `reverb_mix` | 0.0 - 1.0 | 0.0 | Reverb wet/dry mix |
| `master_gain` | 0.0 - 2.0 | 1.0 | Final output level |

## 🔧 Hardware Connections

### Audio I/O (Daisy Seed)
- **Input 1:** Pin 16 (ADC 0) - Guitar input (requires buffer/preamp)
- **Input 2:** Pin 17 (ADC 1) - Second guitar input
- **Output 1:** Pin 18 (DAC 0) - Left/Channel 1 output
- **Output 2:** Pin 19 (DAC 1) - Right/Channel 2 output

### Recommended Input Circuit
For optimal guitar input impedance, use one of:
1. **Op-amp buffer** (TL072, OPA2134)
2. **LPB-1/LPB-2 style booster** circuit
3. **Commercial guitar preamp** (DI box with line out)

Guitar pickups need ~1MΩ input impedance, but Daisy's ADC inputs are much lower. A buffer circuit is essential for proper tone and loading.

## 🧪 Signal Flow

```
┌─────────────────────────────────────────────────────────┐
│                      Channel 1                          │
│  Guitar 1 → Gain → Drive → Filter* → Delay → Chorus    │
└────────────────────────────┬────────────────────────────┘
                             │
                    Cross Modulation
                             │
┌────────────────────────────┴────────────────────────────┐
│                      Channel 2                          │
│  Guitar 2 → Gain → Drive → Filter* → Delay → Chorus    │
└─────────────────────────────────────────────────────────┘
                             ↓
                    Channel Bleed Mix
                             ↓
                    Stereo Width Control
                             ↓
                      Master Reverb
                             ↓
                    Soft Clip + Master Gain
                             ↓
                      Stereo Output

* Filters can be modulated by opposite channel input signal
```

## 🚀 Performance

- **Sample Rate:** 48 kHz
- **Block Size:** 4 samples (ultra-low latency)
- **Processing:** Fixed-point and floating-point optimized DSP
- **Latency:** ~0.08ms per block (< 1ms total analog-to-analog)
- **Memory Usage:**
  - FLASH: 91,224 bytes (69.60% of 128KB)
  - SRAM: 447,148 bytes (85.29% of 512KB)
- **USB Serial:** Event-driven callback processing

## 💡 Creative Ideas

### Cross-Modulation Experiments
- Set `cross_mod` to 0.5+ and play different rhythms on each guitar
- Channel 1's dynamics will sweep Channel 2's filter (and vice versa)
- Creates complex, evolving tones

### Ping-Pong Delays
- Set different delay times on each channel
- Add `cross_bleed` to route delays between channels
- Adjust `stereo_width` for spatial effects

### Dual Amp Simulation
- Different filter modes per channel (LP on Ch1, HP on Ch2)
- Different drive amounts for clean/dirty blend
- Use `cross_bleed` to mix to taste

## 📝 Protocol Reference

The firmware accepts ASCII commands over USB Serial:
```
<parameter_name>:<float_value>;\n
```

Examples:
```
ch1_gain:1.5;
ch2_drive:0.8;
cross_mod:0.3;
reverb_mix:0.25;
```

## 🔍 Troubleshooting

**GUI won't connect:**
- Ensure you're using Chrome, Edge, or Opera
- Check that Daisy is powered and in normal run mode (not DFU)
- Try unplugging and reconnecting USB

**No audio output:**
- Verify audio connections
- Check input gain levels aren't at 0
- Ensure master gain isn't at 0
- Verify Daisy is running firmware (LED should blink if configured)

**Distorted output:**
- Reduce input gain
- Lower drive amount
- Reduce master gain
- Check for clipping at input stage

## 📄 License

Open source project. Use, modify, and share freely.

## 🙏 Credits

- Built with [libDaisy](https://github.com/electro-smith/libDaisy) and [DaisySP](https://github.com/electro-smith/DaisySP)
- Developed for the Electro-Smith Daisy Seed platform
- Web Serial API for browser-based control
