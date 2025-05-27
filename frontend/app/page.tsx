'use client';
import React, { useState, useEffect, useRef } from "react";
import MonacoEditor from "../components/Editor";
import Navbar from "../components/Navbar";

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
    <>
      <div className="home-root">
        <Navbar />

        <div className="main-container">
          {/* Editor Section */}
          <div className="editor-box">
            <div className="editor-header">
              <span>📝 Editor</span>
              <button className="run-button" onClick={runCode}>
                <span>▶ Run</span>
                <kbd>⌘⏎</kbd>
              </button>
            </div>
            <div className="editor-content">
              <MonacoEditor code={code} setCode={setCode} className="h-full" />
            </div>
          </div>

          {/* Output Section */}
          <div className="output-box">
            <div className="output-header">
              <h2>📤 Output</h2>
            </div>

            {waitingForInput && (
              <div className="input-section">
                <div className="input-label">
                  <span className="arrow">↳</span>
                  <label>{inputPrompt}</label>
                </div>
                <input
                  type="text"
                  value={userInput}
                  onChange={(e) => setUserInput(e.target.value)}
                  onKeyDown={(e) => e.key === 'Enter' && sendInput()}
                  className="input-box"
                  placeholder="Enter input..."
                />
              </div>
            )}

            <pre className="output-area">
              <code>
                {output || (
                  <span className="output-placeholder">
                    // Output will appear here after execution...
                  </span>
                )}
              </code>
            </pre>
          </div>
        </div>
      </div>

      {/* 🎨 Internal CSS */}
      <style>
        {`
          .home-root {
            min-height: 100vh;
            background: linear-gradient(to bottom right, #111827, #09090b, #000);
            color: white;
            font-family: monospace;
            border-radius: 1rem;
            overflow: hidden;
          }

          .main-container {
            display: flex;
            height: calc(100vh - 64px);
            padding: 1rem;
            gap: 1rem;
          }

          .editor-box,
          .output-box {
            flex: 1;
            display: flex;
            flex-direction: column;
            border-radius: 1rem;
            overflow: hidden;
            box-shadow: 0 6px 10px rgba(0,0,0,0.3);
            border: 1px solid #4f46e5;
          }

          .editor-box {
            background: linear-gradient(to top right, #312e81, #1e3a8a, #312e81);
          }

          .output-box {
            background: linear-gradient(to top right, #1f2937, #111827, #000);
            border-color: #374151;
          }

          .editor-header,
          .output-header {
            padding: 0.75rem;
            background-color: #3730a3;
            border-bottom: 1px solid #4338ca;
            display: flex;
            justify-content: space-between;
            align-items: center;
          }

          .output-header {
            background-color: #1f2937;
            border-color: #374151;
          }

          .editor-header span,
          .output-header h2 {
            font-size: 0.875rem;
            font-weight: 600;
            margin-left: 0.5rem;
          }

          .run-button {
            background-color: #22c55e;
            color: white;
            border: 1px solid #16a34a;
            padding: 0.4rem 0.75rem;
            border-radius: 0.375rem;
            font-size: 0.875rem;
            font-weight: 600;
            cursor: pointer;
            display: flex;
            align-items: center;
            gap: 0.5rem;
            box-shadow: 0 2px 4px rgba(0,0,0,0.2);
            transition: background 0.2s;
          }

          .run-button:hover {
            background-color: #4ade80;
          }

          .run-button kbd {
            font-size: 0.75rem;
            padding: 0.25rem 0.375rem;
            background: #15803d;
            border: 1px solid #16a34a;
            border-radius: 0.25rem;
          }

          .editor-content {
            flex: 1;
            overflow: hidden;
          }

          .input-section {
            padding: 1rem;
            border-bottom: 1px solid #374151;
            background-color: rgba(17, 24, 39, 0.6);
          }

          .input-label {
            display: flex;
            align-items: center;
            gap: 0.5rem;
            margin-bottom: 0.5rem;
            font-size: 0.875rem;
            color: #d1d5db;
          }

          .arrow {
            color: #facc15;
          }

          .input-box {
            width: 100%;
            padding: 0.5rem 0.75rem;
            background: #1f2937;
            color: white;
            border-radius: 0.5rem;
            border: 1px solid #4b5563;
            outline: none;
            font-size: 0.875rem;
          }

          .input-box:focus {
            border-color: #facc15;
            box-shadow: 0 0 0 1px #facc15;
          }

          .output-area {
            flex: 1;
            padding: 1rem;
            overflow-y: auto;
            color: #4ade80;
            font-size: 0.875rem;
            line-height: 1.6;
            white-space: pre-wrap;
          }

          .output-placeholder {
            color: #9ca3af;
            font-style: italic;
          }
        `}
      </style>
    </>
  );
}
