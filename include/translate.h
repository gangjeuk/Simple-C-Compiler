#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <variant>
#include <map>
#include <tuple>
#include "common.h"
#include "tokentype.h"
#include "expression.h"
#include "statement.h"

namespace MMC{

namespace Units{

using namespace std;
using namespace MMC::utils;

    class basic_unit
    {
        protected:
            string buffer;
    }; // class basic_unit
    
    class Translator : public basic_unit  // 번역 단위: 외부 선언 | 번역 단위 | 외부 선언
    {   
        private:
            string dataseg;
            string codeseg;
            mutable map<string, bool> reg_usage{{"$t0", false},{"$t1", false},{"$t2", false},{"$t3", false},{"$t4", false},{"$t5", false},{"$t6", false},{"$t7", false}, {"$t8", false}, {"$t9", false}, {"$s7", false}, {"$s6", false}, {"$s5", false}};            
            vector<MMC::Statement::FlowBlock> in;
            bool changed;
            using value = int;
            using scope = int;
            using obj_size = vector<int>;
            map<string, tuple<string, scope, value, obj_size>> identifierDict; // 식벌자, <트랜지스터, 스코프, 값> 로 구성
            void update_iden_dict(MMC::Statement::FlowBlock fb);
            string get_small_reg();
        public:


             void translateUnit(Cstring& buffer); // 외부 입력 임으로 string
             void declaration(Cstring& buffer); 

            // 외부 선언: 함수 정의 | 선언
             bool is_funcdef(Cstring& buffer);

             void externalDeclaration(Cstring& buffer);         // 선언: 선언 지정자 | 초기 선언자 리스트(opt) |   
             void functionDefinition(Cstring& buffer);  // 함수: 선언 지정자(opt) | 선언자 | 선언자 리스트(opt) |  compound-statement
             string dump();

    }; // class Translator





} // namespace Units
} // namespace MMC


#endif