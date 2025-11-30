#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <limits>
#include <memory>

using namespace std;

// 文件路径常量
const string TEACHER_FILE = "teachers.dat";
const string STUDENT_FILE = "students.dat";
const string COURSE_FILE = "courses.dat";
const string QA_FILE = "qa_records.dat";

// 课程基类
class Course {
protected:
    string courseID;
    string courseName;
    string qaTime;

public:
    Course(string id, string name, string time) 
        : courseID(id), courseName(name), qaTime(time) {}

    virtual ~Course() {}
    
    string getCourseID() const { return courseID; }
    string getCourseName() const { return courseName; }
    string getQATime() const { return qaTime; }
    
    virtual string getType() const = 0;  // 纯虚函数
    virtual void showMe() const = 0;     // 纯虚函数
    
    // 文件操作
    virtual void saveToFile(ofstream& out) const {
        out << courseID << "|" << courseName << "|" << qaTime << "|";
    }
    
    virtual void loadFromFile(ifstream& in) {
        string line;
        getline(in, line);
        stringstream ss(line);
        getline(ss, courseID, '|');
        getline(ss, courseName, '|');
        getline(ss, qaTime, '|');
    }
};

// 必修课程类
class BCourse : public Course {
public:
    BCourse(string id, string name, string time) 
        : Course(id, name, time) {}
    
    BCourse(ifstream& in) : Course("", "", "") {
        loadFromFile(in);
    }
    
    string getType() const override { return "必修"; }
    
    void showMe() const override {
        cout << "课程ID: " << courseID 
             << ", 名称: " << courseName
             << ", 类型: 必修"
             << ", 答疑时间: " << qaTime << endl;
    }
    
    void saveToFile(ofstream& out) const override {
        Course::saveToFile(out);
        out << "B" << endl;
    }
    
    void loadFromFile(ifstream& in) override {
        Course::loadFromFile(in);
        char type;
        in >> type; in.ignore();
    }
};

// 选修课程类
class XCourse : public Course {
public:
    XCourse(string id, string name, string time) 
        : Course(id, name, time) {}
    
    XCourse(ifstream& in) : Course("", "", "") {
        loadFromFile(in);
    }
    
    string getType() const override { return "选修"; }
    
    void showMe() const override {
        cout << "课程ID: " << courseID 
             << ", 名称: " << courseName
             << ", 类型: 选修"
             << ", 答疑时间: " << qaTime << endl;
    }
    
    void saveToFile(ofstream& out) const override {
        Course::saveToFile(out);
        out << "X" << endl;
    }
    
    void loadFromFile(ifstream& in) override {
        Course::loadFromFile(in);
        char type;
        in >> type; in.ignore();
    }
};

// 答疑信息结构
struct QAInfo {
    string teacherID;
    string studentID;
    string courseID;
    string time;
    int rating;
    
    QAInfo(string tid, string sid, string cid, string t, int r)
        : teacherID(tid), studentID(sid), courseID(cid), time(t), rating(r) {}
    
    void saveToFile(ofstream& out) const {
        out << teacherID << "|" << studentID << "|" << courseID << "|"
            << time << "|" << rating << endl;
    }
    
    void loadFromFile(ifstream& in) {
        string line;
        getline(in, line);
        stringstream ss(line);
        getline(ss, teacherID, '|');
        getline(ss, studentID, '|');
        getline(ss, courseID, '|');
        getline(ss, time, '|');
        ss >> rating; in.ignore();
    }
    
    void display() const {
        cout << "教师: " << teacherID << ", 学生: " << studentID 
             << ", 课程: " << courseID << ", 时间: " << time 
             << ", 评分: " << rating << "/10" << endl;
    }
};

// 教师类
class Teacher {
private:
    string teacherID;
    string password;
    vector<string> courses; // 教授的课程ID列表
    vector<QAInfo> qaRecords;

public:
    Teacher() : teacherID(""), password("") {}
    Teacher(string id, string pwd) : teacherID(id), password(pwd) {}
    
    string getID() const { return teacherID; }
    string getPassword() const { return password; }
    void setPassword(string pwd) { password = pwd; }
    
    void addCourse(string courseID) {
        if (find(courses.begin(), courses.end(), courseID) == courses.end()) {
            courses.push_back(courseID);
            cout << "课程添加成功!" << endl;
        } else {
            cout << "该课程已存在!" << endl;
        }
    }
    
    void deleteCourse(string courseID) {
        auto it = find(courses.begin(), courses.end(), courseID);
        if (it != courses.end()) {
            courses.erase(it);
            cout << "课程删除成功!" << endl;
        } else {
            cout << "未找到该课程!" << endl;
        }
    }
    
