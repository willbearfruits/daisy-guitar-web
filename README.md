# Daisy Guitar Project

A real-time, web-controlled guitar effects processor for the [Electro-Smith Daisy Seed](https://www.electro-smith.com/daisy/daisy-seed).

**Features:**
- 🎸 **High-Z Input Support:** designed for use with a simple Op-Amp buffer or LPB-2 booster.
- 🎛️ **Web GUI:** Control gain and parameters in real-time via Web Serial (USB).
- 🚀 **Zero-Cost Hosting:** GUI runs entirely on GitHub Pages.

## 📂 Structure
- **`firmware/`**: C++ Source code for the Daisy Seed.
- **`docs/`**: The Web Interface (HTML/JS). Served via GitHub Pages.

## 🛠️ Getting Started

### 1. Build the Hardware
Connect your guitar input to the Daisy Seed (Pin 16) using a buffer or booster (see [Hardware Guide](hardware_guide.md)).

### 2. Flash the Firmware
1. Navigate to the firmware folder:
   ```bash
   cd firmware
   ```
2. Put Daisy in DFU Mode (Hold BOOT, Press RESET).
3. Compile and Flash:
   ```bash
   make clean
   make program-dfu
   ```

### 3. Control via Web
1. Go to the [Live GUI](https://glitches.github.io/dp/docs/) (Link will work after you enable GitHub Pages).
2. Connect USB.
3. Rock out.