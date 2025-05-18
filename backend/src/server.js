const { WebSocketServer } = require('ws');
const { runCompilerLocally } = require('./localRunner');

const wss = new WebSocketServer({ port: 8080 });

wss.on('connection', (ws) => {
  console.log("✅ Client connected");

  ws.on('message', async (message) => {
    console.log("📩 Received message from client:", message.toString());

    try {
      console.log("Inside try");
      const output = await runCompilerLocally(message.toString());
      ws.send(output);
    } catch (err) {
      ws.send(`[ERROR] ${err}`);
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
