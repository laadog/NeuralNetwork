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

function indexOfMax(intArr){
    let max;
    intArr.forEach(element => {
        if(!max || element > max){
            max = element
        }
    });
    return intArr.indexOf(max)
}

function binaryArray(intArr){
    let max = indexOfMax(intArr)
    intArr.fill(false)
    intArr[max] = true
    return intArr
}


module.exports = {
    matrixMultiplication,
    indexOfMax,
    binaryArray,
}