import io from 'socket.io-client';

// --------------------
// Commands
// --------------------

//
export const setPort = async (port, socket, setSocket) => {
    let status = null

    if (!port) return 'Syntax: setport [port]'

    // Tell backend to set port
    await fetch(`/api/port/set/${port}`)
        .then((res) => res.json())
        .then((data) => {
            if (data.status === 'ok') {
                status = 'Success'
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
    let paths = null;

    // Query backend
    await fetch('/api/port/get')
        .then((res) => res.json())
        .then((data) => {
            for(let i in data) {
                if (i === 0) paths = data[i].path + '\n'
                else paths += data[i].path + '\n'
            }
        })
        .catch(err => {
            console.error("Error fetching data:", err);
        });

    if (paths)
        return paths
    else
        return 'No open ports found'
}
