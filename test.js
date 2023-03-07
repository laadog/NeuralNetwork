let {Network} = require('./neuralnetwork')


b = new Network(2, 1)
b.generate([3])


console.time("JS")
for(let i = 0; i< 1000000; i++){
    b.calculate([1, 1])
}
console.timeEnd("JS")
