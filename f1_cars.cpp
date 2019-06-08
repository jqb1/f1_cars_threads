#include <unistd.h>
#include <string>
#include <map>
#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include <ctime>
#include <thread>
#include <cstdlib>
#include <mutex>
#include <map>
#include <vector>
#include <chrono>
#include <utility> 

enum lane{
    left,
    middle,
    right,
};

struct car{
    int id; 

    int position;
    int lap;
    int lap_progress; 
    int fuel;
    int speed;
    bool damaged;

    lane car_lane;
    bool in_pit_stop;
};

void wait_for_end();
void init_track(int row, int col);
void car_race(int row, int col);
void draw_lanes(int maxrow, int maxcol);
void timer_start();
car init_car(car race_car, int id);
void car_move(int maxrow, int maxcol, car race_car);

bool end_animation = false;
std::mutex display_mutex;

int CAR_NUMBER = 6;
std::map<lane,int> lane_mapper;

std::vector<std::thread> cars_vector; 


int main()
{
    std::srand(time(0));
    int row,col;
    initscr();
    curs_set(0);
    getmaxyx(stdscr,row,col);

    std::thread t_wait(wait_for_end);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    init_track(row,col);

    std::vector<car> cars_vector;
    std::vector<std::thread> cars_threads;
    for(int i=0; i<CAR_NUMBER;i++){
        cars_vector.push_back(init_car(car(), i)); 
        cars_threads.push_back(std::thread(car_move, row, col, cars_vector.at(i)));
        sleep(rand()%3+1);
    }

    while(!end_animation){
       ;
    }

    for (int i=0; i<cars_threads.size(); ++i){
        cars_threads.at(i).join();
    }
    t_wait.join();

//     for (int i=0; i<cars_vector.size(); ++i){
//         cars_vector.at(i).join();
//     }

    endwin();
} 

void init_track(int row, int col){
    draw_lanes(row,col);
    timer_start();
    lane_mapper[left] = 2;
    lane_mapper[middle] = 4;
    lane_mapper[right] = 6;
}

void wait_for_end(){
    getch();
    end_animation = true;
}

car init_car(car race_car, int id){
    race_car.lap = 0;
    race_car.lap_progress = 0;
    race_car.damaged = false;
    race_car.fuel = 100;
    race_car.id = id;
    race_car.position = 0;
    race_car.in_pit_stop = false;
    int r = std::rand()%3;
    race_car.car_lane = (lane)r;

    return race_car;
}

void draw_lanes(int maxrow,int maxcol){
    display_mutex.lock();
    mvhline(1, 5, '=', maxcol-10);
    mvhline(3, 5, '-', maxcol-10);
    mvhline(5, 5, '-', maxcol-10);
    mvhline(7, 5, '=', maxcol-10);
        
    //---pit_stop---//
    mvvline(8, maxcol/2, '|', 5);
    mvvline(8, maxcol/2+4, '|', 5);
    mvprintw(13, maxcol/2, " ___ ");
    mvprintw(14, maxcol/2, "[___]");

    refresh();
    display_mutex.unlock();


}

void timer_start(){
    int count_seconds = 3;
    while (count_seconds >= 0) {
        display_mutex.lock();
        mvprintw(3, 1, "%d", count_seconds);
        refresh();
        display_mutex.unlock();
        count_seconds--;
        sleep(1);
    }
    display_mutex.lock();
    mvprintw(3, 1, "GO!", count_seconds);
    refresh();
    display_mutex.unlock();

}

void car_move(int maxrow, int maxcol, car race_car){
    int row = lane_mapper[race_car.car_lane];
    int col = 5;
    while(!end_animation){
        if(col>=maxcol-5)
            col=5;
        
        display_mutex.lock();
        mvprintw(row, col, "#%d#>", race_car.id);
        refresh();
        display_mutex.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        display_mutex.lock();
        mvprintw(row, col, " ");
        display_mutex.unlock();

        col++;

    }
}
