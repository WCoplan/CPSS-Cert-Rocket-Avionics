import './App.css';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';
import { Box } from 'grommet';

const LineChartWidget = (props) => {
    const { gridArea, title, data, xAxisDataKey, dataStrokes, maxData } = props

    return (
        <Box gridArea={gridArea} background="light-2">
            <h4 align="center">{title}</h4>
            <ResponsiveContainer>
                <LineChart data={data} margin={{ top: 5, right: 30, left: 0, bottom: 5 }}>
                <CartesianGrid strokeDasharray="3 3" />
                <XAxis dataKey={xAxisDataKey} domain={[0,100]} />
                <YAxis height='100' domain={[0, maxData]}/>
                <Tooltip />
                <Legend />
                {dataStrokes.map((line) => {
                    return <Line type="monotone" isAnimationActive={false} dataKey={line.key} stroke={line.color} />;
                })}
                </LineChart>
            </ResponsiveContainer>
        </Box>
    )
}

export default LineChartWidget;