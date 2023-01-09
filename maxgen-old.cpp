#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <ctime>
using namespace std;

string arrange(const vector<int>& counters, int max_digits, char fill){
    string ret = "";
    for(int counter : counters){
        string buf = to_string(counter);
        while(buf.size() < max_digits){
            buf = fill + buf;
        }
        ret += buf + " ";
    }
    return ret;
}

vector<int> calc(int num_of_counters, int select, int calc_limit){
    vector<int> counters(num_of_counters);
    ofstream progress_file("maxgen-progress-old.txt");
    int calc_count = 0;

    while(counters[num_of_counters - 1] < num_of_counters - 1){ // num_of_counters-1が作れるまで

        // select-1を1つ作る
        while(counters[select - 1] < select - 1){
            for(int i = 0; i < select - 1; i++){
                counters[i]++;
            }
            counters[select - 1] = 0;
            sort(counters.begin(), counters.end());
            progress_file << arrange(counters, 2, ' ') << endl;
            if(++calc_count == calc_limit) return counters;
        }

        // 合成
        int pos = num_of_counters - 2;
        while(true){
            // 合成の必要無し
            if(counters[pos] < select - 1){
                break;
            }

            // 合成可能
            if(counters[pos] == counters[pos + 1]){
                int start_index = (pos + 1) - (select - 1);
                if(start_index >= 0){
                    for(int i = start_index; i < pos + 1; i++){
                        counters[i]++;
                    }
                    counters[pos + 1] = 0;
                    sort(counters.begin(), counters.end());
                    progress_file << arrange(counters, 2, ' ') << endl;

                    if(++calc_count == calc_limit) return counters;
                    pos = num_of_counters - 2;
                }
                else break;
            }
            else{
                pos--;
            }
        }

        if(counters[num_of_counters - 1] == num_of_counters - 1) break;
    }
    return counters;
}

int main(){
    clock_t start_t, end_t;
    
    // start
    start_t = clock();

    vector<int> counters = calc(100, 8, 10000);

    // end
    end_t = clock();

    // 結果の出力
    ofstream result_file("maxgen-result-old.txt");
    result_file << arrange(counters, 2, ' ') << endl;
    result_file.close();
    cout << "done in " << end_t - start_t << " ms." << endl;
}