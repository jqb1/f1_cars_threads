#include <unistd.h>
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

void wait_for_end();
void car_race(int row, int col);
void draw_lanes(int maxrow, int maxcol);
void timer_start();
bool end_animation = false;

std::mutex display_mutex;

struct car{
    std::mutex lap_progress_mutex;
    std::mutex lap_mutex;
    std::mutex fuel_mutex;
    std::mutex queue_mutex;

    int number; // for recognition purposes

    int position;
    int lap;
    int lap_progress; // 0 - startline of the lap, 50 - halfway of the lap, 100 - overlapping
    int fuel;
    bool damaged;

    enum lane{
        left,
        right,
        middle
    };

    bool in_pit_stop;
};


int main()
{
    std::srand(time(0));
    int row,col;
    initscr();
    curs_set(0);
    getmaxyx(stdscr,row,col);

    std::thread t_wait(wait_for_end);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // std::thread draw_rectangle(rectangle,row/2,col/2,row,col); 

    // -------initialize race------- // 
    draw_lanes(row,col);
    timer_start();

    std::vector<std::thread> cars_vector; 
    int counter = 0;
    while(!end_animation){
       ;
    }


    t_wait.join();
//    draw_rectangle.join();

//     for (int i=0; i<cars_vector.size(); ++i){
//         cars_vector.at(i).join();
//     }

    endwin();
} 

void wait_for_end(){
    getch();
    end_animation = true;
}

void draw_lanes(int maxrow,int maxcol){
    display_mutex.lock();
    mvhline(1, 5, '=', maxcol-10);
    mvhline(3, 5, '-', maxcol-10);
    mvhline(5, 5, '-', maxcol-10);
    mvhline(7, 5, '=', maxcol-10);
    display_mutex.unlock();
    refresh();
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
    mvprintw(3, 1, "GO!", count_seconds);
    refresh();
}

void car_race(int row, int col){

}