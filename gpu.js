const { GPU } = require('gpu.js');


const SIZE = 512
const generateMatrices = () => {
    const matrices = [[], []]
    for (let y = 0; y < SIZE; y++){
      matrices[0].push([])
      matrices[1].push([])
      for (let x = 0; x < SIZE; x++){
        matrices[0][y].push(Math.random())
        matrices[1][y].push(Math.random())
      }
    }
    return matrices
}

function multiply(a, b){
    var c = []
    for(let i = 0; i < a.length; i++){
        c[i] = []
        for(let j = 0; j < b[0].length; j++){
            c[i][j] = 0;
            for(let z = 0; z < a[i].length; z++){
                c[i][j] += a[i][z]*b[z][j] 
            }
        }
    }
    return c;
}


let a = [[1,2,3]]
let b = [[1,2, 3], [3,4, 3], [5, 6, 3]]

const gpu = new GPU();
const multiplyMatrix = gpu.createKernel(function(a, b, length) {
let sum = 0;
for (let i = 0; i < length; i++) {
    sum += a[this.thread.y][i] * b[i][this.thread.x];
}
return sum;
}).setOutput([3, 1])

let gpuOut = multiplyMatrix(a, b, b.length)

console.log(gpuOut)
console.log(multiply(a, b))

// console.time("Cpu")
// const cpuOut = multiply(matrices[0], matrices[1])
// console.timeEnd("Cpu")
// console.time("Gpu")
// gpuOut = multiplyMatrix(matrices[0], matrices[1], SIZE)
// console.timeEnd("Gpu")




