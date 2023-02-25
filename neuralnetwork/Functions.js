
function matrixMultiplication(a, b){
    var c = []
    for (let i = 0; i < a.length; i++) {
        c[i] = []
        for (let j = 0; j < b[0].length; j++) {
            c[i][j] = 0;
            for (let z = 0; z < a[i].length; z++) {
                c[i][j] += a[i][z] * b[z][j]
            }
        }
    }
    return c;
}