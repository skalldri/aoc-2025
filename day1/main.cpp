#include <iostream>
#include <string>
#include <filesystem>
#include <format>
#include <fstream>
#include <vector>

using namespace std;

class ComboLock {
    public:

        ComboLock() = default;
        ~ComboLock() = default;

        void left(size_t count);

        void right(size_t count);

        size_t get_dial() {
            return this->dial;
        }

        size_t get_zero_count() {
            return this->zero_count;
        }
    
    private:
        size_t dial = 50;
        size_t zero_count = 0;
        size_t dial_size = 100; // 0 - 99 are valid, EX: modulo this number
};

void ComboLock::left(size_t count) {
    long int result = (this->dial - count);

    while (result < 0) {
        result += this->dial_size;
    }

    if (result == 0) {
        this->zero_count++;
    }

    // The dial ends up where it ends up
    this->dial = result;
}

void ComboLock::right(size_t count) {
    size_t result = (this->dial + count) % this->dial_size;

    // If we are at zero, count this as part of the password
    if (result == 0) {
        this->zero_count++;
    }

    // The dial ends up where it ends up
    this->dial = result;
}

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

struct ComboLockMove {
    bool left;
    size_t count;
};

ComboLockMove parseLockMove(const string& input) {
    // Expect format <L|R><integer>

    ComboLockMove out;

    if (input.at(0) == 'L') {
        out.left = true;
    } else if (input.at(0) == 'R') {
        out.left = false;
    } else {
        throw runtime_error(format("{} is a malformed combo lock move", input));
    }

    // Parse out the integer component
    out.count = stoi(input.substr(1));

    return out;
}

int main(int argc, char** argv) {

    cout << "Hello world!" << endl;

    string fileContents = readFileIntoString(filesystem::path("/workspaces/aoc-2025/day1/input"));

    //cout << fileContents << endl;

    vector<string> fileLines = splitStringByChar(fileContents, '\n');

    ComboLock lock;

    for (auto& line : fileLines) {
        
        ComboLockMove move = parseLockMove(line);

        cout << line << ": " << (move.left ? "LEFT, " : "RIGHT, ") << move.count << endl;

        if (move.left) {
            lock.left(move.count);
        } else {
            lock.right(move.count);
        }

        cout << "Lock Dial: " << lock.get_dial() << endl;
    }

    cout << "Password: " << lock.get_zero_count() << endl;

    cout << "File Lines: " << fileLines.size() << endl;

    

    return 0;
}