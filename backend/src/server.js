const { WebSocketServer } = require('ws');
const { startCompilerWithIO, sendInputToCompiler } = require('./localRunner');

const wss = new WebSocketServer({ port: 8080 });

wss.on('connection', (ws) => {
  console.log("✅ Client connected");

  ws.on('message', async (message) => {
    try {
      const parsed = JSON.parse(message);
      if (parsed.type === 'input') {
        sendInputToCompiler(parsed.value);
      }
    } catch {
      // Not JSON => treat as code
      startCompilerWithIO(
        message.toString(),
        (msg) => ws.send(JSON.stringify({ type: 'output', message: msg })),
        (prompt) => ws.send(JSON.stringify({ type: 'input', prompt }))
      );
    }
  });

  ws.on('error', (err) => {
    console.error("❌ WebSocket error:", err);
  });

  ws.on('close', () => {
    console.log("🔌 Client disconnected");
  });

  ws.send('Connected to Compiler Server');
});

console.log('🚀 WebSocket server running on ws://localhost:8080');
