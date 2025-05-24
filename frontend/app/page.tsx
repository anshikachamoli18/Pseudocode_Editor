// Updated page.tsx
'use client';

import React, { useState, useEffect, useRef } from "react";
import MonacoEditor from "../components/Editor";

export default function Home() {
  const [code, setCode] = useState("// Write your pseudocode here...");
  const [output, setOutput] = useState("");
  const [waitingForInput, setWaitingForInput] = useState(false);
  const [inputPrompt, setInputPrompt] = useState("");
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
    <div className="min-h-screen bg-gray-950 text-white px-6 py-8 font-mono">
      <h1 className="text-3xl font-bold mb-6 text-center">🧠 Pseudocode IDE</h1>

      <div className="max-w-7xl mx-auto flex flex-col md:flex-row gap-6">
        {/* Left: Code Editor */}
        <div className="md:w-3/5 w-full space-y-4">
          <MonacoEditor code={code} setCode={setCode} />
          <div className="flex justify-end">
            <button
              onClick={runCode}
              className="bg-blue-600 hover:bg-blue-500 text-white px-6 py-2 rounded-lg shadow-md transition"
            >
              ▶️ Run
            </button>
          </div>
        </div>

        {/* Right: Output Console */}
        <div className="md:w-2/5 w-full bg-gray-900 rounded-xl p-4 shadow-inner h-[460px] overflow-y-auto">
          <h2 className="text-xl font-semibold mb-2">📤 Output:</h2>
          <pre className="whitespace-pre-wrap text-green-400 text-sm">
            {output || "// Output will appear here..."}
          </pre>
          {waitingForInput && (
            <div className="mt-4">
              <label className="block mb-1 text-sm">{inputPrompt}</label>
              <input
                type="text"
                value={userInput}
                onChange={(e) => setUserInput(e.target.value)}
                onKeyDown={(e) => {
                  if (e.key === "Enter") sendInput();
                }}
                className="w-full p-2 bg-gray-800 text-white border border-gray-600 rounded"
              />
            </div>
          )}
        </div>
      </div>
    </div>
  );
}
