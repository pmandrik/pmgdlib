// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef TEST_TIME_HH
#define TEST_TIME_HH 1

#include "pmgdlib_time.h"

using namespace pmgd;

bool tick_timer(Ticker & ticker, int n_ticks, int val=1){
    bool answer = true;
    while(n_ticks > 0){
        answer = ticker.Tick(val);
        n_ticks--;
    }
    return answer;
}

TEST(pmlib_time, test_ticker) {
    // Timer will go from [0, MAX_TIME]
    int MAX_TIME = 100;
    Ticker ticker(MAX_TIME);

    // Timer must start from 0
    EXPECT_EQ(ticker.itime, 0);

    // tick return true if still can tick (itime < MAX_TIME)
    // After one tick itime == 1
    EXPECT_EQ(tick_timer(ticker, 1), true);
    EXPECT_EQ(ticker.itime, 1);

    // after Reset() itime == 0 again
    ticker.Reset();
    EXPECT_EQ(ticker.itime, 0);

    // after MAX_TIME ticks
    EXPECT_EQ(tick_timer(ticker, MAX_TIME), false);
    EXPECT_EQ(ticker.itime, MAX_TIME);
    ticker.Reset();

    // example of tick loop
    while(ticker.Tick()) continue;
    EXPECT_EQ(ticker.itime, MAX_TIME);
    ticker.Reset();

    // after MAX_TIME ticks
    EXPECT_EQ(tick_timer(ticker, MAX_TIME/10, 10), false);
    EXPECT_EQ(ticker.itime, MAX_TIME);
    ticker.Reset();

    // ftime
    std::cout << ticker.ftime << " " << MAX_TIME/2. << std::endl;
    ticker.Tick(MAX_TIME/2);
    EXPECT_EQ(ticker.ftime, MAX_TIME/2./float(MAX_TIME));
}

#endif




