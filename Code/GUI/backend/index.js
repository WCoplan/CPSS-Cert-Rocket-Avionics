const express = require("express");
const SerialPort = require('serialport')
const path = require("path");
let Readline = SerialPort.parsers.Readline;

const BAUD_RATE = 9600;
const DATA_COLS = ['battery', 'lat', 'lon', 'height', 'time', 'alt', 'vx',
                    'vy', 'vz', 'ax', 'ay', 'az', 'mx', 'my', 'mz' ]

// Express setup
const app = express();
const PORT = process.env.PORT || 3001;
app.listen(PORT, () => console.log(`Server listening on port ${PORT}`));
app.use(express.json({ limit: '1mb' }));
app.use(express.static(path.resolve(__dirname, '../frontend/build')));

// Serial setup with parser
let radioPort = null;
let parser = new Readline({ delimiter: '\r\n' });

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
    // const { port, baud } = req.body;

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

    parser = radioPort.pipe(parser);
    return res.json({ status: 'ok' });
})

// Get stored data
app.get('/api/data/', async (req, res) => {
    // Ensure port is initialized
    if (!radioPort || !parser) return res.json({ status: 'error', error: 'Port uninitialized' });

    // Get data
    try {
        let data = await readPort();
        return res.json({ status: 'ok', data: data });
    } catch (err) {
        console.error(err)
        return res.json({ status: 'error' });
    }
});

// All other GET requests not handled before will return the app
app.get('*', (req, res) => {
    res.sendFile(path.resolve(__dirname, '../frontend/build/', 'index.html'));
});

// Functions
const readPort = () => {
    if (!radioPort || !parser) return null;
    return new Promise((resolve, reject) => {
        parser.once('data', (data) => {
            let parsed = data.split(',');
            if (parsed[0] != '\x02' || parsed[parsed.length - 1] != '\x03') {
                reject('Invalid start and stop bytes');
            }
            parsed = parsed.slice(1,-1)

            let data_obj = {};
            for (let i = 0; i < DATA_COLS.length; i++) {
                data_obj[DATA_COLS[i]] = parsed[i];
            }

            resolve(data_obj);
        });
    });
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
        }, 10);
    })
}