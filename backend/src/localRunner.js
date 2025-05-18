const { spawn } = require('child_process');
const fs = require('fs');
const path = require('path');

function runCompilerLocally(code) {
  return new Promise((resolve, reject) => {
    const testsDir = path.resolve(__dirname, '../../compiler/tests');
    const inputPath = path.join(testsDir, 'input.txt');
    const compilerPath = path.resolve(__dirname, '../../compiler/build/Debug/pseudocode_compiler.exe');

    // Ensure the tests folder exists
    if (!fs.existsSync(testsDir)) {
      fs.mkdirSync(testsDir, { recursive: true });
      console.log('Created missing tests directory:', testsDir);
    }

    // Write the input file
    fs.writeFileSync(inputPath, code);
    console.log('Input file written to:', inputPath);

    // Run the compiler
    const process = spawn(compilerPath, { cwd: path.dirname(compilerPath) });

    let output = '';
    let error = '';

    process.stdout.on('data', (data) => {
      output += data.toString();
    });

    process.stderr.on('data', (data) => {
      error += data.toString();
    });

    process.on('close', (code) => {
      if (code === 0) {
        // Optionally read compiler/tests/output.txt
        const outputFile = path.join(testsDir, 'output.txt');
        if (fs.existsSync(outputFile)) {
          const result = fs.readFileSync(outputFile, 'utf-8');
          resolve(result);
        } else {
          resolve(output || 'Compiler finished with no output file.');
        }
      } else {
        reject(`Compiler exited with code ${code}: ${error}`);
      }
    });
  });
}

module.exports = { runCompilerLocally };
