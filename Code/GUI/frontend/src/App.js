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
					gridArea="acc_z">
				</LineChartWidget>

				<LineChartWidget
					data={data}
					title={"Acceleration: X,Y"}
					dataStrokes={[{key: "ax", color: "#8884d8"},
								{key: "ay", color: "#880088"}]}
					xAxisDataKey={"time"}
					gridArea="acc_xy">
				</LineChartWidget>

				{/* Velocity Graphs */}
				<LineChartWidget
					data={data}
					title={"Velocity: Z"}
					dataStrokes={[{key: "vz", color: "#8884d8"}]}
					xAxisDataKey={"time"}
					gridArea="vel_z">
				</LineChartWidget>

				<LineChartWidget
					data={data}
					title={"Velocity: X,Y"}
					dataStrokes={[{key: "vx", color: "#8884d8"},
								{key: "vy", color: "#880088"}]}
					xAxisDataKey={"time"}
					gridArea="vel_xy">
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

				<Box gridArea="status" background="light-2">
					<h4 align="center"> Status</h4>
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