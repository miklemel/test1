#include <iostream>
#include <random>
#include <mutex>
#include <thread>
#include <vector>
#include <condition_variable>
#include <queue>


int a=1;
int b=100;//максимальное и минимальное значение для псевдослучайного значения датчика

std::mutex              g_lockprint;
std::mutex              g_lockqueue;
std::condition_variable g_queuecheck;
std::queue<int>         g_codes;
bool                    g_done;
bool                    g_notified;





class Sensor
{
    public:
        Sensor(int id_m):id(id_m){

        }
        void sensFunc( std::mt19937 &generator)
        {

                // стартовое сообщение
                {
                    std::unique_lock<std::mutex> locker(g_lockprint);
                    std::cout << "[sensor " << this->id << "]\trunning..." << std::endl;
                }
            for(;;) {
                // симуляция работы
                std::this_thread::sleep_for(std::chrono::seconds(1));
      //        std::this_thread::sleep_for(std::chrono::seconds(1 + generator() % 5));

                int r = generator()  % (b - a + 1) + a;

                {
                    std::unique_lock<std::mutex> locker(g_lockprint);
                    std::cout << "[sensor" << "]\tsensor's id: " << this->id << "  value:" << r << std::endl;
                }
                // сообщаем об ошибке
                if (r > 50) {
                    std::unique_lock<std::mutex> locker(g_lockqueue);
                    g_codes.push(this->id);
                    g_notified = true;
                    g_queuecheck.notify_one();
                }
            }
        }

    private:
        int id;

};





void loggerFunc()
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
        while(!g_codes.empty())
        {
            std::unique_lock<std::mutex> locker(g_lockprint);
            std::cout << "[logger]\tprocessing error on id:" << g_codes.front()  << std::endl;
            g_codes.pop();
        }
        g_notified = false;
    }
}



int main() {


    // инициализация генератора псевдо-случайных чисел
    std::mt19937 generator((unsigned int) std::chrono::system_clock::now().time_since_epoch().count());
    // запуск регистратора
    std::thread loggerThread(loggerFunc);
    std::vector<std::thread> threads;
    std::vector<Sensor> sensors;

    std::cout << "Insert sensor count(0<n=<10)\n";
    int count=0;
    std::cin >> count;


    // запуск датчиков
   for (int i = 0; i < count; ++i)
   {
     //  sensors.push_back(Sensor(i));
       threads.push_back(std::thread(&Sensor::sensFunc,  Sensor(i), std::ref(generator)));
   }
    for (auto &t: threads)
    {
        t.join();
    }
    // сообщаем регистратору о завершении и ожидаем его
    g_done = true;
    loggerThread.join();
    return 0;




}
