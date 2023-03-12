let {Network, Bot, Trainer, functions} = require('./neuralnetwork')
const fs = require("fs")
const csvParser = require("csv-parser");

let training = [];

fs.createReadStream('./data/mnistTrain.csv', {encoding: "utf-8"}).pipe(csvParser()).on("data", (data) => {

    training.push({input: Object.values(data).slice(0, -1).map(Number), output: Object.values(data).slice(-1).map(Number)})

}).on("end", ()=>{

    let b = new Network(784, 10)
    b.generate([])
    
    trainer = new Trainer({
        model: b,
        generate: false,
        count: 100,
        checks: 1000,
        mutation: [0.25, 5],
        answer: ()=>{
            return(training[Math.floor(Math.random()*training.length)])
        },
        miss: (output, input) => {
            if(functions.indexOfMax(output) == input[0]){
                return 0;
            }
            return 1

        },
        generation: (gen, currentNetwork, offset) => {
            console.log(`Gen: ${gen}, offset: ${offset}`)
        },
    })

    console.time("Node")
    let network = trainer.train(10)  
    console.timeEnd("Node")

})



