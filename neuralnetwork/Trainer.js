const {Network} = require("./Network")
const {GPU} = require("gpu.js")

class Trainer {
    train({model, count, checks, generations, mutation, answer, miss}) {
        var networks = []
        if(true){
            for (var i = 0; i < count; i++) {
                networks[i] = new Network(model.inputs, model.outputs)
                networks[i].generate(model.depths)
            }
        }else{
            networks[0] = model;
            for (var i = 1; i < count; i++) {
                networks[i] = model.mutate(...mutation)
            }
        }


        for (var i = 0; i < generations; i++) {
            let closest, smallestOffset;
            for (var j = 0; j < networks.length; j++) {
                var offset = 0;
                for (var z = 0; z < checks; z++) {
                    let prompt = answer();
                    let a = networks[j].calculate(prompt.input)
                    offset += miss(a, prompt.output)
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

    trainGPU({model, count, checks, generations, mutation, answer, miss, generation = ()=>{}}) {

        var networks = []
        if(true){
            for (var i = 0; i < count; i++) {
                networks[i] = new Network(model.inputs, model.outputs)
                networks[i].generate(model.depths)
                networks[i] = networks[i].layers
            }
        }else{
            networks[0] = model;
            for (var i = 1; i < count; i++) {
                networks[i] = model.mutate(...mutation)
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
                for (let z = 0; z < checks; z++) {
                    let t = answer()
                    input[j][z] = t.input;
                    output[j][z] = t.output;
                }
            }
            
            for (let j = 0; j < networks[0].length; j++) {
                let layers = [];
                for(let z = 0; z < networks.length; z++){
                    layers[z] = networks[z][j]
                }
                multiply.setOutput([networks[0][j][0].length, checks, count])
                input = multiply(input,   layers  , networks[0][j].length);
            }
            
            let closest, smallestOffset;
            for (let j = 0; j < networks.length; j++) {
                let offset = 0;
                for (let z = 0; z < checks; z++) {
                    offset += miss(input[j][z], output[j][z])
                }
                offset /= checks
                if (!smallestOffset || offset < smallestOffset) {
                    smallestOffset = offset
                    closest = j;
                }
            }

            networks[0] = networks[closest]
            let t = new Network(model.inputs, model.outputs);
            t.layers = networks[0]
            generation(i,t,smallestOffset)
            for (let j = 1; j < networks.length; j++) {
                networks[j] = t.mutate(...mutation).layers
            }
        }

        let t = new Network(model.inputs, model.outputs);
        t.layers = networks[0]
        return t;

    }

}

module.exports = {Trainer}