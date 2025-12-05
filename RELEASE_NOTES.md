# DP v2.1 - Full USB Serial Support

## Release Date: 2025-12-05

### 🎉 Complete Feature Restoration

USB Serial communication is now fully operational! Control all 24+ parameters in real-time from your browser.

---

## 🆕 What's New in v2.1

### Major Updates

**✅ USB Serial Communication Restored**
- Web Serial API fully functional for parameter control
- Real-time parameter updates from browser Dashboard
- Automatic device enumeration (no drivers needed on modern OS)
- Universal compatibility with all Daisy Seed boards

**📊 Memory Optimization**
- FLASH: 91,224 bytes (69.60%)
- SRAM: 447,148 bytes (85.29%)
- USB stack adds ~18KB, well within limits

**🔧 Implementation Details**
- USB CDC receive callback for non-blocking serial processing
- Buffer overflow protection maintained
- Event-driven parameter updates
- 100ms USB enumeration delay for stability

---

## 🔄 Upgrade from v2.0

**All users should upgrade to v2.1** for full functionality:
1. Flash the new firmware via Web Flasher or CLI
2. Reset Daisy Seed after flashing
3. Connect via Dashboard tab for real-time control

v2.0 users: Your audio processing works, but parameter control was disabled. v2.1 restores it!

---

# DP v2.0 - Dual Processing Guitar FX

## Release Date: 2025-12-05 (Superseded by v2.1)

### 🎉 Production-Ready Release

The DP (Dual Processing) guitar effects processor is now production-ready with professional-grade firmware, modern web interface, and comprehensive security fixes.

---

## 🆕 What's New in v2.0

### Major Features

**Dual-Channel Architecture:**
- 2 independent guitar input channels
- 2 stereo outputs
- Complete effect chains per channel
- Cross-channel modulation and routing

**Effects Per Channel:**
- Input Gain (0-2x)
- Overdrive/Distortion
- State Variable Filter (Lowpass/Bandpass/Highpass)
  - Cutoff: 20Hz - 20kHz
  - Resonance control
- Delay (up to 1 second with feedback)
- Chorus (adjustable depth and rate)

**Cross-Channel Features:**
- Cross-modulation (Ch1 modulates Ch2 filter and vice versa)
- Channel bleed mixing
- Stereo width control (mid-side processing)
- Master gain with soft-clipping

**Modern Web Interface:**
- Tailwind CSS dark theme with teal/pink accents
- Real-time connection status with pulsing indicator
- Toast notifications for all events
- Automatic reconnection (up to 3 attempts with exponential backoff)
- Firmware flasher tab with step-by-step instructions
- 24+ controllable parameters

---

## 🔒 Security & Stability

### Critical Fixes
✅ Buffer overflow protection in serial parser
✅ NaN/Inf validation in audio callback
✅ Width-specified sscanf (prevents stack overflow)
✅ Named constants (eliminated magic numbers)
✅ Input sanitization and validation

### Enhanced Error Handling
- Automatic connection recovery
- Heartbeat monitoring
- Event-driven architecture
- Comprehensive error messages
- Graceful degradation

---

## 📊 Performance

- **Sample Rate:** 48 kHz
- **Block Size:** 4 samples (ultra-low latency)
- **Latency:** ~0.08ms per block
- **Memory Usage (v2.1):**
  - FLASH: 91,224 bytes (69.60%)
  - SRAM: 447,148 bytes (85.29%)
- **Memory Usage (v2.0 - deprecated):**
  - FLASH: 72,852 bytes (55.58%)
  - SRAM: 435,356 bytes (83.04%)

---

## 📦 Installation

### Requirements
- Daisy Seed development board
- ARM GCC Toolchain
- libDaisy and DaisySP libraries
- dfu-util for flashing
- Chrome, Edge, or Opera browser (for Web Serial API)

### Quick Start

1. **Flash Firmware:**
   ```bash
   # Put Daisy in DFU mode (Hold BOOT, press RESET, release both)
   dfu-util -a 0 -s 0x08000000:leave -D DaisyGuitar.bin
   ```

2. **Access Web Interface:**
   Visit: https://willbearfruits.github.io/daisy-guitar-web/

3. **Connect & Play:**
   - Click "CONNECT" button
   - Select Daisy Seed from port list
   - Tweak 24+ parameters in real-time

---

## 🎛️ Parameters

### Channel Controls (x2)
| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Gain | 0.0 - 2.0 | 1.0 | Input level |
| Drive | 0.0 - 1.0 | 0.0 | Overdrive amount |
| Filter Mode | 0, 1, 2 | 0 | LP / BP / HP |
| Filter Cutoff | 20 - 20000 Hz | 10000 | Filter frequency |
| Resonance | 0.0 - 1.0 | 0.1 | Filter Q |
| Delay Time | 0.0 - 1.0 s | 0.0 | Delay length |
| Delay Feedback | 0.0 - 0.95 | 0.0 | Delay repeats |
| Delay Mix | 0.0 - 1.0 | 0.0 | Wet/dry blend |
| Chorus Depth | 0.0 - 1.0 | 0.0 | Modulation amount |
| Chorus Rate | 0.01 - 10 Hz | 0.5 | LFO speed |

### Master Controls
| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Cross Modulation | 0.0 - 1.0 | 0.0 | Filter cross-mod |
| Channel Bleed | 0.0 - 1.0 | 0.0 | Channel mixing |
| Stereo Width | 0.0 - 2.0 | 1.0 | Stereo field |
| Master Gain | 0.0 - 2.0 | 1.0 | Final output |

---

## 🔧 Known Issues (v2.0 Only - Fixed in v2.1)

- ~~**USB Serial disabled** in this build for compatibility~~ **FIXED in v2.1**
- **Reverb simplified** due to API compatibility
  - Full reverb will return in future version with proper libDaisy integration

---

## 🚀 Future Roadmap (v2.2+)

- 🔄 Add proper ReverbSc integration
- 🔄 MIDI input support
- 🔄 Preset saving/loading to QSPI flash
- 🔄 Visual waveform display
- 🔄 Advanced modulation routing

---

## 📝 Technical Details

**Build Information:**
- Compiler: ARM GCC (arm-none-eabi-g++)
- Optimization: -O2
- Target: STM32H750IB (Cortex-M7)
- Toolchain: GNU ARM Embedded

**Dependencies:**
- libDaisy (Electro-Smith)
- DaisySP (Electro-Smith)
- CMSIS DSP Library

---

## 🙏 Credits

- **Platform:** Electro-Smith Daisy Seed
- **DSP Library:** DaisySP
- **Hardware Library:** libDaisy
- **UI Framework:** Tailwind CSS
- **Web API:** Web Serial API

---

## 📄 License

MIT License - See LICENSE file for details

---

## 🐛 Bug Reports

Report issues at: https://github.com/willbearfruits/daisy-guitar-web/issues

---

## 🎸 Happy Playing!

Transform your guitar sound with professional dual-channel processing.
