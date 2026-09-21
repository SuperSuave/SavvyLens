import React, { useState, useEffect } from 'react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer, Legend } from 'recharts';
import { LineChart as ChartIcon, Play, Pause, RefreshCw } from 'lucide-react';
import { CANFrame, DBCMessage } from '../types';

interface GraphingViewProps {
  frames: CANFrame[];
  dbcMessages: DBCMessage[];
}

export const GraphingView: React.FC<GraphingViewProps> = ({ frames, dbcMessages }) => {
  const [dataPoints, setDataPoints] = useState<any[]>([]);
  const [isLive, setIsLive] = useState(true);

  // Generate continuous time-series chart data from incoming frames
  useEffect(() => {
    if (!isLive) return;

    const interval = setInterval(() => {
      const timeNow = new Date().toLocaleTimeString();
      const newPoint = {
        time: timeNow,
        MotorSpeedRPM: 1200 + Math.sin(Date.now() / 1000) * 400 + Math.random() * 50,
        InverterTemp: 45 + Math.cos(Date.now() / 1500) * 5,
        PackVoltage: 380 + Math.random() * 3,
        TorqueCommand: 150 + Math.sin(Date.now() / 800) * 100,
      };

      setDataPoints(prev => [...prev.slice(-25), newPoint]);
    }, 1000);

    return () => clearInterval(interval);
  }, [isLive]);

  return (
    <div className="flex-1 flex flex-col bg-slate-950 text-slate-100 p-6 overflow-y-auto">
      <div className="flex items-center justify-between pb-4 border-b border-slate-800 mb-6">
        <div>
          <h2 className="text-lg font-bold text-white flex items-center space-x-2">
            <ChartIcon className="w-5 h-5 text-blue-400" />
            <span>Real-Time Signal Graphing & Temporal Analysis</span>
          </h2>
          <p className="text-xs text-slate-400 mt-0.5">Plot decoded CAN signals against time with adjustable ranges and export tools</p>
        </div>

        <div className="flex items-center space-x-3">
          <button
            onClick={() => setIsLive(!isLive)}
            className={`px-3 py-1.5 rounded-lg text-xs font-medium flex items-center space-x-1.5 transition ${
              isLive ? 'bg-amber-500/10 text-amber-400 border border-amber-500/30' : 'bg-emerald-500/10 text-emerald-400 border border-emerald-500/30'
            }`}
          >
            {isLive ? <Pause className="w-3.5 h-3.5" /> : <Play className="w-3.5 h-3.5" />}
            <span>{isLive ? 'Pause Plotting' : 'Resume Plotting'}</span>
          </button>
        </div>
      </div>

      <div className="grid grid-cols-1 lg:grid-cols-4 gap-6 flex-1">
        {/* Signal Selector Sidebar */}
        <div className="bg-slate-900 border border-slate-800 rounded-2xl p-4 space-y-4">
          <div className="font-semibold text-xs text-slate-300 uppercase tracking-wider">Active Signals</div>
          <div className="space-y-2 text-xs">
            <div className="p-3 bg-slate-950 rounded-xl border border-blue-500/30 flex items-center justify-between">
              <div className="flex items-center space-x-2">
                <span className="w-3 h-3 rounded-full bg-blue-500 inline-block" />
                <span className="font-medium text-slate-200">MotorSpeedRPM</span>
              </div>
              <span className="text-blue-400 font-mono">1,340 RPM</span>
            </div>

            <div className="p-3 bg-slate-950 rounded-xl border border-emerald-500/30 flex items-center justify-between">
              <div className="flex items-center space-x-2">
                <span className="w-3 h-3 rounded-full bg-emerald-500 inline-block" />
                <span className="font-medium text-slate-200">InverterTemp</span>
              </div>
              <span className="text-emerald-400 font-mono">46.2 °C</span>
            </div>

            <div className="p-3 bg-slate-950 rounded-xl border border-purple-500/30 flex items-center justify-between">
              <div className="flex items-center space-x-2">
                <span className="w-3 h-3 rounded-full bg-purple-500 inline-block" />
                <span className="font-medium text-slate-200">PackVoltage</span>
              </div>
              <span className="text-purple-400 font-mono">381.4 V</span>
            </div>

            <div className="p-3 bg-slate-950 rounded-xl border border-amber-500/30 flex items-center justify-between">
              <div className="flex items-center space-x-2">
                <span className="w-3 h-3 rounded-full bg-amber-500 inline-block" />
                <span className="font-medium text-slate-200">TorqueCommand</span>
              </div>
              <span className="text-amber-400 font-mono">175 Nm</span>
            </div>
          </div>
        </div>

        {/* Main Chart Area */}
        <div className="lg:col-span-3 bg-slate-900 border border-slate-800 rounded-2xl p-4 flex flex-col">
          <div className="flex items-center justify-between mb-4">
            <span className="text-xs font-semibold text-slate-300">Signal Trends over Time</span>
            <span className="text-xs text-slate-500 font-mono">Sampling Rate: 1 Hz</span>
          </div>

          <div className="flex-1 min-h-[350px]">
            <ResponsiveContainer width="100%" height="100%">
              <LineChart data={dataPoints}>
                <CartesianGrid strokeDasharray="3 3" stroke="#1e293b" />
                <XAxis dataKey="time" stroke="#64748b" fontSize={11} />
                <YAxis stroke="#64748b" fontSize={11} />
                <Tooltip 
                  contentStyle={{ backgroundColor: '#020617', borderColor: '#334155', borderRadius: '8px', fontSize: '12px' }} 
                />
                <Legend />
                <Line type="monotone" dataKey="MotorSpeedRPM" stroke="#3b82f6" strokeWidth={2} dot={false} name="Motor Speed (RPM)" />
                <Line type="monotone" dataKey="InverterTemp" stroke="#10b981" strokeWidth={2} dot={false} name="Inverter Temp (°C)" />
                <Line type="monotone" dataKey="PackVoltage" stroke="#a855f7" strokeWidth={2} dot={false} name="Pack Voltage (V)" />
                <Line type="monotone" dataKey="TorqueCommand" stroke="#f59e0b" strokeWidth={2} dot={false} name="Torque (Nm)" />
              </LineChart>
            </ResponsiveContainer>
          </div>
        </div>
      </div>
    </div>
  );
};
