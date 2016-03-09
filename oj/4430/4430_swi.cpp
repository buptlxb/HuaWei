#include <iostream>
#include <map>
#include <queue>
#include <vector>

using namespace std;

struct Swi{
    unsigned int swiId;
    unsigned int prio;
    void (* proc)(void);
    Swi (unsigned int id=100, unsigned int po=32, void (* pc)(void)=0) : swiId(id), prio(po), proc(pc) {}
    friend bool operator< (const Swi &lhs, const Swi &rhs) { return lhs.prio < rhs.prio; }
};

map<unsigned int, Swi> swis;
priority_queue<Swi> waitQ;
unsigned int cur = 100;

int SwiCreate(unsigned int swiId, unsigned int prio, void (* proc)(void))
{
    if (swiId > 99 || prio > 31 || !proc || swis.find(swiId) != swis.end())
        return -1;
    swis[swiId] = Swi(swiId, prio, proc);
    return 0;
}

int SwiActivate(unsigned int swiId)
{
    if (swis.find(swiId) == swis.end())
        return -1;
    waitQ.push(swis[swiId]);
    while (!waitQ.empty()) {
        Swi n = waitQ.top(); 
        if (n.swiId != cur) {
            int prev = cur;
            cur = n.swiId;
            n.proc();
            cur = prev;
            waitQ.pop();
        } else
            break;
    }
    return 0;
}

void Clear(void)
{
    cur = 100;
    swis.clear();
    while (!waitQ.empty()) {
        waitQ.pop();
    }
}

int main(void)
{
    return 0;
}
