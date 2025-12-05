/**
 * DaisyBridge - Connects your web app to Daisy Seed via USB Serial
 * Handles bidirectional communication for real-time parameter control
 */
export class DaisyBridge {
    constructor() {
        this.port = null;
        this.writer = null;
        this.reader = null;
        this.isConnected = false;
    }

    /**
     * Connect to Daisy Seed via Web Serial API
     * @returns {boolean} Success status
     */
    async connect() {
        if (!("serial" in navigator)) {
            console.error("Web Serial is not supported in this browser.");
            alert("Web Serial API not supported. Use Chrome, Edge, or Opera.");
            return false;
        }

        try {
            // Request port access
            this.port = await navigator.serial.requestPort();

            // Open with standard baud rate
            await this.port.open({ baudRate: 9600 });

            // Set up text encoder for writing
            const textEncoder = new TextEncoderStream();
            const writableStreamClosed = textEncoder.readable.pipeTo(this.port.writable);
            this.writer = textEncoder.writable.getWriter();

            this.isConnected = true;
            console.log("✓ Connected to Daisy Seed");
            return true;

        } catch (err) {
            console.error("Connection failed:", err);
            alert(`Failed to connect: ${err.message}`);
            return false;
        }
    }

    /**
     * Send a parameter update to Daisy
     * @param {string} paramName - Parameter name (e.g., "ch1_gain")
     * @param {number} value - Parameter value
     */
    async sendParam(paramName, value) {
        if (!this.isConnected || !this.writer) {
            console.warn("Not connected to Daisy");
            return;
        }

        try {
            const message = `${paramName}:${value.toFixed(4)};\n`;
            await this.writer.write(message);
            // console.log(`Sent: ${message.trim()}`);
        } catch (err) {
            console.error("Write failed:", err);
        }
    }

    /**
     * Legacy method for backwards compatibility
     * @param {number} value - Gain value
     */
    async setGain(value) {
        await this.sendParam('ch1_gain', value);
    }

    /**
     * Disconnect from Daisy
     */
    async disconnect() {
        if (this.writer) {
            try {
                await this.writer.close();
            } catch (err) {
                console.error("Error closing writer:", err);
            }
        }

        if (this.port) {
            try {
                await this.port.close();
            } catch (err) {
                console.error("Error closing port:", err);
            }
        }

        this.isConnected = false;
        this.writer = null;
        this.reader = null;
        this.port = null;
        console.log("✓ Disconnected from Daisy");
    }
}
