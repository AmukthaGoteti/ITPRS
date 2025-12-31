#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <fstream>
#include <ctime>
#include <iomanip>

using namespace std;

/* =========================
   TASK MODEL
   ========================= */
struct Task {
    string name;
    int importance;      // 1–10
    int duration;        // hours
    time_t deadline;
    double priority;
    int preferredDay;    // 0–6 (Mon–Sun), -1 = no preference
};

/* =========================
   PRIORITY ENGINE
   ========================= */
class PriorityEngine {
public:
    static double compute(const Task& t) {
        time_t now = time(nullptr);
        double hoursLeft = difftime(t.deadline, now) / 3600.0;

        if (hoursLeft <= 0) return 1e9; // overdue
        return (t.importance * 10.0) / (hoursLeft + t.duration);
    }
};

/* =========================
   PRIORITY QUEUE COMPARATOR
   ========================= */
struct CompareTask {
    bool operator()(const Task& a, const Task& b) {
        return a.priority < b.priority; // max heap
    }
};

/* =========================
   WEEKLY TIME SLOT MODEL
   ========================= */
struct TimeSlot {
    int day;          // 0–6
    int startHour;
    int duration;
    string taskName;
};

/* =========================
   DAY NAME HELPER
   ========================= */
string getDayName(int day) {
    static const string days[] = {
        "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday", "Sunday"
    };
    return days[day];
}

/* =========================
   PLANNER CORE
   ========================= */
class Planner {
private:
    vector<Task> tasks;
    const string filename = "tasks.txt";

public:
    Planner() {
        load();
    }

    /* ===== ADD TASK ===== */
    void addTask() {
        Task t;
        int hoursFromNow;

        cin.ignore();
        cout << "Task Name: ";
        getline(cin, t.name);

        cout << "Importance (1-10): ";
        cin >> t.importance;

        cout << "Duration (hours): ";
        cin >> t.duration;

        cout << "Deadline (hours from now): ";
        cin >> hoursFromNow;

        cout << "Preferred Day (0=Mon … 6=Sun, -1=None): ";
        cin >> t.preferredDay;

        t.deadline = time(nullptr) + hoursFromNow * 3600;
        t.priority = PriorityEngine::compute(t);

        tasks.push_back(t);
        save();
    }

    /* ===== REMOVE TASK ===== */
    void removeTask() {
        if (tasks.empty()) return;

        showTasks();
        int i;
        cout << "Delete task #: ";
        cin >> i;

        if (i >= 1 && i <= tasks.size()) {
            tasks.erase(tasks.begin() + i - 1);
            save();
        }
    }

    /* ===== OPTIMIZED VIEW ===== */
    void showOptimized() {
        priority_queue<Task, vector<Task>, CompareTask> pq;

        for (auto& t : tasks) {
            t.priority = PriorityEngine::compute(t);
            pq.push(t);
        }

        int i = 1;
        cout << "\n--- Optimized Schedule ---\n";
        while (!pq.empty()) {
            auto t = pq.top(); pq.pop();
            cout << i++ << ". " << t.name
                 << " | Priority: " << fixed << setprecision(2)
                 << t.priority << "\n";
        }
    }

