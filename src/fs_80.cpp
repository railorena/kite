#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <boost/algorithm/string.hpp>
#include <cmath>
#include <sstream>

using namespace std;
using namespace boost::algorithm;

void printVector(vector<string> vectorToPrint){
    for (std::vector<string>::const_iterator i = vectorToPrint.begin(); i != vectorToPrint.end(); ++i)
        std::cout << *i << ' ';
}

vector<string> getCondition(vector<string> vectorHeader, vector<string> cond1, vector<string> cond2){
    vector<string> conditionsIndex;

    for (std::vector<string>::const_iterator i = vectorHeader.begin(); i != vectorHeader.end(); ++i)
        if ( std::find(cond1.begin(), cond1.end(), *i) != cond1.end() )
            conditionsIndex.push_back("0");
        else if ( std::find(cond2.begin(), cond2.end(), *i) != cond2.end() )
            conditionsIndex.push_back("1");
        else
            conditionsIndex.push_back("2");
    return (conditionsIndex);
}

int checkZeros(std::vector<int> vecCond1Check, std::vector<int> vecCond2Check){
    int diffentZcond1 = 0,  diffentZcond2 = 0;
    int diffZero = 0;

    for (std::vector<int>::const_iterator i = vecCond1Check.begin(); i != vecCond1Check.end(); ++i){
        if(*i != 0){
            diffentZcond1++;
        }
    }

    for (std::vector<int>::const_iterator i = vecCond2Check.begin(); i != vecCond2Check.end(); ++i){
        if(*i != 0){
            diffentZcond2++;
        }
    }

    if((diffentZcond1 > (vecCond1Check.size()*0.8)) || (diffentZcond2 > (vecCond2Check.size()*0.8)))
        diffZero = 1;

    return (diffZero);
}


vector<int> removeOutlier(vector<int> vecCond){
   std::vector<int> vecCondCleaned;
   sort(vecCond.begin(), vecCond.end());

    int q34 = vecCond[vecCond.size()*3/4];
    for (std::vector<int>::const_iterator i = vecCond.begin(); i != vecCond.end(); ++i){
        if(*i < q34){
            vecCondCleaned.push_back(*i);
        }
    }
    return vecCondCleaned;
}


float sdCond(float averageC1, float averageC2, float average){
    vector<float> vec = {averageC1, averageC2};
    float summation = 0.0, st = 0.0;
    int n = 2;
    for(int i = 0; i < n; ++i) {
        summation += pow(vec[i] - average, 2);
    }
    st = sqrt(summation / n);

    return (st);
}



float calculateCV(vector<int> cond1, vector<int> cond2){
    float stdev = 0.0, average = 0.0, cv = 0.0;

    float averageC1 = accumulate(cond1.begin(), cond1.end(), 0.0) / cond1.size();
    float averageC2 = accumulate(cond2.begin(), cond2.end(), 0.0) / cond2.size();

    average = (averageC1 + averageC2) /2;
    stdev = sdCond(averageC1, averageC2, average);
    cv = stdev/average;
    return (cv);
}

void writeRow(ofstream outfile, vector<string> splittedRow){
    for (std::vector<string>::const_iterator i = splittedRow.begin(); i != splittedRow.end(); ++i)
        outfile << *i << ' ';
}

float processingRow(vector<string> vectorHeader, vector<string> splittedRow){
    std::vector<int> vecCond1, cond1Quart;
    std::vector<int> vecCond2, cond2Quart;
    float cvConditions = 0.0;
    //vecCond1.clear();
    //vecCond2.clear();
    int diff = 0;

    for (long long i=1; i<splittedRow.size(); ++i){
        if(vectorHeader[i] == "0"){
            vecCond1.push_back(std::stoll(splittedRow[i]));
        }else if(vectorHeader[i] == "1"){
            vecCond2.push_back(std::stoll(splittedRow[i]));
        }

    }

    diff = checkZeros(vecCond1, vecCond2);

    if(diff == 1){
        cond1Quart = removeOutlier(vecCond1);
        cond2Quart = removeOutlier(vecCond2);
        cvConditions = calculateCV(cond1Quart, cond2Quart);
    }else{
        cvConditions = 0;
    }



    return (cvConditions);
}


int main (int argc, char **argv){

    time_t t;
    struct tm * tt;
    time (&t);
    tt = localtime(&t);

    //cout << argv[1] << endl;
    char * filename = argv[1];
    //char * filename = "test.tsv";

    //======================================//
    //---reading the config file---//
    string lineconfig;
    std::vector<std::string> cond1;
    std::vector<std::string> cond2;
    std::ifstream infileconf("samples_cond");
    int nrowconfig = 0;

    cond1.clear();
    while (getline (infileconf, lineconfig)){
            std::istringstream bylineconfig(lineconfig);
            std::string itemconfig;


            if(nrowconfig == 0){
                while(getline(bylineconfig, itemconfig, ','))
                    cond1.push_back(itemconfig);
            }else{
                while(getline(bylineconfig, itemconfig, ','))
                    cond2.push_back(itemconfig);
            }
            nrowconfig++;

    }

    //======================================//
    //---reading the couting table---//
    ifstream infile(filename);
    string line;
    char buffer[4000000];
    infile.rdbuf()->pubsetbuf(buffer, sizeof(buffer));

    std::vector<std::string> splittedString;
    std::vector<std::string> headerInd;
    std::vector<std::string> header;
    ofstream outfile;
    long long nrow = 0;


    while (getline (infile, line)){
        splittedString.clear();

        //first line, print the header in the file
        if(nrow == 0){
            std::istringstream byline(line);
            std::string item;

            while(getline (byline, item, ' '))
                splittedString.push_back(item);


            headerInd = getCondition(splittedString, cond1, cond2);

            //======================================//
            //---Printing the header in the file---//
            outfile.open("output.tsv");

            for (std::vector<string>::const_iterator i = splittedString.begin(); i != splittedString.end(); ++i)
                outfile << *i << '\t';
            outfile << endl;

        //checking the other rows
        }else{

            std::istringstream byline(line);
            std::string item;

            while(getline(byline, item, ' '))
                splittedString.push_back(item);

            //printVector(splittedString);
            //cout << endl;

            //checking the CV for the current row
            float resultVec = processingRow(headerInd, splittedString);
            //cout << resultVec << endl;
            //if CV high, print current row
            if(resultVec >= 1.0){
                //cout << resultVec << endl;
                for (std::vector<string>::const_iterator i = splittedString.begin(); i != splittedString.end(); ++i)
                    outfile << *i << '\t';
                outfile << endl;
            }
        }

        nrow++;
    }

    infile.close();
    outfile.close();

    time (&t);
    tt = localtime(&t);

    return 0;
}
