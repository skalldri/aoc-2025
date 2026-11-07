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
        // Helper logic to detect zero crossings.
        // Contains the special logic for handling starting at 0
        bool crossedZero(size_t start, bool left, size_t count);

        size_t simCounter(bool left, size_t count);

        size_t dial = 50;
        size_t zero_count = 0;
        size_t dial_size = 100; // 0 - 99 are valid, EX: modulo this number
};

bool ComboLock::crossedZero(size_t start, bool left, size_t count) {

    if (count > this->dial_size) {
        throw runtime_error(format("count ({}) must be less than dial_size ({})", count, this->dial_size));
    }

    // Special conditions for when we start on 0
    if (start == 0) {
        // If you start on 0 and move one full rotation, then you end up back on zero, so that counts
        if (count == this->dial_size) {
            return true;
        } 
        // All other conditions fail, since anything less than a full rotation does not get us back to zero, and this function
        // rejects movements larger than the dial size
        else {
            return false;
        }
    }
    else {
        if (left) {
            if ((start - count) < 0) {
                return true;
            }
        } else {
            if ((start + count) > this->dial_size) {
                return true;
            }
        }
    }

    return false;
}

size_t ComboLock::simCounter(bool left, size_t count)
{
    long int direction = left ? -1 : 1;
    long int localDial = this->dial;
    size_t zeroCrossingCount = 0;

    for(size_t i = 0; i < count; i++) {
        localDial += direction;

        if (localDial < 0) {
            // Wrap around. -1 -> 99. Zero crossing already captured.
            localDial += this->dial_size;
        } else if (localDial == this->dial_size) {
            // Wrap around. 100 -> 0. Zero crossing NOT YET CAPTURED (see below)
            localDial = 0;
        }

        if (localDial == 0) {
            zeroCrossingCount++;
        }
    }

    return zeroCrossingCount;
}

void ComboLock::left(size_t count) {
    long int result = (this->dial - count);

    size_t initialZeros = this->zero_count;

    if (result < 0) {
        while (result < 0) {
            cout << "Left loop!" << endl;
            result += this->dial_size;
            this->zero_count++;
        }
    }
    else if (result == 0) {
        this->zero_count++;
    }

    // If the dial starts at 0, then we will have over-counted by 1
    if (this->dial == 0) {
        cout << "Dial zero special case" << endl;
        this->zero_count--;
    }

    // If we end on a zero AND we had to wrap-around, we will under-count by 1
    if (result == 0 && count > this->dial_size) {
        cout << "Result zero special case" << endl;
        this->zero_count++;
    }

    // Hard-simulate the zero crossings
    size_t expectedZeros = this->simCounter(true, count);

    size_t actualZeros = this->zero_count - initialZeros;

    if (expectedZeros != actualZeros) {
        cout << "Initial zeros: " << initialZeros << endl;
        cout << "Final zeros: " << this->zero_count << endl;

        throw runtime_error(format("STEP FAILED! expected ({}) != actual ({})", expectedZeros, actualZeros));
    }

    // The dial ends up where it ends up
    this->dial = result;
}

void ComboLock::right(size_t count) {
    size_t result = (this->dial + count);

    if (result >= this->dial_size) {
        while (result >= this->dial_size) {
            result -= this->dial_size;
            this->zero_count++;
        }
    }
    // If we are at zero, count this as part of the password
    else if (result == 0) {
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

    vector<string> fileLines = splitStringByChar(fileContents, '\n');

    ComboLock lock;

    cout << "Lock Dial: " << lock.get_dial() << endl;
    cout << "Lock Count: " << lock.get_zero_count() << endl << endl;

    for (auto& line : fileLines) {
        
        ComboLockMove move = parseLockMove(line);

        cout << line << ": " << (move.left ? "LEFT, " : "RIGHT, ") << move.count << endl;

        if (move.left) {
            lock.left(move.count);
        } else {
            lock.right(move.count);
        }

        cout << "Lock Dial: " << lock.get_dial() << endl;
        cout << "Lock Count: " << lock.get_zero_count() << endl << endl;
    }

    cout << "Password: " << lock.get_zero_count() << endl;
    cout << "File Lines: " << fileLines.size() << endl;

    ComboLock lock2;
    lock2.left(40);
    cout << "Lock2 Dial: " << lock2.get_dial() << endl;
    cout << "Lock2 Count: " << lock2.get_zero_count() << endl << endl;

    lock2.left(910);
    cout << "Lock2 Dial: " << lock2.get_dial() << endl;
    cout << "Lock2 Count: " << lock2.get_zero_count() << endl << endl;

    return 0;
}
