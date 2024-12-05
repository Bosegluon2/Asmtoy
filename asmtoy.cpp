#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <stack>
#include <stdexcept>
#include <vector>
#include <windows.h>
using namespace std;
unordered_map<string, int> numVars;          // 存储num变量
unordered_map<string, string> strVars;       // 存储str变量
unordered_map<string, int> labels;           // 存储label和其行号
vector<string> instructions;                 // 存储所有指令
unordered_map<string, vector<int>> procBody; // 记录每个函数的内容行号
stack<int> callStack;                        // 用来存储函数调用的返回地址
int re = -100;
string processEscapeSequences(const string &input)
{
    stringstream result;
    size_t i = 0;
    while (i < input.length())
    {
        if (input[i] == '\\' && i + 1 < input.length())
        {
            // 检查转义字符
            switch (input[i + 1])
            {
            case 'n':
                result << '\n';
                break;
            case 't':
                result << '\t';
                break;
            case '\\':
                result << '\\';
                break;
            case '"':
                result << '"';
                break;
            default:
                result << input[i]; // 如果不是有效转义字符，原样输出
            }
            i += 2; // 跳过转义字符
        }
        else
        {
            result << input[i];
            i++;
        }
    }
    return result.str();
}
int executeInstruction(int &pointer)
{
    istringstream iss(instructions[pointer]);
    string command;
    iss >> command;
    try
    {
        string lineToExecute = instructions[pointer];
        if (lineToExecute[0] == '|')
        {
            lineToExecute.erase(0, 1); // 去掉开头的 | 符号
            iss.clear();
            iss.str(lineToExecute);
            iss >> command;
        }
        if (command == "show")
        {
            string value;
            iss >> ws;
            getline(iss, value);
            if (value.front() == '"' && value.back() == '"')
            {
                value = value.substr(1, value.size() - 2);
                value = processEscapeSequences(value);
                cout << value;
            }
            else if (numVars.count(value))
            {
                cout << numVars[value];
            }
            else if (strVars.count(value))
            {
                cout << strVars[value];
            }
            else
            {
                throw runtime_error("Invalid variable in show: " + value);
            }
            pointer++;
        }
        else if (command == "num")
        {
            string varName;
            iss >> varName;
            numVars[varName] = 0;
            pointer++;
        }
        else if (command == "assign")
        {
            string varName, value;
            iss >> varName >> value;
            if (numVars.count(varName))
            {
                if (isdigit(value[0]) || (value[0] == '-' && isdigit(value[1])))
                {
                    try
                    {
                        numVars[varName] = stoi(value);
                    }
                    catch (const std::exception &e)
                    {
                        throw runtime_error("Invalid integer value in assign: " + value);
                    }
                }
                else if (numVars.count(value))
                {
                    numVars[varName] = numVars[value];
                }
                else
                {
                    throw runtime_error("Invalid value for assign: " + value);
                }
            }
            else
            {
                throw runtime_error("Variable not declared: " + varName);
            }
            pointer++;
        }
        else if (command == "inputnum")
        {
            string varName;
            iss >> varName;
            if (numVars.count(varName))
            {
                cin >> numVars[varName];
            }
            else
            {
                throw runtime_error("Variable not declared: " + varName);
            }
            pointer++;
        }
        else if (command == "str")
        {
            string varName;
            iss >> varName;
            strVars[varName] = "";
            pointer++;
        }
        else if (command == "cpystr")
        {
            string dest, src;
            iss >> dest >> ws;
            getline(iss, src);
            if (src.front() == '"' && src.back() == '"')
            {
                src = src.substr(1, src.size() - 2);
                strVars[dest] = src;
            }
            else if (strVars.count(src))
            {
                strVars[dest] = strVars[src];
            }
            else
            {
                throw runtime_error("Invalid value for cpystr: " + src);
            }
            pointer++;
        }
        else if (command == "goto")
        {
            string label;
            iss >> ws;
            getline(iss, label);

            if (label.front() == '"' && label.back() == '"')
            {
                label = label.substr(1, label.size() - 2); // 去掉双引号
            }

            // 查找标签对应的行号
            if (labels.count(label))
            {
                pointer = labels[label]; // 跳转到标签所在的行号
            }
            else
            {
                throw runtime_error("Label not found: " + label);
            }
        }
        else if (command == "label")
        {
            pointer++;
        }
        else if (command == "if")
        {
            string first, second;
            iss >> first >> second;

            // 判断第一个参数是否是一个变量（即存在于 numVars 中）
            bool isFirstNumVar = numVars.count(first);
            bool isSecondNumVar = numVars.count(second);

            // 获取第一个和第二个值，分别是变量的值或直接的整数值
            int firstValue = isFirstNumVar ? numVars[first] : stoi(first);
            int secondValue = isSecondNumVar ? numVars[second] : stoi(second);

            // 比较两个值
            if (firstValue == secondValue)
            {
                pointer++;
            }
            else
            {
                pointer += 2;
                return 3;
            }
        }
        else if (command == "add")
        {
            string varName, operand1, operand2;
            iss >> varName >> operand1 >> operand2;

            // 检查a是否是一个已经声明的num类型变量
            if (numVars.count(varName) == 0)
            {
                throw runtime_error("Variable not declared: " + varName);
            }

            // 处理第一个操作数b
            int value1;
            if (numVars.count(operand1))
            {
                value1 = numVars[operand1]; // 如果operand1是变量，获取它的值
            }
            else
            {
                value1 = stoi(operand1); // 如果operand1是常量，转换为整数
            }

            // 处理第二个操作数c
            int value2;
            if (numVars.count(operand2))
            {
                value2 = numVars[operand2]; // 如果operand2是变量，获取它的值
            }
            else
            {
                value2 = stoi(operand2); // 如果operand2是常量，转换为整数
            }

            // 执行加法并赋值给a
            numVars[varName] = value1 + value2;

            pointer++;
        }
        else if (command == "sub")
        {
            string varName, operand1, operand2;
            iss >> varName >> operand1 >> operand2;

            // 检查a是否是一个已经声明的num类型变量
            if (numVars.count(varName) == 0)
            {
                throw runtime_error("Variable not declared: " + varName);
            }

            // 处理第一个操作数b
            int value1;
            if (numVars.count(operand1))
            {
                value1 = numVars[operand1]; // 如果operand1是变量，获取它的值
            }
            else
            {
                value1 = stoi(operand1); // 如果operand1是常量，转换为整数
            }

            // 处理第二个操作数c
            int value2;
            if (numVars.count(operand2))
            {
                value2 = numVars[operand2]; // 如果operand2是变量，获取它的值
            }
            else
            {
                value2 = stoi(operand2); // 如果operand2是常量，转换为整数
            }

            // 执行加法并赋值给a
            numVars[varName] = value1 - value2;

            pointer++;
        }
        else if (command == "mul")
        {
            string varName, operand1, operand2;
            iss >> varName >> operand1 >> operand2;

            // 检查a是否是一个已经声明的num类型变量
            if (numVars.count(varName) == 0)
            {
                throw runtime_error("Variable not declared: " + varName);
            }

            // 处理第一个操作数b
            int value1;
            if (numVars.count(operand1))
            {
                value1 = numVars[operand1]; // 如果operand1是变量，获取它的值
            }
            else
            {
                value1 = stoi(operand1); // 如果operand1是常量，转换为整数
            }

            // 处理第二个操作数c
            int value2;
            if (numVars.count(operand2))
            {
                value2 = numVars[operand2]; // 如果operand2是变量，获取它的值
            }
            else
            {
                value2 = stoi(operand2); // 如果operand2是常量，转换为整数
            }

            // 执行加法并赋值给a
            numVars[varName] = value1 * value2;

            pointer++;
        }
        else if (command == "div")
        {
            string varName, operand1, operand2;
            iss >> varName >> operand1 >> operand2;

            // 检查a是否是一个已经声明的num类型变量
            if (numVars.count(varName) == 0)
            {
                throw runtime_error("Variable not declared: " + varName);
            }

            // 处理第一个操作数b
            int value1;
            if (numVars.count(operand1))
            {
                value1 = numVars[operand1]; // 如果operand1是变量，获取它的值
            }
            else
            {
                value1 = stoi(operand1); // 如果operand1是常量，转换为整数
            }

            // 处理第二个操作数c
            int value2;
            if (numVars.count(operand2))
            {
                value2 = numVars[operand2]; // 如果operand2是变量，获取它的值
            }
            else
            {
                value2 = stoi(operand2); // 如果operand2是常量，转换为整数
            }

            // 执行加法并赋值给a
            numVars[varName] = value1 / value2;

            pointer++;
        }
        else if (command == "call")
        {
            string procName;
            iss >> ws;
            getline(iss, procName); // 获取函数名

            // 检查函数是否已定义
            if (procBody.find(procName) == procBody.end())
            {
                throw runtime_error("Error: Function " + procName + " not defined.");
            }

            // 在调用函数之前，保存当前行（即调用指令的下一行）到栈中
            callStack.push(pointer + 1);

            // 执行函数体内容

            for (int line : procBody[procName])
            {
                if (re == 3)
                {
                    line += 2;
                }
                re = executeInstruction(line); // 逐行执行函数内容
                // cout<<"re:"<<re<<endl;
                if (re == 2)
                {
                    break;
                }
            }
            pointer++;
        }
        else if (command == "proc")
        {
            string procName;
            iss >> ws;
            getline(iss, procName); // 获取函数名
            int lastElement = procBody[procName].back();
            pointer = lastElement + 1;
        }
        else if (command == "return")
        {
            pointer = callStack.top();
            // cout<<pointer;
            return 2;
            callStack.pop();
        }
        else
        {
            pointer++;
            throw runtime_error("Unknown command: " + command);
        }
    }
    catch (const exception &e)
    {
        // 捕获并输出错误信息，包括行号和出错的指令
        cerr << "Error in line " << pointer + 1 << ": " << instructions[pointer] << endl;
        cerr << "Exception: " << e.what() << endl;
        throw; // 重新抛出异常，以便程序可以停止或进一步处理
    }
    return 0;
}

