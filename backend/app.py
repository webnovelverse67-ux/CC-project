from flask import Flask, request, jsonify
from flask_cors import CORS
import subprocess
import json
import os
import tempfile

app = Flask(__name__)
CORS(app)

COMPILER_PATH = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'compiler', 'mycompiler'))

@app.route('/compile', methods=['POST'])
def compile_code():
    data = request.json
    source_code = data.get('sourceCode', '')
    target_language = data.get('targetLanguage', 'python').lower()

    if not source_code:
        return jsonify({"error": "No source code provided"}), 400

    # Run the C compiler
    try:
        # We need to run the compiler in the same dir as the temp file to ensure target_code.out is written cleanly,
        # or we just rely on the C compiler writing it to its current working directory
        with tempfile.TemporaryDirectory() as temp_dir:
            out_file = os.path.join(temp_dir, "target_code.out")
            process = subprocess.run(
                [COMPILER_PATH, target_language, out_file],
                cwd=temp_dir,
                input=source_code.encode('utf-8'),
                capture_output=True,
                timeout=5
            )

            compiler_stdout = process.stdout.decode('utf-8')
            compiler_stderr = process.stderr.decode('utf-8')

            if process.returncode != 0 and not compiler_stdout:
                return jsonify({
                    "error": "Compiler crashed or failed to parse.",
                    "details": compiler_stderr
                }), 500

            try:
                compiler_results = json.loads(compiler_stdout)
            except json.JSONDecodeError as e:
                return jsonify({
                    "error": "Failed to parse compiler JSON output.",
                    "raw_output": compiler_stdout,
                    "stderr": compiler_stderr
                }), 500

            # Now execute the target code
            target_code = compiler_results.get("target_code", "")
            runtime_output = ""

            if target_code:
                if target_language == "python":
                    py_file = os.path.join(temp_dir, "prog.py")
                    with open(py_file, 'w') as f:
                        f.write(target_code)
                    try:
                        run_proc = subprocess.run(["python3", py_file], capture_output=True, timeout=5)
                        runtime_output = run_proc.stdout.decode('utf-8') + run_proc.stderr.decode('utf-8')
                    except Exception as e:
                        runtime_output = f"Runtime Execution Error: {str(e)}"
                elif target_language == "cpp":
                    cpp_file = os.path.join(temp_dir, "prog.cpp")
                    cpp_out = os.path.join(temp_dir, "prog.out")
                    with open(cpp_file, 'w') as f:
                        f.write(target_code)
                    try:
                        compile_proc = subprocess.run(["g++", cpp_file, "-o", cpp_out], capture_output=True, timeout=5)
                        if compile_proc.returncode == 0:
                            run_proc = subprocess.run([cpp_out], capture_output=True, timeout=5)
                            runtime_output = run_proc.stdout.decode('utf-8') + run_proc.stderr.decode('utf-8')
                        else:
                            runtime_output = f"C++ Compilation Error:\n{compile_proc.stderr.decode('utf-8')}"
                    except Exception as e:
                        runtime_output = f"Runtime Execution Error: {str(e)}"

            compiler_results["runtime_output"] = runtime_output
            return jsonify(compiler_results)

    except subprocess.TimeoutExpired:
        return jsonify({"error": "Compilation timed out"}), 500
    except Exception as e:
        return jsonify({"error": str(e)}), 500

if __name__ == '__main__':
    app.run(debug=True, port=5000, host='0.0.0.0')
