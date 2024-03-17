
#include <gtest/gtest.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
using namespace std;

TEST(spimTest, qsortTest)
{
    string output;
    string qsort_output = "Loaded: ./exceptions.s\n\n\n\n1\n1\n2\n3\n4\n5\n7\n8\n9\n10\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\n";

    testing::internal::CaptureStdout();
    system("./spim -delayed_branches -delayed_loads  -exception_file ./exceptions.s -file qsort.c.asm");
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, qsort_output);
}

TEST(spimTest, primeTest)
{
    string output;

    string prime_output = "Loaded: ./exceptions.s\n501\n1000\n1000\n3\n\n\n\n1\t999\n1\t998\n1\t997\n1\t995\n1\t993\n1\t989\n1\t987\n1\t983\n1\t981\n1\t977\n1\t971\n1\t969\n1\t963\n1\t959\n1\t957\n1\t953\n1\t947\n1\t941\n1\t939\n1\t933\n1\t929\n1\t927\n1\t921\n1\t917\n1\t911\n1\t903\n1\t899\n1\t897\n1\t893\n1\t891\n1\t887\n1\t873\n1\t869\n1\t863\n1\t861\n1\t851\n1\t849\n1\t843\n1\t837\n1\t833\n1\t827\n1\t821\n1\t819\n1\t809\n1\t807\n1\t803\n1\t801\n1\t789\n1\t777\n1\t773\n1\t771\n1\t767\n1\t761\n1\t759\n1\t749\n1\t743\n1\t737\n1\t731\n1\t729\n1\t723\n1\t719\n1\t717\n1\t707\n1\t693\n1\t689\n1\t687\n1\t683\n1\t669\n1\t663\n1\t653\n1\t651\n1\t647\n1\t641\n1\t633\n1\t627\n1\t621\n1\t617\n1\t611\n1\t603\n1\t599\n1\t591\n1\t581\n1\t579\n1\t569\n1\t567\n1\t561\n1\t557\n1\t551\n1\t543\n1\t539\n1\t537\n1\t533\n1\t521\n1\t513\n1\t509\n1\t501\n1\t497\n1\t491\n1\t479\n1\t477\n1\t459\n1\t453\n1\t443\n1\t437\n1\t431\n1\t429\n1\t423\n1\t413\n1\t407\n1\t401\n1\t399\n1\t393\n1\t387\n1\t383\n1\t381\n1\t369\n1\t359\n1\t357\n1\t353\n1\t347\n1\t341\n1\t339\n1\t327\n1\t323\n1\t317\n1\t309\n1\t299\n1\t291\n1\t281\n1\t273\n1\t267\n1\t261\n1\t257\n1\t249\n1\t243\n1\t239\n1\t231\n1\t227\n1\t213\n1\t203\n1\t191\n1\t189\n1\t179\n1\t177\n1\t173\n1\t171\n1\t161\n1\t147\n1\t143\n1\t141\n1\t137\n1\t123\n1\t119\n1\t117\n1\t113\n1\t93\n1\t89\n1\t81\n1\t71\n1\t63\n1\t59\n1\t53\n1\t47\n1\t33\n1\t29\n1\t23\n1\t17\n1\t9\n1\t3\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\n";

    testing::internal::CaptureStdout();
    system("./spim -delayed_branches -delayed_loads  -exception_file ./exceptions.s -file prime.c.asm");
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, prime_output);
}
TEST(spimTest, fizzbuzzTest)
{
    string output;

    string fizzbuzz_output = "Loaded: ./exceptions.s\n31\n10\n6\n2\n30\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\n";
    testing::internal::CaptureStdout();
    system("./spim -delayed_branches -delayed_loads  -exception_file ./exceptions.s -file fizzbuzz.c.asm");
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, fizzbuzz_output);
}

TEST(spimTest, addfuncTest)
{
    string output;

    string add_func_output = "Loaded: ./exceptions.s\n14\n4\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\n";
    testing::internal::CaptureStdout();
    system("./spim -delayed_branches -delayed_loads  -exception_file ./exceptions.s -file add_func.c.asm");
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, add_func_output);
}

TEST(spimTest, arrSumTest)
{
    string output;

    string arr_sum = "Loaded: ./exceptions.s\n19\n22\n43\n50\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\n";
    testing::internal::CaptureStdout();
    system("./spim -delayed_branches -delayed_loads  -exception_file ./exceptions.s -file arr_sum.c.asm");
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, arr_sum);
}

TEST(spimTest, arrFileTest)
{
    string output;

    string arr_file_output = "Loaded: ./exceptions.s\n\n3\n3\n5\n5\n5\n5\n6\n7\n7\n7\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\n";

    testing::internal::CaptureStdout();
    system("./spim -delayed_branches -delayed_loads  -exception_file ./exceptions.s -file arr_fill.c.asm");
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, arr_file_output);
}

TEST(spimTest, factTest)
{
    string output;

    string fact_output = "Loaded: ./exceptions.s\n2\n1\n3\n2\n4\n6\n5\n24\n6\n120\n6\n120\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\n";
    testing::internal::CaptureStdout();
    system("./spim -delayed_branches -delayed_loads  -exception_file ./exceptions.s -file fact.c.asm");
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, fact_output);
}

TEST(spimTest, compareTest)
{
    string output;

    string compare_output = "Loaded: ./exceptions.s\n2\n2\n2\n\n3\n3\n\n2\n2\n2\n\n2\n2\n2\n2\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\n";
    testing::internal::CaptureStdout();
    system("./spim -delayed_branches -delayed_loads  -exception_file ./exceptions.s -file compare.c.asm");
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, compare_output);
}

TEST(spimTest, arrExchangeTest)
{
    string output;

    string arr_exchanged_output = "Loaded: ./exceptions.s\n5\t9\n9\t5\n9\t5\n5\t9\n9\t5\n5\t9\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\nReg 0 = 0x00000000 (0)\n\n";

    testing::internal::CaptureStdout();
    system("./spim -delayed_branches -delayed_loads  -exception_file ./exceptions.s -file arr_exchange.c.asm");
    output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, arr_exchanged_output);
}