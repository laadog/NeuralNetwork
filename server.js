const express = require('express')
var bodyParser = require('body-parser')
const {fork, ChildProcess} = require("child_process")
var cors = require('cors')
const {Network, functions} = require("./neuralnetwork")
const fs = require("fs")
const csvParser = require("csv-parser");
const app = express()
const port = 3000

var network = new Network(784, 10)
var training = []

app.use(bodyParser.urlencoded({ extended: false }))
app.use(bodyParser.json())
app.use(cors())

fs.createReadStream('./data/mnistTrain.csv', {encoding: "utf-8"}).pipe(csvParser()).on("data", (data) => {
        
    training.push({input: Object.values(data).slice(0, -1).map(Number), output: Object.values(data).slice(-1).map(Number)})

})

let data = fs.readFileSync("./network.json", {encoding: "utf-8", flag: "r"})
data = JSON.parse(data)
network.layers = data.layers

trainer = fork("./main.js")

app.get("/start", (req, res) => {
    trainer.send("start")
    res.send("Trainer started")
})

app.get("/network", (req, res) => {
    res.send(network)
})

app.post("/classify", (req, res) => {
    let answer = network.calculate(req.body)
    res.send({output: functions.indexOfMax(answer)})
})

app.get("/training", (req, res) => {
    res.send(training[Math.floor(Math.random() * training.length)])
})

trainer.on("message", (message) => {
    data = fs.readFileSync("./network.json", {encoding: "utf-8", flag: "r"})
    data = JSON.parse(data)
    network.layers = data.layers
})

app.post("/network", (req, res) => {
    console.log(req.body)
})

app.listen(port)