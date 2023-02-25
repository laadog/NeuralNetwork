let {Network, Bot, Trainer, functions} = require('./neuralnetwork')
// const fs = require("fs")
// const csvParser = require("csv-parser");

// const training = []
// let c = 0;
// fs.createReadStream('./data/mnistTrain.csv', {encoding: "utf-8"}).pipe(csvParser()).on("data", (data) => {
//     training.push({input: Object.values(data).slice(0, -1), output: Object.values(data).slice(-1)})

// }).on("end", ()=>{
//     b = new Network(784, 10)
//     b.generate([64, 64])
    
//     trainer = new Trainer()
    
//     trainer.trainGPU({
//         model: b,
//         count: 100,
//         checks: 10,
//         generations: 1000,
//         mutation: [0.25, 10],
//         answer: ()=>{
//             return(training[Math.floor(Math.random()*training.length)])
//         },
//         miss: (output, input) => {
//             if(functions.indexOfMax(output) == input[0]){
//                 return 0;
//             }
//             return(1)

//         }
//     })
// })



b = new Network(2, 1)
b.generate([])

trainer = new Trainer()

trainer.trainGPU({
    model: b,
    count: 100,
    checks: 10,
    generations: 100,
    mutation: [0.5, 0.1],
    answer: ()=>{
        let x = Math.floor(Math.random()*10)
        let y = Math.floor(Math.random()*10)
        return({input:[x, y], output: [x+y]})
    },
    miss: (output, input) => {
        return Math.abs(output[0]-input[0])

    }
})