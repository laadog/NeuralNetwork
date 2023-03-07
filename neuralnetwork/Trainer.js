const {Network} = require("./Network")
const {GPU} = require("gpu.js")

class Trainer {
    model;
    generate;
    count;
    checks;
    mutation;
    answer;
    miss;
    generation = () =>{}

    constructor({model, generate, count, checks, mutation, answer, miss, generation}){
        this.model = model;
        this.generate = generate;
        this.count = count;
        this.checks = checks;
        this.mutation = mutation
        this.answer = checks
        this.answer = answer
        this.miss = miss
        this.generation = generation
        
    }

    train(generations) {
        var networks = []
        if(this.generate){
            for (var i = 0; i < this.count; i++) {
                networks[i] = new Network(this.model.inputs, this.model.outputs)
                networks[i].generate(this.model.depths)
            }
        }else{
            networks[0] = this.model;
            for (var i = 1; i < this.count; i++) {
                networks[i] = this.model.mutate(...this.mutation)
            }
        }


        for (var i = 0; i < generations; i++) {
            let closest, smallestOffset;
            for (var j = 0; j < networks.length; j++) {
                var offset = 0;
                for (var z = 0; z < this.checks; z++) {
                    let prompt = this.answer();
                    let a = networks[j].calculate(prompt.input)
                    offset += this.miss(a, prompt.output)
                }
                offset /= this.checks;
                if (!smallestOffset || offset < smallestOffset) {
                    smallestOffset = offset
                    closest = j;
                }
            }
            networks[0] = networks[closest];
            this.generation(i,networks[0],smallestOffset)
            for (var j = 1; j < networks.length; j++) {
                networks[j] = networks[closest].mutate(...this.mutation);
            }
        }
        return networks[0]

    }

    trainGPU(generations) {

        var networks = []
        if(this.generate){
            for (var i = 0; i < this.count; i++) {
                networks[i] = new Network(this.model.inputs, this.model.outputs)
                networks[i].generate(this.model.depths)
                networks[i] = networks[i].layers
            }
        }else{
            networks[0] = this.model;
            for (var i = 1; i < this.count; i++) {
                networks[i] = this.model.mutate(...this.mutation)
                networks[i] = networks[i].layers
            }
        }   
        const gpu = new GPU();
        const multiply = gpu.createKernel(function (a, b, length) {
            let sum = 0;
            for (let i = 0; i < length; i++) {
                sum += a[this.thread.z][this.thread.y][i] * b[this.thread.z][i][this.thread.x];
            }
            return sum;
        }, {
            dynamicOutput: true,
            dynamicArguments: true
        })
        
        for (let i = 0; i < generations; i++) {
            let input = []
            let output = []
            for (let j = 0; j < networks.length; j++) {
                input[j] = []
                output[j] = []
                for (let z = 0; z < this.checks; z++) {
                    let t = this.answer()
                    input[j][z] = t.input;
                    output[j][z] = t.output;
                }
            }
            
            for (let j = 0; j < networks[0].length; j++) {
                let layers = [];
                for(let z = 0; z < networks.length; z++){
                    layers[z] = networks[z][j]
                }
                multiply.setOutput([networks[0][j][0].length, this.checks, this.count])
                input = multiply(input,   layers  , networks[0][j].length);
            }
            
            let closest, smallestOffset;
            for (let j = 0; j < networks.length; j++) {
                let offset = 0;
                for (let z = 0; z < this.checks; z++) {
                    offset += this.miss(input[j][z], output[j][z])
                }
                offset /= this.checks
                if (!smallestOffset || offset < smallestOffset) {
                    smallestOffset = offset
                    closest = j;
                }
            }

            networks[0] = networks[closest]
            let t = new Network(this.model.inputs, this.model.outputs);
            t.layers = networks[0]
            this.generation(i,t,smallestOffset)
            for (let j = 1; j < networks.length; j++) {
                networks[j] = t.mutate(...this.mutation).layers
            }
        }

        let t = new Network(this.model.inputs, this.model.outputs);
        t.layers = networks[0]
        return t;

    }

}

module.exports = {Trainer}