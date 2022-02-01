import './App.css';
import React from 'react';

import {  LineChart,
          Line,
          XAxis,
          YAxis,
          CartesianGrid,
          Tooltip, Legend,
          ResponsiveContainer } from 'recharts';
import { Box } from 'grommet';

class LineChartWidget extends React.Component {

  constructor(props) {
    super();
    this.state = {data : props.data,
                  dataKeys: props.dataKeys,
                  dataStrokes: props.dataStrokes,
                  xAxisDataKey: props.xAxisDataKey,
                  title: props.title,
                  xAxisTitle: props.xAxisTitle,
                  yAxisTitle: props.yAxisTitle,
                  gridArea : props.gridArea};
  }

  render() {
    return(
      <Box gridArea={this.state.gridArea} background="light-2">
        <h4 align="center">{this.state.title}</h4>
        <ResponsiveContainer>
          <LineChart
            data={this.state.data}
            margin={{
              top: 5,
              right: 30,
              left: 0,
              bottom: 5,
            }}>
            <CartesianGrid strokeDasharray="3 3" />
            <XAxis dataKey={this.state.xAxisDataKey} />
            <YAxis />
            <Tooltip />
            <Legend />
            {this.state.dataStrokes.map(function(line, i){
                return <Line type="monotone" dataKey={line.key} stroke={line.color} />;
            })
            }
          </LineChart>
      </ResponsiveContainer>
    </Box>
    );
  }
}
export default LineChartWidget
// ReactDOM.render(<App/>, mountNode)
