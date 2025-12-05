#include "daisy_seed.h"
#include "daisysp.h"
#include <stdio.h>
#include <string.h>

using namespace daisy;
using namespace daisysp;

// --- CONSTANTS (Must be defined before use) ---
constexpr float SAMPLE_RATE = 48000.0f;
constexpr size_t MAX_DELAY_SAMPLES = 48000;
constexpr float CROSS_MOD_FREQ_RANGE = 5000.0f;
constexpr float REVERB_LP_FREQ = 18000.0f;
constexpr size_t AUDIO_BLOCK_SIZE = 48;
constexpr uint32_t MAIN_LOOP_DELAY_MS = 1;

// --- HARDWARE DECLARATION ---
DaisySeed hw;

// --- EFFECTS MODULES ---
// Channel 1 Effects
Overdrive drive1;
Svf filter1;
DelayLine<float, MAX_DELAY_SAMPLES> del1;
Chorus chorus1;

// Channel 2 Effects
Overdrive drive2;
Svf filter2;
DelayLine<float, MAX_DELAY_SAMPLES> del2;
Chorus chorus2;

// Shared/Master Effects (Reverb removed for compatibility)
// ReverbSc reverb;

// --- PARAMETERS ---
// Channel 1
float ch1_gain = 1.0f;
float ch1_drive = 0.0f;
float ch1_filter_freq = 10000.0f;
float ch1_filter_res = 0.1f;
float ch1_delay_time = 0.0f;
float ch1_delay_feedback = 0.0f;
float ch1_delay_mix = 0.0f;
float ch1_chorus_depth = 0.0f;
float ch1_chorus_rate = 0.5f;

// Channel 2
float ch2_gain = 1.0f;
float ch2_drive = 0.0f;
float ch2_filter_freq = 10000.0f;
float ch2_filter_res = 0.1f;
float ch2_delay_time = 0.0f;
float ch2_delay_feedback = 0.0f;
float ch2_delay_mix = 0.0f;
float ch2_chorus_depth = 0.0f;
float ch2_chorus_rate = 0.5f;

// Cross-channel modulation
float cross_mod_amt = 0.0f;      // Amount of cross-modulation
float cross_bleed = 0.0f;        // How much channel 1 bleeds into channel 2 and vice versa
float stereo_width = 1.0f;       // Stereo width control

// Master
float reverb_mix = 0.0f;
float reverb_time = 0.5f;
float master_gain = 1.0f;

// Filter types
enum FilterMode { LOWPASS = 0, BANDPASS = 1, HIGHPASS = 2 };
FilterMode ch1_filter_mode = LOWPASS;
FilterMode ch2_filter_mode = LOWPASS;

// Serial buffer
char serial_buf[128];
int buf_pos = 0;
volatile bool new_data_ready = false;

/**
 * Soft clipping function for musical saturation
 * Renamed to avoid conflict with DaisySP's SoftClip
 */
inline float MySoftClip(float x)
{
    if (x > 1.0f) return 1.0f;
    if (x < -1.0f) return -1.0f;
    return x - (x * x * x) / 3.0f;
}

