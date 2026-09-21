import React, { useState } from 'react';
import { Code, Play, Square, Terminal, CheckCircle } from 'lucide-react';
import { ScriptItem } from '../types';
import { INITIAL_SCRIPTS } from '../data/mockData';

interface ScriptingViewProps {
  onRunScript: (script: ScriptItem) => void;
}

export const ScriptingView: React.FC<ScriptingViewProps> = ({ onRunScript }) => {
  const [scripts, setScripts] = useState<ScriptItem[]>(INITIAL_SCRIPTS);
  const [selectedScriptId, setSelectedScriptId] = useState<string>(INITIAL_SCRIPTS[0].id);
  const [consoleOutput, setConsoleOutput] = useState<string[]>([
    '[SavvyLens JS VM] Ready. V8 engine initialized.',
    '[SavvyLens JS VM] Available APIs: sendCanFrame(id, data, bus), sleep(ms), log(msg)'
  ]);

  const currentScript = scripts.find(s => s.id === selectedScriptId) || scripts[0];

  const handleToggleRun = (scriptId: string) => {
    setScripts(prev => prev.map(s => {
      if (s.id === scriptId) {
        const nextActive = !s.active;
        if (nextActive) {
          setConsoleOutput(c => [`[${new Date().toLocaleTimeString()}] Executing script: ${s.name}...`, ...c]);
          onRunScript(s);
        } else {
          setConsoleOutput(c => [`[${new Date().toLocaleTimeString()}] Stopped script: ${s.name}`, ...c]);
        }
        return { ...s, active: nextActive };
      }
      return s;
    }));
  };

  return (
    <div className="flex-1 flex overflow-hidden bg-slate-950 text-slate-100">
      {/* Script List Sidebar */}
      <div className="w-80 bg-slate-900 border-r border-slate-800 flex flex-col">
        <div className="p-3 border-b border-slate-800 flex items-center justify-between">
          <div className="flex items-center space-x-2">
            <Code className="w-4 h-4 text-blue-400" />
            <h2 className="font-bold text-sm text-white">Automation Scripts</h2>
          </div>
          <span className="text-xs text-slate-400 font-mono">JavaScript</span>
        </div>

        <div className="flex-1 overflow-auto p-2 space-y-1">
          {scripts.map(script => (
            <div
              key={script.id}
              onClick={() => setSelectedScriptId(script.id)}
              className={`p-3 rounded-xl cursor-pointer transition border ${
                selectedScriptId === script.id
                  ? 'bg-blue-600/15 border-blue-500/40 text-blue-300'
                  : 'bg-slate-950/60 border-slate-800/80 hover:bg-slate-800/60 text-slate-300'
              }`}
            >
              <div className="flex items-center justify-between">
                <span className="font-medium text-xs text-slate-200">{script.name}</span>
                <span className={`w-2 h-2 rounded-full ${script.active ? 'bg-emerald-500 animate-ping' : 'bg-slate-600'}`} />
              </div>
              <p className="text-[11px] text-slate-400 mt-1 line-clamp-2">{script.description}</p>
            </div>
          ))}
        </div>
      </div>

      {/* Editor & Output Panel */}
      <div className="flex-1 flex flex-col p-6 overflow-y-auto space-y-6">
        <div className="flex items-center justify-between pb-4 border-b border-slate-800">
          <div>
            <h2 className="text-lg font-bold text-white">{currentScript.name}</h2>
            <p className="text-xs text-slate-400 mt-0.5">{currentScript.description}</p>
          </div>

          <button
            onClick={() => handleToggleRun(currentScript.id)}
            className={`px-4 py-2 rounded-xl text-xs font-semibold flex items-center space-x-2 transition shadow-sm ${
              currentScript.active 
                ? 'bg-rose-600 hover:bg-rose-500 text-white' 
                : 'bg-emerald-600 hover:bg-emerald-500 text-white'
            }`}
          >
            {currentScript.active ? <Square className="w-4 h-4 fill-white" /> : <Play className="w-4 h-4 fill-white" />}
            <span>{currentScript.active ? 'Stop Script' : 'Run Script'}</span>
          </button>
        </div>

        {/* Code Editor Box */}
        <div className="bg-slate-900 border border-slate-800 rounded-2xl overflow-hidden flex flex-col">
          <div className="bg-slate-950 px-4 py-2.5 border-b border-slate-800 flex items-center justify-between text-xs text-slate-400">
            <span className="font-mono">script.js</span>
            <span>JavaScript (ES2022)</span>
          </div>
          <textarea
            value={currentScript.code}
            onChange={e => {
              const val = e.target.value;
              setScripts(prev => prev.map(s => s.id === currentScript.id ? { ...s, code: val } : s));
            }}
            className="w-full h-64 bg-slate-900 text-slate-200 font-mono text-xs p-4 focus:outline-none resize-none leading-relaxed"
            spellCheck={false}
          />
        </div>

        {/* Script Console Output */}
        <div className="bg-slate-900 border border-slate-800 rounded-2xl p-4 flex flex-col">
          <div className="text-xs font-semibold text-slate-300 mb-2 flex items-center justify-between">
            <span className="flex items-center space-x-1.5"><Terminal className="w-4 h-4 text-blue-400" /><span>VM Execution Console</span></span>
            <button 
              onClick={() => setConsoleOutput([])}
              className="text-[11px] text-slate-400 hover:text-white"
            >
              Clear Console
            </button>
          </div>
          <div className="bg-slate-950 rounded-xl p-3 border border-slate-800 font-mono text-xs text-slate-300 h-36 overflow-y-auto space-y-1">
            {consoleOutput.map((log, i) => (
              <div key={i} className={log.includes('Started') || log.includes('Executing') ? 'text-emerald-400' : 'text-slate-300'}>
                {log}
              </div>
            ))}
          </div>
        </div>
      </div>
    </div>
  );
};
