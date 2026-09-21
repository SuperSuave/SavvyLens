import React from 'react';
import { Activity, Radio, Download, Upload, Settings, Play, Pause, RefreshCw, Cpu } from 'lucide-react';
import { ConnectionConfig } from '../types';

interface HeaderProps {
  activeTab: string;
  setActiveTab: (tab: string) => void;
  connections: ConnectionConfig[];
  isCapturing: boolean;
  setIsCapturing: React.Dispatch<React.SetStateAction<boolean>>;
  onOpenConnections: () => void;
  onExportLogs: () => void;
  onImportLogs: () => void;
  frameCount: number;
}

export const Header: React.FC<HeaderProps> = ({
  activeTab,
  setActiveTab,
  connections,
  isCapturing,
  setIsCapturing,
  onOpenConnections,
  onExportLogs,
  onImportLogs,
  frameCount
}) => {
  const activeConn = connections.find(c => c.status === 'Connected') || connections[0];

  return (
    <header className="bg-slate-900 border-b border-slate-800 text-white px-4 py-3 flex flex-wrap items-center justify-between gap-4 select-none">
      <div className="flex items-center space-x-3">
        <div className="bg-blue-600/20 border border-blue-500/40 p-2 rounded-xl flex items-center justify-center text-blue-400 shadow-inner">
          <Radio className="w-6 h-6 animate-pulse" />
        </div>
        <div>
          <div className="flex items-center space-x-2">
            <h1 className="font-bold text-lg tracking-tight bg-gradient-to-r from-blue-400 to-indigo-300 bg-clip-text text-transparent">
              SavvyLens Web
            </h1>
            <span className="text-xs px-2 py-0.5 rounded-full bg-blue-500/10 text-blue-400 border border-blue-500/20 font-medium">
              v2.5 Pro
            </span>
          </div>
          <p className="text-xs text-slate-400">Automotive CAN Bus Analyzer & Reverse Engineering Studio</p>
        </div>
      </div>

      <div className="flex items-center space-x-3 bg-slate-950/60 px-3 py-1.5 rounded-lg border border-slate-800 text-xs">
        <div className="flex items-center space-x-2">
          <span className={`w-2.5 h-2.5 rounded-full ${activeConn?.status === 'Connected' ? 'bg-emerald-500 animate-ping' : 'bg-amber-500'}`} />
          <span className="font-medium text-slate-200">{activeConn?.name || 'No Connection'}</span>
        </div>
        <span className="text-slate-600">|</span>
        <button 
          onClick={onOpenConnections}
          className="text-blue-400 hover:text-blue-300 font-medium underline underline-offset-2 flex items-center space-x-1"
        >
          <Cpu className="w-3.5 h-3.5 mr-1" />
          <span>Configure</span>
        </button>
      </div>

      <div className="flex items-center space-x-2">
        <div className="bg-slate-950 px-3 py-1 rounded-lg border border-slate-800 text-xs text-slate-300 flex items-center space-x-2">
          <Activity className="w-3.5 h-3.5 text-blue-400" />
          <span>Frames: <strong className="text-white font-mono">{frameCount}</strong></span>
        </div>

        <button
          onClick={() => setIsCapturing(!isCapturing)}
          className={`px-3 py-1.5 rounded-lg text-xs font-medium flex items-center space-x-1.5 transition-colors ${
            isCapturing 
              ? 'bg-amber-500/10 text-amber-400 border border-amber-500/30 hover:bg-amber-500/20' 
              : 'bg-emerald-500/10 text-emerald-400 border border-emerald-500/30 hover:bg-emerald-500/20'
          }`}
        >
          {isCapturing ? <Pause className="w-3.5 h-3.5" /> : <Play className="w-3.5 h-3.5" />}
          <span>{isCapturing ? 'Pause Feed' : 'Resume Feed'}</span>
        </button>

        <button
          onClick={onExportLogs}
          title="Export CAN Log"
          className="p-1.5 bg-slate-800 hover:bg-slate-700 text-slate-200 rounded-lg border border-slate-700 transition"
        >
          <Download className="w-4 h-4" />
        </button>

        <button
          onClick={onImportLogs}
          title="Import CAN Log / DBC"
          className="p-1.5 bg-slate-800 hover:bg-slate-700 text-slate-200 rounded-lg border border-slate-700 transition"
        >
          <Upload className="w-4 h-4" />
        </button>
      </div>
    </header>
  );
};
