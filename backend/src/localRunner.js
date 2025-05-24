// PATCH: Updated localRunner.js to support interactive input

const { spawn } = require('child_process');
const fs = require('fs');
const path = require('path');

const testsDir = path.resolve(__dirname, '../../compiler/tests');
const inputPath = path.join(testsDir, 'input.txt');
const inputQueuePath = path.join(testsDir, 'input_queue.txt');
const compilerPath = path.resolve(__dirname, '../../compiler/build/Debug/pseudocode_compiler.exe');

let compilerProcess = null;

function startCompilerWithIO(code, onOutput, onPrompt) {
  // Ensure the tests directory exists
  if (!fs.existsSync(testsDir)) {
    fs.mkdirSync(testsDir, { recursive: true });
    console.log('Created tests directory:', testsDir);
  }

  // Write input.txt
  fs.writeFileSync(inputPath, code);
  fs.writeFileSync(inputQueuePath, '');

  compilerProcess = spawn(compilerPath, { cwd: path.dirname(compilerPath) });

  compilerProcess.stdout.on('data', (data) => {
    const lines = data.toString().split('\n');
    for (const line of lines) {
      if (!line.trim()) continue;
      try {
        const json = JSON.parse(line);
        if (json.type === 'output') onOutput(json.message);
        else if (json.type === 'input') onPrompt(json.prompt);
      } catch (err) {
        onOutput(line);
      }
    }
  });

  compilerProcess.stderr.on('data', (data) => {
    onOutput('[stderr] ' + data.toString());
  });

  compilerProcess.on('close', (code) => {
    onOutput(`\n[Process exited with code ${code}]`);
  });
}

function sendInputToCompiler(val) {
  fs.writeFileSync(inputQueuePath, val + '\n');
}

module.exports = {
  startCompilerWithIO,
  sendInputToCompiler,
};
