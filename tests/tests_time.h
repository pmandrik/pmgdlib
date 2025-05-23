// P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
//                   https://inspirehep.net/authors/1395721
//                   https://vk.com/pmandrik

#ifndef TEST_TIME_HH
#define TEST_TIME_HH 1

#include "pmgdlib_time.h"

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

  // Timer must start from 0time
  EXPECT_EQ(ticker.itime, 0);

  // tick return `false` if it is not finished (itime < MAX_TIME)
  // After one tick itime == 1
  EXPECT_EQ(tick_timer(ticker, 1), false);
  EXPECT_EQ(ticker.itime, 1);

  // after Reset() itime == 0 again
  ticker.Reset();
  EXPECT_EQ(ticker.itime, 0);

  // after MAX_TIME ticks
  EXPECT_EQ(tick_timer(ticker, MAX_TIME), true);
  EXPECT_EQ(ticker.itime, MAX_TIME);
  ticker.Reset();

  // examples of tick loop
  while(not ticker.Tick()) continue;
  EXPECT_EQ(ticker.itime, MAX_TIME);
  ticker.Reset();

  int finish_counters = 0;
  int zero_counters   = 0;
  for(int i = 0; i < MAX_TIME*10; i++){
    if(ticker.itime == 0) zero_counters++;
    bool finish = ticker.Tick();
    if(finish) {
      finish_counters++;
      ticker.Reset();
    }
  }

  // after MAX_TIME ticks
  EXPECT_EQ(tick_timer(ticker, MAX_TIME/10, 10), true);
  EXPECT_EQ(ticker.itime, MAX_TIME);
  ticker.Reset();

  // ftime
  ticker.Tick(MAX_TIME/2);
  EXPECT_FLOAT_EQ(ticker.ftime, MAX_TIME/2./float(MAX_TIME));

  // Set()
  ticker.Set(MAX_TIME/5);
  EXPECT_FLOAT_EQ(ticker.ftime, MAX_TIME/5./float(MAX_TIME));
  EXPECT_FLOAT_EQ(ticker.itime, MAX_TIME/5);
}

#endif




