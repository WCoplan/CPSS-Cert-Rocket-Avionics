export const setPort = async (port) => {
    let status = null

    if (!port) return 'Syntax: setport [port]'

    // Tell backend to set port
    await fetch(`/api/port/set/${port}`)
        .then((res) => res.json())
        .then((data) => {
            if (data.status == 'ok') {
                status = 'Success'
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
                if (i == 0) paths = data[i].path + ' - ' + data[i].manufacturer + '\n'
                else paths += data[i].path + ' - ' + data[i].manufacturer + '\n'
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

export const getData = async () => {
    let r = null;
    let status = null;

    // Query backend
    await fetch('/api/data')
        .then((res) => res.json())
        .then((data) => {
            if (data.status == 'ok')
                r = data.data;
            else
                status = data.error
        })
        .catch(err => {
            console.error("Error fetching data:", err);
        });

    if (r) return JSON.stringify(r)
    else return status ? status : 'Unknown error'
}