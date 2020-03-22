# Progress

This document are mainly used to write down problems, experiences, keynotes etc. 

## 22 March 2020

I have done some testing with the oscillscope. I added six threads, each toogling a pin every millisecond. The picture below shows how long time it takes to  switch between threads. With a -O3 level this switch takes about 2.5 us. With no optimization the switch takes about 5 us. 

<img src="https://github.com/bjornbrodtkorb/BlackOS/blob/master/BlackOS%20Graphics/thread_switch_1ms_delay.PNG" width="600">
