#include <helib/helib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <sys/stat.h>
#include <chrono>

using namespace std;
using namespace helib;
using namespace std::chrono;

// --- Utility: hex encode ---
string toHex(const string& input)
{
    static const char* hexDigits = "0123456789ABCDEF";
    string output;
    output.reserve(input.size() * 2);
    for (unsigned char c : input) {
        output.push_back(hexDigits[c >> 4]);
        output.push_back(hexDigits[c & 0xF]);
    }
    return output;
}

// --- Candidate mapping (7 positions, last = EXTRA) ---
map<string, vector<string>> candidates = {
    {"President", {"4CB22CB001","4CB22CB002","4CB22CB003","4CB22CB004"}},
    {"Vice President", {"4CB22CB005","4CB22CB006","4CB22CB007","4CB22CB008"}},
    {"General Secretary", {"4CB22CB009","4CB22CB010","4CB22CB011","4CB22CB012"}},
    {"Joint Secretary", {"4CB22CB013","4CB22CB014","4CB22CB015","4CB22CB016"}},
    {"Cultural Secretary", {"4CB22CB017","4CB22CB018","4CB22CB019","4CB22CB020"}},
    {"Sports Secretary", {"4CB22CB021","4CB22CB022","4CB22CB023","4CB22CB024"}},
    {"EXTRA", {"4CB22CB025","4CB22CB026","4CB22CB027","4CB22CB028"}}
};

// --- Struct for vote row ---
struct VoteRow {
    string President, VicePresident, GeneralSecretary, JointSecretary,
           CulturalSecretary, SportsSecretary, EXTRA;
};

// --- Read CSV (skips header) ---
vector<VoteRow> readVotesCSV(const string& filename)
{
    vector<VoteRow> votes;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "❌ Cannot open " << filename << endl;
        return votes;
    }

    string line;
    getline(file, line); // skip header

    while (getline(file, line)) {
        if (line.find_first_not_of(" \t\r\n") == string::npos) continue;
        stringstream ss(line);
        VoteRow row;
        getline(ss, row.President, ',');
        getline(ss, row.VicePresident, ',');
        getline(ss, row.GeneralSecretary, ',');
        getline(ss, row.JointSecretary, ',');
        getline(ss, row.CulturalSecretary, ',');
        getline(ss, row.SportsSecretary, ',');
        getline(ss, row.EXTRA, ',');
        votes.push_back(row);
    }
    return votes;
}

// --- Convert ID to one-hot vector ---
vector<long> idToOneHot(const vector<string>& candidateIds, const string& selectedId)
{
    vector<long> oneHot(candidateIds.size(), 0);
    for (size_t i = 0; i < candidateIds.size(); ++i) {
        if (candidateIds[i] == selectedId) {
            oneHot[i] = 1;
            break;
        }
    }
    return oneHot;
}

// --- Encrypt one-hot vector ---
string encryptOneHot(const vector<long>& oneHot, const PubKey& publicKey, const Context& context)
{
    Ptxt<BGV> ptxt(context);
    for (size_t i = 0; i < oneHot.size(); ++i)
        ptxt[i] = oneHot[i];

    Ctxt ctxt(publicKey);
    publicKey.Encrypt(ctxt, ptxt);

    stringstream ss;
    ctxt.writeTo(ss);
    return ss.str();
}

int main()
{
    cout << "🔹 Loading context and public key..." << endl;

    // --- Load context ---
    ifstream ctxFile("context.bin", ios::binary);
    if (!ctxFile) {
        cerr << "❌ Cannot open context.bin\n";
        return 1;
    }
    Context context = Context::readFrom(ctxFile);
    ctxFile.close();

    // --- Load public key ---
    ifstream pkFile("publicKey.bin", ios::binary);
    if (!pkFile) {
        cerr << "❌ Cannot open publicKey.bin\n";
        return 1;
    }
    PubKey publicKey = PubKey::readFrom(pkFile, context);
    pkFile.close();

    // --- Read votes CSV ---
    auto votes = readVotesCSV("voteCollected.csv");
    if (votes.empty()) {
        cerr << "❌ No votes found!\n";
        return 1;
    }

    // --- Prepare output CSV ---
    string outFileName = "encryptvotes.csv";
    ofstream outFile(outFileName, ios::trunc);
    if (!outFile) {
        cerr << "❌ Cannot open " << outFileName << " for writing\n";
        return 1;
    }

    auto t0 = high_resolution_clock::now();
    size_t totalCipherBytes = 0;

    // --- Encrypt votes ---
    for (auto& row : votes) {
        string pres = encryptOneHot(idToOneHot(candidates["President"], row.President), publicKey, context);
        string vp   = encryptOneHot(idToOneHot(candidates["Vice President"], row.VicePresident), publicKey, context);
        string gs   = encryptOneHot(idToOneHot(candidates["General Secretary"], row.GeneralSecretary), publicKey, context);
        string js   = encryptOneHot(idToOneHot(candidates["Joint Secretary"], row.JointSecretary), publicKey, context);
        string cs   = encryptOneHot(idToOneHot(candidates["Cultural Secretary"], row.CulturalSecretary), publicKey, context);
        string ss   = encryptOneHot(idToOneHot(candidates["Sports Secretary"], row.SportsSecretary), publicKey, context);
        string ex   = encryptOneHot(idToOneHot(candidates["EXTRA"], row.EXTRA), publicKey, context);

        totalCipherBytes += pres.size() + vp.size() + gs.size() + js.size() + cs.size() + ss.size() + ex.size();

        outFile << toHex(pres) << "," << toHex(vp) << "," << toHex(gs) << "," << toHex(js)
                << "," << toHex(cs) << "," << toHex(ss) << "," << toHex(ex) << "\n";
    }

    outFile.close();

    auto t1 = high_resolution_clock::now();
    double totalTimeSec = duration_cast<duration<double>>(t1 - t0).count();

    cout << "✅ All votes encrypted and saved to " << outFileName << endl;
    cout << "⏱️ Total encryption time: " << totalTimeSec << " seconds" << endl;
    // cout << "💾 Total ciphertext size: " << totalCipherBytes << " bytes ("
    //      << (totalCipherBytes / (1024.0 * 1024.0)) << " MB)" << endl;

    return 0;
}
