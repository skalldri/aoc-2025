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

// Return TRUE if the id is INVALID (since we are trying to detect INVALID ids)
bool isInvalidId(string idStr) {

    // Input string of length N
    // ID is invalid if it is composed of M repeating patterns
    // How long can the repeating patterns be?
    // Max length is (N / 2), Min length is 1
    // Repeating pattern must be a whole number of characters
    // Iterate over the string, producing candidate substrings each time, of a candidate length

    cout << "Testing ID String: " << idStr << endl;

    // i is the substring length we are testing
    // i can be no larger than (idStr.length() / 2): two repetitions is the minimum to make an ID invalid. 
    // If `i` were larger than (idStr.length() / 2), then by definition the ID would be valid.
    for (size_t i = 1; i <= (idStr.length() / 2); i++) {
        cout << "Testing substring length: " << i << endl;

        // i must be an integer divisor of the ID string length
        if ((idStr.length() % i) != 0) {
            cout << "Rejected: " << idStr.length() << " % " << i << " is non-zero" << endl;
            continue;
        }

        // How many times do we propose this substring would be represented in the ID?
        size_t numInstances = idStr.length() / i;

        cout << "NumInstances: " << numInstances << endl;

        // We can always pull from position 0 up to length i
        // idSubStr is the proposed "chunk"
        string idSubStr = idStr.substr(0, i);

        cout << "idSubStr: " << idSubStr << endl;

        bool noMatch = false;

        // Loop over the idStr and see if it's made up of numInstances of idSubStr
        for (size_t j = 0; j < numInstances; j++) {
            string testStr = idStr.substr(j * i, i);

            // If our test chunk pulled from the idStr is not the same as our candidate, then 
            // this ID is not a repeating pattern of candidate, therefore we don't need to test it any more
            if (testStr != idSubStr) {
                cout << "Rejected: " << testStr << " != " << idSubStr << endl;

                noMatch = true;
                break;
            }
        }

        if (noMatch) {
            cout << "Substring length " << i << "does not produce a good match" << endl;
            continue;
        }

        // If we got here, then we have determined that idStr is composed of numInstances of idSubStr.
        // This means it is an invalid ID.
        return true;
    }


    // If we did not detect a pattern in the string, it is VALID, so return true.
    return false;
}

vector<size_t> findInvalidIds(Range range) {
    vector<size_t> invalidIds;
    // Loop through all IDs
    for (size_t id = range.start; id <= range.end; id++) {
        string idStr = to_string(id);

        if (isInvalidId(idStr)) {
            cout << "Invalid ID! " << id << endl;
            invalidIds.push_back(id);
        }
    }

    return invalidIds;
}

int main(int argc, char** argv) {

    cout << "Hello world!" << endl;

    string fileContents = readFileIntoString(filesystem::path("/workspaces/aoc-2025/day2.2/input"));

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
