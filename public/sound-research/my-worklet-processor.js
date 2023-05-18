class MyWorkletProcessor extends AudioWorkletProcessor {
    constructor() {
        super();
        this.analyser = new AnalyserNode(this.context, {fftSize: 2048});
        this.dataArray = new Float32Array(this.analyser.frequencyBinCount);
    }

    process(inputs, outputs) {
        // Get the input buffer and connect it to the analyser
        const inputBuffer = inputs[0][0];
        this.analyser.connect(this.context.destination);
        this.analyser.disconnect();
        this.context.destination.channelInterpretation = 'discrete';
        this.context.destination.channelCountMode = 'explicit';
        this.context.destination.channelCount = inputBuffer.length;
        this.context.destination.copyToChannel(inputBuffer, 0);

        // Compute the frequency data and send it to the main thread
        this.analyser.getFloatFrequencyData(this.dataArray);
        this.port.postMessage({
            type: 'frequencyData',
            frequencyData: Array.from(this.dataArray)
        });

        return true;
    }
}

registerProcessor('my-worklet-processor', MyWorkletProcessor);
