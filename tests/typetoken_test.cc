
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "tokentype.h"

namespace MMC{
namespace TokenType{
namespace {

    class token_typeTest: public ::testing::Test{
    protected:

        token_typeTest(){

        }
        ~token_typeTest() override{

        }

        void SetUp() override{
            std::cout << "Setting up CstringTest" << "\n";
        }

        void TearDown() override{

        }
    }; // class ExpressionTest

TEST_F(token_typeTest, functionCheck)
{

    EXPECT_TRUE(token_type::is_storageClass("auto"));
    EXPECT_TRUE(token_type::is_storageClass("extern"));
    EXPECT_TRUE(token_type::is_storageClass("typedef"));
    EXPECT_TRUE(token_type::is_storageClass("register"));

    EXPECT_TRUE(token_type::is_typeQualifier("const"));
    EXPECT_TRUE(token_type::is_typeQualifier("volatile"));
        
    EXPECT_TRUE(token_type::is_typeSpecifier("void"));
    EXPECT_TRUE(token_type::is_typeSpecifier("char"));
    EXPECT_TRUE(token_type::is_typeSpecifier("short"));
    EXPECT_TRUE(token_type::is_typeSpecifier("int"));
    EXPECT_TRUE(token_type::is_typeSpecifier("long"));
    EXPECT_TRUE(token_type::is_typeSpecifier("float"));

    EXPECT_TRUE(token_type::is_pointerToken("*"));

}

}// namespace
}// namespace Expression
}// namespace MMC
