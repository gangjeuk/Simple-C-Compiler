#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "translate.h"
#include "statement.h"

using namespace std;
namespace MMC{

    class Compiler{
        private:
            std::string cfile;
            Units::Translator translator;
            std::string fname;

        public:
        Compiler(std::string filename){
            fname = filename;
            fstream in(filename);
            if(in.is_open()) {
                in.seekg(0, std::ios::end);

                long unsigned int size = in.tellg();

                cfile.resize(size);

                in.seekg(0, std::ios::beg);

                in.read(&cfile[0], size);

                in.close();
                cout << cfile;

                
            }else{
                cout << "no such file" << "\n";
                exit(EXIT_FAILURE);
            }
        }

        void compile(){
            if(cfile.empty())
            {
                std::cout << "no such file" << "\n";
                exit(EXIT_FAILURE);
            }

            utils::Cstring cstr(this->cfile);
            translator.translateUnit(cstr);

        }
        void compile(std::string filename){
            Compiler compiler(filename);
            return compiler.compile();
        }
        
        void to_assemble()
        {
            string asm_file_name =  (fname + ".asm");

            ofstream out(asm_file_name);
            
            if(out.is_open()) {
                out << translator.dump();
                
            }
        }
    };
}

using namespace MMC::utils;
using namespace MMC::Units;
using namespace MMC::Declare;

int main(int argc, char* argv[]){
    if(argc < 2)
    {
        cout <<"usage: mmc [filename]";
        return -1;
    }
    for(int i = 1; i < argc; i++)
    {
        string filename = argv[i];
        MMC::Compiler my_minor_c_compiler(filename);

        my_minor_c_compiler.compile();

        my_minor_c_compiler.to_assemble();
    }

}