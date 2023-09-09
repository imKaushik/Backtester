//
// Created by Kaushik on 08/09/23.
//
#include "MinuteBarRecord.h"

// Get string representation for object.
std::string MinuteBarRecord::str() const {
    return "{SecurityId: " + std::to_string(security_id) + ", "
           + "Start_time: " + std::to_string(start_time) + ", "
           + "Price: " + std::to_string(price) + ", "
           + "Spread: " + std::to_string(spread) + ", "
           + "Volume: " + std::to_string(volume) + "}";
}

// Reset data members.
void MinuteBarRecord::clear() {
    security_id = 0;
    start_time = 0;
    price = 0;
    spread = 0;
    volume = 0;
}

// Get the associate timestamp.
Time MinuteBarRecord::getTime() const {
    return start_time;
}

// Populate an object after parsing a CSV row.
bool MinuteBarRecord::parseCSVRow(char line[1024]) {
    char *parse_token;
    int idx = 0;
    parse_token = strtok(line, ",");
    // Decide which data member to store based on the idx.
    try {
        while (parse_token != nullptr) {
            switch (idx) {
                case 0:
                    security_id = std::stoi(parse_token);
                    break;
                case 1:
                    start_time = std::stoll(parse_token);
                    break;
                case 2:
                    // Do not need to store end_time
                    break;
                case 3:
                    price = std::stod(parse_token);
                    break;
                case 4:
                    spread = std::stod(parse_token);
                    break;
                case 5:
                    volume = std::stod(parse_token);
                    break;
                default:
                    break;
            }
            idx++;
            parse_token = strtok(nullptr, ",");
        }
    } catch (const std::invalid_argument &e) {
        std::cerr << "[MinuteBarRecord] Error Parsing CSV Row" << e.what() << std::endl;
        return false;
    } catch (const std::out_of_range &e) {
        std::cerr << "[MinuteBarRecord] Error Parsing CSV Row" << e.what() << std::endl;
        return false;
    }
    return idx == 6;
}
