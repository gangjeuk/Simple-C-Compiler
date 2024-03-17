
#include <gtest/gtest.h>
#include "common.h"
#include "statement.h"


namespace MMC{
namespace Statement{
namespace {

    class StatementInfoTest: public ::testing::Test{
    protected:

        StatementInfoTest(){

        }
        ~StatementInfoTest() override{

        }

        void SetUp() override{
            std::cout << "Setting up StatementInfoTest" << "\n";
        }

        void TearDown() override{

        }
    }; // class StatementInfoTest

    TEST_F(StatementInfoTest, func_check)
    {
        Cstring str04("num = 1 + 2 * 3 + 4;");
        Cstring str05("max = (num1 > num2) ? num1 : num2;");
        Cstring str06("{*ptr = *(int*)address;}");
        StatementInfo t;

        EXPECT_TRUE(t.empty());

        t = StatementInfo::getStatementInfo(str04);
        EXPECT_FALSE(t.empty());
        //t.dump(); cout << "\n";
        t = StatementInfo::getStatementInfo(str05);
        EXPECT_FALSE(t.empty());
        //t.dump(); cout << "\n";
        t = StatementInfo::getStatementInfo(str06);
        EXPECT_FALSE(t.empty());
        //t.dump(); cout << "\n";


        Cstring str07("{ int cond, result; cond = 1; while(cond > 0) { result += 1; cond -= 1; } }");
        Cstring str08("{int max; int num1, num2; num1 = 2; num2 = 2; max = (num1 > num2) ? num1 : num2;}");
        Cstring str09("{ int cond, result; cond = 1; if (cond != 1){ result = 1;} }");
        Cstring str10("{ int i = 0; if(i == 0){ i = 4;} else if(i == 1) { i == 3;} else {i = 5;} }");
        Cstring str11("{ int cond, result; cond = 1; while (cond > 0) { result += 1; cond -= 1; }}");
        Cstring str13("{int a, b; a = 0; b = 0; funcall(); return 0;}");
        Cstring str12("{int a, b; a = 0; b = 0; func(a,b); return 0;}");
         t = StatementInfo::getStatementInfo(str07);
         //t.dump(); cout << "\n";
         t = StatementInfo::getStatementInfo(str08);
         //t.dump(); cout << "\n";
         t = StatementInfo::getStatementInfo(str09);
         //t.dump(); cout << "\n";
         t = StatementInfo::getStatementInfo(str10);
         //t.dump(); cout << "\n";
         t = StatementInfo::getStatementInfo(str11);
         //t.dump(); cout << "\n";
         t = StatementInfo::getStatementInfo(str12);
         //t.dump(); cout << "\n";
         t = StatementInfo::getStatementInfo(str13);
         //t.dump(); cout << "\n";
        Cstring str14 = Cstring("{int a, b, biel, ared; for(a = 0; a < 10; a += 1){ if(a == 2){b = 10;} else if(a == 3){b ==11;}} return 0; }");
        t = StatementInfo::getStatementInfo(str14);
        //t.dump(); cout << "\n";

    }

}// namespace
}// namespace Expression
}// namespace MMC