    void searchCourses() const {
        if (courses.empty()) {
            cout << "暂无教授课程!" << endl;
            return;
        }
        cout << "教授的课程列表:" << endl;
        for (const auto& cid : courses) {
            cout << "- " << cid << endl;
        }
    }
    
    void addQA(string sid, string cid, string time) {
        qaRecords.emplace_back(teacherID, sid, cid, time, 0);
        cout << "答疑记录添加成功!" << endl;
    }
    
    void showRatings() const {
        if (qaRecords.empty()) {
            cout << "暂无答疑记录!" << endl;
            return;
        }
        
        vector<int> ratings;
        for (const auto& qa : qaRecords) {
            if (qa.rating > 0) {
                ratings.push_back(qa.rating);
            }
        }
        
        if (ratings.empty()) {
            cout << "暂无评分记录!" << endl;
            return;
        }
        
        int sum = 0, max = 0, min = 10;
        for (int r : ratings) {
            sum += r;
            if (r > max) max = r;
            if (r < min) min = r;
        }
        
        cout << "评分统计: "
             << "最高分: " << max << ", "
             << "最低分: " << min << ", "
             << "平均分: " << fixed << setprecision(1) 
             << static_cast<double>(sum) / ratings.size() << endl;
    }
    
    void displayQARecords() const {
        if (qaRecords.empty()) {
            cout << "暂无答疑记录!" << endl;
            return;
        }
        
        cout << "答疑记录:" << endl;
        for (const auto& qa : qaRecords) {
            qa.display();
        }
    }
    
    const vector<string>& getCourses() const { return courses; }
    vector<QAInfo>& getQARecords() { return qaRecords; }
    
    // 文件操作
    void saveToFile(ofstream& out) const {
        out << teacherID << "|" << password << "|";
        for (size_t i = 0; i < courses.size(); i++) {
            out << courses[i];
            if (i < courses.size() - 1) out << ",";
        }
        out << endl;
    }
    
    void loadFromFile(ifstream& in) {
        string line;
        getline(in, line);
        stringstream ss(line);
        getline(ss, teacherID, '|');
        getline(ss, password, '|');
        
        string courseList;
        getline(ss, courseList);
        stringstream cl(courseList);
        string courseID;
        while (getline(cl, courseID, ',')) {
            if (!courseID.empty()) {
                courses.push_back(courseID);
            }
        }
    }
};

// 学生类
class Student {
private:
    string studentID;
    string password;
    vector<string> courses; // 选修的课程ID列表

public:
    Student() : studentID(""), password("") {}
    Student(string id, string pwd) : studentID(id), password(pwd) {}
    
    string getID() const { return studentID; }
    string getPassword() const { return password; }
    void setPassword(string pwd) { password = pwd; }
    
    void selectCourse(string courseID) {
        if (find(courses.begin(), courses.end(), courseID) == courses.end()) {
            courses.push_back(courseID);
            cout << "课程选修成功!" << endl;
        } else {
            cout << "该课程已选修!" << endl;
        }
    }
    
    void unselectCourse(string courseID) {
        auto it = find(courses.begin(), courses.end(), courseID);
        if (it != courses.end()) {
            courses.erase(it);
            cout << "课程退选成功!" << endl;
        } else {
            cout << "未找到该课程!" << endl;
        }
    }
    
    void searchCourses() const {
        if (courses.empty()) {
            cout << "暂无选修课程!" << endl;
            return;
        }
        cout << "选修的课程列表:" << endl;
        for (const auto& cid : courses) {
            cout << "- " << cid << endl;
        }
    }
    
    const vector<string>& getCourses() const { return courses; }
    
    // 文件操作
    void saveToFile(ofstream& out) const {
        out << studentID << "|" << password << "|";
        for (size_t i = 0; i < courses.size(); i++) {
            out << courses[i];
            if (i < courses.size() - 1) out << ",";
        }
        out << endl;
    }
    
    void loadFromFile(ifstream& in) {
        string line;
        getline(in, line);
        stringstream ss(line);
        getline(ss, studentID, '|');
        getline(ss, password, '|');
        
        string courseList;
        getline(ss, courseList);
        stringstream cl(courseList);
        string courseID;
        while (getline(cl, courseID, ',')) {
            if (!courseID.empty()) {
                courses.push_back(courseID);
            }
        }
    }
};

// 管理系统类
class ManagementSystem {
private:
    map<string, Teacher> teachers;
    map<string, Student> students;
    map<string, unique_ptr<Course>> courses;
    vector<QAInfo> allQARecords;
    