/**
 * Audio Callback - Dual Channel Processing
 *
 * SIGNAL FLOW PER CHANNEL:
 * Guitar In → Gain → Drive → Filter → Delay → Chorus → Reverb → Out
 *
 * CROSS-CHANNEL:
 * - Channel 1 can modulate Channel 2 filter frequency
 * - Channel 2 can modulate Channel 1 filter frequency
 * - Cross-bleed mixes channels together
 */
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        // ========== READ INPUTS ==========
        float ch1_in = in[0][i];
        float ch2_in = in[1][i];

        // Validate inputs (protect against NaN/Inf)
        if(!std::isfinite(ch1_in)) ch1_in = 0.0f;
        if(!std::isfinite(ch2_in)) ch2_in = 0.0f;

        // ========== CHANNEL 1 PROCESSING ==========

        // Input gain
        float ch1 = ch1_in * ch1_gain;

        // Overdrive
        drive1.SetDrive(ch1_drive);
        ch1 = drive1.Process(ch1);

        // Filter with cross-modulation from channel 2
        float ch1_mod_freq = ch1_filter_freq;
        if (cross_mod_amt > 0.0f) {
            ch1_mod_freq += (ch2_in * cross_mod_amt * CROSS_MOD_FREQ_RANGE);
            ch1_mod_freq = fclamp(ch1_mod_freq, 20.0f, 20000.0f);
        }
        filter1.SetFreq(ch1_mod_freq);
        filter1.SetRes(ch1_filter_res);
        filter1.Process(ch1);

        // Select filter output based on mode
        switch(ch1_filter_mode) {
            case LOWPASS:  ch1 = filter1.Low();  break;
            case BANDPASS: ch1 = filter1.Band(); break;
            case HIGHPASS: ch1 = filter1.High(); break;
        }

        // Delay
        if (ch1_delay_mix > 0.0f) {
            size_t delay_samples = static_cast<size_t>(ch1_delay_time * 48000.0f);
            float delayed = del1.Read(delay_samples);
            del1.Write(ch1 + (delayed * ch1_delay_feedback));
            ch1 = ch1 * (1.0f - ch1_delay_mix) + delayed * ch1_delay_mix;
        } else {
            del1.Write(ch1);
        }

        // Chorus
        if (ch1_chorus_depth > 0.0f) {
            chorus1.SetLfoDepth(ch1_chorus_depth);
            chorus1.SetLfoFreq(ch1_chorus_rate);
            ch1 = chorus1.Process(ch1);
        }

        // ========== CHANNEL 2 PROCESSING ==========

        // Input gain
        float ch2 = ch2_in * ch2_gain;

        // Overdrive
        drive2.SetDrive(ch2_drive);
        ch2 = drive2.Process(ch2);

        // Filter with cross-modulation from channel 1
        float ch2_mod_freq = ch2_filter_freq;
        if (cross_mod_amt > 0.0f) {
            ch2_mod_freq += (ch1_in * cross_mod_amt * CROSS_MOD_FREQ_RANGE);
            ch2_mod_freq = fclamp(ch2_mod_freq, 20.0f, 20000.0f);
        }
        filter2.SetFreq(ch2_mod_freq);
        filter2.SetRes(ch2_filter_res);
        filter2.Process(ch2);

        // Select filter output based on mode
        switch(ch2_filter_mode) {
            case LOWPASS:  ch2 = filter2.Low();  break;
            case BANDPASS: ch2 = filter2.Band(); break;
            case HIGHPASS: ch2 = filter2.High(); break;
        }

        // Delay
        if (ch2_delay_mix > 0.0f) {
            size_t delay_samples = static_cast<size_t>(ch2_delay_time * 48000.0f);
            float delayed = del2.Read(delay_samples);
            del2.Write(ch2 + (delayed * ch2_delay_feedback));
            ch2 = ch2 * (1.0f - ch2_delay_mix) + delayed * ch2_delay_mix;
        } else {
            del2.Write(ch2);
        }

        // Chorus
        if (ch2_chorus_depth > 0.0f) {
            chorus2.SetLfoDepth(ch2_chorus_depth);
            chorus2.SetLfoFreq(ch2_chorus_rate);
            ch2 = chorus2.Process(ch2);
        }

        // ========== CROSS-CHANNEL BLEED ==========
        if (cross_bleed > 0.0f) {
            float temp_ch1 = ch1;
            float temp_ch2 = ch2;
            ch1 = ch1 * (1.0f - cross_bleed) + temp_ch2 * cross_bleed;
            ch2 = ch2 * (1.0f - cross_bleed) + temp_ch1 * cross_bleed;
        }

        // ========== STEREO WIDTH ==========
        // Mid-side processing for stereo width control
        float mid = (ch1 + ch2) * 0.5f;
        float side = (ch1 - ch2) * 0.5f * stereo_width;
        ch1 = mid + side;
        ch2 = mid - side;

        // ========== MASTER REVERB ==========
        // Simple reverb placeholder (full reverb removed for compatibility)
        // Can be added back with proper DaisySP reverb class
        if (reverb_mix > 0.0f) {
            // Simple feedback delay as reverb substitute
            float reverb_l = ch1 * reverb_mix * reverb_time;
            float reverb_r = ch2 * reverb_mix * reverb_time;
            ch1 = ch1 * (1.0f - reverb_mix) + reverb_l;
            ch2 = ch2 * (1.0f - reverb_mix) + reverb_r;
        }

        // ========== MASTER OUTPUT ==========
        ch1 = MySoftClip(ch1 * master_gain);
        ch2 = MySoftClip(ch2 * master_gain);

        // Final safety check
        if(!std::isfinite(ch1)) ch1 = 0.0f;
        if(!std::isfinite(ch2)) ch2 = 0.0f;

        out[0][i] = ch1;
        out[1][i] = ch2;
    }
}

