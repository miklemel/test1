

#include "Test1.h"

int a=1;
int b=100;//максимальное и минимальное значение для псевдослучайного значения датчика

std::mutex              g_lockprint;
std::mutex              g_lockqueue;
std::condition_variable g_queuecheck;
std::queue<int>         g_codes;
std::queue<std::pair<int,int>>         g_codes1;
bool                    g_done;
bool                    g_notified;



    void Sensor::sensFunc( std::mt19937 &generator)
    {
        // стартовое сообщение
        {
            std::unique_lock<std::mutex> locker(g_lockprint);
            std::cout << "[sensor " << this->id << "]\trunning..." << std::endl;
        }
        for(int i=0;i<8;++i) {
            // симуляция работы
            std::this_thread::sleep_for(std::chrono::seconds(1));
            //        std::this_thread::sleep_for(std::chrono::seconds(1 + generator() % 5));

            int r = generator() % (b - a + 1) + a;

            {
                std::unique_lock<std::mutex> locker(g_lockprint);
                std::cout << "[sensor" << "]\tsensor's id: " << this->id << "  value:" << r << std::endl;
            }

            // сообщаем об ошибке
            {
                std::unique_lock<std::mutex> locker(g_lockqueue);
                g_codes1.push(std::make_pair(this->id, r));
                g_notified = true;
                g_queuecheck.notify_one();
            }

        }
        g_notified=true;
        g_done = true;
    }


    void Loger::logerFunc()
    {
        // стартовое сообщение
        {
            std::unique_lock<std::mutex> locker(g_lockprint);
            std::cout << "[logger]\trunning..." << std::endl;
        }
        // до тех пор, пока g_done не станет true в main(походу никогда не станет)
        while(!g_done)
        {
            std::unique_lock<std::mutex> locker(g_lockqueue);
            while(!g_notified) // от ложных пробуждений
                g_queuecheck.wait(locker);
            // если есть ошибки в очереди, обрабатывать их
            while(!g_codes1.empty())
            {
                std::unique_lock<std::mutex> locker(g_lockprint);
                std::pair<int, int> top=g_codes1.front();
                if(top.second > 50)
                     std::cout << "[logger]\tprocessing error on id:" << top.first << " value:" <<top.second << std::endl;
                g_codes1.pop();
            }
            g_notified = false;
        }
    }