    // 获取当前时间
    string getCurrentTime() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        stringstream ss;
        ss << 1900 + ltm->tm_year << "-"
           << setfill('0') << setw(2) << 1 + ltm->tm_mon << "-"
           << setw(2) << ltm->tm_mday << " "
           << setw(2) << ltm->tm_hour << ":"
           << setw(2) << ltm->tm_min;
        return ss.str();
    }
    
public:
    ManagementSystem() {
        loadData();
    }
    
    ~ManagementSystem() {
        saveData();
    }
    
    // 加载数据
    void loadData() {
        // 加载教师数据
        ifstream tfile(TEACHER_FILE);
        if (tfile) {
            while (tfile.peek() != EOF) {
                Teacher t("", "");
                t.loadFromFile(tfile);
                teachers[t.getID()] = t;
            }
            tfile.close();
        }
        
        // 加载学生数据
        ifstream sfile(STUDENT_FILE);
        if (sfile) {
            while (sfile.peek() != EOF) {
                Student s("", "");
                s.loadFromFile(sfile);
                students[s.getID()] = s;
            }
            sfile.close();
        }
        
        // 加载课程数据
        ifstream cfile(COURSE_FILE);
        if (cfile) {
            while (cfile.peek() != EOF) {
                char type;
                cfile >> type; cfile.ignore();
                
                if (type == 'B') {
                    unique_ptr<Course> course = make_unique<BCourse>(cfile);
                    courses[course->getCourseID()] = move(course);
                } else if (type == 'X') {
                    unique_ptr<Course> course = make_unique<XCourse>(cfile);
                    courses[course->getCourseID()] = move(course);
                }
            }
            cfile.close();
        }
        
        // 加载答疑记录
        ifstream qfile(QA_FILE);
        if (qfile) {
            while (qfile.peek() != EOF) {
                QAInfo qa("", "", "", "", 0);
                qa.loadFromFile(qfile);
                allQARecords.push_back(qa);
            }
            qfile.close();
        }
    }
    
    // 保存数据
    void saveData() {
        // 保存教师数据
        ofstream tfile(TEACHER_FILE);
        if (tfile) {
            for (const auto& t : teachers) {
                t.second.saveToFile(tfile);
            }
            tfile.close();
        }
        
        // 保存学生数据
        ofstream sfile(STUDENT_FILE);
        if (sfile) {
            for (const auto& s : students) {
                s.second.saveToFile(sfile);
            }
            sfile.close();
        }
        
        // 保存课程数据
        ofstream cfile(COURSE_FILE);
        if (cfile) {
            for (const auto& c : courses) {
                c.second->saveToFile(cfile);
            }
            cfile.close();
        }
        
        // 保存答疑记录
        ofstream qfile(QA_FILE);
        if (qfile) {
            for (const auto& qa : allQARecords) {
                qa.saveToFile(qfile);
            }
            qfile.close();
        }
    }
    
    // 用户认证
    Teacher* authenticateTeacher(string id, string pwd) {
        auto it = teachers.find(id);
        if (it != teachers.end()) {
            if (it->second.getPassword() == pwd) {
                return &it->second;
            }
            return nullptr; // 密码错误
        }
        
        // 新教师注册
        teachers.emplace(id, Teacher(id, pwd));
        return &teachers[id];
    }
    
    Student* authenticateStudent(string id, string pwd) {
        auto it = students.find(id);
        if (it != students.end()) {
            if (it->second.getPassword() == pwd) {
                return &it->second;
            }
            return nullptr; // 密码错误
        }
        
        // 新学生注册
        students.emplace(id, Student(id, pwd));
        return &students[id];
    }
    
    // 课程管理
    void addNewCourse(string id, string name, string time, string type) {
        if (courses.find(id) != courses.end()) {
            cout << "课程ID已存在!" << endl;
            return;
        }
        
        if (type == "必修") {
            courses[id] = make_unique<BCourse>(id, name, time);
        } else if (type == "选修") {
            courses[id] = make_unique<XCourse>(id, name, time);
        } else {
            cout << "无效的课程类型!" << endl;
            return;
        }
        cout << "课程创建成功!" << endl;
    }
    
    Course* getCourse(string id) {
        auto it = courses.find(id);
        if (it != courses.end()) {
            return it->second.get();
        }
        return nullptr;
    }
    
    void displayAllCourses() const {
        if (courses.empty()) {
            cout << "暂无课程信息!" << endl;
            return;
        }
        cout << "==============================================" << endl;
        cout << "                 所有课程信息                 " << endl;
        cout << "==============================================" << endl;
        for (const auto& c : courses) {
            c.second->showMe();
        }
        cout << "==============================================" << endl;
    }
    
    // 答疑管理
    void addQA(Teacher* t, string sid, string cid) {
        if (!t) return;
        
        // 检查教师是否教授该课程
        const vector<string>& teacherCourses = t->getCourses();
        if (find(teacherCourses.begin(), teacherCourses.end(), cid) == teacherCourses.end()) {
            cout << "您不教授此课程!" << endl;
            return;
        }
        
        // 检查学生是否选修该课程
        auto sit = students.find(sid);
        if (sit == students.end()) {
            cout << "学生不存在!" << endl;
            return;
        }
        
        const vector<string>& studentCourses = sit->second.getCourses();
        if (find(studentCourses.begin(), studentCourses.end(), cid) == studentCourses.end()) {
            cout << "该学生未选修此课程!" << endl;
            return;
        }
        
        string time = getCurrentTime();
        t->addQA(sid, cid, time);
        allQARecords.emplace_back(t->getID(), sid, cid, time, 0);
    }
    
    void rateQA(Student* s, string tid, string cid) {
        if (!s) return;
        
        // 查找未评分的答疑记录
        for (auto& qa : allQARecords) {
            if (qa.studentID == s->getID() && 
                qa.teacherID == tid && 
                qa.courseID == cid && 
                qa.rating == 0) {
                
                int rating;
                do {
                    cout << "请为本次答疑评分(1-10): ";
                    cin >> rating;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');//清空缓冲区
                } while (rating < 1 || rating > 10);
                
                qa.rating = rating;
                
                // 更新教师记录中的评分
                auto tit = teachers.find(tid);
                if (tit != teachers.end()) {
                    for (auto& tqa : tit->second.getQARecords()) {
                        if (tqa.studentID == s->getID() && 
                            tqa.courseID == cid && 
                            tqa.rating == 0) {
                            tqa.rating = rating;
                            break;
                        }
                    }
                }
                
                cout << "评分成功!" << endl;
                return;
            }
        }
        cout << "未找到可评分的答疑记录!" << endl;
    }
};

