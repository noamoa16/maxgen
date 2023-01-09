#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>
#include <string_view>
#include <tuple>
#include <ctime>
#include <cmath>
using namespace std;

// パラメータ
constexpr int NUM_OF_COUNTERS = 100;
constexpr int SELECT = 8;
constexpr int CALC_LIMIT = 100000000;
constexpr bool PRINT_RESULT = true;
constexpr bool PRINT_RESULT_TO_FILE = true;
constexpr string_view RESULT_FILE_PATH = "maxgen-result.txt";

template<class T>
const string key_value_str(const vector<T>& v){
    string ret = "{";
    for(int i = 0; i < v.size(); i++){
        ret += to_string(i) + ": " + to_string(v[i]);
        if(i != v.size() - 1) ret += ", ";
    }
    return ret + "}";
}

class maxgen{
    public:

    int num_of_counters, select, calc_limit;
    vector<int> counters;
    int steps = 0;

    maxgen(const int num_of_counters, const int select, const int calc_limit){
        this->num_of_counters = num_of_counters;
        this->select = select;
        this->calc_limit = calc_limit;
        counters = vector<int>(this->num_of_counters);
    }

    void run(const string& progress_file_path){
        if(PRINT_RESULT_TO_FILE) progress_file = ofstream(progress_file_path);
        counters[0] = num_of_counters;
        while(counters[num_of_counters - select] < select){
            steps++;

            // 合成するindexを決める
            int index = select_index();
            operate(index, num_of_counters - select - 1);

            if(steps == calc_limit) break;
            if(is_goalable()) break;
        }
        output();
        for(int i = 0; i < num_of_counters - 1; i++){
            if(is_operable(i) && counters[i] >= 2){
                steps++;
                bool printed = operate(i, num_of_counters - 1);
                if(!printed) output();
            }
        }
        if(PRINT_RESULT_TO_FILE) progress_file.close();
    }

    private:

    int max_index = 0;
    ofstream progress_file;

    void output(){
        string outstr = to_string(steps) + ": " + key_value_str(counters);
        if(PRINT_RESULT) cout << outstr << endl << endl;
        if(PRINT_RESULT_TO_FILE) progress_file << outstr << endl;
    }

    // 合成するselect個のカウンターの最大値としてindexを選んだ時の評価値
    double eval(const int index) const{
        if(!is_operable(index)) return -1;

        if(counters[index] >= select){
            return counters[index] + (1 << 16) - (double)index / num_of_counters;
        }
        else if(counters[index] >= 2){
            return counters[index] + 1 - (double)index / num_of_counters;
        }
        else if(counters[index] == 1){
            return 1 - (double)index / num_of_counters;
        }
        else{
            return -1; // unreachable
        }
    }

    int select_index() const{
        int best_index = -1;
        double best_evaluated_value = -(1 << 16);
        for(int i = 0; i <= max_index; i++){
            double value = eval(i);
            if(value > best_evaluated_value){
                best_evaluated_value = value;
                best_index = i;
            }
        }
        if(best_index == -1){
            cerr << "error: best_index == -1" << endl;
        }
        return best_index;
    }

    bool operate(const int index, int max_index_limit){
        // maxをclear
        counters[index]--;
        counters[0]++;

        // 残りをインクリメント
        int remain = select - 1;
        int i = index;
        while(remain > 0){
            int diff = min(counters[i], remain);
            counters[i + 1] += diff;
            counters[i] -= diff;
            remain -= diff;
            i--;
        }

        bool printed = false;

        // max_indexの更新
        if(max_index < max_index_limit){
            if(max_index + 1 < num_of_counters - 1 && counters[max_index + 1] > 0){
                max_index++;
                output();
                printed = true;
            }
        }
        if(!printed && steps % 1000000 == 0){
            output();
            printed = true;
        }

        return printed;
    }

    bool is_operable(int index) const{
        if(counters[index] == 0) return false;
        int sum = 0;
        for(int i = index; i >= 0; i--){
            sum += counters[i];
            if(sum >= select){
                return true;
            }
        }
        return false;
    }

    bool is_goalable() const{
        int count = 0;
        for(int i = 0; i < num_of_counters - 1; i++){
            count += is_operable(i) ? counters[i] : 0;
            count -= ceil((double)count / select);

            // ゴール不可能だと確定したらfalse
            if(count == 0 && i >= max_index){
                return false;
            }
        }
        return count >= 1;
    }
};

int main(int argc, char* argv[]){
    clock_t start_t, end_t;
    
    // start
    start_t = clock();

    maxgen gen(NUM_OF_COUNTERS, SELECT, CALC_LIMIT);
    gen.run(RESULT_FILE_PATH.data());
    vector<int> counters = gen.counters;
    int steps = gen.steps;

    // end
    end_t = clock();

    // 結果の出力
    if(PRINT_RESULT){
        cout << steps << ": " << key_value_str(counters) << endl;
        cout << "done in " << end_t - start_t << " ms." << endl;
    }
}