void loadInstructions(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        throw runtime_error("Failed to open file: " + filename);
    }
    string line;
    int lineNumber = 0;
    string currentProc = "";
    while (getline(file, line))
    {
        // 去掉首尾的空白字符
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        // 跳过空行或注释行
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        instructions.push_back(line);

        istringstream iss(line);
        string command;
        iss >> command;

        // 处理label指令
        if (command == "label")
        {
            string label;
            iss >> ws;
            getline(iss, label);
            if (label.front() == '"' && label.back() == '"')
            {
                label = label.substr(1, label.size() - 2); // 去掉双引号
                labels[label] = lineNumber;                // 将标签名和行号存储起来
            }
            else
            {
                throw runtime_error("Invalid label format: " + label);
            }
        }
        // 处理 proc 定义
        if (line.find("proc") == 0)
        {
            currentProc = line.substr(5); // 获取函数名
            procBody[currentProc] = {};   // 初始化函数内容
        }
        // 如果是函数体内容（以 | 开头）
        else if (line[0] == '|')
        {

            procBody[currentProc].push_back(lineNumber);
        }
        // 如果遇到非函数体内容，表示函数定义完成
        else if (!currentProc.empty())
        {
            currentProc = ""; // 重置当前函数
        }

        lineNumber++;
    }
}

int main(int argc, char* argv[]) {
    // 检查是否提供了文件名
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif // _WIN32
    if (argc < 2) {
        std::cerr << "用法: " << argv[0] << " <文件名>" << std::endl;
        return 1;
    }
    try
    {
        loadInstructions(argv[1]);
        int pointer = 0;
        while (pointer < instructions.size())
        {
            executeInstruction(pointer);
        }
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
    }
    return 0;
}
