#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

template <typename T>
concept ProgramLayoutLike =
    requires(T prog, std::string label, std::string instruction, int value, bool inText) {
        prog.addInstruction(instruction);
        prog.addData(value);
        prog.addLabel(label, inText);
    };

template <typename AddrT, typename DataT>
struct ProgramLayoutVonNewman {
    std::vector<std::string> text;
    std::vector<int> data;
    std::unordered_map<std::string, DataT> labels;

    void addInstruction(const std::string& instr) {
        text.push_back(instr);
        address++;
    }

    void addData(int value) {
        data.push_back(value);
        address++;
    }

    void addLabel(const std::string& label, bool) {
        labels[label] = static_cast<uint16_t>(address);
    }

   private:
    AddrT address = 0;
};

template <typename AddrT, typename DataT>
struct ProgramLayoutHarvard {
    std::vector<std::string> text;
    std::vector<DataT> data;
    std::unordered_map<std::string, AddrT> textLabels;
    std::unordered_map<std::string, AddrT> dataLabels;

    void addInstruction(const std::string& instr) {
        text.push_back(instr);
        textAddress++;
    }

    void addData(int value) {
        data.push_back(value);
        dataAddress++;
    }

    void addLabel(const std::string& label, bool inText) {
        if (inText)
            textLabels[label] = textAddress;
        else
            dataLabels[label] = dataAddress;
    }

   private:
    AddrT textAddress = 0;
    AddrT dataAddress = 0;
};

using ProgramLayoutAcc = ProgramLayoutVonNewman<uint16_t, int16_t>;
using ProgramLayoutRisc = ProgramLayoutHarvard<uint32_t, int32_t>;