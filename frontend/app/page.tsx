'use client';

import React, { useState, useEffect, useRef } from "react";
import MonacoEditor from "../components/Editor";

export default function Home() {
  const [code, setCode] = useState("// Write your pseudocode here...");
  const [output, setOutput] = useState("");
  const [waitingForInput, setWaitingForInput] = useState(false);
  const [inputPrompt, setInputPrompt] = useState("");//for dynamic input
  const [userInput, setUserInput] = useState("");
  const ws = useRef<WebSocket | null>(null);

  useEffect(() => {
    ws.current = new WebSocket("ws://localhost:8080");

    ws.current.onopen = () => console.log("✅ WebSocket connected");

    ws.current.onmessage = (event) => {
      try {
        const parsed = JSON.parse(event.data);
        if (parsed.type === "output") {
          setOutput((prev) => prev + parsed.message + "\n");
        } else if (parsed.type === "input") {
          setInputPrompt(parsed.prompt);
          setWaitingForInput(true);
        }
      } catch {
        console.warn("📦 Non-JSON output:", event.data);
        setOutput((prev) => prev + event.data + "\n");
      }
    };

    ws.current.onerror = (err) => console.error("WebSocket error", err);
    ws.current.onclose = () => console.log("WebSocket disconnected");

    return () => ws.current?.close();
  }, []);

  const runCode = () => {
    setOutput("");
    if (ws.current && ws.current.readyState === WebSocket.OPEN) {
      ws.current.send(code);
    } else {
      console.error("WebSocket is not ready.");
    }
  };

  const sendInput = () => {
    if (ws.current && ws.current.readyState === WebSocket.OPEN) {
      ws.current.send(JSON.stringify({ type: "input", value: userInput }));
      setUserInput("");
      setWaitingForInput(false);
    }
  };

  return (
    <div className="min-h-screen bg-gray-950 text-white font-mono">
      {/* Header */}
      <header className="px-6 py-4 border-b border-gray-700 bg-gray-900">
        <h1 className="text-10xl font-bold flex items-center gap-3">
          <span>Welcome to PseudoX : Pseudocode Execution Platform</span>
        </h1>
      </header>

      <div className="flex h-[calc(100vh)] px-2 gap-4">
  {/* Editor Section */}
  <div className="flex-1 flex flex-col border border-gray-700 rounded-lg overflow-hidden">
    <div className="p-2 bg-gray-900 flex items-center justify-between border-b border-gray-700">
      <span className="text-sm text-gray-300 ml-2">EDITOR</span>
      <button
        onClick={runCode}
        className="bg-blue-600 hover:bg-blue-500 text-white px-4 py-1.5 rounded-md 
                 text-sm font-medium mr-2 transition-colors border border-blue-400
                 flex items-center gap-2 shadow-md"
      >
        <span>▶ Run</span>
        <kbd className="text-xs px-1.5 py-0.5 bg-gray-800 rounded border border-gray-600">⌘⏎</kbd>
      </button>
    </div>
    <div className="flex-1 overflow-hidden">
      <MonacoEditor code={code} setCode={setCode} className="h-full" />
    </div>
  </div>

  {/* Output Section */}
<div className="flex-1 flex flex-col border border-gray-700 rounded-lg overflow-hidden">
  <div className="p-2 bg-gray-900 border-b border-gray-700 flex items-center">
    <h2 className="text-sm text-gray-300 ml-2">OUTPUT</h2>
  </div>

  {waitingForInput && (
    <div className="p-4 border-b border-gray-700 bg-gray-800/50">
      <div className="flex items-center gap-2 mb-2 text-sm">
        <span className="text-yellow-400">↳</span>
        <label className="text-gray-300">{inputPrompt}</label>
      </div>
      <input
        type="text"
        value={userInput}
        onChange={(e) => setUserInput(e.target.value)}
        onKeyDown={(e) => e.key === 'Enter' && sendInput()}
        className="w-full px-3 py-2 bg-gray-900 text-gray-100 rounded 
                 border border-gray-600 focus:border-blue-500 focus:ring-1 
                 focus:ring-blue-500 outline-none transition-all text-sm"
        placeholder="Enter input..."
      />
    </div>
  )}

  <pre className="flex-1 p-4 overflow-y-auto scrollbar-thin scrollbar-thumb-gray-700 scrollbar-track-gray-900">
    <code className="whitespace-pre-wrap text-green-400 font-mono text-sm leading-relaxed">
      {output || (
        <span className="text-gray-400 italic">
          // Output will appear here after execution...
        </span>
      )}
    </code>
  </pre>
</div>

</div>
</div>
  );
}
