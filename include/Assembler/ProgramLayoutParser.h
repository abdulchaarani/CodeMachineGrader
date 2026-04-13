#pragma once
#include <fstream>
#include <stdexcept>
#include <string>

#include "ProgramLayout.h"

class ProgramLayoutParser {
   public:
    template <ProgramLayoutLike Layout>
    static Layout parse(const std::string& filename) {
        std::ifstream infile(filename);
        if (!infile) throw std::runtime_error("Cannot open file: " + filename);

        Layout prog;
        std::string line;

        bool inText = false, inData = false;

        while (std::getline(infile, line)) {
            line = stripComment(trim(line));
            if (line.empty()) continue;

            if (line == ".text") {
                inText = true;
                inData = false;
                continue;
            }
            if (line == ".data") {
                inText = false;
                inData = true;
                continue;
            }

            if (line.ends_with(":")) {
                prog.addLabel(line.substr(0, line.size() - 1), inText);
                continue;
            }

            if (inText) {
                prog.addInstruction(line);
            } else if (inData) {
                auto parts = tokenise(line);

                if (parts[0].ends_with(":")) {
                    std::string label = parts[0].substr(0, parts[0].size() - 1);
                    prog.addLabel(label, false);

                    for (size_t i = 1; i < parts.size(); ++i) {
                        prog.addData(std::stoi(parts[i]));
                    }
                } else {
                    prog.addData(std::stoi(parts[0]));
                }
            }
        }

        return prog;
    }

   private:
    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r\f\v");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r\f\v");
        return str.substr(first, last - first + 1);
    }

    static std::string stripComment(const std::string& line) {
        size_t p = line.find('#');
        size_t q = line.find("//");
        size_t cut = std::string::npos;
        if (p != std::string::npos) cut = p;
        if (q != std::string::npos) cut = (cut == std::string::npos) ? q : std::min(cut, q);
        return (cut != std::string::npos) ? trim(line.substr(0, cut)) : line;
    }

    static std::vector<std::string> tokenise(const std::string& s) {
        std::vector<std::string> result;
        std::string cur;
        for (char c : s) {
            if (c == ' ' || c == '\t' || c == ',') {
                auto t = trim(cur);
                if (!t.empty()) result.push_back(t);
                cur.clear();
            } else {
                cur += c;
            }
        }
        auto t = trim(cur);
        if (!t.empty()) result.push_back(t);
        return result;
    }
};