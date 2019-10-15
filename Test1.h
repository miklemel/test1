
#ifndef TEST1_TEST1_H
#define TEST1_TEST1_H

#include <random>
#include <iostream>
#include <mutex>
#include <thread>

#include <condition_variable>
#include <queue>


extern  std::mutex              g_lockprint;
extern  std::mutex              g_lockqueue;
extern  std::condition_variable g_queuecheck;
extern  std::queue<int>         g_codes;
extern std::queue<std::pair<int,int>>         g_codes1;

extern bool                    g_notified;
extern bool                    g_done;

class Sensor
{
    public:
        Sensor(int id_m):id(id_m){}
        void sensFunc( std::mt19937 &generator);

    private:
        int id;
};

class Loger
{
public:
    void logerFunc();

};

#endif //TEST1_TEST1_H

