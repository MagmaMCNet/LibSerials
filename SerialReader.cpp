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

size_t SerialReader::GetLinePadding() {
    size_t maxLength = 0;
    for (const auto& pair : data_map)
        maxLength = std::max(maxLength, pair.first.length());
    return maxLength + 2; // count for [,]
}

std::string SerialReader::FormatRow(const std::string& title, const std::string& value) {
    size_t titleLength = title.length();
    size_t MaxPadding = GetLinePadding();
    size_t padding = (MaxPadding > titleLength) ? (MaxPadding - titleLength) : 0;
    std::string paddedTitle = title + std::string(padding, ' ');
    return "\033[1;36m" + paddedTitle + "\033[1;37m  " + value;
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
        return str.substr(1, str.size() - 2);
    }
    return str;
}
