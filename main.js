let {Network, Bot, Trainer, functions} = require('./neuralnetwork')
const fs = require("fs")
const csvParser = require("csv-parser");

let current = new Network(784, 10);

process.on("message", (message) => {
    const training = []
        let c = 0;
        fs.createReadStream('./data/mnistTrain.csv', {encoding: "utf-8"}).pipe(csvParser()).on("data", (data) => {
        
            training.push({input: Object.values(data).slice(0, -1).map(Number), output: Object.values(data).slice(-1).map(Number)})
        
        }).on("end", ()=>{
            b = new Network(784, 10)
            b.generate([32])
            
            trainer = new Trainer()
            let network = trainer.trainGPU({
                model: b,
                count: 100,
                checks: 1000,
                generations: 100,
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
                    fs.writeFileSync("network.json", JSON.stringify(currentNetwork))
                    process.send("new")
                },
            })
        
            console.log(network)
            fs.writeFileSync("network.json", JSON.stringify(network.layers))
            
    })
})




