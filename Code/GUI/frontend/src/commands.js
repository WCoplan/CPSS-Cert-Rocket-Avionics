import io from 'socket.io-client';

// --------------------
// Commands
// --------------------

//
export const setPort = async (port, socket, setSocket) => {
    let status = null

    if (!port) return 'Syntax: setport [port]'

    // Disconnect old socket
    if (socket) {
        socket.emit('end');
    }

    // Tell backend to set port
    await fetch(`/api/port/set/${port}`)
        .then((res) => res.json())
        .then((data) => {
            if (data.status === 'ok') {
                status = 'Success'

                // Connect to socket
                try {
                    setSocket(io('http://localhost:3001'));
                } catch (err) {
                    console.error(err)
                }
            } else {
                status = `Error: ${data.error}`
            }
        })
        .catch(err => {
            status = `Error fetching data: ${err}`;
        });

    // Return updated status
    return status;
}

export const getPorts = async () => {
    let paths = [];

    // Query backend
    await fetch('/api/port/get')
        .then((res) => res.json())
        .then((data) => {
            for(const i in data) {
                paths.push(data[i].path)
            }
        })
        .catch(err => {
            console.error("Error fetching data:", err);
        });

    if (paths) {
        return paths.join('\n')
    }
    else
        return 'No open ports found'
}

export const endConnection = async (socket) => {
    // Disconnect old socket
    if (socket) {
        socket.emit('end');
    }

    // Tell backend to end connection
    let status = 'Successfully disconnected';
    await fetch('/api/port/end')
        .then((res) => res.json())
        .then((data) => {
            if (data.status !== 'ok') {
                status = data.error;
            }
        })
        .catch(err => {
            console.error("Error fetching data: ", err);
        });

    return status
}