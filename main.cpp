
#include <vector>
#include "Test1.h"



int main() {


    // инициализация генератора псевдо-случайных чисел
    std::mt19937 generator((unsigned int)
    std::chrono::system_clock::now().time_since_epoch().count());
    // запуск регистратора
    Loger loger=Loger();
    std::thread loggerThread(&Loger::logerFunc,loger);
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
