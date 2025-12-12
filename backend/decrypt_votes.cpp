#include <helib/helib.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include <iomanip>

using namespace std;
using namespace helib;
using json = nlohmann::json;

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

int main() {
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

    vector<string> positions = {
        "President","Vice President","General Secretary","Joint Secretary","Cultural Secretary","Sports Secretary"
    };

    json results_json = json::array();

    cerr << "🔹 Decrypting totals..." << endl;

    for(const auto& pos : positions) {
        string filename = pos + "_total.bin";
        ifstream inFile(filename, ios::binary);
        if(!inFile){ cerr<<"❌ Cannot open "<<filename<<endl; continue; }

        Ctxt total_ctxt(secretKey);
        total_ctxt.read(inFile);  // read ciphertext from file
        inFile.close();

        vector<long> totals;
        ea.decrypt(total_ctxt, secretKey, totals);

        const auto& candIds = candidates[pos];
        json pos_result;
        for(size_t j=0; j<candIds.size() && j<totals.size(); ++j)
            pos_result[candidateNames[candIds[j]]] = totals[j];

        results_json.push_back({
            {"position", pos},
            {"votes", pos_result}
        });
    }

    cout << "###JSON_START###\n";
    cout << results_json.dump(4) << "\n";
    cout << "###JSON_END###\n";

    cerr << "✅ Decryption completed!" << endl;

    return 0;
}