// 用户界面函数
void teacherMenu(Teacher* teacher, ManagementSystem& system);
void studentMenu(Student* student, ManagementSystem& system);

// 初始化系统数据
void initializeSystemData() {
    // 创建示例教师
    ofstream tfile(TEACHER_FILE);
    if (tfile) {
        tfile << "T001|pass123|C101,C102\n";
        tfile << "T002|pass456|C201,C203\n";
        tfile << "T003|pass124|C301,C303\n";
        tfile << "T004|pass135|C201,C203\n";
        tfile << "T005|pass147|C201,C203\n";
        tfile.close();
    }
    
    // 创建示例学生
    ofstream sfile(STUDENT_FILE);
    if (sfile) {
        sfile << "S1001|pass789|C101,C201\n";
        sfile << "S1002|pass123|C102,C203\n";
        sfile << "S1003|pass456|C302,C301\n";
        sfile << "S1004|pass567|C302,C202\n";
        sfile << "S1005|pass678|C303,C101\n";
        sfile.close();
    }
    
    // 创建示例课程
    ofstream cfile(COURSE_FILE);
    if (cfile) {
        cfile << "B|C101|高等数学|周一 14:00-16:00|\n";
        cfile << "X|C102|计算机基础|周三 10:00-12:00|\n";
        cfile << "B|C201|大学英语|周二 09:00-11:00|\n";
        cfile << "B|C202|数据结构|周四 15:00-17:00|\n";
        cfile << "B|C203|操作系统|周四 15:00-17:00|\n";
        cfile << "B|C301|计算机组成原理|周四 15:00-17:00|\n";
        cfile << "X|C302|电影美学|周四 15:00-17:00|\n";
        cfile << "X|C303|改革开放史|周四 15:00-17:00|\n";
        cfile.close();
    }
}

int main() {
    // 初始化系统数据
    initializeSystemData();
    
    ManagementSystem system;
    
    while (true) {
        cout << "\n==============================================" << endl;
        cout << "      教师在线答疑辅导管理系统 - 主菜单       " << endl;
        cout << "==============================================" << endl;
        cout << "1. 教师登录" << endl;
        cout << "2. 学生登录" << endl;
        cout << "3. 查看所有课程" << endl;
        cout << "4. 退出系统" << endl;
        cout << "==============================================" << endl;
        cout << "请选择: ";
        
        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (choice == 4) {
            system.saveData();
            cout << "数据已保存，感谢使用!" << endl;
            break;
        }
        
        string id, pwd;
        if (choice == 1 || choice == 2) {
            cout << "请输入ID: ";
            getline(cin, id);
            cout << "请输入密码: ";
            getline(cin, pwd);
        }
        
        if (choice == 1) {
            Teacher* teacher = system.authenticateTeacher(id, pwd);
            if (teacher) {
                teacherMenu(teacher, system);
            } else {
                cout << "登录失败! 工号或密码错误." << endl;
            }
        } else if (choice == 2) {
            Student* student = system.authenticateStudent(id, pwd);
            if (student) {
                studentMenu(student, system);
            } else {
                cout << "登录失败! 学号或密码错误." << endl;
            }
        } else if (choice == 3) {
            system.displayAllCourses();
        } else {
            cout << "无效选择!" << endl;
        }
    }
    
    return 0;
}

