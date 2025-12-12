#include <helib/helib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <iomanip>
#include <nlohmann/json.hpp>

using namespace std;
using namespace helib;
using json = nlohmann::json;
using namespace std::chrono;

// --- Candidate IDs ---
map<string, vector<string>> candidates = {
    {"President", {"4CB22CB001","4CB22CB002","4CB22CB003","4CB22CB004"}},
    {"Vice President", {"4CB22CB005","4CB22CB006","4CB22CB007","4CB22CB008"}},
    {"General Secretary", {"4CB22CB009","4CB22CB010","4CB22CB011","4CB22CB012"}},
    {"Joint Secretary", {"4CB22CB013","4CB22CB014","4CB22CB015","4CB22CB016"}},
    {"Cultural Secretary", {"4CB22CB017","4CB22CB018","4CB22CB019","4CB22CB020"}},
    {"Sports Secretary", {"4CB22CB021","4CB22CB022","4CB22CB023","4CB22CB024"}},
    {"EXTRA", {"4CB22CB025","4CB22CB026","4CB22CB027","4CB22CB028"}}
};

// --- Candidate Names ---
map<string, string> candidateNames = {
    {"4CB22CB001","Alice"},{"4CB22CB002","Bob"},{"4CB22CB003","Charlie"},{"4CB22CB004","David"},
    {"4CB22CB005","Eve"},{"4CB22CB006","Frank"},{"4CB22CB007","Grace"},{"4CB22CB008","Hannah"},
    {"4CB22CB009","Ivy"},{"4CB22CB010","Jack"},{"4CB22CB011","Karen"},{"4CB22CB012","Leo"},
    {"4CB22CB013","Mia"},{"4CB22CB014","Nina"},{"4CB22CB015","Oscar"},{"4CB22CB016","Paul"},
    {"4CB22CB017","Quinn"},{"4CB22CB018","Rita"},{"4CB22CB019","Sam"},{"4CB22CB020","Tina"},
    {"4CB22CB021","Uma"},{"4CB22CB022","Victor"},{"4CB22CB023","Wendy"},{"4CB22CB024","Xander"},
    {"4CB22CB025","Yvonne"},{"4CB22CB026","Zack"},{"4CB22CB027","Aaron"},{"4CB22CB028","Bella"}
};

// --- Hex → binary ---
string hexToBinary(const string& hex) {
    string bin;
    bin.reserve(hex.size() / 2);
    for (size_t i=0;i<hex.size();i+=2) {
        unsigned char byte = static_cast<unsigned char>(stoi(hex.substr(i,2),nullptr,16));
        bin.push_back(byte);
    }
    return bin;
}

// --- Read CSV ---
vector<vector<string>> readCSV(const string& filename){
    vector<vector<string>> rows;
    ifstream file(filename);
    if(!file.is_open()){ cerr << "❌ Cannot open " << filename << endl; return rows; }

    string line;
    while(getline(file,line)){
        if(line.empty()) continue;
        stringstream ss(line);
        string cell;
        vector<string> row;
        while(getline(ss,cell,',')){
            if(!cell.empty() && cell.back()=='\r') cell.pop_back();
            row.push_back(cell);
        }
        rows.push_back(row);
    }
    return rows;
}

int main(){
    cout << fixed << setprecision(3);

    cerr << "🔹 Loading context and secret key..." << endl;

    // Load context
    ifstream ctxFile("context.bin", ios::binary);
    if(!ctxFile){ cerr<<"❌ context.bin missing\n"; return 1; }
    Context context = Context::readFrom(ctxFile);
    ctxFile.close();

    // Load secret key
    ifstream skFile("secretKey.bin", ios::binary);
    if(!skFile){ cerr<<"❌ secretKey.bin missing\n"; return 1; }
    SecKey secretKey = SecKey::readFrom(skFile, context, false);
    skFile.close();

    const EncryptedArray& ea = context.getEA();

    // Read encrypted votes
    auto rows = readCSV("encryptvotes.csv");
    if(rows.empty()){ cerr<<"❌ No encrypted votes!\n"; return 1; }

    vector<string> positions = {
        "President","Vice President","General Secretary","Joint Secretary","Cultural Secretary","Sports Secretary"
    };

    cerr << "🔹 Performing homomorphic addition..." << endl;

    // Homomorphic addition
    vector<Ctxt> total_ctxts;
    total_ctxts.reserve(positions.size());

    for(size_t posIdx=0; posIdx<positions.size(); ++posIdx){
        Ctxt total_ctxt(secretKey); // initialize with secret key
        bool first=true;

        for(const auto& vote : rows){
            if(posIdx>=vote.size()) continue;
            const string& hexStr = vote[posIdx];
            if(hexStr.empty()) continue;

            string binStr = hexToBinary(hexStr);
            stringstream ss(binStr);
            Ctxt ctxt(secretKey);
            ctxt.read(ss);

            if(first){ total_ctxt = ctxt; first=false; }
            else total_ctxt += ctxt;
        }
        total_ctxts.push_back(total_ctxt);
    }

    // Save totals to binary files
    for(size_t i=0;i<positions.size();++i){
        string filename = positions[i] + "_total.bin";
        ofstream outFile(filename, ios::binary);
        if(!outFile){ cerr<<"❌ Cannot write "<<filename<<endl; continue; }
        total_ctxts[i].writeTo(outFile); // ✅ correct for HElib v2.x
        outFile.close();
    }

    cerr << "✅ Homomorphic totals saved!" << endl;

    return 0;
}
