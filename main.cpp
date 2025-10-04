#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <ctime>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

enum class Priority {
    LOW,
    MEDIUM,
    HIGH,
    CRITICAL
};

enum class Status {
    ACTIVE,
    COMPLETED,
    ARCHIVED
};

class Task {
private:
    int id;
    std::string title;
    std::string description;
    Priority priority;
    Status status;
    std::vector<std::string> tags;
    std::time_t created;
    std::time_t deadline;

public:
    Task(int id, const std::string& title, Priority priority = Priority::MEDIUM)
        : id(id), title(title), priority(priority), status(Status::ACTIVE),
          created(std::time(nullptr)), deadline(0) {}

    int getId() const { return id; }
    std::string getTitle() const { return title; }
    Priority getPriority() const { return priority; }
    Status getStatus() const { return status; }
    
    void setStatus(Status newStatus) { status = newStatus; }
    void setPriority(Priority newPriority) { priority = newPriority; }
    void addTag(const std::string& tag) { tags.push_back(tag); }
    
    std::string getPriorityString() const {
        switch(priority) {
            case Priority::LOW: return "Low";
            case Priority::MEDIUM: return "Medium";
            case Priority::HIGH: return "High";
            case Priority::CRITICAL: return "Critical";
            default: return "Unknown";
        }
    }
    
    std::string getPriorityColor() const {
        switch(priority) {
            case Priority::LOW: return GREEN;
            case Priority::MEDIUM: return YELLOW;
            case Priority::HIGH: return MAGENTA;
            case Priority::CRITICAL: return RED;
            default: return RESET;
        }
    }
    
    void display() const {
        std::cout << getPriorityColor() << "[" << id << "] " 
                  << (status == Status::COMPLETED ? "✓ " : "○ ")
                  << BOLD << title << RESET << getPriorityColor()
                  << " (" << getPriorityString() << ")" << RESET << std::endl;
        
        if (!tags.empty()) {
            std::cout << "    Tags: ";
            for (const auto& tag : tags) {
                std::cout << CYAN << "#" << tag << " " << RESET;
            }
            std::cout << std::endl;
        }
    }
};

class TaskManager {
private:
    std::vector<Task> tasks;
    int nextId;

public:
    TaskManager() : nextId(1) {}
    
    void addTask(const std::string& title, Priority priority = Priority::MEDIUM) {
        tasks.emplace_back(nextId++, title, priority);
        std::cout << GREEN << "✓ Task added successfully!" << RESET << std::endl;
    }
    
    void listTasks(Status filterStatus = Status::ACTIVE) {
        if (tasks.empty()) {
            std::cout << YELLOW << "No tasks found." << RESET << std::endl;
            return;
        }
        
        std::cout << BOLD << "\n=== Task List ===" << RESET << std::endl;
        for (const auto& task : tasks) {
            if (task.getStatus() == filterStatus || filterStatus == Status::ARCHIVED) {
                task.display();
            }
        }
        std::cout << std::endl;
    }
    
    void completeTask(int id) {
        for (auto& task : tasks) {
            if (task.getId() == id) {
                task.setStatus(Status::COMPLETED);
                std::cout << GREEN << "✓ Task #" << id << " completed!" << RESET << std::endl;
                return;
            }
        }
        std::cout << RED << "✗ Task #" << id << " not found." << RESET << std::endl;
    }
    
    void deleteTask(int id) {
        auto it = std::remove_if(tasks.begin(), tasks.end(),
            [id](const Task& t) { return t.getId() == id; });
        
        if (it != tasks.end()) {
            tasks.erase(it, tasks.end());
            std::cout << GREEN << "✓ Task #" << id << " deleted!" << RESET << std::endl;
        } else {
            std::cout << RED << "✗ Task #" << id << " not found." << RESET << std::endl;
        }
    }
    
    void showStats() {
        int active = 0, completed = 0;
        for (const auto& task : tasks) {
            if (task.getStatus() == Status::ACTIVE) active++;
            else if (task.getStatus() == Status::COMPLETED) completed++;
        }
        
        std::cout << BOLD << "\n=== Statistics ===" << RESET << std::endl;
        std::cout << "Total tasks: " << tasks.size() << std::endl;
        std::cout << GREEN << "Completed: " << completed << RESET << std::endl;
        std::cout << YELLOW << "Active: " << active << RESET << std::endl;
        if (tasks.size() > 0) {
            std::cout << "Completion rate: " 
                      << std::fixed << std::setprecision(1)
                      << (completed * 100.0 / tasks.size()) << "%" << std::endl;
        }
        std::cout << std::endl;
    }
};

void printHelp() {
    std::cout << BOLD << "\nTaskFlow - Console Task Manager" << RESET << std::endl;
    std::cout << "\nCommands:" << std::endl;
    std::cout << "  add <title> [priority]  - Add new task (priority: low/medium/high/critical)" << std::endl;
    std::cout << "  list                    - Show all active tasks" << std::endl;
    std::cout << "  complete <id>           - Mark task as completed" << std::endl;
    std::cout << "  delete <id>             - Delete task" << std::endl;
    std::cout << "  stats                   - Show statistics" << std::endl;
    std::cout << "  help                    - Show this help" << std::endl;
    std::cout << "  exit                    - Exit program" << std::endl;
    std::cout << std::endl;
}

int main() {
    TaskManager manager;
    std::string command;
    
    std::cout << BOLD << CYAN << "Welcome to TaskFlow!" << RESET << std::endl;
    printHelp();
    
    while (true) {
        std::cout << BLUE << "taskflow> " << RESET;
        std::getline(std::cin, command);
        
        if (command.empty()) continue;
        
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "exit" || cmd == "quit") {
            std::cout << "Goodbye!" << std::endl;
            break;
        } else if (cmd == "help") {
            printHelp();
        } else if (cmd == "add") {
            std::string title;
            std::getline(iss >> std::ws, title);
            if (!title.empty()) {
                manager.addTask(title);
            } else {
                std::cout << RED << "Please provide a task title." << RESET << std::endl;
            }
        } else if (cmd == "list") {
            manager.listTasks();
        } else if (cmd == "complete") {
            int id;
            if (iss >> id) {
                manager.completeTask(id);
            } else {
                std::cout << RED << "Please provide a task ID." << RESET << std::endl;
            }
        } else if (cmd == "delete") {
            int id;
            if (iss >> id) {
                manager.deleteTask(id);
            } else {
                std::cout << RED << "Please provide a task ID." << RESET << std::endl;
            }
        } else if (cmd == "stats") {
            manager.showStats();
        } else {
            std::cout << RED << "Unknown command. Type 'help' for available commands." 
                      << RESET << std::endl;
        }
    }
    
    return 0;
}