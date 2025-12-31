import tkinter as tk
from tkinter import ttk, messagebox
import time
import heapq
import os

FILE_NAME = "tasks.txt"
DAYS = ["Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday", "Sunday"]

# =========================
# TASK MODEL
# =========================
class Task:
    def __init__(self, name, importance, duration, deadline, preferred_day):
        self.name = name
        self.importance = importance
        self.duration = duration
        self.deadline = deadline
        self.preferred_day = preferred_day
        self.priority = self.compute_priority()

    def compute_priority(self):
        hours_left = (self.deadline - time.time()) / 3600
        if hours_left <= 0:
            return 1e9
        return (self.importance * 10) / (hours_left + self.duration)

    def __lt__(self, other):
        return self.priority > other.priority  # max heap


# =========================
# PLANNER CORE
# =========================
class Planner:
    def __init__(self):
        self.tasks = []
        self.load()

    def add_task(self, name, importance, duration, hours_from_now, preferred_day):
        deadline = time.time() + hours_from_now * 3600
        self.tasks.append(Task(name, importance, duration, deadline, preferred_day))
        self.save()

    def remove_task(self, index):
        if 0 <= index < len(self.tasks):
            self.tasks.pop(index)
            self.save()

    def priority_queue(self):
        pq = []
        for t in self.tasks:
            t.priority = t.compute_priority()
            heapq.heappush(pq, t)
        return pq

    def weekly_schedule(self):
        DAILY_HOURS = 8
        week = [[] for _ in range(7)]
        used = [0] * 7

        pq = self.priority_queue()
        remaining = []

        # Preferred-day placement
        while pq:
            t = heapq.heappop(pq)
            d = t.preferred_day
            if 0 <= d < 7 and used[d] + t.duration <= DAILY_HOURS:
                week[d].append((used[d], t.duration, t.name))
                used[d] += t.duration
            else:
                remaining.append(t)

        # Fallback placement
        for t in remaining:
            for d in range(7):
                if used[d] + t.duration <= DAILY_HOURS:
                    week[d].append((used[d], t.duration, t.name))
                    used[d] += t.duration
                    break

        return week

    def analytics(self):
        overdue = 0
        avg_imp = 0
        now = time.time()

        for t in self.tasks:
            avg_imp += t.importance
            if t.deadline < now:
                overdue += 1

        if self.tasks:
            avg_imp /= len(self.tasks)

        return len(self.tasks), overdue, avg_imp

    # -------- FILE I/O --------
    def save(self):
        with open(FILE_NAME, "w") as f:
            for t in self.tasks:
                f.write(f"{t.name}|{t.importance}|{t.duration}|"
                        f"{int(t.deadline)}|{t.preferred_day}\n")

    def load(self):
        if not os.path.exists(FILE_NAME):
            return

        self.tasks.clear()
        with open(FILE_NAME) as f:
            for line in f:
                try:
                    name, imp, dur, dl, day = line.strip().split("|")
                    self.tasks.append(
                        Task(name, int(imp), int(dur), int(dl), int(day))
                    )
                except:
                    continue


# =========================
# GUI
# =========================
class PlannerGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Intelligent Task Planner")
        self.planner = Planner()

        self.build_ui()
        self.refresh_tasks()

    def build_ui(self):
        frame = tk.Frame(self.root, padx=12, pady=12)
        frame.pack()

        # Inputs
        self.entries = {}
        labels = ["Task Name", "Importance (1–10)",
                  "Duration (hrs)", "Deadline (hrs from now)",
                  "Preferred Day (-1 or 0–6)"]

        for i, lbl in enumerate(labels):
            tk.Label(frame, text=lbl).grid(row=i, column=0, sticky="w")
            e = tk.Entry(frame)
            e.grid(row=i, column=1)
            self.entries[lbl] = e

        tk.Button(frame, text="Add Task", command=self.add_task)\
            .grid(row=5, column=0, columnspan=2, pady=6)

        # Task list
        self.tree = ttk.Treeview(frame, columns=("Task"), show="headings", height=8)
        self.tree.heading("Task", text="Task (Priority)")
        self.tree.grid(row=6, column=0, columnspan=2, pady=8)

        tk.Button(frame, text="Delete Selected", command=self.delete_task)\
            .grid(row=7, column=0, columnspan=2)

        # Actions
        tk.Button(frame, text="Weekly Planner", command=self.show_week)\
            .grid(row=8, column=0, pady=5)
        tk.Button(frame, text="Analytics", command=self.show_analytics)\
            .grid(row=8, column=1, pady=5)

    def refresh_tasks(self):
        self.tree.delete(*self.tree.get_children())
        pq = self.planner.priority_queue()
        while pq:
            t = heapq.heappop(pq)
            self.tree.insert("", "end",
                             values=(f"{t.name} | {t.priority:.2f}",))

    def add_task(self):
        try:
            self.planner.add_task(
                self.entries["Task Name"].get(),
                int(self.entries["Importance (1–10)"].get()),
                int(self.entries["Duration (hrs)"].get()),
                int(self.entries["Deadline (hrs from now)"].get()),
                int(self.entries["Preferred Day (-1 or 0–6)"].get())
            )
            self.refresh_tasks()
        except:
            messagebox.showerror("Error", "Invalid input")

    def delete_task(self):
        sel = self.tree.selection()
        if not sel:
            return
        idx = self.tree.index(sel[0])
        self.planner.remove_task(idx)
        self.refresh_tasks()

    def show_week(self):
        week = self.planner.weekly_schedule()
        text = ""
        for d in range(7):
            text += f"\n{DAYS[d]}:\n"
            if not week[d]:
                text += "  No tasks\n"
            for start, dur, name in week[d]:
                text += f"  {start}:00–{start+dur}:00  {name}\n"
        messagebox.showinfo("Weekly Planner", text)

    def show_analytics(self):
        total, overdue, avg = self.planner.analytics()
        messagebox.showinfo(
            "Analytics",
            f"Total Tasks: {total}\nOverdue: {overdue}\nAvg Importance: {avg:.2f}"
        )


# =========================
# MAIN
# =========================
if __name__ == "__main__":
    root = tk.Tk()
    PlannerGUI(root)
    root.mainloop()