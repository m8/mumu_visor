#ifndef PARSER
#define PARSER

#include <stdint.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

class SimpleParser
{
struct node {
    int id;
    int interval;
    std::string source_path;
    node(int _id, int _interval, string _source_path) : id(_id), interval(_interval), source_path(_source_path){};
};
private:
    vector<node* > vm_confs;

    vector<string> split (const string &s, char delim) {
        vector<string> result;
        stringstream ss (s);
        string item;

        while (getline (ss, item, delim)) {
            result.push_back (item);
        }

        return result;
    }


public:
    SimpleParser(){

    };

    node* parse(string _source_path){
        std::ifstream input(_source_path);
        for(std::string line; getline( input, line ); )
        {
            auto splitted = split(line,',');
            vm_confs.push_back(new node(stoi(splitted[0]),stoi(splitted[1]),splitted[2]));
        }
    }

    ~SimpleParser(){
        
    };
};

#endif