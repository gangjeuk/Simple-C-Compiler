
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "translate.h"


using namespace std;

namespace MMC{
namespace Units{
namespace{

    
    class TranslatorTest: public ::testing::Test{
    protected:

        TranslatorTest(){

        }
        ~TranslatorTest() override{

        }

        void SetUp() override{
            std::cout << "Setting up TranslatorTest" << "\n";
        }

        void TearDown() override{

        }
    }; // class ExpressionTest

    TEST_F(TranslatorTest, funcCheck){
        Translator test;
        string output;
        Cstring func01("int main() {}");
        Cstring func02("int a;");
        Cstring func03("int qsort(int arr, char *arr);");
        Cstring func04("int qsort(int arr, char *arr, int(*cmp)(int, int) ){};");

        EXPECT_NO_THROW(test.is_funcdef(func01));
        EXPECT_NO_THROW(test.is_funcdef(func02));
        EXPECT_NO_THROW(test.is_funcdef(func03));
        EXPECT_NO_THROW(test.is_funcdef(func04));

        func01 = Cstring("int main() {}");
        func02 = Cstring("int a;");
        func03 = Cstring("int qsort(int arr, char *arr);");
        func04 = Cstring("int qsort(int arr, char *arr, int(*cmp)(int, int) ){};");

        EXPECT_TRUE(test.is_funcdef(func01));
        EXPECT_FALSE(test.is_funcdef(func02));
        EXPECT_FALSE(test.is_funcdef(func03)); // IMPORTANT func03은 함수가 아니라 함수 '선언' 이다!!!
        EXPECT_TRUE(test.is_funcdef(func04));

        func01 = Cstring("int main() {}");
        func02 = Cstring("static int a;");
        func03 = Cstring("int fibo(int n, int a, int b, int c);");
        func04 = Cstring("int qsort(int arr, char *arr, int(*cmp)(int, int) );");

        testing::internal::CaptureStdout();
        test.declaration(func02);
        output = testing::internal::GetCapturedStdout();
        EXPECT_EQ("a : -> static  int, \n", output);

        testing::internal::CaptureStdout();
        test.declaration(func03);
        output = testing::internal::GetCapturedStdout();
        EXPECT_EQ("fibo : (   int,   int,   int,   int,  )(   int,   int,   int,   int,  )->  int, \n", output);


        testing::internal::CaptureStdout();
        test.declaration(func04);
        output = testing::internal::GetCapturedStdout();
        EXPECT_EQ("qsort : (   int, **  char, **(   int,   int,  )(   int,   int,  )  int,  )(   int, **  char, **(   int,   int,  )(   int,   int,  )  int,  )->  int, \n", output);


        func01 = Cstring("int main() {}");
        func02 = Cstring("int a;");
        func03 = Cstring("int qsort(int arr, char *arr);");
        func04 = Cstring("int qsort(int arr, char *arr, int(*cmp)(int, int) ){}");

        
        test.functionDefinition(func04);
    }

} // namespace
} // namespace utils
} // namespace MMC

