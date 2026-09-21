import React, { useState } from 'react';
import { Cpu, Radio, Plus, Check, Trash2, X } from 'lucide-react';
import { ConnectionConfig } from '../types';

interface ConnectionModalProps {
  isOpen: boolean;
  onClose: () => void;
  connections: ConnectionConfig[];
  setConnections: React.Dispatch<React.SetStateAction<ConnectionConfig[]>>;
}

export const ConnectionModal: React.FC<ConnectionModalProps> = ({
  isOpen,
  onClose,
  connections,
  setConnections
}) => {
  const [newConnName, setNewConnName] = useState('');
  const [newConnType, setNewConnType] = useState<'GVRET' | 'SocketCAN' | 'Lawicel' | 'MQTT' | 'Simulated'>('GVRET');
  const [newPort, setNewPort] = useState('/dev/ttyUSB0');

  if (!isOpen) return null;

  const handleAdd = (e: React.FormEvent) => {
    e.preventDefault();
    if (!newConnName) return;
    const newConn: ConnectionConfig = {
      id: 'conn-' + Date.now(),
      name: newConnName,
      type: newConnType,
      status: 'Disconnected',
      baudRate: 500000,
      port: newPort,
      isLogging: false
    };
    setConnections([...connections, newConn]);
    setNewConnName('');
  };

  const handleConnectToggle = (id: string) => {
    setConnections(prev => prev.map(c => {
      if (c.id === id) {
        return { ...c, status: c.status === 'Connected' ? 'Disconnected' : 'Connected' };
      }
      return { ...c, status: 'Disconnected' }; // Only one active connection at a time
    }));
  };

  return (
    <div className="fixed inset-0 bg-slate-950/80 backdrop-blur-sm z-50 flex items-center justify-center p-4">
      <div className="bg-slate-900 border border-slate-800 w-full max-w-2xl rounded-2xl shadow-2xl overflow-hidden flex flex-col text-slate-100">
        <div className="p-4 bg-slate-950 border-b border-slate-800 flex items-center justify-between">
          <div className="flex items-center space-x-2">
            <Cpu className="w-5 h-5 text-blue-400" />
            <h2 className="font-bold text-base text-white">CAN Connection & Hardware Manager</h2>
          </div>
          <button onClick={onClose} className="p-1 rounded-lg text-slate-400 hover:text-white">
            <X className="w-5 h-5" />
          </button>
        </div>

        <div className="p-6 space-y-6 overflow-y-auto max-h-[75vh]">
          {/* Active Connections List */}
          <div className="space-y-3">
            <h3 className="text-xs font-semibold text-slate-400 uppercase tracking-wider">Configured Interfaces</h3>
            <div className="space-y-2">
              {connections.map(conn => (
                <div key={conn.id} className="bg-slate-950 border border-slate-800 p-4 rounded-xl flex items-center justify-between">
                  <div>
                    <div className="flex items-center space-x-2">
                      <span className={`w-2.5 h-2.5 rounded-full ${conn.status === 'Connected' ? 'bg-emerald-500' : 'bg-slate-600'}`} />
                      <span className="font-bold text-sm text-white">{conn.name}</span>
                      <span className="text-[10px] bg-blue-600/20 text-blue-400 px-2 py-0.5 rounded border border-blue-500/30">{conn.type}</span>
                    </div>
                    <p className="text-xs text-slate-400 mt-1 font-mono">Port: {conn.port} | Baud: {conn.baudRate}</p>
                  </div>
                  <button
                    onClick={() => handleConnectToggle(conn.id)}
                    className={`px-4 py-2 rounded-xl text-xs font-semibold transition ${
                      conn.status === 'Connected'
                        ? 'bg-rose-600/20 text-rose-400 border border-rose-500/30 hover:bg-rose-600/30'
                        : 'bg-blue-600 hover:bg-blue-500 text-white shadow-sm'
                    }`}
                  >
                    {conn.status === 'Connected' ? 'Disconnect' : 'Connect'}
                  </button>
                </div>
              ))}
            </div>
          </div>

          {/* Add Connection Form */}
          <form onSubmit={handleAdd} className="bg-slate-950 border border-slate-800 p-4 rounded-xl space-y-3">
            <h3 className="text-xs font-semibold text-slate-300">Add New Connection Interface</h3>
            <div className="grid grid-cols-2 gap-3">
              <div>
                <label className="block text-xs text-slate-400 mb-1">Interface Name</label>
                <input
                  type="text"
                  placeholder="e.g. OBD2 USB Adapter"
                  value={newConnName}
                  onChange={e => setNewConnName(e.target.value)}
                  className="w-full bg-slate-900 border border-slate-700 rounded-lg px-3 py-1.5 text-xs text-white"
                />
              </div>
              <div>
                <label className="block text-xs text-slate-400 mb-1">Protocol Type</label>
                <select
                  value={newConnType}
                  onChange={e => setNewConnType(e.target.value as any)}
                  className="w-full bg-slate-900 border border-slate-700 rounded-lg px-3 py-1.5 text-xs text-white"
                >
                  <option value="GVRET">GVRET / SavvyCAN Serial</option>
                  <option value="SocketCAN">SocketCAN (Linux)</option>
                  <option value="Lawicel">Lawicel / CANtact</option>
                  <option value="MQTT">MQTT CAN Bridge</option>
                  <option value="Simulated">Simulated Bus</option>
                </select>
              </div>
            </div>
            <div>
              <label className="block text-xs text-slate-400 mb-1">Port / Device Path</label>
              <input
                type="text"
                value={newPort}
                onChange={e => setNewPort(e.target.value)}
                className="w-full bg-slate-900 border border-slate-700 rounded-lg px-3 py-1.5 text-xs text-white font-mono"
              />
            </div>
            <button
              type="submit"
              className="w-full py-2 bg-blue-600 hover:bg-blue-500 text-white rounded-lg text-xs font-semibold transition"
            >
              Add Interface
            </button>
          </form>
        </div>
      </div>
    </div>
  );
};
