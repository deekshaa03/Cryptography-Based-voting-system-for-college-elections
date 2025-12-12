#include <helib/helib.h>
#include <iostream>
#include <chrono>

using namespace std;
using namespace helib;

int main() {
    // ---------------- Parameters ----------------
    unsigned long m = 50000;    
    unsigned long p = 4999;    
    unsigned long r = 1;        
    unsigned long bits =175; 
    unsigned long c = 2;        

    auto start_time = chrono::high_resolution_clock::now();

    // Create context
    Context context = ContextBuilder<BGV>()
                          .m(m)
                          .p(p)
                          .r(r)
                          .bits(bits)
                          .c(c)
                          .build();

    // Compute security level
    double helib_sec = context.securityLevel();

    // Generate secret key
    SecKey secretKey(context);
    secretKey.GenSecKey();

    auto end_time = chrono::high_resolution_clock::now();

    // Calculate total time in seconds
    double total_time_sec =
        chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() / 1000.0;

    // Print results
    cout << "Estimated security level: " << helib_sec << " bits" << endl;
    cout << "Total keygen time: " << total_time_sec << " seconds" << endl;

    return 0;
}












