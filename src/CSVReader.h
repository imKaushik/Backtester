//
// Created by Kaushik on 04/09/23.
//

#ifndef CUBIST_CSVREADER_H
#define CUBIST_CSVREADER_H

#include <fstream>
#include <queue>
#include <string>
#include <iostream>


/*
 * CSVReader : Treat it like a file reader or socket reader, once consumed
 * it is erased and you cannot get back. You can look ahead, by fetching the next
 * record's timestamp to decide if you want to read or not.
 *
 */

template<typename T>
class CSVReader {
public:
    // Should only be used with objects inherited from Data Model class.
    static_assert(is_derived_from_data_model<T>::value, "Template typename class must be derived from Data Model.");

    // Illegal : Meaningless to create a CSVReader without a csv file.
    CSVReader() = delete;

    // Parameterized Constructor : Opens the file and reads the header.
    explicit CSVReader(const std::string &file_name) noexcept(false);

    // Get the next record data, but not consumed from the reader buffer.
    Time getNextTimestamp() noexcept(false) ;

    // Check if you have reached end or not.
    bool hasNext();

    // Consume a record from the buffer if available. Once consumed it is erased from the reader's buffer.
    void consumeRecord(T &record) noexcept(false) ;

    // Print the header read from the CSV file, just a customary function.
    void printHeader();

private:
    // Read `count` number of lines from the CSV file, and create objects  of T.
    int fillRecords(int count);

    // Read a line from the CSV file (if still not eof), create T and add it to the reader's buffer.
    bool fillRecord();

    // Reader's state data, all of them are RAII safe, so need to handle them specially in destructor.
    std::queue<T> buffer;
    std::ifstream file;
    std::string header;

    // The following are used for parsing and constructing a record in readRecord function.
    // Declared here to avoid creating temporary objects again and again.
    char parse_buffer[1024];
};

/* Constructor for CSVReader
 *
 * Opens the csv file and reads the header.
 */
template<typename T>
CSVReader<T>::CSVReader(const std::string &file_name) noexcept(false) {
    file.open(file_name);
    if (!file.is_open()) {
        // If a reader could not opened, it is fatal.
        throw std::runtime_error("[CSVReader] Failed to open file : " + file_name);
    }
    getline(file, header);
}

/*
 * Get the timestamp of the next record available for consumption.
 * Throw error, indicating end of the reader. So users, should
 * check if there is another record to consume before calling this
 * function.
 */
template<typename T>
Time CSVReader<T>::getNextTimestamp() noexcept(false) {
    if (!hasNext()) {
        // Accessing front of empty queue may or may not raise error,
        // so I'm just raising one here.
        throw std::runtime_error("No elements left to read!");
    }
    return buffer.front().getTime();
}

// Check if there are any more records to read from the file.
template<typename T>
bool CSVReader<T>::hasNext() {
    return !buffer.empty() || fillRecord();
}

/* Consume a record from the buffer if available. Once consume it is erased
 * from the reader's buffer.
 *
 * @param T& r : Record reference to which the consumed record needs to be assigned.
 */
template<typename T>
void CSVReader<T>::consumeRecord(T &record) noexcept(false) {
    if (!hasNext()) {
        // Accessing front of empty queue may or may not raise error,
        // so I'm just raising one here.
        throw std::runtime_error("No elements left to read!");
    }

    record = buffer.front();
    buffer.pop();
}

/* Read `count` number of lines from the CSV file sequentially, and create objects of T.
 * Not used at the moment, but created an abstraction for possible future use cases.
 *
 * @param count : Number of records to read.
 */
template<typename T>
int CSVReader<T>::fillRecords(int count) {
    int read_count = 0;
    while ((read_count < count) && !file.eof()) {
        if (!fillRecord()) break;
        read_count++;
    }

    return read_count;
}

/*
 * Read a line from the CSV file (if still not eof), create CSVReader and add it to the reader's buffer.
 * Parsing could have been done using istringstream instead of strtok, but the former is 4 times slower than latter.
 */
template<typename T>
bool CSVReader<T>::fillRecord() {
    if (file.eof())
        return false;

    int idx = 0;
    T record;

    file.getline(parse_buffer, sizeof(parse_buffer));
    if (!record.parseCSVRow(parse_buffer)) {
        return false;
    }

    buffer.push(record);
    return true;
}

/*
 * Print the header read from the CSV file, just a customary function.
 */
template<typename T>
void CSVReader<T>::printHeader() {
    std::cout << header << std::endl;
}

#include "MinuteBarRecord.h"
#include "TargetPositionRecord.h"

// Typedefing for easy usage.
typedef CSVReader<MinuteBarRecord> MarketDataReader;
typedef CSVReader<TargetPositionRecord> TargetPositionReader;

#endif //CUBIST_CSVREADER_H
