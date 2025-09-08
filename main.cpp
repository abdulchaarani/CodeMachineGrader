#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

std::vector<std::string> split(const std::string& s, char delimiter = ' ') {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    return result;
}

int main() {
    std::filesystem::path file{
        "C:\\Users\\chaar\\Documents\\Code\\Poly\\INF1600\\CodeMachine\\code.s"};
    std::ifstream infile(file);

    std::vector<std::string> text;
    std::vector<int> data;
    std::unordered_map<std::string, int> labels;

    std::string line;

    size_t textSize = 0;
    size_t dataSize = 0;
    size_t codeSize = 0;

    // First pass: parse labels
    while (std::getline(infile, line)) {
        if (line.size() == 0 || line.starts_with("#")) {
            continue;
        }

        if (line.ends_with(":")) {
            labels[line] = codeSize;
            continue;
        }

        if (line == ".data") {
            break;
        }

        text.push_back(line);

        ++textSize;
        ++codeSize;
    }

    // First pass: parse data
    while (std::getline(infile, line)) {
        if (line.size() == 0 || line.starts_with("#")) {
            continue;
        }

        const auto items = split(line);
        std::string label = items[0];
        label.pop_back(); // remove ":"

        int value = std::stoi(items[1]);
        labels[label] = value;
        data.push_back(value);

        ++dataSize;
        ++codeSize;
    }

    for (auto l : text) {
        std::cout << l << '\n';
    }
    for (auto l : data) {
        std::cout << l << '\n';
    }
}