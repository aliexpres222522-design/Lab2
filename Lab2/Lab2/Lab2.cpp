#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Student {
    string last, first;
    int grade, room, bus;
};

struct Teacher {
    string last, first;
    int room;
};

vector<string> split(const string& s) {
    vector<string> res;
    stringstream ss(s);
    string item;
    while (getline(ss, item, ',')) {
        while (!item.empty() && isspace((unsigned char)item.front())) item.erase(item.begin());
        while (!item.empty() && isspace((unsigned char)item.back())) item.pop_back();
        res.push_back(item);
    }
    return res;
}

int safeStoi(const string& s) {
    try {
        return stoi(s);
    }
    catch (...) {
        return -1;
    }
}

int main() {
    vector<Student> st;
    vector<Teacher> tc;

    ifstream f1("list.txt");
    if (!f1.is_open()) {
        cout << "Error: list.txt not found in current folder\n";
        return 1;
    }

    string line;
    while (getline(f1, line)) {
        auto p = split(line);
        if (p.size() >= 5) {
            st.push_back({ p[0], p[1], safeStoi(p[2]), safeStoi(p[3]), safeStoi(p[4]) });
        }
    }

    ifstream f2("teacher.txt");
    if (!f2.is_open()) f2.open("teachers.txt");
    if (!f2.is_open()) {
        cout << "Error: teacher.txt / teachers.txt not found in current folder\n";
        return 1;
    }

    while (getline(f2, line)) {
        auto p = split(line);
        if (p.size() >= 3) {
            tc.push_back({ p[0], p[1], safeStoi(p[2]) });
        }
    }

    cout << "Loaded: " << st.size() << " students, " << tc.size() << " teachers. Enter command (or Quit):\n";

    while (getline(cin, line)) {
        while (!line.empty() && isspace((unsigned char)line.front())) line.erase(line.begin());
        while (!line.empty() && isspace((unsigned char)line.back())) line.pop_back();

        if (line.empty()) continue;

        if (line == "Quit" || line == "Q") {
            cout << "bye!\n";
            break;
        }

        auto t1 = chrono::steady_clock::now();

        if (line.rfind("G:", 0) == 0 || line.rfind("Grade:", 0) == 0) {
            auto pos = line.find(':');
            string rest = line.substr(pos + 1);
            if (rest.find('T') != string::npos) {
                int g = safeStoi(rest);
                for (auto& s : st) {
                    if (s.grade == g) {
                        for (auto& t : tc) {
                            if (t.room == s.room) cout << t.last << ", " << t.first << "\n";
                        }
                    }
                }
            }
            else {
                int g = safeStoi(rest);
                for (auto& s : st) {
                    if (s.grade == g) cout << s.last << ", " << s.first << "\n";
                }
            }
        }
        else if (line.rfind("C:", 0) == 0 || line.rfind("Classroom:", 0) == 0) {
            auto pos = line.find(':');
            string rest = line.substr(pos + 1);
            if (rest.find('T') != string::npos) {
                int r = safeStoi(rest);
                for (auto& t : tc) {
                    if (t.room == r) cout << t.last << ", " << t.first << "\n";
                }
            }
            else {
                int r = safeStoi(rest);
                for (auto& s : st) {
                    if (s.room == r) cout << s.last << ", " << s.first << "\n";
                }
            }
        }
        else if (line.rfind("S:", 0) == 0 || line.rfind("Student:", 0) == 0) {
            auto pos = line.find(':');
            string rest = line.substr(pos + 1);
            while (!rest.empty() && isspace((unsigned char)rest.front())) rest.erase(rest.begin());

            bool needBus = false;
            if (rest.size() >= 2 && rest.substr(rest.size() - 2) == " B") {
                needBus = true;
                rest = rest.substr(0, rest.size() - 2);
            }
            else if (rest.size() >= 4 && rest.substr(rest.size() - 4) == " Bus") {
                needBus = true;
                rest = rest.substr(0, rest.size() - 4);
            }
            while (!rest.empty() && isspace((unsigned char)rest.back())) rest.pop_back();

            for (auto& s : st) {
                if (s.last == rest) {
                    if (needBus) {
                        cout << s.last << ", " << s.first << ", " << s.bus << "\n";
                    }
                    else {
                        for (auto& t : tc) {
                            if (t.room == s.room) {
                                cout << s.last << ", " << s.first << ", " << s.room
                                    << ", " << t.last << ", " << t.first << "\n";
                            }
                        }
                    }
                }
            }
        }
        else if (line.rfind("T:", 0) == 0 || line.rfind("Teacher:", 0) == 0) {
            string name = line.substr(line.find(':') + 1);
            while (!name.empty() && isspace((unsigned char)name.front())) name.erase(name.begin());
            for (auto& t : tc) {
                if (t.last == name) {
                    for (auto& s : st) {
                        if (s.room == t.room) cout << s.last << ", " << s.first << "\n";
                    }
                }
            }
        }
        else if (line.rfind("B:", 0) == 0 || line.rfind("Bus:", 0) == 0) {
            int b = safeStoi(line.substr(line.find(':') + 1));
            for (auto& s : st) {
                if (s.bus == b) cout << s.last << ", " << s.first << "\n";
            }
        }
        else if (line.rfind("Delete ", 0) == 0) {
            string name = line.substr(7);
            while (!name.empty() && isspace((unsigned char)name.front())) name.erase(name.begin());
            int removed = 0;
            for (size_t i = 0; i < st.size(); ) {
                if (st[i].last == name) {
                    st.erase(st.begin() + i);
                    removed++;
                }
                else i++;
            }
            cout << "Deleted: " << removed << "\n";
        }
        else if (line.rfind("Update ", 0) == 0) {
            auto p = line.rfind(" Bus ");
            if (p != string::npos) {
                string name = line.substr(7, p - 7);
                while (!name.empty() && isspace((unsigned char)name.front())) name.erase(name.begin());
                while (!name.empty() && isspace((unsigned char)name.back())) name.pop_back();
                int newBus = safeStoi(line.substr(p + 5));
                int updated = 0;
                for (auto& s : st) {
                    if (s.last == name) {
                        s.bus = newBus;
                        updated++;
                    }
                }
                cout << "Updated: " << updated << "\n";
            }
        }

        auto t2 = chrono::steady_clock::now();
        auto ms = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
        cout << ms << "ms\n";
    }
    return 0;
}