void teacherMenu(Teacher* teacher, ManagementSystem& system) {
    while (true) {
        cout << "\n==============================================" << endl;
        cout << "      教师管理菜单 (" << teacher->getID() << ")       " << endl;
        cout << "==============================================" << endl;
        cout << "1. 添加课程" << endl;
        cout << "2. 删除课程" << endl;
        cout << "3. 查询课程" << endl;
        cout << "4. 添加答疑记录" << endl;
        cout << "5. 查看评分统计" << endl;
        cout << "6. 查看答疑记录" << endl;
        cout << "7. 修改密码" << endl;
        cout << "8. 返回主菜单" << endl;
        cout << "==============================================" << endl;
        cout << "请选择: ";
        
        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (choice == 8) break;
        
        string cid, sid;
        switch (choice) {
            case 1: { // 添加课程
                string id, name, time, type;
                cout << "请输入课程ID: ";
                getline(cin, id);
                cout << "请输入课程名称: ";
                getline(cin, name);
                cout << "请输入答疑时间: ";
                getline(cin, time);
                cout << "请输入课程类型(必修/选修): ";
                getline(cin, type);
                
                // 添加新课程到系统
                system.addNewCourse(id, name, time, type);
                // 添加课程到教师
                teacher->addCourse(id);
                break;
            }
                
            case 2: // 删除课程
                teacher->searchCourses();
                cout << "请输入要删除的课程ID: ";
                getline(cin, cid);
                teacher->deleteCourse(cid);
                break;
                
            case 3: // 查询课程
                teacher->searchCourses();
                break;
                
            case 4: // 添加答疑记录
                cout << "请输入学生ID: ";
                getline(cin, sid);
                cout << "请输入课程ID: ";
                getline(cin, cid);
                system.addQA(teacher, sid, cid);
                break;
                
            case 5: // 查看评分统计
                teacher->showRatings();
                break;
                
            case 6: // 查看答疑记录
                teacher->displayQARecords();
                break;
                
            case 7: { // 修改密码
                string newPwd;
                cout << "请输入新密码: ";
                getline(cin, newPwd);
                teacher->setPassword(newPwd);
                cout << "密码修改成功!" << endl;
                break;
            }
                
            default:
                cout << "无效选择!" << endl;
        }
    }
}

void studentMenu(Student* student, ManagementSystem& system) {
    while (true) {
        cout << "\n==============================================" << endl;
        cout << "      学生管理菜单 (" << student->getID() << ")       " << endl;
        cout << "==============================================" << endl;
        cout << "1. 选修课程" << endl;
        cout << "2. 退选课程" << endl;
        cout << "3. 查询课程" << endl;
        cout << "4. 查询答疑信息" << endl;
        cout << "5. 修改密码" << endl;
        cout << "6. 返回主菜单" << endl;
        cout << "==============================================" << endl;
        cout << "请选择: ";
        
        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (choice == 6) break;
        
        string cid, tid;
        switch (choice) {
            case 1: // 选修课程
                system.displayAllCourses();
                cout << "请输入要选修的课程ID: ";
                getline(cin, cid);
                student->selectCourse(cid);
                break;
                
            case 2: // 退选课程
                student->searchCourses();
                cout << "请输入要退选的课程ID: ";
                getline(cin, cid);
                student->unselectCourse(cid);
                break;
                
            case 3: // 查询课程
                student->searchCourses();
                break;
                
            case 4: // 查询答疑信息
                student->searchCourses();
                cout << "请输入课程ID: ";
                getline(cin, cid);
                cout << "请输入教师ID: ";
                getline(cin, tid);
                system.rateQA(student, tid, cid);
                break;
                
            case 5: { // 修改密码
                string newPwd;
                cout << "请输入新密码: ";
                getline(cin, newPwd);
                student->setPassword(newPwd);
                cout << "密码修改成功!" << endl;
                break;
            }
   
            default:
                cout << "无效选择!" << endl;
        }
    }
}