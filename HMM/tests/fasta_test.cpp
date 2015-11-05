#include <string>
#include "fasta.hpp"
#include "gmock/gmock.h"

using namespace std;
namespace
{
    class FastaTest : public ::testing::Test
    {
    protected:
        FastaTest(): reader("/Users/hanb/Dropbox/git/pacBio/HMM/HMM/tests/polyA.fa")
        {
        }
    public:
        FastaReader<> reader;
    };
    
    TEST_F(FastaTest, FastaReaderIterator)
    {
        auto fa = reader.begin();
        {
            EXPECT_EQ(fa->size(), 880);
            Sequence<> s {"TCAcccGGCCTGCAGGAGAAGGAGGACCTGCAGGAGCTCAATGATCGCTTGGCGGTCTACATCGACCGTGTGCGCTCGCTGGAAACGGAGAACGCAGGGCTGCGCCTTCGCATCACCGAGTCTGAAGAGGTGGTCAGCCGCGAGGTGTCCGGCATCAAGGCCGCCTACGAGGCCGAGCTCGGGGATGCCCGCAAGACCCTTGACTCAGTAGCCAAGGAGCGCGCCCGCCTGCAGCTGGAGCTGAGCAAAGTGCGTGAGGAGTTTAAGGAGCTGAAAGCGCGCAATACCAAGAAGGAGGGTGACCTGATAGCTGCTCAGGCTCGGCTGAAGGACCTGGAGGCTCTGCTGAACTCCAAGGAGGCCGCACTGAGCACTGCTCTCAGTGAGAAGCGCACGCTGGAGGGCGAGCTGCATGATCTGCGGGGCCAGGTGGCCAAGCTTGAGGCAGCCCTAGGTgaggCCAAGAAGCAACTTCAGGATGAGATGCTGCGGCGGGTGGATGCTGAGAACAGGCTGCaGACCATGAAGGAGGAACTGGACTTCCAGAAGAACATCTACAGTGAGGAGCTGCGTGAGACCAAGCGCCGTCATGAGACCCGACTGGTGGAGATTGACAATGGGAAGCAGCGTGAGTTTGAGAGCCGGCTGGCGGATGCGCTGCAGGAACTGCGGGCCCAGCATGAGGACCAGGTGGAGCAGTATAAGAAGGAGCTGGAGAAGACTTATTCTGCCAAGCTGGACAATGCCAGGCAGTCTGCTGAGAGGAACAGCAACCTGGTGGGGGCTGCCCACGAGGAGCTGCAGCAGTCGCGCATCCGCATCGACAGCCTCTCTGCCCAGCTCAGCCAGCTCCAGAAGCAGCTGGCAGCCAAGGAGGCGa"};
            EXPECT_TRUE(static_cast<Sequence<>>(fa->seq_) == s);
        }
        ++fa;
        {
            EXPECT_EQ(fa->size(), 851);
            Sequence<> s {"AGCTTCGAGACCTGGAGGACTCACTGGCCCGTGAGCGGGACACCAGCCGGCGGCTGCTGGCGGAAAAGGAGCGGGAGATGGCCGAGATGCGGGCAAGGATGCAGCAGCAGCTGGACGAGTACCAGGAGCTTCTGGACATCAAGCTGGCCCTGGACATGGAGATCCACGCCTACCGCAAGCTCTTGGAGGGCGAGGAGGAGAGGCTACGCCTGTCCCCCAGCCCTACCTCGCAGCGCAGCCGTGGCCGTGCTTCCTCTCACTCATCCCAGACACAGGGTGGGGGCAGCGTCACCAAAAAGCGCAAACTGGAGTCCACTGAGAGCCGCAGCAGCTTCTCACAGCACGCACGCACTAGCGGGCGCGTGGCCGTGGAGGAGGTGGATGAGGAGGGCAAGTTTGTCCGGCTGCGCAACAAGTCCAATGAGGACCAGTCCATGGGCAATTGGCAGATCAAGCGCCAGAATGGAGATGATCCCTTGCTGACTTACCGGTTCCCACCAAAGTTCACCCTGAAGGCTGGGCAGGTGGTGACGATCTGGGCTGCAGGAGCTGGGGCCACCCACAGCCCCCCTACCGACCTGGTGTGGAAGGCACAGAACACCTGGGGCTGCGGGAACAGCCTGCGTACGGCTCTCATCAACTCCACTGGGGAAGAAGTGGCCATGCGCAAGCTGGTGCGCTCAGTGACTGTGGTTGAGGACGACGAGGATGAGGATGGAGATGACCTGCTCCATCACCACCATGTGAGTGGTAGCCGCCGCTGAGGCCGAGCCTGCACTGGGGCCACCCAGCCAGGCCTGGGGGCAGCCTCTCCCCAGCCTCCCCGTGCCAAAAAAAGAGATGaaaaaaaa"};
            EXPECT_TRUE(static_cast<Sequence<>>(fa->seq_) == s);
        }
        ++fa;
        EXPECT_TRUE(*fa == Fasta<>{});
    }
    
    TEST_F(FastaTest, FastaReaderForLoop)
    {
        size_t sizes[] = {880, 851, 0};
        int i = 0;
        for(auto fa : reader)
        {
            EXPECT_EQ(fa.size(), sizes[i++]);
        }
    }
    
    TEST_F(FastaTest, FastaSequence)
    {
        auto faiter = reader.begin();
        {
            auto striter = faiter->seq_.begin();
            EXPECT_EQ(*striter, 't');
            ++striter;
            EXPECT_EQ(*striter, 'c');
            ++striter;
            EXPECT_EQ(*striter, 'A');
        }
        {
            auto striter = faiter->seq_.rbegin();
            EXPECT_EQ(*striter, 'A');
            ++striter;
            EXPECT_EQ(*striter, 'G');
            ++striter;
            EXPECT_EQ(*striter, 'C');
        }
    }
}

int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
