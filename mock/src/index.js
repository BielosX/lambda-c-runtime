const express = require('express')
const app = express()
const port = 8080

app.get('/runtime/invocation/next', (req, res) => {
    res.append('Lambda-Runtime-Aws-Request-Id', '8476a536-e9f4-11e8-9739-2dfe598c3fcd')
    res.append('Lambda-Runtime-Deadline-Ms', '1542409706888')
    res.append('Content-Type', 'application/json')
    res.send({
        firstField: "value",
        secondField: "value2"
    })
})

app.listen(port, () => {
    console.log(`Mock app listening on port ${port}`)
})