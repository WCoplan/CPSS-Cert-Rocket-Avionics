import './App.css';
import React from 'react';

import LineChartWidget from './lineChartWidget'
import StatusWidget from './statusWidget';


import {Grid, Box, Text, Grommet } from 'grommet';
import { grommet } from 'grommet/themes';

import mapImage from "./calpolymap.png";

import { ReactTerminal, TerminalContextProvider } from "react-terminal";
class App extends React.Component {

  constructor() {
    super();
    this.state = {};
  }

  render() {
    const data = [
      {
        t: 0,
        AccX: 0,
        AccY: 5
      },
      {
        t: 10,
        AccX: 2,
        AccY: 10
      },
      {
        t: 20,
        AccX: 4,
        AccY: 15
      },
      {
        t: 30,
        AccX: 8,
        AccY: 20
      },
      {
        t: 40,
        AccX: 16,
        AccY: 25
      },
      {
        t: 50,
        AccX: 32,
        AccY: 30
      },
    ]

    const commands = {
      whoami: "curtisbucher",
    };

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
            dataStrokes={[{key: "AccX", color: "#8884d8"},
                          {key: "AccY", color: "#880088"}]}
            xAxisDataKey={"t"}
            gridArea="acc_z">
          </LineChartWidget>

          <LineChartWidget
            data={data}
            title={"Acceleration: X,Y"}
            dataStrokes={[{key: "AccX", color: "#8884d8"},
                          {key: "AccY", color: "#880088"}]}
            xAxisDataKey={"t"}
            gridArea="acc_xy">
          </LineChartWidget>

          {/* Velocity Graphs */}
          <LineChartWidget
            data={data}
            title={"Velocity: Z"}
            dataStrokes={[{key: "AccX", color: "#8884d8"},
                          {key: "AccY", color: "#880088"}]}
            xAxisDataKey={"t"}
            gridArea="vel_z">
          </LineChartWidget>

          <LineChartWidget
            data={data}
            title={"Velocity: X,Y"}
            dataStrokes={[{key: "AccX", color: "#8884d8"},
                          {key: "AccY", color: "#880088"}]}
            xAxisDataKey={"t"}
            gridArea="vel_xy">
          </LineChartWidget>

          <Box gridArea="terminal">
            <TerminalContextProvider>
              <ReactTerminal
                welcomeMessage={<p>CPPS Groundstation</p>}
                commands={commands}
                theme="material-dark"
                showControlBar={false}
              />
            </TerminalContextProvider>
          </Box>

          <Box gridArea="status" background="light-2">
            <h4 align="center"> Status</h4>
            <p>blah blah</p>
          </Box>

          <Box gridArea="gps" >
            <img src={mapImage} height="100%"></img>
          </Box>
        </Grid>
        ) : (<Text>Grid is not supported by your browser</Text>)
        }
      </Grommet>
      );
  }
}
export default App
// ReactDOM.render(<App/>, mountNode)
