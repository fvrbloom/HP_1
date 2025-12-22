#include <iostream>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <omp.h>

using namespace std;

// ---------- Linked List ----------
struct Node {
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};

class LinkedList {
private:
    Node* head;
public:
    LinkedList() : head(nullptr) {}

    void append(int value) {
        Node* newNode = new Node(value);
        if (!head) {
            head = newNode;
            return;
        }
        Node* cur = head;
        while (cur->next) {
            cur = cur->next;
        }
        cur->next = newNode;
    }

    bool remove(int value) {
        if (!head) return false;

        if (head->data == value) {
            Node* temp = head;
            head = head->next;
            delete temp;
            return true;
        }

        Node* cur = head;
        while (cur->next && cur->next->data != value) {
            cur = cur->next;
        }

        if (cur->next) {
            Node* temp = cur->next;
            cur->next = cur->next->next;
            delete temp;
            return true;
        }
        return false;
    }

    bool search(int value) {
        Node* cur = head;
        while (cur) {
            if (cur->data == value) return true;
            cur = cur->next;
        }
        return false;
    }

    void display() {
        Node* cur = head;
        while (cur) {
            cout << cur->data << " -> ";
            cur = cur->next;
        }
        cout << "NULL\n";
    }

    ~LinkedList() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

// ---------- Stack ----------
class Stack {
private:
    Node* top;
public:
    Stack() : top(nullptr) {}

    void push(int value) {
        Node* newNode = new Node(value);
        newNode->next = top;
        top = newNode;
    }

    int pop() {
        if (isEmpty()) return -1;
        Node* temp = top;
        int val = temp->data;
        top = top->next;
        delete temp;
        return val;
    }

    bool isEmpty() {
        return top == nullptr;
    }

    void display() {
        Node* cur = top;
        while (cur) {
            cout << cur->data << " ";
            cur = cur->next;
        }
        cout << "\n";
    }

    ~Stack() {
        while (!isEmpty()) pop();
    }
};

// ---------- Queue ----------
class Queue {
private:
    Node* front;
    Node* rear;
public:
    Queue() : front(nullptr), rear(nullptr) {}

    void enqueue(int value) {
        Node* newNode = new Node(value);
        if (!rear) {
            front = rear = newNode;
            return;
        }
        rear->next = newNode;
        rear = newNode;
    }

    int dequeue() {
        if (isEmpty()) return -1;
        Node* temp = front;
        int val = temp->data;
        front = front->next;
        if (!front) rear = nullptr;
        delete temp;
        return val;
    }

    bool isEmpty() {
        return front == nullptr;
    }

    void display() {
        Node* cur = front;
        while (cur) {
            cout << cur->data << " ";
            cur = cur->next;
        }
        cout << "\n";
    }

    ~Queue() {
        while (!isEmpty()) dequeue();
    }
};



double averageSequential(int* arr, int n) {
    long long sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return (double)sum / n;
}

double averageParallel(int* arr, int n) {
    long long sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return (double)sum / n;
}


int main() {
    srand((unsigned)time(nullptr));

    int N;
    cout << "Enter array size: ";
    cin >> N;

    if (N <= 0) return 1;

    int* data = new int[N];

    for (int i = 0; i < N; i++) {
        data[i] = rand() % 100 + 1;
    }

    // ---------- MIN / MAX ----------
    auto t1 = chrono::high_resolution_clock::now();
    int minSeq = data[0], maxSeq = data[0];
    for (int i = 1; i < N; i++) {
        if (data[i] < minSeq) minSeq = data[i];
        if (data[i] > maxSeq) maxSeq = data[i];
    }
    auto t2 = chrono::high_resolution_clock::now();

    int minPar = INT_MAX, maxPar = INT_MIN;
    auto t3 = chrono::high_resolution_clock::now();
    #pragma omp parallel for reduction(min:minPar) reduction(max:maxPar)
    for (int i = 0; i < N; i++) {
        if (data[i] < minPar) minPar = data[i];
        if (data[i] > maxPar) maxPar = data[i];
    }
    auto t4 = chrono::high_resolution_clock::now();

    cout << "Sequential min/max: " << minSeq << " / " << maxSeq << endl;
    cout << "Parallel   min/max: " << minPar << " / " << maxPar << endl;

    // ---------- AVERAGE ----------
    auto t5 = chrono::high_resolution_clock::now();
    double avgSeq = averageSequential(data, N);
    auto t6 = chrono::high_resolution_clock::now();

    auto t7 = chrono::high_resolution_clock::now();
    double avgPar = averageParallel(data, N);
    auto t8 = chrono::high_resolution_clock::now();

    cout << "Sequential average: " << avgSeq << endl;
    cout << "Parallel   average: " << avgPar << endl;

    // ---------- DATA STRUCTURES ----------
    LinkedList list;
    list.append(10);
    list.append(20);
    list.append(30);
    list.display();

    Stack stack;
    stack.push(1);
    stack.push(2);
    stack.push(3);
    stack.display();

    Queue queue;
    queue.enqueue(5);
    queue.enqueue(6);
    queue.enqueue(7);
    queue.display();

    delete[] data;
    return 0;
}
