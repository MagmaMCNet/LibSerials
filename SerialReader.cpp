#include "SerialReader.h"
#include <sstream>

SerialReader::SerialReader(const std::string& data) {
    std::istringstream stream(data);
    std::string line;
    bool serialDataFound = false;

    while (std::getline(stream, line)) {
        line = TrimLineEndings(line);

        if (line.empty()) continue;

        if (line == "SerialData") {
            serialDataFound = true;
            continue;
        }

        size_t delimiter_pos = line.find(' ');
        if (delimiter_pos != std::string::npos) {
            std::string key = line.substr(0, delimiter_pos);
            std::string value = line.substr(delimiter_pos + 1);
            key = RemoveBrackets(key);
            data_map[key] = value;
        }
    }
}

bool SerialReader::WriteRow(const std::string& row, const std::string& value) {
    data_map[row] = value;
    return true;
}

bool SerialReader::ReadRow(const std::string& row, std::string& value) {
    auto it = data_map.find(row);
    if (it != data_map.end()) {
        value = it->second;
        return true;
    }
    return false;
}

std::string SerialReader::Export() const {
    std::ostringstream export_stream;
    for (const auto& entry : data_map) {
        export_stream << "[" << entry.first << "] " << entry.second << std::endl;
    }
    return export_stream.str();
}

std::string SerialReader::TrimLineEndings(const std::string& str) {
    size_t start = 0;
    size_t end = str.length();

    while (end > start && (str[end - 1] == '\n' || str[end - 1] == '\r')) {
        --end;
    }

    return str.substr(start, end - start);
}

std::string SerialReader::RemoveBrackets(const std::string& str) {
    if (str.front() == '[' && str.back() == ']') {
        return str.substr(1, str.size() - 2);  // Remove the brackets
    }
    return str;
}
