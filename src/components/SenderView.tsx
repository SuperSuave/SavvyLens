import React, { useState } from 'react';
import { Send, Play, Square, RefreshCw, Cpu, Zap } from 'lucide-react';

interface SenderViewProps {
  onSendFrame: (id: string, data: number[], intervalMs?: number) => void;
}

export const SenderView: React.FC<SenderViewProps> = ({ onSendFrame }) => {
  const [canId, setCanId] = useState('0x123');
  const [dlc, setDlc] = useState(8);
  const [dataBytes, setDataBytes] = useState('01, 02, 03, 04, 05, 06, 07, 08');
  const [intervalMs, setIntervalMs] = useState(0);
  const [isCyclicSending, setIsCyclicSending] = useState(false);
  const [sentHistory, setSentHistory] = useState<string[]>([]);

  const handleSend = () => {
    const bytes = dataBytes.split(',').map(b => parseInt(b.trim(), 16) || 0);
    onSendFrame(canId, bytes, intervalMs > 0 ? intervalMs : undefined);
    setSentHistory(prev => [`[${new Date().toLocaleTimeString()}] Sent ${canId} [${bytes.map(b => b.toString(16).padStart(2, '0')).join(' ')}]`, ...prev.slice(0, 15)]);
  };

  return (
    <div className="flex-1 flex flex-col bg-slate-950 text-slate-100 p-6 overflow-y-auto">
      <div className="max-w-4xl mx-auto w-full space-y-6">
        <div className="flex items-center justify-between pb-4 border-b border-slate-800">
          <div>
            <h2 className="text-lg font-bold text-white flex items-center space-x-2">
              <Send className="w-5 h-5 text-blue-400" />
              <span>Custom Frame Sender & Cyclic Fuzzing</span>
            </h2>
            <p className="text-xs text-slate-400 mt-0.5">Craft and transmit custom CAN frames or setup periodic injection streams</p>
          </div>
        </div>

        <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
          <div className="md:col-span-2 bg-slate-900 border border-slate-800 rounded-2xl p-5 space-y-4">
            <div className="text-xs font-semibold text-slate-300">Frame Transmission Parameters</div>

            <div className="grid grid-cols-2 gap-4">
              <div>
                <label className="block text-xs text-slate-400 mb-1">CAN ID (Hex)</label>
                <input
                  type="text"
                  value={canId}
                  onChange={e => setCanId(e.target.value)}
                  className="w-full bg-slate-950 border border-slate-700 rounded-lg px-3 py-2 text-xs text-white font-mono"
                />
              </div>
              <div>
                <label className="block text-xs text-slate-400 mb-1">DLC (Data Length)</label>
                <select
                  value={dlc}
                  onChange={e => setDlc(Number(e.target.value))}
                  className="w-full bg-slate-950 border border-slate-700 rounded-lg px-3 py-2 text-xs text-white font-mono"
                >
                  {[1, 2, 3, 4, 5, 6, 7, 8].map(n => (
                    <option key={n} value={n}>{n} Bytes</option>
                  ))}
                </select>
              </div>
            </div>

            <div>
              <label className="block text-xs text-slate-400 mb-1">Data Bytes (Hex separated by comma)</label>
              <input
                type="text"
                value={dataBytes}
                onChange={e => setDataBytes(e.target.value)}
                className="w-full bg-slate-950 border border-slate-700 rounded-lg px-3 py-2 text-xs text-white font-mono"
                placeholder="00, 00, 00, 00, 00, 00, 00, 00"
              />
            </div>

            <div>
              <label className="block text-xs text-slate-400 mb-1">Cyclic Interval (0 for single shot)</label>
              <div className="flex items-center space-x-3">
                <input
                  type="number"
                  value={intervalMs}
                  onChange={e => setIntervalMs(Number(e.target.value))}
                  className="w-full bg-slate-950 border border-slate-700 rounded-lg px-3 py-2 text-xs text-white font-mono"
                  placeholder="Interval in ms"
                />
                <span className="text-xs text-slate-400 whitespace-nowrap">ms</span>
              </div>
            </div>

            <div className="pt-2 flex space-x-3">
              <button
                onClick={handleSend}
                className="flex-1 py-2.5 bg-blue-600 hover:bg-blue-500 text-white rounded-xl font-medium text-xs transition flex items-center justify-center space-x-2 shadow-sm"
              >
                <Send className="w-4 h-4" />
                <span>Transmit Frame Now</span>
              </button>
            </div>
          </div>

          {/* Transmission History */}
          <div className="bg-slate-900 border border-slate-800 rounded-2xl p-5 flex flex-col">
            <div className="text-xs font-semibold text-slate-300 mb-3 flex items-center justify-between">
              <span>Transmission Log</span>
              <span className="text-[10px] text-slate-400 font-mono">{sentHistory.length} sent</span>
            </div>
            <div className="flex-1 bg-slate-950 rounded-xl p-3 border border-slate-800 font-mono text-[11px] text-slate-300 overflow-y-auto space-y-1.5 max-h-[300px]">
              {sentHistory.length === 0 ? (
                <span className="text-slate-600">No frames transmitted yet.</span>
              ) : (
                sentHistory.map((item, i) => (
                  <div key={i} className="text-blue-400">{item}</div>
                ))
              )}
            </div>
          </div>
        </div>
      </div>
    </div>
  );
};
