from flask import Flask, jsonify, send_from_directory
from flask_cors import CORS
import subprocess
import json
import os

app = Flask(__name__, static_folder='results-frontend')
CORS(app)

# Serve index.html at root
@app.route('/')
def index():
    return send_from_directory(app.static_folder, 'index.html')

# Serve CSS and JS automatically
@app.route('/<path:path>')
def static_files(path):
    return send_from_directory(app.static_folder, path)

# Results endpoint
@app.route('/results')
def get_results():
    try:
        # Step 0: Optional - compile decrypt_votes.cpp
        subprocess.run(
            ["g++", "-O2", "-std=c++17", "decrypt_votes.cpp", "-o", "decrypt_votes", 
             "-lhelib", "-lgmp", "-lntl", "-pthread"], 
            check=True
        )

        # Step 1: Run homomorphic addition first (optional if needed)
        subprocess.run(["./homomorphic_addition"], check=True)

        # Step 2: Run decrypt_votes and capture JSON output
        output = subprocess.check_output(["./decrypt_votes"])
        text = output.decode()

        # Extract JSON
        start = text.find("###JSON_START###") + len("###JSON_START###")
        end = text.find("###JSON_END###")
        json_data = json.loads(text[start:end].strip())

        return jsonify(json_data)

    except subprocess.CalledProcessError as e:
        return jsonify({"error": "Failed to compile/run C++ program", "details": str(e)}), 500
    except json.JSONDecodeError as e:
        return jsonify({"error": "Failed to parse JSON output", "details": str(e)}), 500

if __name__ == '__main__':
    app.run(debug=True)
