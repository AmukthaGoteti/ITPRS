#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <iomanip>

using namespace std;

/* =========================
   TASK DATA MODEL
   ========================= */
struct Task {
    string name;
    int importance;        // 1–10
    int duration;          // hours
    time_t deadline;       // UNIX timestamp
    double priorityScore;
};

/* =========================
   PRIORITY ENGINE
   ========================= */
class PriorityEngine {
public:
    static double calculatePriority(const Task& task) {
        time_t now = time(nullptr);
        double hoursLeft = difftime(task.deadline, now) / 3600.0;

        if (hoursLeft <= 0)
            return 1e9; // overdue tasks

        return (task.importance * 10.0) / (hoursLeft + task.duration);
    }
};

/* =========================
   SCHEDULER
   ========================= */
class Scheduler {
public:
    static void optimize(vector<Task>& tasks) {
        for (auto& task : tasks) {
            task.priorityScore = PriorityEngine::calculatePriority(task);
        }

        sort(tasks.begin(), tasks.end(),
             [](const Task& a, const Task& b) {
                 return a.priorityScore > b.priorityScore;
             });
    }

    static void display(const vector<Task>& tasks) {
        if (tasks.empty()) {
            cout << "\nNo tasks available.\n";
            return;
        }

        cout << "\n==============================\n";
        cout << " Optimized Task Schedule\n";
        cout << "==============================\n";

        for (size_t i = 0; i < tasks.size(); ++i) {
            cout << i + 1 << ". " << tasks[i].name << "\n";
            cout << "   Importance : " << tasks[i].importance << "\n";
            cout << "   Duration   : " << tasks[i].duration << " hrs\n";
            cout << "   Priority   : " << fixed << setprecision(2)
                 << tasks[i].priorityScore << "\n\n";
        }
    }
};

/* =========================
   PLANNER CONTROLLER
   ========================= */
class Planner {
private:
    vector<Task> tasks;

public:
    void addTask() {
        Task t;
        int hoursFromNow;

        cin.ignore();
        cout << "Task Name: ";
        getline(cin, t.name);

        cout << "Importance (1-10): ";
        cin >> t.importance;

        cout << "Estimated Duration (hours): ";
        cin >> t.duration;

        cout << "Deadline (hours from now): ";
        cin >> hoursFromNow;

        t.deadline = time(nullptr) + hoursFromNow * 3600;
        t.priorityScore = 0.0;

        tasks.push_back(t);
        recalculate();
    }

    void updateTask() {
        if (tasks.empty()) {
            cout << "No tasks to update.\n";
            return;
        }

        int index, hoursFromNow;
        cout << "Enter task number to update: ";
        cin >> index;

        if (index < 1 || index > tasks.size()) {
            cout << "Invalid task number.\n";
            return;
        }

        Task& t = tasks[index - 1];

        cout << "New Importance (1-10): ";
        cin >> t.importance;

        cout << "New Duration (hours): ";
        cin >> t.duration;

        cout << "New Deadline (hours from now): ";
        cin >> hoursFromNow;

        t.deadline = time(nullptr) + hoursFromNow * 3600;
        recalculate();
    }

    void removeTask() {
        if (tasks.empty()) {
            cout << "No tasks to delete.\n";
            return;
        }

        int index;
        cout << "Enter task number to delete: ";
        cin >> index;

        if (index < 1 || index > tasks.size()) {
            cout << "Invalid task number.\n";
            return;
        }

        tasks.erase(tasks.begin() + index - 1);
        recalculate();
    }

    void showSchedule() const {
        Scheduler::display(tasks);
    }

private:
    void recalculate() {
        Scheduler::optimize(tasks);
    }
};

/* =========================
   MENU
   ========================= */
void showMenu() {
    cout << "\n====== Intelligent Task Planner ======\n";
    cout << "1. Add Task\n";
    cout << "2. Update Task\n";
    cout << "3. Delete Task\n";
    cout << "4. View Optimized Schedule\n";
    cout << "5. Exit\n";
    cout << "Choose an option: ";
}

/* =========================
   MAIN
   ========================= */
int main() {
    Planner planner;
    int choice;

    do {
        showMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                planner.addTask();
                break;
            case 2:
                planner.updateTask();
                break;
            case 3:
                planner.removeTask();
                break;
            case 4:
                planner.showSchedule();
                break;
            case 5:
                cout << "Exiting planner. Stay organized.\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 5);

    return 0;
}