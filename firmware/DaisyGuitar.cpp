#include "daisy_seed.h"
#include "daisysp.h"
#include <stdio.h>
#include <string.h>

using namespace daisy;
using namespace daisysp;

// --- HARDWARE DECLARATION ---
DaisySeed hw;

// --- GLOBAL VARIABLES ---
float gain_level = 0.5f; // Default gain (controlled via USB)
char serial_buf[64];     // Buffer for incoming USB serial commands
int buf_pos = 0;         // Current position in buffer

/**
 * Audio Callback
 * Handles real-time audio processing.
 * 
 * HARDWARE CONNECTIONS:
 * Input:  Daisy Seed "Audio In" pins (Pin 16 & 17 usually)
 * Output: Daisy Seed "Audio Out" pins (Pin 18 & 19 usually)
 */
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        // Get Input Samples (Left and Right)
        float left_in = in[0][i];
        float right_in = in[1][i];

        // Apply Gain (Volume Control)
        // This is a simple multiplication. 
        // 1.0 = Unity Gain, 0.5 = Half Volume, 2.0 = Double Volume
        out[0][i] = left_in * gain_level;
        out[1][i] = right_in * gain_level;
    }
}

/**
 * Serial Command Parser
 * Reads commands from USB and updates global variables.
 * Format expected: "gain:0.5;\n"
 */
void ProcessSerial()
{
    // Check if data is available on USB Serial
    if(hw.usb_serial.Readable())
    {
        // Read one character
        char c = hw.usb_serial.GetChar();

        // Check for end of command (newline or semicolon)
        if(c == '\n' || c == ';')
        {
            // Null-terminate the string so we can read it
            serial_buf[buf_pos] = '\0';

            // Parse the command
            float new_val;
            // sscanf looks for the pattern "gain:%f" inside serial_buf
            if(sscanf(serial_buf, "gain:%f", &new_val) == 1)
            {
                // Clamp value between 0.0 and 2.0 for safety
                if(new_val < 0.0f) new_val = 0.0f;
                if(new_val > 2.0f) new_val = 2.0f;
                
                gain_level = new_val;
            }

            // Reset buffer for next command
            buf_pos = 0;
        }
        else
        {
            // Add character to buffer if there is space
            if(buf_pos < 63)
            {
                serial_buf[buf_pos++] = c;
            }
        }
    }
}

int main(void)
{
    // 1. Initialize Daisy Seed Hardware
    hw.Init();

    // 2. Configure Audio
    hw.SetAudioBlockSize(4); // Low latency (4 samples per block)
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

    // 3. Initialize USB Serial for WebGUI
    hw.usb_serial.Init();

    // 4. Start Audio Callback
    hw.StartAudio(AudioCallback);

    // 5. Main Loop (Non-Audio Tasks)
    while(1)
    {
        // Handle USB Serial communication
        ProcessSerial();
        
        // Small delay to prevent locking up the CPU with empty loops
        // (Audio happens in the background interrupt)
        System::Delay(1);
    }
}
