
#include <gtest/gtest.h>
#include "common.h"

namespace MMC{
namespace utils{
namespace {

    class IsOperatorTest: public ::testing::Test{
    protected:

        IsOperatorTest(){

        }
        ~IsOperatorTest() override{

        }

        void SetUp() override{
            std::cout << "Setting up CstringTest" << "\n";
        }

        void TearDown() override{

        }
    }; // class IsOperatorTest

TEST_F(IsOperatorTest, OperatorCheck)
{
    IsOperator op;
    EXPECT_TRUE(op.is_binaryOperator("+"));
    EXPECT_TRUE(op.is_binaryOperator("-"));

    EXPECT_TRUE(op.is_prefixOperator("sizeof"));
    EXPECT_TRUE(op.is_prefixOperator("+"));
    
    EXPECT_TRUE(op.is_assign_op("="));
    EXPECT_TRUE(op.is_assign_op("<<="));
    EXPECT_TRUE(op.is_assign_op("+="));
}

TEST_F(IsOperatorTest, GetBianaryLevelTest)
{
    IsOperator op;
    EXPECT_EQ(op.get_binary_level("*"), 1);
    EXPECT_EQ(op.get_binary_level("^"), 7);
    EXPECT_EQ(op.get_binary_level("|"), 8);
    EXPECT_EQ(op.get_binary_level("||"), 10);
    EXPECT_EQ(op.get_binary_level("=="), 5);
    EXPECT_EQ(op.get_binary_level("<<"), 3);
}

class CstringTest : public ::testing::Test{
    protected:

    CstringTest(){

    }
    ~CstringTest() override{

    }

    void SetUp() override{
        std::cout << "Setting up CstringTest" << "\n";
    }

