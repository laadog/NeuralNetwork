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
        let closest, smallestOffset;
        for(var j = 0; j < networks.length;j++){
            var offset = 0;
            for(var z = 0; z < checks; z++){
                let prompt = answer();
                let a = networks[j].calculate(prompt.input)[0]
                offset += Math.abs(a-prompt.output)
            }
            offset /= checks;
            if(!closest || offset < smallestOffset){
                smallestOffset = offset
                closest = j;
            }
        }
        console.log(`Gen: ${i}, offset: ${smallestOffset}`)
        networks[0] = networks[closest];
        for(var j = 1; j < networks.length; j++){
            networks[j] = networks[closest].mutate(...mutation);
        }
    }
    return networks[0]

}
var bot = train(new Network(2, 1), [6, 6], 1000, 10, 10000, [0.5, 0.005], ()=>{
    var x = Math.random()*2
    var y = Math.random()*3
    var r = x*y;
    return {input: [x, y], output: r}
})
