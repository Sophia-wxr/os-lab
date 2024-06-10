#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>

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
        std::cerr << "无法创建更多的进程，已达到pid上限。" << std::endl;
        return nullptr;
    }

    int priority = rand() % 50;  // 进程优先级，0到49范围内的随机整数
    int life = 1 + rand() % 5;   // 进程生命周期，1到5范围内的随机整数

    PCB* newProcess = new PCB(pid, priority, life);
    readyQueues[priority].push(newProcess);  // 将进程加入对应优先级的就绪队列
    return newProcess;
}

// 调度器
void scheduler() {
    for (int priority = 0; priority < 50; ++priority) {
        while (!readyQueues[priority].empty()) {
            PCB* process = readyQueues[priority].front();
            readyQueues[priority].pop();

            process->status = Status::Run;  // 将进程状态设置为"运行"
            std::cout << "运行进程 PID: " << process->pid << "，优先级: " << process->priority << "，生命周期: " << process->life << std::endl;

            // 模拟进程运行
            process->life--;
            if (process->life > 0) {
                process->status = Status::Ready;
                readyQueues[priority].push(process);  // 进程生命周期未结束，放回就绪队列
            } else {
                // 进程生命周期结束
                pidArray[process->pid] = true;  // 释放pid
                delete process;  // 释放进程控制块
            }
        }
    }
}

int main() {
    srand(time(0));  // 初始化随机数种子
    initializePidArray();

    // 创建一些进程
    for (int i = 0; i < 10; ++i) {
        createProcess();
    }

    // 调度进程
    scheduler();

    return 0;
}