    /* ===== WEEKLY PLANNER (DAY-AWARE) ===== */
    void weeklySchedule() {
        if (tasks.empty()) {
            cout << "\nNo tasks available.\n";
            return;
        }

        const int DAILY_HOURS = 8;
        vector<vector<TimeSlot>> week(7);
        vector<int> usedHours(7, 0);

        priority_queue<Task, vector<Task>, CompareTask> pq;

        for (auto& t : tasks) {
            t.priority = PriorityEngine::compute(t);
            pq.push(t);
        }

        vector<Task> remaining;

        // 1️⃣ Place preferred-day tasks first
        while (!pq.empty()) {
            Task t = pq.top(); pq.pop();

            if (t.preferredDay >= 0 && t.preferredDay < 7 &&
                usedHours[t.preferredDay] + t.duration <= DAILY_HOURS) {

                week[t.preferredDay].push_back({
                    t.preferredDay,
                    usedHours[t.preferredDay],
                    t.duration,
                    t.name
                });

                usedHours[t.preferredDay] += t.duration;
            } else {
                remaining.push_back(t);
            }
        }

        // 2️⃣ Place remaining tasks greedily
        for (auto& t : remaining) {
            for (int day = 0; day < 7; day++) {
                if (usedHours[day] + t.duration <= DAILY_HOURS) {
                    week[day].push_back({
                        day,
                        usedHours[day],
                        t.duration,
                        t.name
                    });
                    usedHours[day] += t.duration;
                    break;
                }
            }
        }

        // 3️⃣ Display
        cout << "\n========== WEEKLY PLANNER ==========\n";
        for (int day = 0; day < 7; day++) {
            cout << "\n" << getDayName(day) << ":\n";

            if (week[day].empty()) {
                cout << "  No tasks scheduled\n";
                continue;
            }

            for (auto& slot : week[day]) {
                cout << "  "
                     << setw(2) << slot.startHour << ":00 - "
                     << setw(2) << (slot.startHour + slot.duration)
                     << ":00 | " << slot.taskName << "\n";
            }
        }
        cout << "\n====================================\n";
    }

    /* ===== ANALYTICS ===== */
    void analytics() {
        int overdue = 0;
        double avgImp = 0;
        time_t now = time(nullptr);

        for (auto& t : tasks) {
            avgImp += t.importance;
            if (t.deadline < now) overdue++;
        }

        if (!tasks.empty()) avgImp /= tasks.size();

        cout << "\n--- Productivity Analytics ---\n";
        cout << "Total Tasks    : " << tasks.size() << "\n";
        cout << "Overdue Tasks  : " << overdue << "\n";
        cout << "Avg Importance : " << fixed << setprecision(2)
             << avgImp << "\n";
    }

    /* ===== FILE I/O ===== */
    void save() {
        ofstream out(filename);
        for (auto& t : tasks) {
            out << t.name << "|"
                << t.importance << "|"
                << t.duration << "|"
                << t.deadline << "|"
                << t.preferredDay << "\n";
        }
        out.close();
    }

    void load() {
        ifstream in(filename);
        if (!in) return;

        tasks.clear();
        string line;

        while (getline(in, line)) {
            Task t;
            size_t p1 = line.find('|');
            size_t p2 = line.find('|', p1 + 1);
            size_t p3 = line.find('|', p2 + 1);
            size_t p4 = line.find('|', p3 + 1);

            t.name = line.substr(0, p1);
            t.importance = stoi(line.substr(p1 + 1, p2 - p1 - 1));
            t.duration = stoi(line.substr(p2 + 1, p3 - p2 - 1));
            t.deadline = stol(line.substr(p3 + 1, p4 - p3 - 1));
            t.preferredDay = stoi(line.substr(p4 + 1));
            t.priority = 0;

            tasks.push_back(t);
        }
        in.close();
    }

    void showTasks() {
        for (int i = 0; i < tasks.size(); i++) {
            cout << i + 1 << ". " << tasks[i].name << "\n";
        }
    }
};

/* =========================
   MENU
   ========================= */
void menu() {
    cout << "\n==== Intelligent Planner ====\n";
    cout << "1. Add Task\n";
    cout << "2. Delete Task\n";
    cout << "3. View Optimized Schedule\n";
    cout << "4. Weekly Planner\n";
    cout << "5. Productivity Analytics\n";
    cout << "6. Exit\n";
    cout << "Choose: ";
}

/* =========================
   MAIN
   ========================= */
int main() {
    Planner planner;
    int choice;

    do {
        menu();
        cin >> choice;

        switch (choice) {
            case 1: planner.addTask(); break;
            case 2: planner.removeTask(); break;
            case 3: planner.showOptimized(); break;
            case 4: planner.weeklySchedule(); break;
            case 5: planner.analytics(); break;
            case 6: cout << "Goodbye.\n"; break;
            default: cout << "Invalid option.\n";
        }
    } while (choice != 6);

    return 0;
}