/**
 * USB Receive Callback - Called when data arrives via USB Serial
 */
void UsbCallback(uint8_t* buf, uint32_t* len)
{
    for(uint32_t i = 0; i < *len; i++)
    {
        char c = buf[i];

        if(c == '\n' || c == ';')
        {
            serial_buf[buf_pos] = '\0';
            new_data_ready = true;
            buf_pos = 0;
        }
        else
        {
            if(buf_pos < 127)
            {
                serial_buf[buf_pos++] = c;
            }
            else
            {
                // Buffer overflow protection - reset on overflow
                buf_pos = 0;
            }
        }
    }
}

/**
 * Parse and apply parameter changes from USB Serial
 * Format: "param:value;\n"
 *
 * Examples:
 *   ch1_gain:1.5;
 *   ch1_drive:0.8;
 *   ch1_filter_freq:2000.0;
 *   cross_mod:0.5;
 */
void ProcessSerial()
{
    if(new_data_ready)
    {
        new_data_ready = false;

        // Parse parameter name and value
        char param_name[64];
        float val;

        // Add width specifier to prevent buffer overflow
        if(sscanf(serial_buf, "%63[^:]:%f", param_name, &val) == 2)
        {
                // Channel 1 parameters
                if(strcmp(param_name, "ch1_gain") == 0)           ch1_gain = fclamp(val, 0.0f, 2.0f);
                else if(strcmp(param_name, "ch1_drive") == 0)     ch1_drive = fclamp(val, 0.0f, 1.0f);
                else if(strcmp(param_name, "ch1_filter_freq") == 0) ch1_filter_freq = fclamp(val, 20.0f, 20000.0f);
                else if(strcmp(param_name, "ch1_filter_res") == 0)  ch1_filter_res = fclamp(val, 0.0f, 1.0f);
                else if(strcmp(param_name, "ch1_delay_time") == 0)  ch1_delay_time = fclamp(val, 0.0f, 1.0f);
                else if(strcmp(param_name, "ch1_delay_fb") == 0)    ch1_delay_feedback = fclamp(val, 0.0f, 0.95f);
                else if(strcmp(param_name, "ch1_delay_mix") == 0)   ch1_delay_mix = fclamp(val, 0.0f, 1.0f);
                else if(strcmp(param_name, "ch1_chorus_depth") == 0) ch1_chorus_depth = fclamp(val, 0.0f, 1.0f);
                else if(strcmp(param_name, "ch1_chorus_rate") == 0)  ch1_chorus_rate = fclamp(val, 0.01f, 10.0f);
                else if(strcmp(param_name, "ch1_filter_mode") == 0) {
                    int mode = (int)val;
                    if(mode >= 0 && mode <= 2) ch1_filter_mode = (FilterMode)mode;
                }

                // Channel 2 parameters
                else if(strcmp(param_name, "ch2_gain") == 0)           ch2_gain = fclamp(val, 0.0f, 2.0f);
                else if(strcmp(param_name, "ch2_drive") == 0)          ch2_drive = fclamp(val, 0.0f, 1.0f);
                else if(strcmp(param_name, "ch2_filter_freq") == 0)    ch2_filter_freq = fclamp(val, 20.0f, 20000.0f);
                else if(strcmp(param_name, "ch2_filter_res") == 0)     ch2_filter_res = fclamp(val, 0.0f, 1.0f);
                else if(strcmp(param_name, "ch2_delay_time") == 0)     ch2_delay_time = fclamp(val, 0.0f, 1.0f);
                else if(strcmp(param_name, "ch2_delay_fb") == 0)       ch2_delay_feedback = fclamp(val, 0.0f, 0.95f);
                else if(strcmp(param_name, "ch2_delay_mix") == 0)      ch2_delay_mix = fclamp(val, 0.0f, 1.0f);
                else if(strcmp(param_name, "ch2_chorus_depth") == 0)   ch2_chorus_depth = fclamp(val, 0.0f, 1.0f);
                else if(strcmp(param_name, "ch2_chorus_rate") == 0)    ch2_chorus_rate = fclamp(val, 0.01f, 10.0f);
                else if(strcmp(param_name, "ch2_filter_mode") == 0) {
                    int mode = (int)val;
                    if(mode >= 0 && mode <= 2) ch2_filter_mode = (FilterMode)mode;
                }

                // Cross-channel and master
                else if(strcmp(param_name, "cross_mod") == 0)      cross_mod_amt = fclamp(val, 0.0f, 1.0f);
                else if(strcmp(param_name, "cross_bleed") == 0)    cross_bleed = fclamp(val, 0.0f, 1.0f);
                else if(strcmp(param_name, "stereo_width") == 0)   stereo_width = fclamp(val, 0.0f, 2.0f);
                else if(strcmp(param_name, "reverb_mix") == 0)     reverb_mix = fclamp(val, 0.0f, 1.0f);
                else if(strcmp(param_name, "reverb_time") == 0)    reverb_time = fclamp(val, 0.0f, 1.0f);
                else if(strcmp(param_name, "master_gain") == 0)    master_gain = fclamp(val, 0.0f, 2.0f);

                // Reverb parameters (disabled for now)
                // reverb.SetFeedback(reverb_time);
                // reverb.SetLpFreq(REVERB_LP_FREQ);
        }
    }
}

