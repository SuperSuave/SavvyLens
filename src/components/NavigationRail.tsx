import React from 'react';
import { Layers, Database, LineChart, Send, Code, Cpu } from 'lucide-react';

interface NavigationRailProps {
  activeTab: string;
  setActiveTab: (tab: string) => void;
}

export const NavigationRail: React.FC<NavigationRailProps> = ({ activeTab, setActiveTab }) => {
  const tabs = [
    { id: 'sniffer', label: 'Live Sniffer', icon: Layers, description: 'Real-time CAN frame grid & sniffer' },
    { id: 'dbc', label: 'DBC Manager', icon: Database, description: 'Signal decoders & message database' },
    { id: 'graphing', label: 'Graphing & Signals', icon: LineChart, description: 'Real-time time-series plots' },
    { id: 'sender', label: 'Frame Sender', icon: Send, description: 'Cyclic & manual CAN frame transmission' },
    { id: 'scripting', label: 'Scripting Engine', icon: Code, description: 'JavaScript automation & fuzzing' },
    { id: 'connections', label: 'Connections & Logs', icon: Cpu, description: 'Hardware interfaces & log servers' },
  ];

  return (
    <nav className="bg-slate-900/90 border-r border-slate-800 flex flex-col w-64 p-3 select-none shrink-0">
      <div className="text-[11px] font-semibold text-slate-400 uppercase tracking-wider px-3 mb-2">
        Workspace Views
      </div>
      <div className="space-y-1">
        {tabs.map((tab) => {
          const Icon = tab.icon;
          const isActive = activeTab === tab.id;
          return (
            <button
              key={tab.id}
              onClick={() => setActiveTab(tab.id)}
              className={`w-full flex items-start space-x-3 px-3 py-2.5 rounded-xl transition text-left group ${
                isActive 
                  ? 'bg-blue-600/15 text-blue-400 border border-blue-500/30 shadow-sm' 
                  : 'text-slate-400 hover:text-slate-200 hover:bg-slate-800/50 border border-transparent'
              }`}
            >
              <div className={`p-1.5 rounded-lg mt-0.5 ${isActive ? 'bg-blue-600 text-white' : 'bg-slate-800 text-slate-400 group-hover:text-slate-200'}`}>
                <Icon className="w-4 h-4" />
              </div>
              <div>
                <div className={`text-xs font-semibold ${isActive ? 'text-blue-300' : 'text-slate-200'}`}>
                  {tab.label}
                </div>
                <div className="text-[11px] text-slate-400 leading-tight mt-0.5">
                  {tab.description}
                </div>
              </div>
            </button>
          );
        })}
      </div>

      <div className="mt-auto pt-4 border-t border-slate-800 px-3">
        <div className="bg-slate-950/60 p-3 rounded-xl border border-slate-800/80 text-xs">
          <div className="font-medium text-slate-300 mb-1">CAN Bus Status</div>
          <div className="flex justify-between text-slate-400 text-[11px] mb-0.5">
            <span>Bus Load:</span>
            <span className="text-emerald-400 font-mono">24.5%</span>
          </div>
          <div className="flex justify-between text-slate-400 text-[11px]">
            <span>Error Frames:</span>
            <span className="text-emerald-400 font-mono">0</span>
          </div>
        </div>
      </div>
    </nav>
  );
};
