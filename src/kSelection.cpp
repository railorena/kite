#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <boost/algorithm/string.hpp>
#include <cmath>
#include <sstream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <mutex>

using namespace std;
using namespace boost::algorithm;

// Function declarations
vector<string> getCondition(const vector<string>& vectorHeader, const vector<string>& cond1, const vector<string>& cond2);
float mannWhitneyU(const vector<int>& group1, const vector<int>& group2);
float pValue(float U, int n1, int n2);
void writeRow(ofstream& outfile, const vector<string>& splittedRow);
float processRow(const vector<string>& headerInd, const vector<string>& splittedRow);
void workerThread(vector<string> linesBlock, const vector<string>& headerInd, ofstream& outfile, ofstream& outpval, float threshold, char sep_type);

// Mutex for thread-safe file writing
mutex outfileMutex;

int mannWhitneyCount = 0;
mutex mannWhitneyMutex;

// Function to print a vector (for debugging)
void printVector(const vector<string>& vectorToPrint) {
    for (const auto& item : vectorToPrint) {
        std::cout << item << ' ';
    }
}

// Function to determine conditions based on header and condition vectors
vector<string> getCondition(const vector<string>& vectorHeader, const vector<string>& cond1, const vector<string>& cond2) {
    vector<string> conditionsIndex;
    conditionsIndex.reserve(vectorHeader.size());

    for (const auto& headerItem : vectorHeader) {
        if (std::find(cond1.begin(), cond1.end(), headerItem) != cond1.end())
            conditionsIndex.push_back("0");
        else if (std::find(cond2.begin(), cond2.end(), headerItem) != cond2.end())
            conditionsIndex.push_back("1");
        else
            conditionsIndex.push_back("2");
    }
    return conditionsIndex;
}

// Function to calculate U
float mannWhitneyU(const vector<int>& group1, const vector<int>& group2) {
    lock_guard<mutex> lock(mannWhitneyMutex);
    ++mannWhitneyCount;

    vector<pair<int, int>> combined;
    for (const auto& val : group1) combined.emplace_back(val, 0);
    for (const auto& val : group2) combined.emplace_back(val, 1);

    sort(combined.begin(), combined.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
        return a.first < b.first;
    });

    // Rank assignments
    vector<float> ranks(combined.size());
    size_t i = 0;
    while (i < combined.size()) {
        size_t j = i + 1;
        while (j < combined.size() && combined[i].first == combined[j].first) ++j;

        float avg_rank = (i + j + 1) / 2.0;
        for (size_t k = i; k < j; ++k) {
            ranks[k] = avg_rank;
        }
        i = j;
    }

    // Sum ranks for group 1
    float R1 = 0.0f;
    for (size_t k = 0; k < combined.size(); ++k) {
        if (combined[k].second == 0) R1 += ranks[k];
    }

    int n1 = group1.size();
    int n2 = group2.size();
    float U1 = R1 - n1 * (n1 + 1) / 2.0f;

    return U1;
}

float pValue(float U, int n1, int n2) {
    float mean_U = n1 * n2 / 2.0f;
    float sd_U = sqrt(n1 * n2 * (n1 + n2 + 1) / 12.0f);
    float z = (U - mean_U) / sd_U;
    float p = erfc(fabs(z) / sqrt(2.0f));  // two-tailed
    return p;
}

// Function to write a row to the output file (thread-safe)
void writeRow(ofstream& outfile, const vector<string>& splittedRow) {
    lock_guard<mutex> guard(outfileMutex);  // Lock for thread-safe writing
    for (size_t i = 0; i < splittedRow.size(); ++i) {
        outfile << splittedRow[i];
        if (i != splittedRow.size() - 1) {
            outfile << '\t';  // Avoid tab at the end of line
        }
    }
    outfile << endl;
}

// Function to process each row using Mann-Whitney
float processRow(const vector<string>& headerInd, const vector<string>& splittedRow, float threshold) {
    vector<int> vecCond1, vecCond2;

    for (size_t i = 1; i < splittedRow.size(); ++i) {
        if (headerInd[i] == "0") vecCond1.push_back(stoi(splittedRow[i]));
        else if (headerInd[i] == "1") vecCond2.push_back(stoi(splittedRow[i]));
    }

    int presCond1 = count_if(vecCond1.begin(), vecCond1.end(), [](int x) { return x > 0; });
    int presCond2 = count_if(vecCond2.begin(), vecCond2.end(), [](int x) { return x > 0; });

    float fracCond1 = (float)presCond1 / vecCond1.size();
    float fracCond2 = (float)presCond2 / vecCond2.size();

    // Minimum presence filter
    // The code computes the fraction of nonzero entries in two vectors.
    if (fabs(fracCond1 - fracCond2) < threshold)
        return 1.0f;

    float U = mannWhitneyU(vecCond1, vecCond2);
    float p = pValue(U, vecCond1.size(), vecCond2.size());
    return p;
}

