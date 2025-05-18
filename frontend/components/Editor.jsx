import { Editor } from "@monaco-editor/react";
import React from "react";

export default function MonacoEditor({ code, setCode }) {
  return (
    <div className="rounded-xl overflow-hidden shadow-md border border-gray-700">
      <Editor
        height="400px"
        defaultLanguage="plaintext"
        value={code}
        onChange={(value) => setCode(value || "")}
        theme="vs-dark"
        options={{
          fontSize: 14,
          minimap: { enabled: false },
          scrollBeyondLastLine: false,
        }}
      />
    </div>
  );
}
