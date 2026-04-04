#pragma once
#include <stdexcept>
#include <string>
#include <vector>

template <typename MEM_T, typename Layout_T>
void setVariable(MEM_T& mem, const Layout_T& labels, const std::string& varName,
                 typename MEM_T::value_type value) {
    auto it = labels.find(varName);
    if (it == labels.end()) throw std::runtime_error("Variable not found: " + varName);
    mem[it->second] = value;
}

template <typename MEM_T, typename Layout_T>
void setArrayElement(MEM_T& mem, const Layout_T& labels, const std::string& arrayName, size_t index,
                     typename MEM_T::value_type value) {
    auto it = labels.find(arrayName);
    if (it == labels.end()) throw std::runtime_error("Array not found: " + arrayName);
    mem[it->second + index] = value;
}

template <typename MEM_T, typename Layout_T>
typename MEM_T::value_type getVariable(const MEM_T& mem, const Layout_T& labels,
                                       const std::string& varName) {
    auto it = labels.find(varName);
    if (it == labels.end()) throw std::runtime_error("Variable not found: " + varName);
    return mem[it->second];
}

template <typename MEM_T, typename Layout_T>
typename MEM_T::value_type getArrayElement(const MEM_T& mem, const Layout_T& labels,
                                           const std::string& arrayName, size_t index) {
    auto it = labels.find(arrayName);
    if (it == labels.end()) throw std::runtime_error("Array not found: " + arrayName);
    return mem[it->second + index];
}

template <typename MEM_T, typename Layout_T>
void appendArrayElement(MEM_T& mem, const Layout_T& labels, const std::string& arrayName,
                        size_t index, typename MEM_T::value_type value) {
    auto it = labels.find(arrayName);
    if (it == labels.end()) throw std::runtime_error("Array not found: " + arrayName);
    mem[it->second + index] = value;
}

template <typename MEM_T, typename Layout_T>
std::vector<typename MEM_T::value_type> getArray(const MEM_T& mem, const Layout_T& labels,
                                                 const std::string& arrayName, size_t arraySize) {
    auto it = labels.find(arrayName);
    if (it == labels.end()) throw std::runtime_error("Array not found: " + arrayName);
    std::vector<typename MEM_T::value_type> result;
    result.reserve(arraySize);
    for (size_t i = 0; i < arraySize; ++i) result.push_back(mem[it->second + i]);
    return result;
}

template <typename MEM_T, typename Layout_T>
void setArray(MEM_T& mem, const Layout_T& labels, const std::string& arrayName,
              const std::vector<typename MEM_T::value_type>& values) {
    auto it = labels.find(arrayName);
    if (it == labels.end()) throw std::runtime_error("Array not found: " + arrayName);
    for (size_t i = 0; i < values.size(); ++i) mem[it->second + i] = values[i];
}

template <typename MEM_T, typename Layout_T>
void clearMemoryPastLabel(MEM_T& mem, const Layout_T& labels, const std::string& labelName) {
    auto it = labels.find(labelName);
    if (it == labels.end()) throw std::runtime_error("Label not found: " + labelName);
    for (size_t i = it->second; i < mem.size(); ++i) mem[i] = {};
}