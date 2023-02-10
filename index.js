const { GPU } = require('gpu.js');

class Network {
    inputs;
    outputs;
    layers = [];

    constructor(inputs, outputs) {
        this.inputs = inputs;
        this.outputs = outputs;
    }

    generate(depths) {
        for (var i = 0; i < depths.length + 1; i++) {
            this.layers[i] = []
            for (var j = 0; j < [this.inputs, ...depths, this.outputs][i]; j++) {
                this.layers[i][j] = []
                for (var z = 0; z < [this.inputs, ...depths, this.outputs][i + 1]; z++) {
                    this.layers[i][j][z] = Math.floor((1 - 2 * Math.random()) * 10)
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
        for (let i = 0; i < this.layers.length; i++) {
            const multiplyMatrix = gpu.createKernel(function (a, b, size) {
                let sum = 0;
                for (let i = 0; i < size; i++) {
                    sum += a[this.thread.y][i] * b[i][this.thread.x];
                }
                return sum;
            }).setOutput([this.layers[i][0].length, 1])
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

class Trainer {
    train(inputs, outputs, layers, count, checks, generations, mutation, answer) {
        var networks = []
        for (var i = 0; i < count; i++) {
            networks[i] = new Network(inputs, outputs)
            networks[i].generate(layers)
        }

        for (var i = 0; i < generations; i++) {
            let closest, smallestOffset;
            for (var j = 0; j < networks.length; j++) {
                var offset = 0;
                for (var z = 0; z < checks; z++) {
                    let prompt = answer();
                    let a = networks[j].calculate(prompt.input)[0]
                    offset += Math.abs(a - prompt.output)
                }
                offset /= checks;
                if (!smallestOffset || offset < smallestOffset) {
                    smallestOffset = offset
                    closest = j;
                }
            }
            console.log(`Gen: ${i}, offset: ${smallestOffset}`)
            networks[0] = networks[closest];
            for (var j = 1; j < networks.length; j++) {
                networks[j] = networks[closest].mutate(...mutation);
            }
        }
        return networks[0]

    }

    trainGPU(inputs, outputs, layers, count, checks, generations, mutation, answer) {
        var networks = [];

        for (let i = 0; i < count; i++) {
            let t = new Network(inputs, outputs)
            t.generate(layers)
            networks[i] = t.layers;
        }

        const gpu = new GPU();

        const multiply = gpu.createKernel(function (a, b, length) {
            let sum = 0;
            for (let i = 0; i < length; i++) {
                sum += a[this.thread.z][this.thread.y][i] * b[this.thread.z][i][this.thread.x];
            }
            return sum;
        })

        multiply.setDynamicOutput(true)

        for (let i = 0; i < generations; i++) {
            let input = []
            let output = []
            for (let j = 0; j < networks.length; j++) {
                input[j] = []
                output[j] = []
                for (let z = 0; z < checks; z++) {
                    let t = answer()
                    input[j][z] = t.input;
                    output[j][z] = t.output;
                }
            }

            let n = new Network(inputs, outputs)
            n.layers = networks[0]

            for (let j = 0; j < networks[0].length; j++) {
                multiply.setOutput([networks[0][j][0].length, checks, count])
                input = multiply(input, networks, networks[0][j].length);
            }

            let closest, smallestOffset;
            for (let j = 0; j < networks.length; j++) {
                let offset = 0;
                for (let z = 0; z < checks; z++) {
                    offset += Math.abs(output[j][z] - input[j][z][0])
                }
                offset /= checks
                if (!smallestOffset || offset < smallestOffset) {
                    smallestOffset = offset
                    closest = j;
                }
            }

            console.log(`Gen: ${i}, offset: ${smallestOffset}`)
            networks[0] = networks[closest]
            let t = new Network(inputs, outputs);
            t.layers = networks[0]
            for (let j = 1; j < networks.length; j++) {
                networks[j] = t.mutate(...mutation).layers
            }
        }

        let t = new Network(inputs, outputs);
        t.layers = networks[0]
        return t;

    }

}

var trainer = new Trainer();

trainer.trainGPU(2, 1, [], 10, 10, 1000, [0.5, 0.1], () => {
    let x = Math.floor(Math.random() * 10)
    let y = Math.floor(Math.random() * 10)
    let output = x + y
    return { output, input: [x, y] }
})


