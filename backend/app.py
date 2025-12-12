

from flask import Flask, request, jsonify
from flask_cors import CORS
import csv
import os
import hashlib
import subprocess
from db import get_connection

app = Flask(__name__)
CORS(app)

CSV_FILE = "voteCollected.csv"
ZIP_FILE = "voteCollected.zip"
ZIP_PASSWORD = "admin@123"


def hash_password(password):
    return hashlib.sha256(password.encode()).hexdigest()


@app.route("/")
def home():
    return "Backend Running!", 200


@app.route("/submit_vote", methods=["POST"])
def submit_vote():
    data = request.get_json()

    if "votes" not in data or "password" not in data:
        return jsonify({"error": "Invalid data"}), 400

    votes = data["votes"]
    password = data["password"]

    db = get_connection()
    cursor = db.cursor()

    password_hash = hash_password(password)

    cursor.execute(
        "SELECT id, has_voted FROM users WHERE password_hash = %s",
        (password_hash,)
    )
    result = cursor.fetchone()

    if not result:
        return jsonify({"error": "Invalid password"}), 401

    user_id, has_voted = result

    if has_voted == 1:
        return jsonify({"error": "You have already voted"}), 403

    cursor.execute("UPDATE users SET has_voted = 1 WHERE id = %s", (user_id,))
    db.commit()

    extra_value = "4CB22CB099"

    # ------------------------------------------------------------------
    # STEP 1: Extract existing CSV from ZIP (if ZIP already exists)
    # ------------------------------------------------------------------
    if os.path.exists(ZIP_FILE):
        subprocess.run([
            "unzip", "-P", ZIP_PASSWORD, ZIP_FILE
        ])

    # ------------------------------------------------------------------
    # STEP 2: Create CSV with header if not exists
    # ------------------------------------------------------------------
    file_exists = os.path.exists(CSV_FILE)

    with open(CSV_FILE, "a", newline="") as file:
        writer = csv.writer(file)

        if not file_exists:
            writer.writerow([
                "President", "Vice President", "General Secretary", "Joint Secretary",
                "Cultural Secretary", "Sports Secretary", "EXTRA"
            ])

        writer.writerow([
            votes.get("President"),
            votes.get("Vice President"),
            votes.get("General Secretary"),
            votes.get("Joint Secretary"),
            votes.get("Cultural Secretary"),
            votes.get("Sports Secretary"),
            extra_value
        ])

    # ------------------------------------------------------------------
    # STEP 3: Repack into password-protected ZIP
    # ------------------------------------------------------------------
    subprocess.run([
        "zip", "-P", ZIP_PASSWORD, ZIP_FILE, CSV_FILE
    ])

    # ------------------------------------------------------------------
    # STEP 4: Delete extracted CSV (only zip remains)
    # ------------------------------------------------------------------
    if os.path.exists(CSV_FILE):
        os.remove(CSV_FILE)

    return jsonify({"message": "Vote submitted successfully"}), 200


if __name__ == "__main__":
    print("🚀 Starting Flask backend on http://127.0.0.1:5000 ...")
    app.run(host="0.0.0.0", port=5000, debug=True)
