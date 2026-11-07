#include <iostream>
#include <string>
#include <filesystem>
#include <format>
#include <fstream>
#include <vector>

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

struct Range {
    size_t start;
    size_t end;
};

Range parseRange(string range) {
    vector<string> startEnd = splitStringByChar(range, '-');

    cout << "Start: " << startEnd.at(0) << endl;
    cout << "End: " << startEnd.at(1) << endl << endl;

    Range r;
    r.start = stol(startEnd.at(0));
    r.end = stol(startEnd.at(1));
    return r;
}

vector<size_t> findInvalidIds(Range range) {
    vector<size_t> invalidIds;
    // Loop through all IDs
    for (size_t id = range.start; id <= range.end; id++) {
        string idStr = to_string(id);

        // An ID is invalid if it's just two instances of a number sequence back-to-back.
        // Odd-length number sequences must therefore always be valid
        if ((idStr.length() % 2) != 0) {
            continue;
        }

        // Even length string: split into two substrs
        string idStrA = idStr.substr(0, idStr.length() / 2);
        string idStrB = idStr.substr(idStr.length() / 2, idStr.length() / 2);

        // cout << "A: " << idStrA << endl;
        // cout << "B: " << idStrB << endl;

        if (idStrA == idStrB) {
            cout << "Invalid ID! " << id << endl;
            invalidIds.push_back(id);
        }
    }

    return invalidIds;
}

int main(int argc, char** argv) {

    cout << "Hello world!" << endl;

    string fileContents = readFileIntoString(filesystem::path("/workspaces/aoc-2025/day2/input"));

    vector<string> fileRanges = splitStringByChar(fileContents, ',');

    // Input parsing
    vector<Range> ranges;
    for (auto& range : fileRanges) {
        ranges.push_back(parseRange(range));
    }

    vector<size_t> invalidIds;
    size_t sumInvalidId = 0;
    for (auto& range : ranges) {

        cout << "Range: " << range.start << "-" << range.end << endl;

        vector<size_t> rangeInvalidIds = findInvalidIds(range);

        cout << "Range Invalid IDs: ";
        for (auto& rangeInvalidId : rangeInvalidIds) {
            cout << rangeInvalidId << ", ";

            sumInvalidId += rangeInvalidId;
        }

        //invalidIds.push_back(rangeInvalidIds);

        cout << endl << endl;
    }

    cout << "sumInvalidId: " << sumInvalidId << endl;

    return 0;
}
