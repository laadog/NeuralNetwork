class Bot{
    network;
    outputFunc;
    constructor(network, outputFunc){
        this.network = network
        this.outputFunc = outputFunc
    }

    getOutput(input){
        return this.outputFunc(this.network.calculate(input))
    }


}

module.exports = {Bot}