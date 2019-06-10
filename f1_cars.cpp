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
    int fuel;
    int speed;
    bool damaged;
    lane car_lane;
    bool in_pit_stop;
    int row;
    int col;
};

void wait_for_end();
void init_track(int row, int col);
void car_race(int row, int col);
void draw_lanes(int maxrow, int maxcol);
void timer_start();
void display_results(int maxrow,int maxcol, car race_car);
void check_if_near(car &race_car);
car init_car(car race_car, int id);
void car_move(int maxrow, int maxcol, int car_num);
void display_car();

bool end_animation = false;
std::mutex display_mutex;
int CAR_NUMBER = 6;
std::map<lane,int> lane_mapper;
std::vector<car> cars_vector;

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

    std::vector<std::thread> cars_threads;
    for(int i=0; i<CAR_NUMBER; i++){
        cars_vector.push_back(init_car(car(), i)); 
        cars_threads.push_back(std::thread(car_move, row, col, i));
        sleep(rand()%3+1);
    }
    
    while(!end_animation){};

    for (int i=0; i<cars_threads.size(); ++i){
        cars_threads.at(i).join();
    }
    t_wait.join();
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
    race_car.damaged = false;
    race_car.fuel = 100;
    race_car.id = id;
    race_car.position = 0;
    race_car.in_pit_stop = false;
    int r = std::rand()%3;
    race_car.car_lane = (lane)r;
    race_car.col = 5;
    race_car.speed = std::rand()%5+1;
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

void car_move(int maxrow, int maxcol, int car_num){
    
    while(!end_animation){
        cars_vector[car_num].row = lane_mapper[cars_vector[car_num].car_lane];

        display_mutex.lock();
        mvprintw(cars_vector[car_num].row, cars_vector[car_num].col, "[%d]>", cars_vector[car_num].id);
        refresh();
        display_mutex.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(200/cars_vector[car_num].speed));

        display_mutex.lock();
        mvprintw(cars_vector[car_num].row, cars_vector[car_num].col, " ");
        display_mutex.unlock();

        cars_vector[car_num].col++;
        check_if_near(cars_vector[car_num]);
        if(cars_vector[car_num].fuel>0 && cars_vector[car_num].col%5==0)
            cars_vector[car_num].fuel -= std::rand()%2;
        
        if(cars_vector[car_num].col>=maxcol-9){
            cars_vector[car_num].lap+=1;
            cars_vector[car_num].col=5;
            display_mutex.lock();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            mvprintw(cars_vector[car_num].row, maxcol-9, "    ");
            display_mutex.unlock();
        }

        display_results(maxrow,maxcol,cars_vector[car_num]);

    }
}

void display_results(int maxrow, int maxcol,car race_car){
    // *id to set up position for every car 
    int print_row = maxrow/2+3*race_car.id;
    display_mutex.lock();
    mvprintw(print_row,0, "Lap:    ", race_car.lap);
    mvprintw(print_row,20, "Fuel:    ", race_car.fuel);
    mvprintw(print_row,40, "Car:   ", race_car.id);

    mvprintw(print_row,0, "Lap: %d", race_car.lap);
    mvprintw(print_row,20, "Fuel: %d", race_car.fuel);
    mvprintw(print_row,40, "Car: %d", race_car.id);
    refresh();
    display_mutex.unlock();

}
void check_if_near(car &race_car){
    lane car_lane = race_car.car_lane;
    int col = race_car.col;
    for(int i=0; i<cars_vector.size(); i++){
        if(col+3==cars_vector.at(i).col && car_lane==cars_vector.at(i).car_lane && cars_vector.at(i).id!=race_car.id && race_car.speed>cars_vector.at(i).speed){
            int r=rand()%3;
            race_car.car_lane = (lane)r;
            display_mutex.lock();
            mvprintw(race_car.row, race_car.col, "     ");
            display_mutex.unlock();

        }
    }
}
void display_car(){
    while(!end_animation){
        display_mutex.lock();
        mvprintw(10,120, "Lap:%d   ", cars_vector[0].lap);
        mvprintw(10,140, "Fuel:%d  ", cars_vector[0].fuel);
        mvprintw(10,160, "Car: %d  ", cars_vector[0].id);
        refresh();
        display_mutex.unlock();
    }
}