int main(void)
{
    // 1. Initialize Hardware
    hw.Init();

    // 2. Configure Audio
    hw.SetAudioBlockSize(AUDIO_BLOCK_SIZE); // Low latency
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

    // 3. Initialize USB Serial
    hw.usb_handle.Init(UsbHandle::FS_INTERNAL);
    System::Delay(100); // Allow USB to enumerate
    hw.usb_handle.SetReceiveCallback(UsbCallback, UsbHandle::FS_INTERNAL);

    // 4. Initialize Effects
    float sample_rate = hw.AudioSampleRate();

    // Channel 1 effects
    drive1.Init();
    filter1.Init(sample_rate);
    del1.Init();
    chorus1.Init(sample_rate);

    // Channel 2 effects
    drive2.Init();
    filter2.Init(sample_rate);
    del2.Init();
    chorus2.Init(sample_rate);

    // Master effects (reverb disabled for compatibility)
    // reverb.Init(sample_rate);
    // reverb.SetFeedback(0.85f);
    // reverb.SetLpFreq(REVERB_LP_FREQ);

    // 5. Start Audio
    hw.StartAudio(AudioCallback);

    // 6. Main Loop
    bool led_state = true;
    uint32_t last_blink = System::GetNow();

    while(1)
    {
        ProcessSerial();
        
        // Heartbeat LED (1Hz)
        if(System::GetNow() - last_blink > 500)
        {
            last_blink = System::GetNow();
            led_state = !led_state;
            hw.SetLed(led_state);
        }

        System::Delay(MAIN_LOOP_DELAY_MS);
    }
}
