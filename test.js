let {Network, Trainer} = require('./neuralnetwork')

let b = new Network(2, 1)
b.generate([])


let trainer = new Trainer({
    model: b,
    generate: false,
    count: 10,
    checks: 10,
    mutation: [0.5, 0.1],
    answer: ()=>{
        let x = Math.floor(Math.random() * 10);
        let y = Math.floor(Math.random() * 10);
        return ({input:[x, y], output: x+y})
    },
    miss: (output, input) => {
        return Math.abs(output[0] - input);
    },
    generation: (gen, currentNetwork, offset) => {
        console.log(`Gen: ${gen}, offset: ${offset}`)
    },
});


console.time("Node")
let network = trainer.train(1000)
console.timeEnd("Node")