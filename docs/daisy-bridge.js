/**
 * DaisyBridge - Production-Ready USB Serial Communication for Daisy Seed
 * Version: 2.0
 *
 * Features:
 * - Automatic connection recovery with exponential backoff
 * - Heartbeat mechanism to detect disconnections
 * - Event-driven architecture for UI updates
 * - Comprehensive error handling
 */
export class DaisyBridge {
    constructor() {
        this.port = null;
        this.writer = null;
        this.reader = null;
        this.isConnected = false;

        // Connection recovery
        this.heartbeatInterval = null;
        this.reconnectAttempts = 0;
        this.maxReconnectAttempts = 3;
        this.reconnectDelay = 1000; // Start with 1 second

        // Statistics
        this.stats = {
            messagesSent: 0,
            messagesFailed: 0,
            lastError: null
        };
    }

    /**
     * Connect to Daisy Seed via Web Serial API
     * @param {number} baudRate - Baud rate (default: 9600)
     * @returns {Promise<boolean>} Success status
     */
    async connect(baudRate = 9600) {
        if (!("serial" in navigator)) {
            const error = "Web Serial API not supported. Use Chrome 89+, Edge 89+, or Opera 75+.";
            console.error(error);
            this.emitEvent('error', { message: error, critical: true });
            return false;
        }

        try {
            // Request port access
            this.port = await navigator.serial.requestPort();

            // Open with specified baud rate
            await this.port.open({ baudRate });

            // Set up text encoder for writing
            const textEncoder = new TextEncoderStream();
            const writableStreamClosed = textEncoder.readable.pipeTo(this.port.writable);
            this.writer = textEncoder.writable.getWriter();

            this.isConnected = true;
            this.reconnectAttempts = 0;
            this.reconnectDelay = 1000;

            console.log(`✓ Connected to Daisy Seed at ${baudRate} baud`);
            this.emitEvent('connected', { baudRate });

            // Start heartbeat monitoring
            this.startHeartbeat();

            return true;

        } catch (err) {
            console.error("Connection failed:", err);
            this.stats.lastError = err.message;
            this.emitEvent('error', { message: `Connection failed: ${err.message}`, error: err });
            return false;
        }
    }

    /**
     * Send a parameter update to Daisy
     * @param {string} paramName - Parameter name (e.g., "ch1_gain")
     * @param {number} value - Parameter value
     * @returns {Promise<boolean>} Success status
     */
    async sendParam(paramName, value) {
        if (!this.isConnected || !this.writer) {
            console.warn("Not connected to Daisy");
            this.emitEvent('error', { message: "Device not connected", severity: 'warning' });
            return false;
        }

        // Validate input
        if (!isFinite(value)) {
            console.error("Invalid parameter value:", value);
            this.stats.messagesFailed++;
            return false;
        }

        try {
            const message = `${paramName}:${value.toFixed(4)};\n`;
            await this.writer.write(message);
            this.stats.messagesSent++;
            return true;

        } catch (err) {
            console.error("Write failed:", err);
            this.stats.messagesFailed++;
            this.stats.lastError = err.message;
            this.handleDisconnect();
            this.emitEvent('error', { message: "Failed to send parameter", error: err });
            return false;
        }
    }

    /**
     * Start heartbeat monitoring to detect disconnections
     */
    startHeartbeat() {
        // Clear any existing heartbeat
        if (this.heartbeatInterval) {
            clearInterval(this.heartbeatInterval);
        }

        this.heartbeatInterval = setInterval(async () => {
            if (!this.isConnected || !this.port) {
                clearInterval(this.heartbeatInterval);
                return;
            }

            try {
                // Try to get port info to verify connection
                const info = this.port.getInfo();
                // If we can get info, connection is alive
            } catch (err) {
                console.error("Heartbeat failed - connection lost");
                this.handleDisconnect();
            }
        }, 5000); // Check every 5 seconds
    }

    /**
     * Handle unexpected disconnection
     */
    handleDisconnect() {
        console.warn("Connection lost");
        clearInterval(this.heartbeatInterval);
        this.isConnected = false;

        this.emitEvent('disconnected', {
            reconnectAttempts: this.reconnectAttempts,
            maxAttempts: this.maxReconnectAttempts
        });

        // Attempt automatic reconnection if under retry limit
        if (this.reconnectAttempts < this.maxReconnectAttempts) {
            this.attemptReconnect();
        } else {
            this.emitEvent('reconnect-failed', {
                message: 'Maximum reconnection attempts reached'
            });
        }
    }

    /**
     * Attempt to reconnect with exponential backoff
     */
    async attemptReconnect() {
        this.reconnectAttempts++;
        const delay = this.reconnectDelay * Math.pow(2, this.reconnectAttempts - 1);

        console.log(`Reconnecting in ${delay}ms (attempt ${this.reconnectAttempts}/${this.maxReconnectAttempts})...`);

        this.emitEvent('reconnecting', {
            attempt: this.reconnectAttempts,
            delay: delay
        });

        setTimeout(async () => {
            try {
                // Try to reopen the same port
                if (this.port) {
                    await this.port.open({ baudRate: 9600 });

                    const textEncoder = new TextEncoderStream();
                    textEncoder.readable.pipeTo(this.port.writable);
                    this.writer = textEncoder.writable.getWriter();

                    this.isConnected = true;
                    this.reconnectAttempts = 0;

                    console.log("✓ Reconnected successfully");
                    this.emitEvent('reconnected', {});
                    this.startHeartbeat();
                } else {
                    throw new Error("Port no longer available");
                }
            } catch (err) {
                console.error("Reconnection attempt failed:", err);
                if (this.reconnectAttempts < this.maxReconnectAttempts) {
                    this.attemptReconnect();
                } else {
                    this.emitEvent('reconnect-failed', {
                        message: 'All reconnection attempts failed'
                    });
                }
            }
        }, delay);
    }

    /**
     * Disconnect from Daisy
     */
    async disconnect() {
        clearInterval(this.heartbeatInterval);

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
        this.emitEvent('disconnected', { manual: true });
    }

    /**
     * Emit custom events for UI updates
     */
    emitEvent(eventName, detail) {
        window.dispatchEvent(new CustomEvent(`daisy-${eventName}`, { detail }));
    }

    /**
     * Get connection statistics
     */
    getStats() {
        return { ...this.stats };
    }

    /**
     * Legacy method for backwards compatibility
     * @param {number} value - Gain value
     */
    async setGain(value) {
        return await this.sendParam('ch1_gain', value);
    }
}
