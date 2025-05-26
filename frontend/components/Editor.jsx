import { Editor } from "@monaco-editor/react";
import React from "react";

export default function MonacoEditor({ code, setCode,className }) {
  return (
    <div className={className}>
      <Editor
        height="100vh"
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
