#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <unistd.h>  // 用于usleep函数
#include <locale.h>  // 用于设置本地化
#include <ncurses.h> // 用于GUI

// 进程状态
enum class Status {
    Ready,  // 就绪
    Run     // 运行
};

// 进程控制块PCB
struct PCB {
    int pid;               // 进程标识符
    Status status;         // 进程状态
    int priority;          // 进程优先级
    int life;              // 进程生命周期
    PCB* next;             // 队列指针

    PCB(int id, int prio, int lifeSpan) 
        : pid(id), status(Status::Ready), priority(prio), life(lifeSpan), next(nullptr) {}
};

// 全局变量
bool pidArray[101];  // 布尔数组，表示pid是否空闲
std::vector<std::queue<PCB*>> readyQueues(50);  // 邻接表，表示50个就绪队列

// 初始化pid数组
void initializePidArray() {
    for (int i = 1; i <= 100; ++i) {
        pidArray[i] = true;  // "真"表示下标对应的进程标识号是空闲的
    }
}

// 创建进程
PCB* createProcess() {
    int pid;
    // 查找空闲的pid
    for (pid = 1; pid <= 100; ++pid) {
        if (pidArray[pid]) {
            pidArray[pid] = false;  // 分配pid
            break;
        }
    }

    if (pid > 100) {
        std::cerr << "Unable to create more processes, PID limit reached." << std::endl;
        return nullptr;
    }

    int priority = rand() % 50;  // 进程优先级，0到49范围内的随机整数
    int life = 1 + rand() % 5;   // 进程生命周期，1到5范围内的随机整数

    PCB* newProcess = new PCB(pid, priority, life);
    readyQueues[priority].push(newProcess);  // 将进程加入对应优先级的就绪队列
    return newProcess;
}

// 显示进程信息
void displayProcesses() {
    clear();  // 清除屏幕
    printw("PID\tStatus\tPriority\tLife\n");
    printw("--------------------------------\n");
    for (int priority = 0; priority < 50; ++priority) {
        std::queue<PCB*> queue = readyQueues[priority];
        while (!queue.empty()) {
            PCB* process = queue.front();
            queue.pop();
            printw("%d\t%s\t%d\t%d\n", process->pid, process->status == Status::Ready ? "Ready" : "Run", process->priority, process->life);
        }
    }
    refresh();  // 刷新屏幕
}

// 调度器
void scheduler() {
    while (true) {
        int ch = getch();
        if (ch == 17) {  // ctrl+q
            break;
        } else if (ch == 6) {  // ctrl+f
            createProcess();
        }

        bool processFound = false;
        for (int priority = 49; priority >= 0; --priority) {
            if (!readyQueues[priority].empty()) {
                PCB* process = readyQueues[priority].front();
                readyQueues[priority].pop();
                process->status = Status::Run;
                displayProcesses();

                usleep(1000000);  // 模拟进程运行一个时间片

                process->priority /= 2;
                process->life--;

                if (process->life > 0) {
                    process->status = Status::Ready;
                    readyQueues[process->priority].push(process);
                } else {
                    pidArray[process->pid] = true;
                    delete process;
                }
                processFound = true;
                break;
            }
        }
        if (!processFound) {
            printw("No ready process available for scheduling.\n");
            refresh();
            usleep(1000000);
        }
    }
}

int main() {
    setlocale(LC_ALL, "");  // 设置本地化
    srand(time(0));  // 初始化随机数种子
    initializePidArray();

    initscr();  // 初始化ncurses
    cbreak();
    noecho();
    timeout(1000);  // 1秒钟超时

    printw("Press Ctrl+F to create a process\n");
    printw("Press Ctrl+Q to exit the scheduling loop\n");
    refresh();

    scheduler();

    endwin();  // 结束ncurses
    return 0;
}

