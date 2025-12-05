/**
 * DaisyBridge - Connects your web app to Daisy Seed via USB
 */
export class DaisyBridge {
    constructor() {
        this.port = null;
        this.writer = null;
        this.isConnected = false;
    }

    async connect() {
        if (!("serial" in navigator)) {
            console.error("Web Serial is not supported.");
            return false;
        }
        try {
            this.port = await navigator.serial.requestPort();
            await this.port.open({ baudRate: 9600 });
            const textEncoder = new TextEncoderStream();
            const writableStreamClosed = textEncoder.readable.pipeTo(this.port.writable);
            this.writer = textEncoder.writable.getWriter();
            this.isConnected = true;
            return true;
        } catch (err) {
            console.error("Connection Failed", err);
            return false;
        }
    }

    async setGain(value) {
        if (!this.isConnected || !this.writer) return;
        const cleanValue = parseFloat(value).toFixed(2);
        try {
            await this.writer.write(`gain:${cleanValue};\n`);
        } catch (err) {
            console.error("Write Failed", err);
        }
    }
}