// Worker function to process part of the file in a thread
void workerThread(vector<string> linesBlock, const vector<string>& headerInd, ofstream& outfile, ofstream& outpval, float threshold, char sep_type)
{
    for (const auto& line : linesBlock) {
        vector<string> splittedRow;
        istringstream byline(line);
        string item;
        while (getline(byline, item, sep_type))
            splittedRow.push_back(item);

        float pvalue = processRow(headerInd, splittedRow, threshold);
        if (pvalue < 0.05) {
            writeRow(outfile, splittedRow);
            outpval << pvalue << endl;
        }
    }
}

int main(int argc, char **argv) {
    auto start = chrono::high_resolution_clock::now();

    string filename;
    int numThreads = 4;
    float threshold = 0.5;
    char sep = 's';

    bool inputProvided = false;
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-i" && i + 1 < argc) {
            filename = argv[i + 1];
            inputProvided = true;
        } else if (arg == "-n" && i + 1 < argc) {
            numThreads = stoi(argv[i + 1]);
        } else if (arg == "-s" && i + 1 < argc) {
            sep = argv[i + 1][0];
        } else if (arg == "-t" && i + 1 < argc) {
            threshold = stod(argv[i + 1]);
        }
    }

    char sep_type;
    if (sep == 's') {
        sep_type = ' ';
    } else if (sep == 't') {
        sep_type = '\t';
    } else if (sep == 'c') {
        sep_type = ',';
    } else {
        sep_type = sep;
    }

    if (!inputProvided) {
        cerr << "Usage: " << argv[0] << " -i <input_file> [-n <num_threads>] [-s <separator>] [-t <threshold>]\n";
        return 1;
    }

    ifstream infile(filename);
    if (!infile) {
        cerr << "Error opening input file!\n";
        return 1;
    }

    // Read first line (header)
    string headerLine;
    if (!getline(infile, headerLine)) {
        cerr << "Error reading header line!\n";
        return 1;
    }

    vector<string> header;
    istringstream headerStream(headerLine);
    string item;
    while (getline(headerStream, item, sep_type))
        header.push_back(item);

    // Load conditions
    ifstream infileconf("samples_cond");
    if (!infileconf) {
        cerr << "Error opening samples_cond file!\n";
        return 1;
    }

    vector<string> cond1, cond2;
    string line;
    int row = 0;
    while (getline(infileconf, line)) {
        istringstream byline(line);
        while (getline(byline, item, ','))
            (row == 0 ? cond1 : cond2).push_back(item);
        row++;
    }

    vector<string> headerInd = getCondition(header, cond1, cond2);

    ofstream outfile("output_pv.tsv");
    ofstream outpval("pvalue.tsv");
    outpval << "pvalue" << endl;
    writeRow(outfile, header);  // Write header once



    // Read file in blocks and distribute to threads
    const int blockSize = 1000; // adjust as needed
    vector<thread> threads;

    while (true) {
        vector<string> linesBlock;
        linesBlock.reserve(blockSize);

        for (int i = 0; i < blockSize && getline(infile, line); ++i) {
            linesBlock.push_back(line);
        }

        if (linesBlock.empty()) break;

        // Wait for available thread slot
        while ((int)threads.size() >= numThreads) {
            threads.front().join();
            threads.erase(threads.begin());
        }

        threads.emplace_back(workerThread, std::move(linesBlock), std::cref(headerInd), std::ref(outfile), std::ref(outpval), threshold, sep_type);
    }

    // Join remaining threads
    for (auto& t : threads) t.join();

    infile.close();
    outfile.close();

    ofstream statfile("stats.txt");
    if (statfile.is_open()) {
        statfile << mannWhitneyCount << endl;
        statfile.close();
    } else {
        cerr << "Error opening stats.txt for writing!" << endl;
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Time of processing k-mer selection: " << duration.count() << " seconds\n";

    return 0;
}
