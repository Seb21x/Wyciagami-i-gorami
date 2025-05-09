#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <climits>
#include <cstdio>  

struct Tile {
    int row, col, time;
};

struct Lift {
    int startCol, startRow, endCol, endRow, travelTime, departureInterval;
};

struct LiftList {
    Lift* lifts;
    int count;
    int capacity;

    LiftList() {
        count = 0;
        capacity = 2;
        lifts = new Lift[capacity];
    }

    void add(const Lift& lift) {
        //we only up the capacity if its needed for our program
        if (count == capacity) {
            capacity *= 2;
            Lift* newLifts = new Lift[capacity];
            for (int i = 0; i < count; ++i)
                newLifts[i] = lifts[i];
            delete[] lifts;
            lifts = newLifts;
        }
        lifts[count++] = lift;
    }

    ~LiftList() {
        delete[] lifts;
    }
};

void swap(Tile& a, Tile& b) {
    Tile temp = a;
    a = b;
    b = temp;
}

void push(const Tile val, Tile* heap, int& heapSize) {
    heap[heapSize] = val;
    int i = heapSize;
    heapSize++;
    //sorting heap
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (heap[parent].time <= heap[i].time)
            break;
        swap(heap[i], heap[parent]);
        i = parent;
    }
}

Tile pop(Tile* heap, int& heapSize) {
    //we want to take tile with lowest time in heap
    Tile minNode = heap[0];
    heapSize--;
    heap[0] = heap[heapSize];

    int i = 0;
    //sorting heap
    while (true) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

        if (left < heapSize && heap[left].time < heap[smallest].time)
            smallest = left;
        if (right < heapSize && heap[right].time < heap[smallest].time)
            smallest = right;

        if (smallest == i) break;
        swap(heap[i], heap[smallest]);
        i = smallest;
    }
    return minNode;
}

void dijkstra(int width, int height, int startCol, int startRow, int endCol, int endRow, int* board, int* times, LiftList** liftLookup) {
    Tile* heap = new Tile[width * height];
    int heapSize = 0;

    times[startRow * width + startCol] = 0;
    push({ startRow, startCol, 0 }, heap, heapSize);

    int possibleX[] = { 0, 0, -1, 1 };
    int possibleY[] = { -1, 1, 0, 0 };

    while (heapSize > 0) {
        Tile current = pop(heap, heapSize);
        int x = current.col;
        int y = current.row;
        int currentIdx = x + y * width;
        //if we reached destination we can cut this function
        if (x == endCol && y == endRow) break;
        //if time needed to get on the spot is greater than already found we don't need to check it
        if (current.time > times[currentIdx]) continue;
        //looking in all 4 possible directions
        for (int i = 0; i < 4; i++) {
            int nextCol = x + possibleX[i];
            int nextRow = y + possibleY[i];
            if (nextCol < 0 || nextRow < 0 || nextCol >= width || nextRow >= height) continue;

            int neighborIdx = nextRow * width + nextCol;
            int currentHeight = board[currentIdx];
            int neighborHeight = board[neighborIdx];
            int moveCost;
            //calculatin move cost
            if (neighborHeight > currentHeight) {
                moveCost = neighborHeight - currentHeight + 1;
            }
            else {
                moveCost = 1;
            }
            //if new cost is lower then change the cost
            if (times[neighborIdx] > times[currentIdx] + moveCost) {
                times[neighborIdx] = times[currentIdx] + moveCost;
                push({ nextRow , nextCol , times[neighborIdx] }, heap, heapSize);
            }
        }
        LiftList* liftList = liftLookup[currentIdx];
        if (liftList != nullptr) {
            for (int i = 0; i < liftList->count; i++) {
                Lift& lift = liftList->lifts[i];
                //time needed to get on the tile + time needed for lift to come - 1 so we ceil when dividing 
                int departTime = ((times[currentIdx] + lift.departureInterval - 1) / lift.departureInterval) * lift.departureInterval;
                int arriveTime = departTime + lift.travelTime;
                int endIdx = lift.endRow * width + lift.endCol;
                if (times[endIdx] > arriveTime) {
                    times[endIdx] = arriveTime;
                    push({ lift.endRow, lift.endCol, arriveTime }, heap, heapSize);
                }
            }
        }
    }

    delete[] heap;
}

int main() {
    //loading input
    int width, height, startCol, startRow, endCol, endRow, numOfLifts;
    scanf("%d %d %d %d %d %d %d", &width, &height, &startCol, &startRow, &endCol, &endRow, &numOfLifts);

    int* board = new int[width * height];
    int* times = new int[width * height];

    LiftList** liftLookup = new LiftList * [width * height];
    for (int i = 0; i < width * height; i++) {
        liftLookup[i] = nullptr;
    }

    for (int i = 0; i < numOfLifts; i++) {
        Lift lift;
        scanf("%d %d %d %d %d %d", &lift.startCol, &lift.startRow, &lift.endCol, &lift.endRow, &lift.travelTime, &lift.departureInterval);
        int idx = lift.startRow * width + lift.startCol;
        if (liftLookup[idx] == nullptr) {
            liftLookup[idx] = new LiftList();
        }
        liftLookup[idx]->add(lift);
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            scanf("%d", &board[i * width + j]);
            times[i * width + j] = INT_MAX;
        }
    }

    dijkstra(width, height, startCol, startRow, endCol, endRow, board, times, liftLookup);

    //printing
    printf("%d\n", times[endRow * width + endCol]);

    //deleting
    for (int i = 0; i < width * height; i++) {
        if (liftLookup[i] != nullptr) {
            delete liftLookup[i];
        }
    }
    delete[] liftLookup;
    delete[] board;
    delete[] times;
    return 0;
}
