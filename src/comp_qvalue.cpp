#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>

struct PvalEntry {
    int index;         //line number
    double p_value;
    double q_value;
};

//function to compute Benjamini-Hochberg q-values
std::vector<PvalEntry> compute_qvalues(const std::vector<double>& p_values, int total_tests) {

    int n = total_tests;
    //int m = p_values.size();
    std::vector<PvalEntry> entries;

    //store p-values with original index
    for (int i = 0; i < n; ++i) {
        entries.push_back({i, p_values[i], 0.0});
    }

    //sort by p-value
    std::sort(entries.begin(), entries.end(),
              [](const PvalEntry& a, const PvalEntry& b) {
                  return a.p_value < b.p_value;
              });

    //apply BH formula
    for (int i = 0; i < n; ++i) {
        entries[i].q_value = std::min(entries[i].p_value * n / (i + 1), 1.0);
    }

    //restore original order
    std::vector<PvalEntry> sorted_by_index(n);
    for (const auto& e : entries) {
        sorted_by_index[e.index] = e;
    }

    return sorted_by_index;
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    const std::string pvalue_file = "pvalue.tsv";
    const std::string input_file = "output_pv.tsv";
    const std::string output_file = "matrix_kf.tsv";
    const std::string qvalue_output = "qvalue.tsv";

    std::vector<double> p_values;

    int total_tests = 0;
    std::ifstream statsin("stats.txt");
    if (!statsin) {
        std::cerr << "Error: cannot open stats.txt" << std::endl;
        return 1;
    }
    statsin >> total_tests;
    statsin.close();

    //read p-values
    std::ifstream pin(pvalue_file);
    if (!pin) {
        std::cerr << "Error: cannot open " << pvalue_file << std::endl;
        return 1;
    }

    std::string line;
    std::getline(pin, line); //header
    while (std::getline(pin, line)) {
        std::istringstream iss(line);
        std::string token;
        if (std::getline(iss, token, '\t')) {
            try {
                p_values.push_back(std::stod(token));
            } catch (...) {
                std::cerr << "Invalid p-value: " << token << std::endl;
            }
        }
    }
    pin.close();

    //compute q-values
    std::vector<PvalEntry> entries = compute_qvalues(p_values, total_tests);

    std::ifstream fin(input_file);
    std::ofstream fout(output_file);
    std::ofstream fqout(qvalue_output);

    if (!fin || !fout || !fqout) {
        std::cerr << "Error opening input/output files." << std::endl;
        return 1;
    }

    // write header for qvalue file
    fqout << "ID\tq-value" << std::endl;

    //copy header from input to output2
    std::string header;
    if (std::getline(fin, header)) {
        fout << header << '\n';
    }

    int current_line = 0;
    while (std::getline(fin, line)) {
        if (current_line >= entries.size()) break;

        const auto& entry = entries[current_line];

        if (entry.q_value < 0.05) {
            // write the full line to output2.tsv
            fout << line << '\n';

            // get the first column from the line
            std::istringstream iss(line);
            std::string first_col;
            if (std::getline(iss, first_col, '\t')) {
                // write first column + q-value to qvalue.tsv
                fqout << first_col << '\t' << entry.q_value << '\n';
            }
        }

        current_line++;
    }


    fin.close();
    fout.close();
    fqout.close();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time of processing of q-value: " << duration.count() << " seconds\n";

    return 0;
}
