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


// let a = [1, 2]
// let b = 
// [
//     [ 
//         [[ 8, -8 ], [ -5, 4 ] ],
//         [ [ 8, -3 ], [ 6, 3 ] ],
//         [ [ -3 ], [ 4 ] ]
//     ],
//     [ 
//         [[ 8, -8 ], [ -5, 4 ] ],
//         [ [ 8, -3 ], [ 6, 3 ] ],
//         [ [ -3 ], [ 4 ] ]
//     ]
// ]


let a = [
    [
        [1,2],
        [4,5]
    ],
    [
        [1,2],
        [4,5]
    ]
]
let b = [
    [
        [5,4],
        [6,8],
    ],
    [
        [3,4],
        [6,8],
    ],
]


/*

[
    [[], []],
    [[], []],
]

*/

const gpu = new GPU();
const multiplyMatrix = gpu.createKernel(function(a, b, length) {
let sum = 0;
for (let i = 0; i < length; i++) {
    sum += a[this.thread.z][this.thread.y][i] * b[this.thread.z][i][this.thread.x];
}
return sum;
}).setOutput([2, 2, 2]) // output length, inputs count,  networks count


let gpuOut = multiplyMatrix(a, b, 2)

console.log(gpuOut)





