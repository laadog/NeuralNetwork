class Network{
    inputs;
    outputs;
    layers = [];

    constructor(inputs, outputs){
        this.inputs = inputs;
        this.outputs = outputs;
    }

    generate(depths){ 
        for(var i = 0; i < depths.length + 1; i++){
            this.layers[i] = []
            for(var j = 0; j < [this.inputs, ...depths, this.outputs][i]; j++){
                this.layers[i][j] = []
                for(var z = 0; z < [this.inputs, ...depths, this.outputs][i+1];z++){
                    this.layers[i][j][z] = 1 - 2*Math.random()
                }
            }
        }

    }

    calculate(input){
        var nextLayer = [];
        for(var i = 0; i <this.layers.length;i++){
            for(var j = 0; j < this.layers[i][0].length;j++){
                for(var z = 0; z < (this.layers[i].length ); z++){
                    
                    nextLayer[j] = (nextLayer[0] || 0) + input[z]*this.layers[i][z][j]
                }
            }
            input = nextLayer
        }
        return nextLayer.slice(0, this.outputs);
    }

    mutate(percentage = 0.8, offset = 0.1){
        var mutated = new Network(this.inputs, this.outputs)

        for(var i = 0; i < this.layers.length; i++){
            
            mutated.layers[i] = []
            for (var j = 0; j < this.layers[i].length; j++) {
                mutated.layers[i][j] = []
                for(var z = 0; z < this.layers[i][j].length; z++){
                    if(Math.random() < percentage){
                        mutated.layers[i][j][z] = this.layers[i][j][z] + (1 - 2*Math.random())*offset

                    }else{
                        mutated.layers[i][j][z] = this.layers[i][j][z]
                    }
                }
                
            }
        }
        return mutated;
    }



    get getLayers(){
        return this.layers
    }
}

function train(base, layers, count,checks , generations, mutation ,answer){
    var networks = []
    for(var i = 0; i < count;i++){
        networks[i] = new Network(2,1)
        networks[i].generate(layers)
    }
    
    for(var i = 0; i < generations; i++){
        let closest, closestOutput;
        let prompt = answer();
        for(var j = 0; j < networks.length;j++){
            let a = networks[j].calculate(prompt.input)[0]
            if(!closest || Math.abs(prompt.output - a) < Math.abs(prompt.output-closestOutput)){
                closestOutput = a;
                closest = j;
            }
        }
        console.log(`Gen: ${i}, Answer: ${prompt.output}, closest: ${closestOutput}, accuracy: ${(prompt.output-Math.abs(closestOutput-prompt.output))/prompt.output*100}%`)
        networks[0] = networks[closest];
        for(var j = 1; j < networks.length; j++){
            networks[j] = networks[closest].mutate(...mutation);
        }
    }
    return networks[0]

}
train(new Network(2, 1), [], 10000, 1000, [0.5, 0.001], ()=>{
    var x = Math.random()
    var y = Math.random()
    var r = x + y;
    return {input: [x, y], output: r}
})