//
// Created by apple on 10/09/23.
//

#ifndef CUBIST_ARGPARSER_H
#define CUBIST_ARGPARSER_H

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

/*
 * A very basic parsing logic for input parameters.
 */
void parse(int argc, char* argv[], double& X, double& Y,
           std::string& input_dir, std::string& output_dir)
{
    int opt;
    int count = 4;
    while ((opt = getopt(argc, argv, "X:Y:i:o:")) != -1) {
        switch (opt) {
            case 'X':
                count--;
                X = atof(optarg);
                break;
            case 'Y':
                count--;
                Y = atof(optarg);
                break;
            case 'i':
                count--;
                input_dir = optarg;
                break;
            case 'o':
                count--;
                output_dir = optarg;
                break;
            default:
                throw std::runtime_error("Usage: -X <double> -Y <double> -i <input_dir> -o <output_dir>");
        }
    }

    if (count != 0) {
        throw std::runtime_error("Usage: -X <double> -Y <double> -i <input_dir> -o <output_dir>");
    }
}

#endif //CUBIST_ARGPARSER_H
