const express = require("express");
const SerialPort = require('serialport')
const path = require("path");
let Readline = SerialPort.parsers.Readline;

const BAUD_RATE = 57600;
const DATA_COLS = ['battery', 'lat', 'lon', 'height', 'time', 'alt', 'vx',
                    'vy', 'vz', 'ax', 'ay', 'az', 'mx', 'my', 'mz' ]

// Express setup
const app = express();
const server = require('http').Server(app);
const io = require('socket.io')(server, {
    cors: {
      origin: "http://localhost:3000",
      methods: ["GET", "POST"]
    }
});
const PORT = process.env.PORT || 3001;

server.listen(PORT, () => console.log(`Server listening on port ${PORT}`));
app.use(express.json({ limit: '1mb' }));
app.use(express.static(path.resolve(__dirname, '../frontend/build')));

// Serial setup with parser
let radioPort = null;
let parser = new Readline({ delimiter: '\r\n' });

// Connect to socket
let connectedSocket = null;
io.on('connection', (socket) => {
    connectedSocket = socket;
});

// Get all ports
app.get('/api/port/get', async (req, res) => {
    let all_ports = {}

    // Get list of ports
    await SerialPort.list().then(
        ports => ports.forEach((port, i) => {
            // Include path and manufacturer
            all_ports[i] = {
                path: port.path,
                manufacturer: port.manufacturer
            }
        }),
        err => console.log(err)
    );

    return res.json(all_ports);
})

// Set serial port
app.get('/api/port/set/:port', async (req, res) => {
    const port = req.params.port;

    // Close port before opening another
    if (radioPort) {
        await closePort();
    }

    // Initialize SerialPort
    radioPort = new SerialPort(port, BAUD_RATE);

    // Check for errors
    try {
        await checkErrors();
    } catch(err) {
        radioPort = null;
        return res.json({ status: 'error', error: err });
    }

    // Turn on parser
    parser = radioPort.pipe(parser);
    parser.on('data', (data) => {
        let parsed = data.split(',');

        if (parsed.length != 15) {
            return 0;
        } else {
            let data_obj = {};

            for (let i = 0; i < DATA_COLS.length; i++) {
                data_obj[DATA_COLS[i]] = parsed[i];
            }

            io.emit('serialdata', data_obj);
        }
    })

    return res.json({ status: 'ok' });
})

// --------------------
// Assist Functions
// --------------------

const readPort = () => {
    return new Promise((res, rej) => {
        parser.on('data', (data) => {
            console.log(data)
        })
    })
}

const closePort = () => {
    return new Promise((resolve, reject) => {
        // Close port
        radioPort.close((err) => {
            // Check for errors
            if (err) console.log(err.message);
            resolve();
        })
    })
}

const checkErrors = () => {
    return new Promise((resolve, reject) => {
        radioPort.on('error', (err) => {
            if (err) {
                reject(err.message)
            }
        })
        setTimeout(() => {
            resolve();
        }, 100);
    })
}