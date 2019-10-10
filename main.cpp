#include <iostream>
#include <random>
#include <mutex>
#include <thread>
#include <vector>
#include <cstdlib> // для функций rand() и srand()

#include <condition_variable>
#include <queue>

std::mutex mtx;
int a=1;
int b=100;
static int counter = 0;
static const int MAX_COUNTER_VAL = 30;



std::mutex              g_lockprint;
std::mutex              g_lockqueue;
std::condition_variable g_queuecheck;
std::queue<int>         g_codes;
bool                    g_done;
bool                    g_notified;




class X{
    public:
    X(int id_m):id(id_m){
        this->id=id_m;
    }
    int getId(){
        return this->id;
    }
     void   do_lengthy_work(int tnum){
         for(;;) {
             g_lock.lock();
             int r = rand() % (b - a + 1) + a;
             std::cout << "random = " << r << '\n';
             int ctr_val = ++counter;
             std::cout << "Thread id: " << tnum << "; counter = " <<
                       ctr_val << std::endl;
             if (r >= 5) { std::cout << "warning!\n"; }
             std::this_thread::sleep_for(std::chrono::seconds(1));
             g_lock.unlock();
         }
     }
    private:
        int id;
        std::mutex g_lock;
};


void thread_proc(int tnum) {
    for(;;) {

        {
            std::lock_guard<std::mutex> lock(mtx);
            if(counter > MAX_COUNTER_VAL)
                break;
            int r = rand() % (b - a + 1) + a;
            std::cout << "random = " << r << '\n' ;
            int ctr_val = ++counter;
            std::cout << "Thread id: " << tnum << "; counter = " <<
                      ctr_val << std::endl;
            if (r>=5) {std::cout << "warning!\n";}

        }




//        std::random_device random_device; // Источник энтропии.
//        std::mt19937 generator(random_device()); // Генератор случайных чисел.
//        std::uniform_int_distribution<> distribution(1, 10); // Равномерное распределение [10, 20]
//
//        int x = distribution(generator); // Случайное число.
//        std::cout << x << '\n' <<std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}



void workerFunc(int id, std::mt19937 &generator)
{
    // стартовое сообщение
    {
        std::unique_lock<std::mutex> locker(g_lockprint);
        std::cout << "[sensor " << id << "]\trunning..." << std::endl;
    }
    // симуляция работы
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    std::this_thread::sleep_for(std::chrono::seconds(1 + generator() % 5));

    int r = rand() % (b - a + 1) + a;
    //std::cout << "random = " << r << '\n';
    // симуляция ошибки
   //int errorcode = id*100+1;

    {
        std::unique_lock<std::mutex> locker(g_lockprint);
        std::cout  << "[sensor" << "]\tsensor's id: " << id << "  value:" << r << std::endl;
    }
    // сообщаем об ошибке
    if(r>5)
    {
        std::unique_lock<std::mutex> locker(g_lockqueue);
        g_codes.push(id);
        g_notified = true;
        g_queuecheck.notify_one();
    }
}

void loggerFunc()
{
    // стартовое сообщение
    {
        std::unique_lock<std::mutex> locker(g_lockprint);
        std::cout << "[logger]\trunning..." << std::endl;
    }
    // до тех пор, пока не будет получен сигнал
    while(!g_done)
    {
        std::unique_lock<std::mutex> locker(g_lockqueue);
        while(!g_notified) // от ложных пробуждений
            g_queuecheck.wait(locker);
        // если есть ошибки в очереди, обрабатывать их
        while(!g_codes.empty())
        {
            std::unique_lock<std::mutex> locker(g_lockprint);
            std::cout << "[logger]\tprocessing error on id:  " << g_codes.front()  << std::endl;
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
    // запуск рабочих
    std::vector<std::thread> threads;
    std::cout << "Insert sensor count(0<n=<10)\n";
    int count=0;
    std::cin >> count;
    for (int i = 0; i < count; ++i)
        threads.push_back(std::thread(workerFunc, i + 1, std::ref(generator)));
    for (auto &t: threads)
    {

        t.join();
    }
    // сообщаем регистратору о завершении и ожидаем его
    g_done = true;
    loggerThread.join();
    return 0;

//    X my_x1(1);
//    X my_x2(2);
//    std::thread t1(&X::do_lengthy_work,&my_x1,my_x1.getId());
//    std::thread t2(&X::do_lengthy_work,&my_x2,my_x2.getId());
//    t1.detach();
//    std::this_thread::sleep_for(std::chrono::milliseconds(600));
//    t2.detach();
//    while(true){}


}
