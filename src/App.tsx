import React, { useState, useEffect } from 'react';
import { Header } from './components/Header';
import { NavigationRail } from './components/NavigationRail';
import { LiveSnifferView } from './components/LiveSnifferView';
import { DBCManagerView } from './components/DBCManagerView';
import { GraphingView } from './components/GraphingView';
import { SenderView } from './components/SenderView';
import { ScriptingView } from './components/ScriptingView';
import { ConnectionModal } from './components/ConnectionModal';
import { MOCK_FRAMES, INITIAL_CONNECTIONS, INITIAL_DBC_MESSAGES } from './data/mockData';
import { CANFrame, ConnectionConfig, DBCMessage, ScriptItem } from './types';

export default function App() {
  const [activeTab, setActiveTab] = useState('sniffer');
  const [frames, setFrames] = useState<CANFrame[]>(MOCK_FRAMES);
  const [connections, setConnections] = useState<ConnectionConfig[]>(INITIAL_CONNECTIONS);
  const [dbcMessages, setDbcMessages] = useState<DBCMessage[]>(INITIAL_DBC_MESSAGES);
  const [isCapturing, setIsCapturing] = useState(true);
  const [isConnModalOpen, setIsConnModalOpen] = useState(false);

  // Simulate incoming real-time CAN traffic
  useEffect(() => {
    if (!isCapturing) return;

    const interval = setInterval(() => {
      const sampleIds = ['0x123', '0x204', '0x318', '0x450', '0x550'];
      const randomId = sampleIds[Math.floor(Math.random() * sampleIds.length)];
      const decId = parseInt(randomId.replace('0x', ''), 16);
      const matchedMsg = dbcMessages.find(m => m.hexId === randomId);
      
      const randomBytes = Array.from({ length: 8 }, () => Math.floor(Math.random() * 256));
      const newFrame: CANFrame = {
        id: randomId,
        decimalId: decId,
        name: matchedMsg ? matchedMsg.name : 'Periodic_Frame',
        timestamp: frames.length > 0 ? frames[frames.length - 1].timestamp + 0.045 : 100.0,
        bus: Math.random() > 0.8 ? 1 : 0,
        dlc: 8,
        data: randomBytes,
        ascii: randomBytes.map(b => (b >= 32 && b <= 126 ? String.fromCharCode(b) : '.')).join(''),
        count: Math.floor(Math.random() * 500) + 1,
        direction: 'RX'
      };

      setFrames(prev => [...prev.slice(-300), newFrame]);
    }, 600);

    return () => clearInterval(interval);
  }, [isCapturing, frames, dbcMessages]);

  const handleClearFrames = () => {
    setFrames([]);
  };

  const handleSendCustomFrame = (id: string, data: number[]) => {
    const decId = parseInt(id.replace('0x', ''), 16);
    const newFrame: CANFrame = {
      id,
      decimalId: decId,
      name: 'Manual_TX_Frame',
      timestamp: frames.length > 0 ? frames[frames.length - 1].timestamp + 0.01 : 100.0,
      bus: 0,
      dlc: data.length,
      data,
      ascii: data.map(b => (b >= 32 && b <= 126 ? String.fromCharCode(b) : '.')).join(''),
      count: 1,
      direction: 'TX'
    };
    setFrames(prev => [...prev, newFrame]);
  };

  const handleExportLogs = () => {
    const csvContent = "data:text/csv;charset=utf-8," + 
      ["Timestamp,Bus,Direction,ID,Name,DLC,Data"]
      .concat(frames.map(f => `${f.timestamp},${f.bus},${f.direction || 'RX'},${f.id},${f.name || ''},${f.dlc},"${f.data.join(' ')}"`))
      .join("\n");
    const encodedUri = encodeURI(csvContent);
    const link = document.createElement("a");
    link.setAttribute("href", encodedUri);
    link.setAttribute("download", "savvylens_export.csv");
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
  };

  const handleImportLogs = () => {
    // Simulated log import
    alert("Log file import dialog: You can load .trc, .csv, .log, or .dbc files here.");
  };

  const handleRunScript = (script: ScriptItem) => {
    // Simulate script action
    setTimeout(() => {
      handleSendCustomFrame("0x123", [0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04]);
    }, 500);
  };

  return (
    <div className="flex flex-col h-screen w-screen overflow-hidden bg-slate-950 font-sans">
      <Header
        activeTab={activeTab}
        setActiveTab={setActiveTab}
        connections={connections}
        isCapturing={isCapturing}
        setIsCapturing={setIsCapturing}
        onOpenConnections={() => setIsConnModalOpen(true)}
        onExportLogs={handleExportLogs}
        onImportLogs={handleImportLogs}
        frameCount={frames.length}
      />

      <div className="flex flex-1 overflow-hidden">
        <NavigationRail activeTab={activeTab} setActiveTab={setActiveTab} />

        <main className="flex-1 flex flex-col min-w-0 overflow-hidden">
          {activeTab === 'sniffer' && (
            <LiveSnifferView
              frames={frames}
              onClearFrames={handleClearFrames}
              dbcMessages={dbcMessages}
              onSendCustomFrame={handleSendCustomFrame}
            />
          )}
          {activeTab === 'dbc' && (
            <DBCManagerView
              dbcMessages={dbcMessages}
              setDbcMessages={setDbcMessages}
            />
          )}
          {activeTab === 'graphing' && (
            <GraphingView
              frames={frames}
              dbcMessages={dbcMessages}
            />
          )}
          {activeTab === 'sender' && (
            <SenderView
              onSendFrame={handleSendCustomFrame}
            />
          )}
          {activeTab === 'scripting' && (
            <ScriptingView
              onRunScript={handleRunScript}
            />
          )}
          {activeTab === 'connections' && (
            <div className="flex-1 p-6 overflow-y-auto">
              <div className="max-w-2xl mx-auto space-y-4">
                <h2 className="text-lg font-bold text-white">CAN Connections & Log Servers</h2>
                <p className="text-xs text-slate-400">Manage your active hardware interfaces, serial ports, and CAN over MQTT bridges.</p>
                <button
                  onClick={() => setIsConnModalOpen(true)}
                  className="px-4 py-2 bg-blue-600 hover:bg-blue-500 text-white rounded-xl text-xs font-semibold transition"
                >
                  Open Connection Manager
                </button>
              </div>
            </div>
          )}
        </main>
      </div>

      <ConnectionModal
        isOpen={isConnModalOpen}
        onClose={() => setIsConnModalOpen(false)}
        connections={connections}
        setConnections={setConnections}
      />
    </div>
  );
}
