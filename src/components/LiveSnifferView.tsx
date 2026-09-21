import React, { useState } from 'react';
import { Search, Trash2, Filter, ArrowUpRight, ArrowDownLeft, Shield, Eye } from 'lucide-react';
import { CANFrame, DBCMessage } from '../types';

interface LiveSnifferViewProps {
  frames: CANFrame[];
  onClearFrames: () => void;
  dbcMessages: DBCMessage[];
  onSendCustomFrame: (id: string, data: number[]) => void;
}

export const LiveSnifferView: React.FC<LiveSnifferViewProps> = ({
  frames,
  onClearFrames,
  dbcMessages,
  onSendCustomFrame
}) => {
  const [searchTerm, setSearchTerm] = useState('');
  const [selectedFrame, setSelectedFrame] = useState<CANFrame | null>(null);

  const filteredFrames = frames.filter(f => 
    f.id.toLowerCase().includes(searchTerm.toLowerCase()) ||
    (f.name && f.name.toLowerCase().includes(searchTerm.toLowerCase())) ||
    f.ascii.toLowerCase().includes(searchTerm.toLowerCase())
  );

  const matchedDbcMsg = selectedFrame 
    ? dbcMessages.find(m => m.hexId.toLowerCase() === selectedFrame.id.toLowerCase() || m.id === selectedFrame.decimalId)
    : null;

  return (
    <div className="flex-1 flex overflow-hidden bg-slate-950 text-slate-100">
      <div className="flex-1 flex flex-col min-w-0">
        {/* Toolbar */}
        <div className="p-3 bg-slate-900 border-b border-slate-800 flex items-center justify-between gap-4">
          <div className="flex items-center space-x-2 flex-1 max-w-md">
            <div className="relative w-full">
              <Search className="absolute left-3 top-2.5 w-4 h-4 text-slate-400" />
              <input
                type="text"
                placeholder="Filter by CAN ID (e.g. 0x123), Name, or ASCII..."
                value={searchTerm}
                onChange={(e) => setSearchTerm(e.target.value)}
                className="w-full bg-slate-950 border border-slate-700 rounded-lg pl-9 pr-3 py-1.5 text-xs text-white placeholder-slate-400 focus:outline-none focus:border-blue-500 font-mono"
              />
            </div>
          </div>

          <div className="flex items-center space-x-2">
            <span className="text-xs text-slate-400">Showing {filteredFrames.length} of {frames.length} frames</span>
            <button
              onClick={onClearFrames}
              className="px-3 py-1.5 bg-slate-800 hover:bg-slate-700 text-slate-200 rounded-lg text-xs font-medium flex items-center space-x-1.5 transition border border-slate-700"
            >
              <Trash2 className="w-3.5 h-3.5 text-rose-400" />
              <span>Clear Log</span>
            </button>
          </div>
        </div>

        {/* CAN Data Grid Table */}
        <div className="flex-1 overflow-auto">
          <table className="w-full text-left border-collapse font-mono text-xs">
            <thead className="bg-slate-900/90 text-slate-400 sticky top-0 border-b border-slate-800 select-none z-10">
              <tr>
                <th className="py-2.5 px-3 font-medium">Timestamp</th>
                <th className="py-2.5 px-3 font-medium">Bus</th>
                <th className="py-2.5 px-3 font-medium">Dir</th>
                <th className="py-2.5 px-3 font-medium">CAN ID</th>
                <th className="py-2.5 px-3 font-medium">Message Name</th>
                <th className="py-2.5 px-3 font-medium">DLC</th>
                <th className="py-2.5 px-3 font-medium">Data Bytes (Hex)</th>
                <th className="py-2.5 px-3 font-medium">ASCII</th>
                <th className="py-2.5 px-3 font-medium text-right">Count</th>
              </tr>
            </thead>
            <tbody className="divide-y divide-slate-900">
              {filteredFrames.length === 0 ? (
                <tr>
                  <td colSpan={9} className="text-center py-12 text-slate-500 font-sans">
                    No CAN frames match your filter. Waiting for bus traffic...
                  </td>
                </tr>
              ) : (
                filteredFrames.map((frame, idx) => {
                  const isSelected = selectedFrame?.id === frame.id && selectedFrame?.timestamp === frame.timestamp;
                  return (
                    <tr 
                      key={`${frame.id}-${idx}`}
                      onClick={() => setSelectedFrame(frame)}
                      className={`cursor-pointer transition-colors ${
                        isSelected 
                          ? 'bg-blue-600/20 border-l-2 border-blue-500' 
                          : 'hover:bg-slate-900/60'
                      }`}
                    >
                      <td className="py-2 px-3 text-slate-400">+{frame.timestamp.toFixed(3)}s</td>
                      <td className="py-2 px-3 text-slate-300">can{frame.bus}</td>
                      <td className="py-2 px-3">
                        {frame.direction === 'TX' ? (
                          <span className="flex items-center text-amber-400"><ArrowUpRight className="w-3 h-3 mr-0.5" /> TX</span>
                        ) : (
                          <span className="flex items-center text-emerald-400"><ArrowDownLeft className="w-3 h-3 mr-0.5" /> RX</span>
                        )}
                      </td>
                      <td className="py-2 px-3 font-bold text-blue-400">{frame.id}</td>
                      <td className="py-2 px-3 text-slate-200 font-sans font-medium">{frame.name || 'Unknown'}</td>
                      <td className="py-2 px-3 text-slate-400">{frame.dlc}</td>
                      <td className="py-2 px-3 tracking-wider text-slate-300">
                        {frame.data.map(b => b.toString(16).toUpperCase().padStart(2, '0')).join(' ')}
                      </td>
                      <td className="py-2 px-3 text-slate-400 font-sans">{frame.ascii}</td>
                      <td className="py-2 px-3 text-right text-slate-400">{frame.count}</td>
                    </tr>
                  );
                })
              )}
            </tbody>
          </table>
        </div>
      </div>

      {/* Frame Detail Inspector Sidebar */}
      {selectedFrame && (
        <div className="w-96 bg-slate-900 border-l border-slate-800 flex flex-col p-4 overflow-y-auto">
          <div className="flex items-center justify-between pb-3 border-b border-slate-800 mb-4">
            <div className="flex items-center space-x-2">
              <Eye className="w-4 h-4 text-blue-400" />
              <h3 className="font-bold text-sm text-white">Frame Details</h3>
            </div>
            <button 
              onClick={() => setSelectedFrame(null)}
              className="text-slate-400 hover:text-white text-xs px-2 py-1 rounded bg-slate-800"
            >
              Close
            </button>
          </div>

          <div className="space-y-4 text-xs">
            <div className="bg-slate-950 p-3 rounded-lg border border-slate-800 space-y-2">
              <div className="flex justify-between">
                <span className="text-slate-400">CAN ID:</span>
                <span className="font-mono font-bold text-blue-400 text-sm">{selectedFrame.id} ({selectedFrame.decimalId})</span>
              </div>
              <div className="flex justify-between">
                <span className="text-slate-400">Message Name:</span>
                <span className="font-medium text-slate-200">{selectedFrame.name || 'Unknown'}</span>
              </div>
              <div className="flex justify-between">
                <span className="text-slate-400">Timestamp:</span>
                <span className="font-mono text-slate-300">+{selectedFrame.timestamp.toFixed(4)}s</span>
              </div>
              <div className="flex justify-between">
                <span className="text-slate-400">Bus / Direction:</span>
                <span className="font-mono text-slate-300">can{selectedFrame.bus} ({selectedFrame.direction || 'RX'})</span>
              </div>
              <div className="flex justify-between">
                <span className="text-slate-400">DLC:</span>
                <span className="font-mono text-slate-300">{selectedFrame.dlc} bytes</span>
              </div>
            </div>

            <div>
              <div className="text-slate-400 font-semibold mb-2">Payload Bytes (Hex / Decimal)</div>
              <div className="grid grid-cols-4 gap-2 font-mono">
                {selectedFrame.data.map((byte, i) => (
                  <div key={i} className="bg-slate-950 p-2 rounded border border-slate-800 text-center">
                    <div className="text-[10px] text-slate-500">Byte {i}</div>
                    <div className="text-blue-400 font-bold">{byte.toString(16).toUpperCase().padStart(2, '0')}</div>
                    <div className="text-[10px] text-slate-400">{byte}</div>
                  </div>
                ))}
              </div>
            </div>

            {matchedDbcMsg ? (
              <div>
                <div className="text-slate-300 font-semibold mb-2 flex items-center justify-between">
                  <span>DBC Decoded Signals</span>
                  <span className="text-[10px] bg-emerald-500/10 text-emerald-400 px-2 py-0.5 rounded border border-emerald-500/20">Matched</span>
                </div>
                <div className="space-y-2">
                  {matchedDbcMsg.signals.map((sig, sIdx) => {
                    // Simple dummy signal extraction calculation
                    const rawVal = (selectedFrame.data[0] || 0) + ((selectedFrame.data[1] || 0) << 8);
                    const scaled = rawVal * sig.factor + sig.offset;
                    return (
                      <div key={sIdx} className="bg-slate-950 p-2.5 rounded-lg border border-slate-800">
                        <div className="flex justify-between font-medium text-slate-200">
                          <span>{sig.name}</span>
                          <span className="text-blue-400 font-mono">{scaled.toFixed(2)} {sig.unit}</span>
                        </div>
                        <div className="text-[10px] text-slate-400 mt-1 flex justify-between">
                          <span>Bits {sig.startBit} - {sig.startBit + sig.length}</span>
                          <span>Range: {sig.min} to {sig.max} {sig.unit}</span>
                        </div>
                      </div>
                    );
                  })}
                </div>
              </div>
            ) : (
              <div className="p-3 bg-slate-950 rounded-lg border border-slate-800 text-slate-400 text-center">
                No DBC signal definition found for CAN ID {selectedFrame.id}. Add mapping in DBC Manager.
              </div>
            )}

            <button
              onClick={() => onSendCustomFrame(selectedFrame.id, selectedFrame.data)}
              className="w-full py-2 bg-blue-600 hover:bg-blue-500 text-white font-medium rounded-lg transition shadow-sm"
            >
              Resend This Frame
            </button>
          </div>
        </div>
      )}
    </div>
  );
};
