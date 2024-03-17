
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <variant>
#include <memory>
#include <iostream>
#include <string>
#include "expression.h"

namespace MMC{
namespace Expression{
namespace {

    class PrimaryExpressionTest: public ::testing::Test{
    protected:

        PrimaryExpressionTest(){

        }
        ~PrimaryExpressionTest() override{

        }

        void SetUp() override{
            std::cout << "Setting up PrimaryExpressionTest" << "\n";
        }

        void TearDown() override{

        }
        
    }; // class ExpressionTest

TEST_F(PrimaryExpressionTest, methodCheck)
{
    Cstring str01("\"Hello, world!\"");
    Cstring str02("num = (1000)");
    Cstring str03("a, b, c, d");
    PrimaryExpression t;

    EXPECT_TRUE(t.empty());

    t = PrimaryExpression::getPrimaryExpression(str01);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";

    t = PrimaryExpression::getPrimaryExpression(str02);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";

    t = PrimaryExpression::getPrimaryExpression(str03);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";
 str03.get_ctoken();
    t = PrimaryExpression::getPrimaryExpression(str03);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";
 str03.get_ctoken();
    t = PrimaryExpression::getPrimaryExpression(str03);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";
 str03.get_ctoken();
    t = PrimaryExpression::getPrimaryExpression(str03);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";
 str03.get_ctoken();

}

/*------------------------------------------------*/


    class AssignmentExpressionTest: public ::testing::Test{
    protected:

        AssignmentExpressionTest(){

        }
        ~AssignmentExpressionTest() override{

        }

        void SetUp() override{
            std::cout << "Setting up PrimaryExpressionTest" << "\n";
        }

        void TearDown() override{

        }
        
    }; // class ExpressionTest

TEST_F(AssignmentExpressionTest, methodCheck)
{
    Cstring str01("num");
    Cstring str02("num = 1");
    Cstring str03("num += 2");
    Cstring str04("num <<= 2");
    Cstring str05("num = 1 + 2 * 3 + 4");
    Cstring str06("max = (num1 > num2) ? num1 : num2");
    Cstring str07("*ptr = *(int)address");
    Cstring str08("*(arr+idx) = value");
    AssignmentExpression t;

    EXPECT_TRUE(t.empty());
    t = AssignmentExpression::getAssignmentExpression(str01);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";

    t = AssignmentExpression::getAssignmentExpression(str02);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";

    t = AssignmentExpression::getAssignmentExpression(str03);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";

    t = AssignmentExpression::getAssignmentExpression(str04);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";

    t = AssignmentExpression::getAssignmentExpression(str05);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";

    t = AssignmentExpression::getAssignmentExpression(str06);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";

    t = AssignmentExpression::getAssignmentExpression(str07);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";

    t = AssignmentExpression::getAssignmentExpression(str08);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";


}



/*------------------------------------------------*/


    class ExpressionInfoTest: public ::testing::Test{
    protected:

        ExpressionInfoTest(){

        }
        ~ExpressionInfoTest() override{

        }

        void SetUp() override{
            std::cout << "Setting up ExpressionInfoTest" << "\n";
        }

        void TearDown() override{

        }
        
    }; // class ExpressionTest

TEST_F(ExpressionInfoTest, methodCheck)
{
    Cstring str01("num");
    Cstring str02("num = 1;");
    Cstring str03("num += 2;");
    Cstring str04("num <<= 2;");
    Cstring str05("num = 1 + 2 * 3 + 4;");
    Cstring str06("max = (num1 > num2) ? num1 : num2;");
    Cstring str07("*ptr = *(int *)address;");
    Cstring str08("*(arr+idx) = value + 1;");
    Cstring str09("cond ? true : false;");
    Cstring str10("func() == true;");
    Cstring str11("func(a,b) == true");
    ExpressionInfo t;

    EXPECT_TRUE(t.empty());

    t = ExpressionInfo::getExpressionInfo(str01);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";
 
    t = ExpressionInfo::getExpressionInfo(str02);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";
 
    t = ExpressionInfo::getExpressionInfo(str03);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";
 
    t = ExpressionInfo::getExpressionInfo(str04);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";
 
    t = ExpressionInfo::getExpressionInfo(str05);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";
 
    t = ExpressionInfo::getExpressionInfo(str06);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";
 
    t = ExpressionInfo::getExpressionInfo(str07);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";
 
    t = ExpressionInfo::getExpressionInfo(str08);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";
 
    t = ExpressionInfo::getExpressionInfo(str09);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";
 
    t = ExpressionInfo::getExpressionInfo(str10);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";
 
    t = ExpressionInfo::getExpressionInfo(str11);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";
 
    
}


/*------------------------------------------------*/


    class BinaryExpressionTest: public ::testing::Test{
    protected:

        BinaryExpressionTest(){

        }
        ~BinaryExpressionTest() override{

        }

        void SetUp() override{
            std::cout << "Setting up BinaryExpressionTest" << "\n";
        }

        void TearDown() override{

        }
        
    }; // class ExpressionTest

TEST_F(BinaryExpressionTest, methodCheck)
{
    Cstring str01("1 + 2 * 3 + 4;");
    Cstring str02("max = (num1 > num2) ? num1 : num2;");
    Cstring str03("*ptr = *(int*)address;");
    Cstring str04("*(arr+idx) = value + 1;");
    BinaryExpression t;

    EXPECT_TRUE(t.empty());

    t = BinaryExpression::getBinaryExpression(str01);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";

    t = BinaryExpression::getBinaryExpression(str02);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";

    t = BinaryExpression::getBinaryExpression(str03);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";

    t = BinaryExpression::getBinaryExpression(str04);
    EXPECT_FALSE(t.empty());
    //t.dump(); cout << "\n";

    
}

/*------------------------------------------------*/


    class CastOperatorTest: public ::testing::Test{
    protected:

        CastOperatorTest(){

        }
        ~CastOperatorTest() override{

        }

        void SetUp() override{
            std::cout << "Setting up PrimaryExpressionTest" << "\n";
        }

        void TearDown() override{

        }
        
    }; // class ExpressionTest

TEST_F(CastOperatorTest, methodCheck)
{
    Cstring str01("(int *)num");
    Cstring str02("(1 + 2)");
    Cstring str03("Hello world");
    CastOperator t;

}


}// namespace
}// namespace Expression
}// namespace MMC
