//
// Created by Kaushik on 08/09/23.
//
#include "TargetPositionRecord.h"

std::string TargetPositionRecord::str() const {
    return "{Security : " + std::to_string(security_id) + ", "
           + "Arrival_time: " + std::to_string(arrival_time) + ", "
           + "End_time: " + std::to_string(end_time) + ", "
           + "Target_position: " + std::to_string(target_position) + "}";
}

void TargetPositionRecord::clear()
{
    security_id = 0;
    arrival_time = 0;
    end_time = 0;
    target_position = 0;
}

Time TargetPositionRecord::getTime() const {
    return arrival_time;
}

bool TargetPositionRecord::parseCSVRow(char line[1024]) {
    char *parse_token;
    int idx = 0;
    parse_token = std::strtok(line, ",");
    try {
        while (parse_token != nullptr) {
            // Decide which data member to store based on the idx.
            switch (idx) {
                case 0:
                    security_id = std::stoi(parse_token);
                    break;
                case 1:
                    arrival_time = std::stoll(parse_token);
                    break;
                case 2:
                    end_time = std::stoll(parse_token);
                    break;
                case 3:
                    target_position = std::stod(parse_token);
                    break;
                default:
                    break;
            }
            idx++;
            parse_token = strtok(nullptr, ",");
        }
    } catch (const std::invalid_argument &e) {
        std::cerr << "[TargetPositionRecord] Error Parsing CSV Row" << e.what() << std::endl;
        return false;
    } catch (const std::out_of_range &e) {
        std::cerr << "[TargetPositionRecord] Error Parsing CSV Row" << e.what() << std::endl;
        return false;
    }

    return idx == 4;
}

