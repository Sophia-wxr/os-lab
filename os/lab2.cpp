#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <list>

using namespace std;

// 生成指令地址流
vector<int> generateAddressStream(int size) {
    vector<int> addresses;
    addresses.reserve(size);
    int dn = 10000;

    srand(time(0));  // 使用当前时间作为随机数生成的种子
    for (int i = 0; i < size; ++i) {
        addresses.push_back(dn);
        int a = rand() % 1024 + 1;
        if (a <= 512) {
            dn = dn + 1;
        } else if (a <= 768) {
            dn = rand() % dn + 1;
        } else {
            dn = rand() % (32767 - dn + 1) + dn;
        }
    }
    return addresses;
}

// 转换指令地址为页号，并合并相邻的相同页号
vector<int> convertToPageNumbers(const vector<int>& addresses, int pageSize) {
    vector<int> pageNumbers;
    int lastPage = -1;
    for (int address : addresses) {
        int pageNumber = address / pageSize;
        if (pageNumber != lastPage) {
            pageNumbers.push_back(pageNumber);
            lastPage = pageNumber;
        }
    }
    return pageNumbers;
}

// OPT页面置换算法
double OPT(const vector<int>& pageNumbers, int frameCount) {
    vector<int> frames;
    int pageFaults = 0;
    for (int i = 0; i < pageNumbers.size(); ++i) {
        int page = pageNumbers[i];
        if (find(frames.begin(), frames.end(), page) == frames.end()) {  // 页面不在内存中
            ++pageFaults;
            if (frames.size() < frameCount) {
                frames.push_back(page);  // 内存未满，直接插入
            } else {
                // 寻找最远未来使用的页面替换
                int farthest = i;
                int index = -1;
                for (int j = 0; j < frames.size(); ++j) {
                    int nextUse = distance(pageNumbers.begin(), find(pageNumbers.begin() + i + 1, pageNumbers.end(), frames[j]));
                    if (nextUse > farthest) {
                        farthest = nextUse;
                        index = j;
                    }
                }
                if (index != -1) {
                    frames[index] = page;
                }
            }
        }
    }
    return static_cast<double>(pageFaults) / pageNumbers.size();
}

// FIFO页面置换算法
double FIFO(const vector<int>& pageNumbers, int frameCount) {
    queue<int> frames;
    unordered_map<int, bool> inFrames;
    int pageFaults = 0;
    for (int page : pageNumbers) {
        if (inFrames.find(page) == inFrames.end()) {  // 页面不在内存中
            ++pageFaults;
            if (frames.size() < frameCount) {
                frames.push(page);
                inFrames[page] = true;
            } else {
                int oldPage = frames.front();
                frames.pop();
                inFrames.erase(oldPage);
                frames.push(page);
                inFrames[page] = true;
            }
        }
    }
    return static_cast<double>(pageFaults) / pageNumbers.size();
}

// LRU页面置换算法
double LRU(const vector<int>& pageNumbers, int frameCount) {
    list<int> frames;
    unordered_map<int, list<int>::iterator> pageMap;
    int pageFaults = 0;
    for (int page : pageNumbers) {
        if (pageMap.find(page) == pageMap.end()) {  // 页面不在内存中
            ++pageFaults;
            if (frames.size() < frameCount) {
                frames.push_front(page);
                pageMap[page] = frames.begin();
            } else {
                int oldPage = frames.back();
                frames.pop_back();
                pageMap.erase(oldPage);
                frames.push_front(page);
                pageMap[page] = frames.begin();
            }
        } else {
            // 页面在内存中，将其移到最前
            frames.erase(pageMap[page]);
            frames.push_front(page);
            pageMap[page] = frames.begin();
        }
    }
    return static_cast<double>(pageFaults) / pageNumbers.size();
}

int main() {
    const int addressCount = 256;
    const vector<int> pageSizes = {1024, 2048, 4096, 8192, 16384};
    vector<int> addresses = generateAddressStream(addressCount);

    // 遍历不同的页面大小
    for (int pageSize : pageSizes) {
        vector<int> pageNumbers = convertToPageNumbers(addresses, pageSize);
        int pageCount = pageNumbers.size();

        cout << "页面大小: " << pageSize << " 字节" << endl;

        // 遍历不同的内存块数
        for (int frameCount = 1; frameCount <= pageCount; ++frameCount) {
            double optRate = OPT(pageNumbers, frameCount);
            double fifoRate = FIFO(pageNumbers, frameCount);
            double lruRate = LRU(pageNumbers, frameCount);

            cout << "内存块数: " << frameCount
                 << ", OPT缺页率: " << optRate
                 << ", FIFO缺页率: " << fifoRate
                 << ", LRU缺页率: " << lruRate << endl;
        }

        cout << endl;
    }

    return 0;
}
