let {Network, Bot, Trainer} = require('./neuralnetwork/NeuralNetwork')


n = new Network(2, 10)
n.generate([]);
b = new Bot(n, (input) => {
    let max = 0;
    for(let i = 0; i<input.length;i++){
        if(input[i] > max){
            max = input[i]
        }
    }
//Test
    for(let i = 0; i < input.length; i++){
        if(input[i] == max){input[i] = true}
        else{
            input[i] = false
        }
    }

    return input

})

console.log(b.getOutput([1, 2]))