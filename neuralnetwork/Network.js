class Network {
    inputs;
    outputs;
    layers = [];
    depths;

    constructor(inputs, outputs) {
        this.inputs = inputs;
        this.outputs = outputs;
    }

    generate(depths) {
        this.depths = depths
        for (var i = 0; i < depths.length + 1; i++) {
            this.layers[i] = []
            for (var j = 0; j < [this.inputs, ...depths, this.outputs][i]; j++) {
                this.layers[i][j] = []
                for (var z = 0; z < [this.inputs, ...depths, this.outputs][i + 1]; z++) {
                    this.layers[i][j][z] = (1 - 2 * Math.random()) * 10
                }
            }
        }

    }

    calculate(input) {
        var nextLayer = [];
        for (var i = 0; i < this.layers.length; i++) {
            for (var j = 0; j < this.layers[i][0].length; j++) {
                nextLayer[j] = 0;
                for (var z = 0; z < input.length; z++) {
                    nextLayer[j] += input[z] * this.layers[i][z][j]
                }
            }
            input = []
            for (let j = 0; j < nextLayer.length; j++) {
                input[j] = nextLayer[j]
            }
            nextLayer = []
        }
        return input;
    }

    calculateGPU(input) {
        const gpu = new GPU();
        const multiplyMatrix = gpu.createKernel(function (a, b, size) {
            let sum = 0;
            for (let i = 0; i < size; i++) {
                sum += a[this.thread.y][i] * b[i][this.thread.x];
            }
            return sum;
        }, {
            dynamicOutput: true,
            dynamicArguments: true
        })

        for (let i = 0; i < this.layers.length; i++) {
            multiplyMatrix.setOutput([this.layers[i][0].length, 1])
            input = multiplyMatrix(input, this.layers[i], this.layers[i].length)
        }
        return input;
    }

    mutate(percentage = 0.8, offset = 0.1) {
        var mutated = new Network(this.inputs, this.outputs)

        for (var i = 0; i < this.layers.length; i++) {

            mutated.layers[i] = []
            for (var j = 0; j < this.layers[i].length; j++) {
                mutated.layers[i][j] = []
                for (var z = 0; z < this.layers[i][j].length; z++) {
                    if (Math.random() < percentage) {
                        mutated.layers[i][j][z] = this.layers[i][j][z] + (1 - 2 * Math.random()) * offset

                    } else {
                        mutated.layers[i][j][z] = this.layers[i][j][z]
                    }
                }

            }
        }
        return mutated;
    }



    get getLayers() {
        return this.layers
    }
}

module.exports = {Network}