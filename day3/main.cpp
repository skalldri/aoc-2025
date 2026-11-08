#include <iostream>
#include <string>
#include <filesystem>
#include <format>
#include <fstream>
#include <vector>
#include <utility>

using namespace std;

string readFileIntoString(filesystem::path file)
{
    if (!filesystem::is_regular_file(file)) {
        throw runtime_error(format("{} must be a regular file", string(file)));
    }

    ifstream fileStream(file);

    if (!fileStream.is_open()) {
        throw runtime_error(format("{} cannot be opened for reading", string(file)));
    }

    ostringstream fileContents;
    fileContents << fileStream.rdbuf();

    return fileContents.str();
}

vector<string> splitStringByChar(const string& input, char c)
{   
    // Find the first instance
    size_t start = 0;
    size_t end = input.find(c);
    vector<string> out;

    // If no instances, return an empty vector
    if (end == std::string::npos) {
        return out;
    }

    // There is at least one split

    do {
        // Split out the string that has been found so far
        out.push_back(input.substr(start, end - start));

        // Advance start -> end so we don't double-find
        start = end + 1;

        if (start >= input.length()) {
            break;
        }

        // Find the next instance
        end = input.find(c, end + 1);
    } while (end != std::string::npos);

    // Take all the remaining characters and dump them onto the end of the string

    if (start < input.length()) {
        out.push_back(input.substr(start, input.length()));
    }
    
    return out;
}

vector<size_t> parseLine(string line) {
    vector<size_t> out;

    for (size_t i = 0; i < line.length(); i++) {
        out.push_back(stoi(line.substr(i, 1)));
    }

    return out;
}

pair<size_t, size_t> pickBestJoltage(vector<size_t> vec) {
    size_t best = 0;
    size_t bestIndex = 0;

    // (vec.size() - 1) because the first digit of the joltage
    // cannot be chosen as the last vector element, since we 
    // need to create a two digit number in-order
    for (size_t i = 0; i < (vec.size() - 1); i++) {
        if (vec[i] > best) {
            best = vec[i];
            bestIndex = i;
        }
    }

    size_t secondBest = 0;
    for (size_t i = bestIndex+1; i < vec.size(); i++) {
        if (vec[i] > secondBest) {
            secondBest = vec[i];
            bestIndex = i;
        }
    }

    return pair<size_t, size_t>(best, secondBest);
}

int main(int argc, char** argv) {

    cout << "Hello world!" << endl;

    string fileContents = readFileIntoString(filesystem::path("/workspaces/aoc-2025/day3/input"));

    vector<string> fileLines = splitStringByChar(fileContents, '\n');

    size_t sumBestJoltage = 0;

    for (auto& line : fileLines) {
        auto vec = parseLine(line);

        for (auto& num : vec) {
            cout << num;
        }

        auto bestJoltage = pickBestJoltage(vec);

        size_t bestJoltageDec = (10 * bestJoltage.first) + bestJoltage.second;

        cout << " - best Joltage " << bestJoltage.first << bestJoltage.second << endl;

        cout << endl;

        sumBestJoltage += bestJoltageDec;
    }

    cout << "Final Best Joltage Sum: " << sumBestJoltage << endl;

    return 0;
}
