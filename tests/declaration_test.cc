
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "translate.h"


using namespace std;

namespace MMC{
namespace Declare{
namespace{

    
    class DeclarationSpecifierTest: public ::testing::Test{
    protected:

        DeclarationSpecifierTest(){

        }
        ~DeclarationSpecifierTest() override{

        }

        void SetUp() override{
            std::cout << "Setting up DeclarationSpecifierTest" << "\n";
        }

        void TearDown() override{

        }
    }; // class ExpressionTest


TEST_F(DeclarationSpecifierTest, getDeclarationSpecifier)
{
    DeclarationSpecifier decl_spec;
    EXPECT_TRUE(decl_spec.empty());
    decl_spec = DeclarationSpecifier::get_declarationSpecifier("int *arr;");
    EXPECT_FALSE(decl_spec.empty());

    EXPECT_TRUE(decl_spec.storageClassSpecifier.empty());
    EXPECT_EQ(decl_spec.typeSpecifier.front(), "int");
    EXPECT_TRUE(decl_spec.typeQualifier.empty());


    decl_spec = DeclarationSpecifier::get_declarationSpecifier("int *(arr);");
    EXPECT_FALSE(decl_spec.empty());

    EXPECT_TRUE(decl_spec.storageClassSpecifier.empty());
    EXPECT_TRUE(decl_spec.typeQualifier.empty());

    decl_spec = DeclarationSpecifier::get_declarationSpecifier("static const unsigned int a");
    EXPECT_EQ(decl_spec.typeSpecifier[0], "unsigned");
    EXPECT_EQ(decl_spec.typeSpecifier[1], "int");
    EXPECT_EQ(decl_spec.storageClassSpecifier, "static");
    EXPECT_EQ(decl_spec.typeQualifier.front(), "const");

}

    class DeclarationInfoTest: public ::testing::Test{
    protected:

        DeclarationInfoTest(){

        }
        ~DeclarationInfoTest() override{

        }

        void SetUp() override{
            std::cout << "Setting up DeclarationInfoTest" << "\n";
        }

        void TearDown() override{

        }
    }; // class ExpressionTest

TEST_F(DeclarationInfoTest, getDeclarationInfo)
{
    DeclarationInfo decl_info;
    Cstring test01("int num;");
    Cstring test02("const int MAX=10;");
    Cstring test03("static unsigned int a, b;");
    Cstring test04("typedef int *int_ptr = ( 1 + 2 );");

    EXPECT_TRUE(decl_info.empty());
    decl_info = DeclarationInfo::getDeclarationInfo(test01);
    EXPECT_FALSE(decl_info.empty());
    EXPECT_FALSE(decl_info.declspec.empty());
    EXPECT_FALSE(decl_info.init_decl_list.empty());
    EXPECT_EQ(decl_info.declspec.typeSpecifier.front(), "int");
    EXPECT_EQ(decl_info.init_decl_list.init_decl.front().dcl.identifier, "num");

    decl_info = DeclarationInfo::getDeclarationInfo(test02);
    EXPECT_FALSE(decl_info.empty());
    EXPECT_FALSE(decl_info.declspec.empty());
    EXPECT_FALSE(decl_info.init_decl_list.empty());
    EXPECT_EQ(decl_info.declspec.typeSpecifier.front(), "int");
    EXPECT_EQ(decl_info.declspec.typeQualifier.front(), "const");
    EXPECT_EQ(decl_info.init_decl_list.init_decl.front().dcl.identifier, "MAX");
    EXPECT_EQ(decl_info.init_decl_list.init_decl.front().initializer, "10");

    decl_info = DeclarationInfo::getDeclarationInfo(test03);
    EXPECT_FALSE(decl_info.empty());
    EXPECT_FALSE(decl_info.declspec.empty());
    EXPECT_FALSE(decl_info.init_decl_list.empty());
    EXPECT_EQ(decl_info.declspec.storageClassSpecifier, "static");
    EXPECT_EQ(decl_info.declspec.typeSpecifier[0], "unsigned");
    EXPECT_EQ(decl_info.declspec.typeSpecifier[1], "int");
    EXPECT_EQ(decl_info.init_decl_list.init_decl[0].dcl.identifier, "a");
    EXPECT_EQ(decl_info.init_decl_list.init_decl[1].dcl.identifier, "b");


    decl_info = DeclarationInfo::getDeclarationInfo(test04);
    EXPECT_FALSE(decl_info.empty());
    EXPECT_FALSE(decl_info.declspec.empty());
    EXPECT_FALSE(decl_info.init_decl_list.empty());
    EXPECT_EQ(decl_info.declspec.storageClassSpecifier, "typedef");
    EXPECT_EQ(decl_info.declspec.typeSpecifier[0], "int");
    EXPECT_EQ(decl_info.init_decl_list.init_decl[0].dcl.identifier, "int_ptr");
    EXPECT_EQ(std::get<PointerStack>(decl_info.init_decl_list.init_decl.front().dcl.descriptor.front()).token, "*");
    EXPECT_EQ(decl_info.init_decl_list.init_decl.front().initializer, "(1+2)");



}

    class DeclaratorTest: public ::testing::Test{
    protected:

        DeclaratorTest(){

        }
        ~DeclaratorTest() override{

        }

        void SetUp() override{
            std::cout << "Setting up DeclaratorTest" << "\n";
        }

        void TearDown() override{

        }
    }; // class ExpressionTest


} // namespace
} // namespace utils
} // namespace MMC

