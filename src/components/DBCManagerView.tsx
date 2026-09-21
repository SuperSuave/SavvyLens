import React, { useState } from 'react';
import { Database, Plus, Edit3, Trash2, Layers, CheckCircle } from 'lucide-react';
import { DBCMessage, DBCSignal } from '../types';

interface DBCManagerViewProps {
  dbcMessages: DBCMessage[];
  setDbcMessages: React.Dispatch<React.SetStateAction<DBCMessage[]>>;
}

export const DBCManagerView: React.FC<DBCManagerViewProps> = ({ dbcMessages, setDbcMessages }) => {
  const [selectedMsgIndex, setSelectedMsgIndex] = useState<number>(0);
  const [newMsgName, setNewMsgName] = useState('');
  const [newMsgId, setNewMsgId] = useState('');
  const [newMsgSender, setNewMsgSender] = useState('');

  const currentMsg = dbcMessages[selectedMsgIndex];

  const handleAddMessage = (e: React.FormEvent) => {
    e.preventDefault();
    if (!newMsgName || !newMsgId) return;
    const decId = parseInt(newMsgId.startsWith('0x') ? newMsgId : '0x' + newMsgId, 16) || parseInt(newMsgId, 10);
    const hexId = '0x' + decId.toString(16).toUpperCase();

    const newMsg: DBCMessage = {
      id: decId,
      hexId,
      name: newMsgName,
      dlc: 8,
      sender: newMsgSender || 'ECU',
      signals: [
        { name: 'DefaultSignal', startBit: 0, length: 8, isBigEndian: false, isSigned: false, factor: 1, offset: 0, min: 0, max: 255, unit: 'count', receiver: ['All'] }
      ]
    };

    setDbcMessages([...dbcMessages, newMsg]);
    setNewMsgName('');
    setNewMsgId('');
    setNewMsgSender('');
  };

  return (
    <div className="flex-1 flex overflow-hidden bg-slate-950 text-slate-100">
      {/* Left Message List */}
      <div className="w-80 bg-slate-900 border-r border-slate-800 flex flex-col">
        <div className="p-3 border-b border-slate-800 flex items-center justify-between">
          <div className="flex items-center space-x-2">
            <Database className="w-4 h-4 text-blue-400" />
            <h2 className="font-bold text-sm text-white">DBC Messages</h2>
          </div>
          <span className="text-xs bg-blue-600/20 text-blue-400 px-2 py-0.5 rounded-full border border-blue-500/30">
            {dbcMessages.length} Messages
          </span>
        </div>

        <div className="flex-1 overflow-auto p-2 space-y-1">
          {dbcMessages.map((msg, idx) => (
            <div
              key={msg.id}
              onClick={() => setSelectedMsgIndex(idx)}
              className={`p-2.5 rounded-xl cursor-pointer transition border ${
                selectedMsgIndex === idx
                  ? 'bg-blue-600/15 border-blue-500/40 text-blue-300'
                  : 'bg-slate-950/60 border-slate-800/80 hover:bg-slate-800/60 text-slate-300'
              }`}
            >
              <div className="flex items-center justify-between">
                <span className="font-mono font-bold text-xs text-blue-400">{msg.hexId}</span>
                <span className="text-[10px] bg-slate-800 px-1.5 py-0.5 rounded text-slate-400">{msg.sender}</span>
              </div>
              <div className="font-medium text-xs text-slate-200 mt-1">{msg.name}</div>
              <div className="text-[11px] text-slate-400 mt-1 flex justify-between">
                <span>DLC: {msg.dlc} bytes</span>
                <span>{msg.signals.length} signals</span>
              </div>
            </div>
          ))}
        </div>

        {/* Add Message Form */}
        <form onSubmit={handleAddMessage} className="p-3 bg-slate-950 border-t border-slate-800 space-y-2">
          <div className="text-xs font-semibold text-slate-300">Add New DBC Message</div>
          <div className="grid grid-cols-2 gap-2">
            <input
              type="text"
              placeholder="Name (e.g. Engine RPM)"
              value={newMsgName}
              onChange={e => setNewMsgName(e.target.value)}
              className="bg-slate-900 border border-slate-700 rounded px-2 py-1 text-xs text-white placeholder-slate-500"
            />
            <input
              type="text"
              placeholder="ID (e.g. 0x150)"
              value={newMsgId}
              onChange={e => setNewMsgId(e.target.value)}
              className="bg-slate-900 border border-slate-700 rounded px-2 py-1 text-xs text-white placeholder-slate-500 font-mono"
            />
          </div>
          <button
            type="submit"
            className="w-full py-1.5 bg-blue-600 hover:bg-blue-500 text-white rounded text-xs font-medium transition"
          >
            Add Message Definition
          </button>
        </form>
      </div>

      {/* Right Signal Inspector */}
      <div className="flex-1 flex flex-col p-6 overflow-y-auto">
        {currentMsg ? (
          <div className="max-w-3xl space-y-6">
            <div className="flex items-center justify-between pb-4 border-b border-slate-800">
              <div>
                <div className="flex items-center space-x-3">
                  <span className="text-lg font-mono font-bold text-blue-400">{currentMsg.hexId}</span>
                  <h2 className="text-lg font-bold text-white">{currentMsg.name}</h2>
                </div>
                <p className="text-xs text-slate-400 mt-1">Sender: {currentMsg.sender} | DLC: {currentMsg.dlc} Bytes</p>
              </div>
              <div className="flex items-center space-x-2">
                <span className="text-xs px-2.5 py-1 rounded-lg bg-emerald-500/10 text-emerald-400 border border-emerald-500/20 flex items-center space-x-1">
                  <CheckCircle className="w-3.5 h-3.5 mr-1" /> DBC Validated
                </span>
              </div>
            </div>

            <div>
              <h3 className="text-sm font-semibold text-slate-200 mb-3 flex items-center justify-between">
                <span>Signals in Message ({currentMsg.signals.length})</span>
                <span className="text-xs text-slate-400">Little Endian / Motorola & Intel</span>
              </h3>

              <div className="space-y-3">
                {currentMsg.signals.map((sig, idx) => (
                  <div key={idx} className="bg-slate-900 border border-slate-800 rounded-xl p-4 space-y-3">
                    <div className="flex items-center justify-between">
                      <div className="font-bold text-sm text-blue-300">{sig.name}</div>
                      <div className="text-xs font-mono bg-slate-950 px-2 py-1 rounded border border-slate-800 text-slate-300">
                        Bits: {sig.startBit} - {sig.startBit + sig.length - 1} ({sig.length} bits)
                      </div>
                    </div>

                    <div className="grid grid-cols-2 md:grid-cols-4 gap-3 text-xs">
                      <div className="bg-slate-950 p-2 rounded border border-slate-800/80">
                        <span className="text-slate-400 block text-[10px]">Factor / Scale</span>
                        <span className="font-mono text-slate-200 font-medium">{sig.factor}</span>
                      </div>
                      <div className="bg-slate-950 p-2 rounded border border-slate-800/80">
                        <span className="text-slate-400 block text-[10px]">Offset</span>
                        <span className="font-mono text-slate-200 font-medium">{sig.offset}</span>
                      </div>
                      <div className="bg-slate-950 p-2 rounded border border-slate-800/80">
                        <span className="text-slate-400 block text-[10px]">Range</span>
                        <span className="font-mono text-slate-200 font-medium">{sig.min} ~ {sig.max} {sig.unit}</span>
                      </div>
                      <div className="bg-slate-950 p-2 rounded border border-slate-800/80">
                        <span className="text-slate-400 block text-[10px]">Endianness</span>
                        <span className="font-mono text-slate-200 font-medium">{sig.isBigEndian ? 'Big (Motorola)' : 'Little (Intel)'}</span>
                      </div>
                    </div>
                  </div>
                ))}
              </div>
            </div>
          </div>
        ) : (
          <div className="flex-1 flex items-center justify-center text-slate-500">
            Select a message to inspect signals
          </div>
        )}
      </div>
    </div>
  );
};
