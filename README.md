# Intelligent Task Planning & Recommendation System (ITPRS)

An intelligent, adaptive task planner designed to optimize productivity by dynamically prioritizing tasks and generating efficient schedules based on deadlines, duration, and importance.

---

## 📌 Overview

Managing multiple tasks with competing deadlines, varying priorities, and limited time often leads to inefficient planning and frequent rework. Most traditional planners are static and require constant manual intervention.

**ITPRS** addresses this problem by acting as a smart scheduling engine that:

* Thinks ahead
* Adapts in real time
* Continuously optimizes task schedules as conditions change

The system is built with a **C++ core** for performance and algorithmic efficiency, complemented by a **Python GUI** and an **HTML/CSS dashboard** for visualization.

---

## 🚀 Features

* **Intelligent Task Prioritization**

  * Ranks tasks using deadlines, estimated duration, and importance
* **Optimized Scheduling**

  * Generates daily and weekly task plans automatically
* **Dynamic Recalculation**

  * Re-optimizes schedules when tasks are added, updated, or delayed
* **User-Friendly Interface**

  * GUI-based interaction for task input and visualization
* **Scalable Architecture**

  * Designed to handle increasing task complexity efficiently

---

## 🛠️ Tech Stack

**Core Logic**

* C++
* Data Structures & Algorithms (Priority Queues, Sorting, Scheduling Heuristics)

**Interface & Dashboard**

* Python (Tkinter GUI)

---

## 📂 Project Structure

```plaintext
ITPRS/
│
├── ITPRS.cpp          # Core C++ scheduling and prioritization engine
├── planner_gui.py     # Python-based GUI for task input and interaction
├── tasks.txt          # taskstxt is added automatically when you run the code
├── README.md
```

---

## ⚙️ How It Works

1. **Task Input**

   * User provides task details: name, deadline, estimated duration, and importance
2. **Priority Evaluation**

   * Tasks are ranked using a weighted priority strategy
3. **Schedule Generation**

   * Optimized time slots are allocated for daily and weekly plans
4. **Real-Time Adaptation**

   * Any task modification triggers automatic re-planning

---

## ▶️ How to Run

### 1. Compile using the C++ Core

```bash
g++ ITPRS.cpp -o itprs
./itprs
```
### (or)

### 2. Launch the GUI

```bash
python3 planner_gui.py
```

---

## 📈 Future Enhancements

* Machine learning–based priority tuning
* Calendar integration (Google / Outlook)
* Notification and reminder system
* Cloud-based task synchronization
* Mobile-friendly dashboard

---

## 👩‍💻 Author

**Amuktha Goteti**
Electronics & Communications Engineer
Focused on building intelligent, efficient, and adaptive systems using strong algorithmic foundations.