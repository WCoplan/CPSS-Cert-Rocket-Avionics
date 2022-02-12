import './App.css';
import React, { useState, useEffect } from 'react';
import LineChartWidget from './lineChartHooks'
import { Grid, Box, Text, Grommet } from 'grommet';
import { grommet } from 'grommet/themes';
import { ReactTerminal, TerminalContextProvider } from "react-terminal";
import { setPort, getPorts, endConnection } from './commands.js';

let buffer = [];
var buff_len = 5		// Size of buffer, releases after that many data points have been obtained
var max_disp = 100		// Maximum number of displayed data points
const Y_ROUND = 50		// The unit for rounding y axises

const App = () => {
	const [data, setData] = useState([]);
	const [socket, setSocket] = useState(null);
	const [maxData, setMaxes] = useState({az : 10, axy : 10, vz : 100, vxy : 100})
	const [status, setStatus] = useState({battery : 0, lat : 0, lon : 0, height : 0, time : 0,
											alt : 0, vx : 0, vy : 0, vz : 0, ax : 0, ay : 0,
											az : 0, mx : 0, my : 0, mz : 0 })

	// Custom commands for terminal
	// TODO: Log loader
	const commands = {
		setport: (port) => {
			return setPort(port, socket, setSocket)
		},
		getports: () => {return getPorts()},
		endconnection: () => {return endConnection(socket)},

		setbuffer: (bufferlen) =>{buff_len = bufferlen; return `Buffer set to ${buff_len}.`},
		getbuffer: () =>{return buff_len},

		setnumpoints: (numpoints) => {max_disp = numpoints; return `Number of datapoints set to ${buff_len}.`},
		getnumpoints: () => {return max_disp}
	};

	// Socket data handler
	const dataHandler = (d) => {
		// Push data to buffer
		buffer.push(d)
	
		// Append buffer to data if buffer max is reached
		if (buffer.length == buff_len) {
			setData(data => [...data, ...buffer])
			buffer = []
		}
	}

	// Enable socket listening on socket change
	useEffect(() => {
        if (socket) socket.on('serialdata', dataHandler);
		return () => {
			// Turn off listener
			if (socket) socket.off('serialdata', dataHandler);
		}
    }, [socket]);

	// Detect change in data
	useEffect(() => {
		// If data length is greater than max, keep newest
        if (data && data.length > max_disp) {
			setData(data.slice(-max_disp))

			// Manually setting max data for charting, because Recharts is ASS
			// TODO: Change scaling based on max of data, not max overall
			for(const d of data){
				var s = maxData
				var az = s.az
				var axy = s.axy
				var vz = s.vz
				var vxy = s.vxy

				if(parseFloat(d.az) > s.az) az = Math.floor(d.az / Y_ROUND + 1) * Y_ROUND

				if(parseFloat(d.vz) > s.vz) vz = Math.floor(d.vz / Y_ROUND + 1) * Y_ROUND

				if(parseFloat(d.ax) > s.axy) axy = Math.floor(d.ax / Y_ROUND + 1) * Y_ROUND
				if(parseFloat(d.ay) > s.axy) axy = Math.floor(d.ay / Y_ROUND + 1) * Y_ROUND

				if(parseFloat(d.vx) > s.vxy) vxy = Math.floor(d.vx / Y_ROUND + 1) * Y_ROUND
				if(parseFloat(d.vy) > s.vxy) vxy = Math.floor(d.vy / Y_ROUND + 1) * Y_ROUND

				setMaxes({az : az, axy : axy, vz : vz, vxy : vxy})
				setStatus({battery : d.battery, lat : d.lat, lon : d.lon, height : d.height, time : d.time,
							alt : d.alt, vx : d.vx, vy : d.vy, vz : d.vz, ax : d.ax, ay : d.ay,
							az : d.az, mx : d.mx, my : d.my, mz : d.mz })
			}
		}
    }, [data, maxData, setStatus]);

	return (
		<Grommet full theme={grommet}>
			{Grid.available ? (
			<Grid
				fill
				rows={['24%', '24%', '24%', '24%']}
				columns={['32%', '32%', '32%']}
				gap="1%"
				areas={[
					{ name: 'acc_z', start: [0, 0], end: [1, 1] },
					{ name: 'acc_xy', start: [1, 0], end: [2, 1] },

					// { name: 'gps', start: [2, 0], end: [2, 1] },

					{ name: 'vel_z', start: [0, 2], end: [1, 3] },
					{ name: 'vel_xy', start: [1, 2], end: [1, 3] },

					{ name: 'status', start: [2, 1], end: [2, 3] },
					{ name: 'terminal', start: [2, 0], end: [2, 1] },
				]}
			>
				{/* Acceleration Graphs */}
				<LineChartWidget
					data={data}
					title={"Acceleration: Z (m/s/s)"}
					dataStrokes={[{key: "az", color: "#8884d8"}]}
					xAxisDataKey={"time"}
					gridArea="acc_z"
					maxData ={maxData.az}>
				</LineChartWidget>

				<LineChartWidget
					data={data}
					title={"Acceleration: X,Y (m/s/s)"}
					dataStrokes={[{key: "ax", color: "#8884d8"},
								{key: "ay", color: "#880088"}]}
					xAxisDataKey={"time"}
					gridArea="acc_xy"
					maxData ={maxData.axy}>
				</LineChartWidget>

				{/* Velocity Graphs */}
				<LineChartWidget
					data={data}
					title={"Velocity: Z (m/s)"}
					dataStrokes={[{key: "vz", color: "#8884d8"}]}
					xAxisDataKey={"time"}
					gridArea="vel_z"
					maxData ={maxData.vz}>
				</LineChartWidget>

				<LineChartWidget
					data={data}
					title={"Velocity: X,Y (m/s)"}
					dataStrokes={[{key: "vx", color: "#8884d8"},
								{key: "vy", color: "#880088"}]}
					xAxisDataKey={"time"}
					gridArea="vel_xy"
					maxData ={maxData.vxy}>
				</LineChartWidget>

				<Box gridArea="terminal">
					<TerminalContextProvider>
						<ReactTerminal
							commands={commands}
							theme="material-dark"
							showControlBar={false}
						/>
					</TerminalContextProvider>
				</Box>

				<Box gridArea="status" background="light-2" height="100%" border="3px solid">
					<h4 align="center"> Status</h4>
					<table style={{borderCollapse: "collapse"}}>
						<tr>
							<td>Max Acc Z: {maxData.az} m/s²</td>
							<td>Acc Z: {status.az} m/s²</td>
						</tr>
						<tr>
							<td>Max Acc Y: {maxData.axy} m/s²</td>
							<td>Acc X: {status.ax} m/s²</td>
						</tr>
						<tr>
							<td></td>
							<td>Acc Y: {status.ay} m/s²</td>
						</tr>
						<br></br>
						<tr>
							<td>Max Vel Z: {maxData.vz} m/s</td>
							<td>Vel Z: {status.vz} m/s</td>
						</tr>
						<tr>
							<td>Max Vel XY: {maxData.vxy} m/s</td>
							<td>Vel X: {status.vx} m/s</td>
						</tr>
						<tr>
							<td></td>
							<td>Vel Y: {status.vy} m/s</td>
						</tr>
						<br></br>
						<tr>
							<td>Mag X: {status.mx}</td>
							<td>Latitude: {status.lat}</td>
						</tr>
						<tr>
							<td>Mag Y: {status.my}</td>
							<td>Longitude: {status.lon}</td>
						</tr>
						<tr>
							<td>Mag Z: {status.mz}</td>
							<td>Height: {status.height} m</td>
						</tr>
						<br></br>
						<tr>
							<td>Battery: {status.battery}%</td>
						</tr>
					</table>
				</Box>

				{/* <Box gridArea="gps" >
					<img src={mapImage} height="100%"></img>
				</Box> */}
			</Grid>
			) : (<Text>Grid is not supported by your browser </Text>)
			}
		</Grommet>
	);
}

export default App;