    void TearDown() override{

    }


}; // class Cstring

TEST_F(CstringTest, CstringGetNumber){
    
    Cstring str("4+2");
    EXPECT_EQ("4", str.get_number());
    EXPECT_EQ("+", str.get_token());
    EXPECT_EQ("2", str.get_number());

}

TEST_F(CstringTest, CstringEtNumber02){

    Cstring str("12314+123/1234125+2321*231");
    EXPECT_EQ("12314", str.get_number());
    EXPECT_EQ("+", str.get_token());
    EXPECT_EQ("123", str.get_number());
    EXPECT_EQ("/", str.get_token());
    EXPECT_EQ("1234125", str.get_number());
    EXPECT_EQ("+", str.get_token());
    EXPECT_EQ("2321", str.get_number());
    EXPECT_EQ("*", str.get_token());
    EXPECT_EQ("231", str.get_number());
}

TEST_F(CstringTest, CstringGetExtra){
    Cstring str("1234+123/=abc=12+12<<=abc==12// /* ===");
    EXPECT_EQ("1234", str.get_ctoken());
    EXPECT_EQ("+", str.get_ctoken());
    EXPECT_EQ("123", str.get_ctoken());
    EXPECT_EQ("/=", str.get_ctoken());
    EXPECT_EQ("abc", str.get_ctoken());
    EXPECT_EQ("=", str.get_ctoken());
    EXPECT_EQ("12", str.get_ctoken());
    EXPECT_EQ("+", str.get_ctoken());
    EXPECT_EQ("12", str.get_ctoken());
    EXPECT_EQ("<<=", str.get_ctoken());
    EXPECT_EQ("abc", str.get_ctoken());
    EXPECT_EQ("==", str.get_ctoken());
    EXPECT_EQ("12", str.get_ctoken());
    EXPECT_EQ("//", str.get_ctoken());
    EXPECT_EQ("/*", str.get_ctoken());
    EXPECT_EQ("==", str.get_ctoken());
    EXPECT_EQ("=", str.get_ctoken());
}

TEST_F(CstringTest, GetCTokenTEST)
{
    Cstring test("int main(){ int a; int b; int c = a + b; return 0;}");
    EXPECT_EQ(test.get_ctoken(), "int");
    EXPECT_EQ(test.get_ctoken(), "main");
    EXPECT_EQ(test.get_ctoken(), "(");
    EXPECT_EQ(test.get_ctoken(), ")");
    EXPECT_EQ(test.get_ctoken(), "{");
    EXPECT_EQ(test.get_ctoken(), "int");
    EXPECT_EQ(test.get_ctoken(), "a");
    EXPECT_EQ(test.get_ctoken(), ";");
    EXPECT_EQ(test.get_ctoken(), "int");
    EXPECT_EQ(test.get_ctoken(), "b");
    EXPECT_EQ(test.get_ctoken(), ";");
    EXPECT_EQ(test.get_ctoken(), "int");
    EXPECT_EQ(test.get_ctoken(), "c");
    EXPECT_EQ(test.get_ctoken(), "=");
    EXPECT_EQ(test.get_ctoken(), "a");
    EXPECT_EQ(test.get_ctoken(), "+");
    EXPECT_EQ(test.get_ctoken(), "b");
    EXPECT_EQ(test.get_ctoken(), ";");
    EXPECT_EQ(test.get_ctoken(), "return");
    EXPECT_EQ(test.get_ctoken(), "0");
    EXPECT_EQ(test.get_ctoken(), ";");
    EXPECT_EQ(test.get_ctoken(), "}");

    test = Cstring("void (*qsort)(int, int)");
    EXPECT_EQ(test.get_ctoken(), "void");
    EXPECT_EQ(test.get_ctoken(), "(");
    EXPECT_EQ(test.get_ctoken(), "*");
    EXPECT_EQ(test.get_ctoken(), "qsort");
    EXPECT_EQ(test.get_ctoken(), ")");
    EXPECT_EQ(test.get_ctoken(), "(");
    EXPECT_EQ(test.get_ctoken(), "int");
    EXPECT_EQ(test.get_ctoken(), ",");
    EXPECT_EQ(test.get_ctoken(), "int");
    EXPECT_EQ(test.get_ctoken(), ")");


    test = Cstring("void qsort(char *arr, int size, int (*func)(int,int))");
    EXPECT_EQ(test.get_ctoken(), "void");
    EXPECT_EQ(test.get_ctoken(), "qsort");
    EXPECT_EQ(test.get_ctoken(), "(");
    EXPECT_EQ(test.get_ctoken(), "char");
    EXPECT_EQ(test.get_ctoken(), "*");
    EXPECT_EQ(test.get_ctoken(), "arr");
    EXPECT_EQ(test.get_ctoken(), ",");
    EXPECT_EQ(test.get_ctoken(), "int");
    EXPECT_EQ(test.get_ctoken(), "size");
    EXPECT_EQ(test.get_ctoken(), ",");
    EXPECT_EQ(test.get_ctoken(), "int");
    EXPECT_EQ(test.get_ctoken(), "(");
    EXPECT_EQ(test.get_ctoken(), "*");
    EXPECT_EQ(test.get_ctoken(), "func");
    EXPECT_EQ(test.get_ctoken(), ")");
    EXPECT_EQ(test.get_ctoken(), "(");
    EXPECT_EQ(test.get_ctoken(), "int");
    EXPECT_EQ(test.get_ctoken(), ",");
    EXPECT_EQ(test.get_ctoken(), "int");
    EXPECT_EQ(test.get_ctoken(), ")");
    EXPECT_EQ(test.get_ctoken(), ")");

    test = Cstring("num += 2;");
    EXPECT_EQ(test.get_ctoken(), "num");
    EXPECT_EQ(test.get_ctoken(), "+=");
    EXPECT_EQ(test.get_ctoken(), "2");


    test = Cstring("max = (num1 > num2) ? num1 : num2;");
    EXPECT_EQ(test.get_ctoken(), "max");
    EXPECT_EQ(test.get_ctoken(), "=");
    EXPECT_EQ(test.get_ctoken(), "(");
    EXPECT_EQ(test.get_ctoken(), "num1");
    EXPECT_EQ(test.get_ctoken(), ">");
    EXPECT_EQ(test.get_ctoken(), "num2");
    EXPECT_EQ(test.get_ctoken(), ")");
    EXPECT_EQ(test.get_ctoken(), "?");
    EXPECT_EQ(test.get_ctoken(), "num1");
    EXPECT_EQ(test.get_ctoken(), ":");
    EXPECT_EQ(test.get_ctoken(), "num2");
    EXPECT_EQ(test.get_ctoken(), ";");


    test = Cstring("*ptr = *(int *)address;");
    EXPECT_EQ(test.get_ctoken(), "*");
    EXPECT_EQ(test.get_ctoken(), "ptr");
    EXPECT_EQ(test.get_ctoken(), "=");
    EXPECT_EQ(test.get_ctoken(), "*");
    EXPECT_EQ(test.get_ctoken(), "(");
    EXPECT_EQ(test.get_ctoken(), "int");
    EXPECT_EQ(test.get_ctoken(), "*");
    EXPECT_EQ(test.get_ctoken(), ")");
    EXPECT_EQ(test.get_ctoken(), "address");
    EXPECT_EQ(test.get_ctoken(), ";");

    test = Cstring("*(arr+idx) = value + 1;");
    EXPECT_EQ(test.get_ctoken(), "*");
    EXPECT_EQ(test.get_ctoken(), "(");
    EXPECT_EQ(test.get_ctoken(), "arr");
    EXPECT_EQ(test.get_ctoken(), "+");
    EXPECT_EQ(test.get_ctoken(), "idx");
    EXPECT_EQ(test.get_ctoken(), ")");
    EXPECT_EQ(test.get_ctoken(), "=");
    EXPECT_EQ(test.get_ctoken(), "value");
    EXPECT_EQ(test.get_ctoken(), "+");
    EXPECT_EQ(test.get_ctoken(), "1");
    EXPECT_EQ(test.get_ctoken(), ";");

    test = Cstring("cond ? true : false;");
    EXPECT_EQ(test.get_ctoken(), "cond");
    EXPECT_EQ(test.get_ctoken(), "?");
    EXPECT_EQ(test.get_ctoken(), "true");
    EXPECT_EQ(test.get_ctoken(), ":");
    EXPECT_EQ(test.get_ctoken(), "false");
    EXPECT_EQ(test.get_ctoken(), ";");

    

}

}// namespace
}// namespace Expression
}// namespace MMC
