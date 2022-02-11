import './App.css';
import React, { useState, useEffect } from 'react';
import LineChartWidget from './lineChartHooks'
import { Grid, Box, Text, Grommet } from 'grommet';
import { grommet } from 'grommet/themes';
import mapImage from "./calpolymap.png";
import { ReactTerminal, TerminalContextProvider } from "react-terminal";
import { setPort, getPorts } from './commands.js';

let buffer = [];
const BUFF_LEN = 10		// Size of buffer, releases after that many data points have been obtained
const MAX_DISP = 50		// Maximum number of displayed data points

const App = () => {
	const [data, setData] = useState([]);
	const [socket, setSocket] = useState(null);
	const [maxData, setMaxes] = useState({az : 0, axy : 0, vz : 0, vxy : 0})
	// const [curData, setCur] = useState({az : 0, axy : 0, vz : 0, vxy : 0})

	// Custom commands for terminal
	const commands = {
		setport: (port) => {return setPort(port, socket, setSocket)},
		getports: () => {return getPorts()}
	};


	// Turn socket on if selected
	useEffect(() => {
        if (socket) {
			socket.on('serialdata', (d) => {
				// Push data to buffer
				buffer.push(d)

				// Append buffer to data if buffer max is reached
				if (buffer.length == BUFF_LEN) {
					setData(data => [...data, ...buffer])
					buffer = []
				}

			});
		}
		return () => {
			if (socket) socket.close()
		}
    }, [socket]);

	// Detect change in data
	useEffect(() => {
		// If data length is greater than max, slice off first buffer size
        if (data && data.length > MAX_DISP) {
			// This is weird, splice returns the spliced part though 🥴
			let newdata = data
			newdata.splice(0, BUFF_LEN)
			setData(newdata)
		}
    }, [data]);

	//For setting max values of graphs
	useEffect(() => {
		// If data length is greater than max, slice off first buffer size
		const c = data[-1]

        for(const d of data){
			var s = maxData
			var az = s.az
			var axy = s.axy
			var vz = s.vz
			var vxy = s.vxy

			if(parseFloat(d.az) > s.az) az = parseFloat(d.az)

			if(parseFloat(d.vz) > s.vz) vz = parseFloat(d.vz)

			if(parseFloat(d.ax) > s.axy) axy = parseFloat(d.ax)
			if(parseFloat(d.ay) > s.axy) axy = parseFloat(d.ay)

			if(parseFloat(d.vx) > s.vxy) vxy = parseFloat(d.vx)
			if(parseFloat(d.vy) > s.vxy) vxy = parseFloat(d.vy)

			setMaxes({az : az, axy : axy, vz : vz, vxy : vxy})
		}
    }, [data, maxData]);

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

					{ name: 'gps', start: [2, 0], end: [2, 1] },

					{ name: 'vel_z', start: [0, 2], end: [1, 3] },
					{ name: 'vel_xy', start: [1, 2], end: [1, 3] },

					{ name: 'status', start: [2, 3], end: [2, 3] },
					{ name: 'terminal', start: [2, 2], end: [2, 2] },
				]}
			>
				{/* Acceleration Graphs */}
				<LineChartWidget
					data={data}
					title={"Acceleration: Z"}
					dataStrokes={[{key: "az", color: "#8884d8"}]}
					xAxisDataKey={"time"}
					gridArea="acc_z"
					maxData ={maxData.az}>
				</LineChartWidget>

				<LineChartWidget
					data={data}
					title={"Acceleration: X,Y"}
					dataStrokes={[{key: "ax", color: "#8884d8"},
								{key: "ay", color: "#880088"}]}
					xAxisDataKey={"time"}
					gridArea="acc_xy"
					maxData ={maxData.axy}>
				</LineChartWidget>

				{/* Velocity Graphs */}
				<LineChartWidget
					data={data}
					title={"Velocity: Z"}
					dataStrokes={[{key: "vz", color: "#8884d8"}]}
					xAxisDataKey={"time"}
					gridArea="vel_z"
					maxData ={maxData.vz}>
				</LineChartWidget>

				<LineChartWidget
					data={data}
					title={"Velocity: X,Y"}
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

				<Box gridArea="status" background="light-2" height="100%">
					<h4 align="center"> Status</h4>
					<table>
						<tr>
							<td>Max Acc  Z: {maxData.az} m/s^2</td>
							<td>Acc Z: {maxData.az} m/s^2</td>
						</tr>
						<tr>
							<td>Max Vel  Z: {maxData.vz} m/s</td>
							<td>Vel Z: {maxData.vz} m/s^2</td>
						</tr>
						<tr>
							<td>Max Acc XY: {maxData.axy} m/s^2</td>
							<td>Acc XY: {maxData.axy} m/s^2</td>
						</tr>
						<tr>
							<td>Max Vel XY: {maxData.vxy} m/s</td>
							<td>Vel XY {maxData.az} m/s^2</td>
						</tr>
					</table>
				</Box>

				<Box gridArea="gps" >
					<img src={mapImage} height="100%"></img>
				</Box>
			</Grid>
			) : (<Text>Grid is not supported by your browser </Text>)
			}
		</Grommet>
	);
}

export default App;