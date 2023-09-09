#include <conio.h>
#include <iostream>
#include <windows.h>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <string>
#include <sstream>
#include <cstdlib>
#include <unordered_map>

using namespace std;

string name;
bool dot;
bool pacman;
bool ghost;
vector<vector<int>> walls = {};
vector<vector<int>> dots = {};
vector<vector<int>> spaces = {};
vector<vector<int>> dots_spaces = {};
vector<vector<int>> spawn = {{0, 0},{0, 0}};

struct Record { //structure for leaderboard
    string name;
    string map;
    string mode;
    double value;
};

void ShowConsoleCursor(bool showFlag);
void goto_xy(short x, short y); //shift cursor to position xy
int point(int no_choice, int menu, int map); //selecting options
void print_point(short x_old, short y_old, short x, short y); //print selector <
int input_point(); //input for selecting option
void menu();
int select_map();
int select_difficulty();
void show_leaderboard();
bool compareRecords(Record a, Record b); //arrange for leaderboard
void game(string name, int map, int difficulty); //pacman game mechanism
int move_input(); //input for pacman movement
int design_input(); //input for map design
vector<vector<int>> print_temp(); //print template for map design
void design_map();
string read_map();
void save_map();
void game_instruction();
void design_instruction();
void print_map(int map);
bool hit_wall(short x, short y, int ch, vector<vector<int>> array, vector<vector<int>> move); //check if pacman hits wall
void print_X(short x_old, short y_old, short x, short y); //print pacman
void print_G(short x_old, short y_old, short x, short y, vector<vector<int>> dots); //print ghost
int checkhit(short x, short y, vector<vector<int>> array, vector<vector<int>> move); //check key hit by user
unordered_map<int, vector<int>> buildNeighborMap(vector<vector<int>> nodes);
vector<int> bfs(int s, int e, unordered_map<int, vector<int>> neighbor); //find shortest path
vector<int> find_path(int s, int e, vector<int> nodes); //reconstructing shortest path
int find_index(int x, int y, vector<vector<int>> nodes); //find index of element in vector

int main(){
    system("cls");
    cout << "Enter name: ";
    cin >> name;
    menu();
    return 0;
}

void menu(){
    system("cls");
    ShowConsoleCursor(false);
    int choice;
    int map;
    int difficulty;
    cout <<
            "                      #################\n"
            "                   #######################\n"
            "                 #############################\n" 
            "               #################   ##############\n"
            "             #####################################\n"
            "           ########################################     ----------------------\n"         
            "          ################################                     Main menu\n"
            "         #########################                      ----------------------\n"              
            "        ########################                           1 ) New game\n"                      
            "        #######################                            2 ) Design Map\n"
            "        ########################                           3 ) Leaderboard\n"
            "         #########################                         4 ) Exit\n"
            "          ################################\n"
            "           ########################################\n"
            "            ######################################\n"
            "              ###################################\n"
            "                ##############################\n"
            "                  ########################\n"
            "                     ##################\n";
    choice = point(4, 1, 0);
    if (choice == 1) {
        map = select_map();
        difficulty = select_difficulty();
        game(name, map, difficulty);
    }
    else if (choice == 2) {
        ShowConsoleCursor(true);
        design_map();
    }
    else if (choice == 3) {
        show_leaderboard();
    }
    else if (choice == 4)
        goto_xy(0, 19);
        exit(0);
}

int point(int no_choice, int menu, int map){
    short x;
    short y;
    short ch;
    if (menu == 1){
        x = 75;
        y = 8;
        goto_xy(x, y);
        cout << "<";
    }
    else{
        x = 20;
        y = 1;
        goto_xy(x, y);
        cout << "<";
    }
    if (map == 1){
        goto_xy(0, no_choice + 5);
        print_map(y);
    }
    while (true){
        ch = input_point();
        if (ch == 13){
            if (menu == 1)
                return y - 7;
            else
                return y;
        }
        else if (menu == 1 && ((y - 7 + ch) > no_choice || (y - 7 + ch) < 1 || ch == 0))
            continue;
        else if (menu != 1 && ((y + ch) > no_choice || (y + ch) < 1 || ch == 0))
            continue;
        else{
            print_point(x, y, x, y + ch);
            y+=ch;
            if (map == 1){
                goto_xy(0, no_choice + 5);
                print_map(y);
            }
        }
    }
}

void print_point(short x_old, short y_old, short x, short y){
    goto_xy(x_old, y_old);
    cout << " ";
    goto_xy(x, y);
    cout << "<";
}

int input_point(){
    int ch = _getch();
    if (ch == 0 || ch == 224) { 
        ch = _getch();
        if (ch == 72)
            return -1;
        else if (ch == 80)
            return 1;
        else
            return 0;
    }
    else if (ch == 13)
        return ch;
    else
        return 0;
}

int select_map(){
    system("cls");
    ifstream inputFile("maplist.txt");
    string line;
    int map_count = 0;
    int choice;
    cout << "Maps" << endl;
    while (getline(inputFile, line)){
        map_count+=1;
        cout << line << endl;
    }
    inputFile.close();
    choice = point(map_count, 0, 1);
    return choice;
}

void ShowConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag;
    SetConsoleCursorInfo(out, &cursorInfo);
}

void goto_xy(short x, short y)
{
    COORD coord = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int select_difficulty(){
    system("cls");
    int choice;
    cout << "--------------"
            "  Difficulty  "
            "--------------"
            "\n1 - Easy \n2 - Medium \n3 - Hard" << endl;
    choice = point(3, 0, 0);
    return choice;
}

void show_leaderboard(){
    system("cls");
    ifstream inputFile("records.txt");
    vector<Record> records;
    string line;

    while (getline(inputFile, line)) {
        Record record;
        istringstream(line) >> record.name >> record.map >> record.mode >> record.value;
        records.push_back(record);
    }

    inputFile.close();
    sort(records.begin(), records.end(), compareRecords);
    cout << "Name\t" << "Map\t" << "Mode\t" << "Time" << endl;
    for (int i = 0; i < records.size(); i++){
        cout << records[i].name << "\t" << records[i].map 
        << "\t" << records[i].mode << "\t" << records[i].value << "s" << endl;
    }
    cout << "\nPress any key to return to menu." << endl;
    if (_getch())
        menu();
}

bool compareRecords(Record a, Record b){
    return a.value < b.value;
}

void game(string name, int map, int difficulty){
    system("cls");
    ofstream out_records("records.txt", ios::app);
    string difficulty_mode;
    if (difficulty == 1){
        difficulty_mode = "easy";
        difficulty = 4;
    }
    else if (difficulty == 2){
        difficulty_mode = "medium";
        difficulty = 3;
    }
    else{
        difficulty_mode = "hard";
        difficulty = 2;
    }
    vector<vector<int>> move = {{-1, +1, 0, 0, 0}, {0, 0, -2, +2, 0}};
    vector<int> path = {};
    string map_name = "map"+ to_string(map);
    unordered_map<int, vector<int>> neighbor;
    neighbor = buildNeighborMap(dots_spaces);
    print_map(map);
    game_instruction();
    int x = spawn[0][0];
    int y = spawn[0][1];
    int x2 = spawn[1][0];
    int y2 = spawn[1][1];
    int x2_old;
    int y2_old;
    int ch;
    while (true){
        ch = move_input();
        if (ch == -1)
            continue;
        else if (ch == 27)
            menu();
        else
            break;
    }
    int mode = 0;
    int points = 0;
    int max_points = dots.size();
    int index;
    auto start_time = chrono::high_resolution_clock::now();
    while (true){
        goto_xy(0, 21);
        cout << "Points: " << points << endl;
        auto end_time = chrono::high_resolution_clock::now();
        auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
        cout << "Time: " << fixed << setprecision(1) << elapsed_time/1000.0 << "s" << endl;
        mode+=1;
        if (hit_wall(x, y, ch, walls, move))
            ch = 5;
        x = x + move[1][ch-1];
        y = y + move[0][ch-1];
        index = find_index(x, y, dots);
        if (index != -1){
           dots.erase(dots.begin() + index);
           points+=1;
        }
        print_X(x - move[1][ch-1], y - move[0][ch-1], x, y);
        if (points == max_points) {
            system("cls");
            goto_xy(0, 0);
            cout << "You win! Your score is recorded in the leaderboard." << endl;
            out_records << name << " " << map_name << " " << difficulty_mode << " " 
            << fixed << setprecision(1) << elapsed_time/1000.0 << endl;
            out_records.close();
            cout << "\nPress any key to return to menu." << endl;
            if (_getch())
                break;
        }
        if (mode % difficulty == 0) {
            path = bfs(find_index(x2, y2, dots_spaces), find_index(x, y, dots_spaces), neighbor);
            x2_old = x2;
            y2_old = y2;
            x2 = dots_spaces[path[1]][0];
            y2 = dots_spaces[path[1]][1];
            print_G(x2_old, y2_old, x2, y2, dots);
        }
        if (x == x2 && y == y2){
            system("cls");
            goto_xy(0, 0);
            cout << "You lose!" << endl;
            cout << "\nPress any key to return to menu." << endl;
            if (_getch())
                break;
        }
        Sleep(200);
        int key = 0;
        key = checkhit(x, y, walls, move);
        if (key == 0)
            continue;
        else{
            ch = key;
            if (ch == 27)
                break;
        }
    }
    menu();
}
  
int move_input(){
    int ch = _getch();
    if (ch == 0 || ch == 224) { 
        ch = _getch();
        if (ch == 72)
            return 1;
        else if (ch == 80)
            return 2;
        else if (ch == 75) 
            return 3;
        else if (ch == 77)
            return 4;   
        else
            return -1;
    }
    else if (ch == 27)
        return ch;
    else
        return -1;
}

bool hit_wall(short x, short y, int ch, vector<vector<int>> array, vector<vector<int>> move){
    for (int i = 0; i < array.size(); i++)
    {
        if (array[i][0] == x + move[1][ch-1] && array[i][1] == y + move[0][ch-1])
            return true;
    }
    return false;
}

void print_X(short x_old, short y_old, short x, short y){
    goto_xy(x_old, y_old);
    cout << " ";
    goto_xy(x, y);
    cout << "X";
}

void print_G(short x_old, short y_old, short x, short y, vector<vector<int>> dots){
    int index;
    index = find_index(x_old, y_old, dots);
    goto_xy(x_old, y_old);
    if (index != -1)
        cout << ".";
    else
        cout << " ";
	goto_xy(x, y);
    cout << "G";
}

int checkhit(short x, short y, vector<vector<int>> array, vector<vector<int>> move){
    if (_kbhit()){
        int ch;
        ch = move_input();
        if (ch == -1)
            return 0;
        else if (ch == 27)
            return ch;
        else if (hit_wall(x, y, ch, array, move))
            return 0;
        else
            return ch;
    }
    return 0;
}

vector<vector<int>> print_temp(){
    vector<vector<int>> outer_wall = {};
    for (int i = 0; i < 20; i++){
        cout << "# ";
        outer_wall.push_back({i*2,0});
    }   
    cout << "\n";
    for (int j = 0; j < 18; j++){
        cout << "# ";
        outer_wall.push_back({0, j+1});
        for (int i = 0; i < 18; i++){
        cout << "_ ";
        }
        cout << "#";
        outer_wall.push_back({38, j+1});
        cout << "\n";
    }
    for (int i = 0; i < 20; i++){
        cout << "# ";
        outer_wall.push_back({i*2, 19});
    }
    return outer_wall;
}

void print_map(int map){
    walls = {};
    dots = {};
    spaces = {};
    dots_spaces = {};
    ifstream inputFile("map" + to_string(map) + ".txt");
    string line;
    while (getline(inputFile, line)){
        cout << line << endl;
    }
    inputFile.close();
    string map_name = "map" + to_string(map);
    ifstream read_map(map_name + ".txt");
    string elements;
    int row = 0;
    while (getline(read_map, elements))
    {
        for (int i = 0; i < 20; i++)
        {
            if (elements[i * 2] == 'X') {
                spawn[0][0] = i * 2;
                spawn[0][1] = row;
                dots_spaces.push_back({ i * 2, row });
            }
            else if (elements[i * 2] == 'G') {
                spawn[1][0] = i * 2;
                spawn[1][1] = row;
                dots_spaces.push_back({ i * 2, row });
            }
            else if (elements[i * 2] == '#')
                walls.push_back({ i * 2, row });
            else if (elements[i * 2] == '.') {
                dots.push_back({ i * 2, row });
                dots_spaces.push_back({ i * 2, row });
            }
            else if (elements[i * 2] == ' ') {
                spaces.push_back({ i * 2, row });
                dots_spaces.push_back({ i * 2, row });
            }
        }
        row += 1;
    }
    read_map.close();
}

void game_instruction() {
    goto_xy(50, 0);
    cout << "Instructions";
    goto_xy(50, 2);
    cout << "- X (Pacman), G (Ghost).";
    goto_xy(50, 4);
    cout << "- Use arrow keys to move.";
    goto_xy(50, 6);
    cout << "- Stay away from G.";
    goto_xy(50, 8);
    cout << "- Finish all '.' to win.";
    goto_xy(50, 10);
    cout << "- Esc to return to menu.";
}

void design_instruction() {
    goto_xy(50, 0);
    cout << "Instructions";
    goto_xy(50, 2);
    cout << "- Use arrow keys to move.";
    goto_xy(50, 4);
    cout << "- Build walls with 'w'.";
    goto_xy(50, 6);
    cout << "- Include one instance of X (Pacman), G (Ghost).";
    goto_xy(50, 8);
    cout << "- Scatter points with '.'";
    goto_xy(50, 10);
    cout << "- Replace symbols with '-'";
    goto_xy(50, 12);
    cout << "- '_' will be replaced with space upon saving.";
    goto_xy(50, 14);
    cout << "- Enter to save.";
    goto_xy(50, 16);
    cout << "- Esc to return to menu.";
    goto_xy(0, 0);
}

void design_map(){
    system("cls");
    design_instruction();
    unordered_map<int, char> characters;
    characters[45] = '_';
    characters[46] = '.';
    characters[119] = '#';
    vector<vector<int>> move = {{-1, +1, 0, 0}, {0, 0, -2, +2}};
    short x = 2;
    short y = 1;
    vector<vector<short>> spawn = {{0,0},{0,0}};
    vector<vector<int>> outer_wall = {};
    outer_wall = print_temp();
    goto_xy(x, y);
    int ch;
    ch = design_input();
    while (ch != 27){
        if (ch == 45 || ch == 46 || ch == 119) {
            if (spawn[0][0] == x && spawn[0][1] == y) {
                spawn[0][0] = 0;
                spawn[0][1] = 0;
            }
            else if (spawn[1][0] == x && spawn[1][1] == y) {
                spawn[1][0] = 0;
                spawn[1][1] = 0;
            }
            cout << characters[ch];
        }
        else if (ch == 88){
            if (spawn[0][0] != 0){
                goto_xy(spawn[0][0], spawn[0][1]);
                cout << "_";
            }
            goto_xy(x, y);
            cout << "X";
            spawn[0][0] = x;
            spawn[0][1] = y;
        }
        else if (ch == 71){
            if (spawn[1][0] != 0){
                goto_xy(spawn[1][0], spawn[1][1]);
                cout << "_";
            }
            goto_xy(x, y);
            cout << "G";
            spawn[1][0] = x;
            spawn[1][1] = y;
        }
        else if (ch == 13){ 
            read_map();
            if (dot == true && pacman == true && ghost == true)
                break;
            goto_xy(0, 21);
            cout << "Error in saving. Please include one instance of X and G and at least one '.'";
            ch = design_input();
            continue;
        }
        else{
            if (hit_wall(x, y, ch, outer_wall, move)){
                ch = design_input();
                continue;
            }
            x = x + move[1][ch - 1];
            y = y + move[0][ch - 1];
        }
        goto_xy(x, y);
        ch = design_input();
    }
    if (ch == 13){
        save_map();
        menu();
    }
    else
        menu();
}

string read_map() {
    dot = false;
    pacman = false;
    ghost = false;
    
    string consoleOutput;
    COORD consoleSize;
    consoleSize.X = 40;
    consoleSize.Y = 20;
    CHAR_INFO consoleBuffer[40 * 20];
    SMALL_RECT consoleWriteArea = { 0, 0, (short)(consoleSize.X - 1), (short)(consoleSize.Y - 1) };

    ReadConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), consoleBuffer, consoleSize, { 0, 0 }, &consoleWriteArea);

    for (int y = 0; y < consoleSize.Y; y++) {
        for (int x = 0; x < consoleSize.X; x++) {
            if (consoleBuffer[y * consoleSize.X + x].Char.AsciiChar == '_') {
                consoleOutput += ' ';
                continue;
            }
            else if (consoleBuffer[y * consoleSize.X + x].Char.AsciiChar == '.')
                dot = true;
            else if (consoleBuffer[y * consoleSize.X + x].Char.AsciiChar == 'X')
                pacman = true;
            else if (consoleBuffer[y * consoleSize.X + x].Char.AsciiChar == 'G')
                ghost = true;
            consoleOutput += consoleBuffer[y * consoleSize.X + x].Char.AsciiChar;
        }
        consoleOutput += "\n";
    }
    return consoleOutput;
}

void save_map(){
    ifstream in_list("maplist.txt");
    ofstream out_list("maplist.txt", ios::app);

    int map_count = 0;
    string line;
    
    while (getline(in_list, line)) {
        map_count++;
    }
    in_list.close();

    string map_name = to_string(map_count + 1) + " - " + "map" + to_string(map_count + 1);
    out_list << map_name << endl;
    out_list.close();

    string consoleOutput = read_map();
    ofstream outputFile("map" + to_string(map_count + 1) + ".txt");
    outputFile << consoleOutput;
    outputFile.close();
}

int design_input(){
    int ch = _getch();
    if (ch == 0 || ch == 224) { 
        ch = _getch();
        if (ch == 72)
            return 1;
        else if (ch == 80)
            return 2;
        else if (ch == 75) 
            return 3;
        else if (ch == 77)
            return 4;
        else
            return design_input();
    }
    else if (ch == 13 || ch == 46 || ch == 119 || ch == 88 || ch == 71 || ch == 45 || ch == 27)
        return ch;
    else
        return design_input();   
}

vector<int> bfs(int s, int e, unordered_map<int, vector<int>> neighbor){
    if (s == e)
        return {s, s};
    int found = false;
    int node;
    vector<int> q = {};
    vector<int> neighbours = {};
    q.push_back(s);
    vector<bool> visited(neighbor.size(), false);
    vector<int> prev(neighbor.size(), 0);
    visited[s] = true;
    while (!found){
        node = q[0];
        q.erase(q.begin());
        neighbours = neighbor[node];
        for (int next : neighbours){
            if (!visited[next]){
                q.push_back(next);
                visited[next] = true;
                prev[next] = node;
                if (next == e){
                    found = true;
                    break;
                }
            }
        }
    }
    return find_path(s, e, prev);
}

unordered_map<int, vector<int>> buildNeighborMap(vector<vector<int>> nodes) {
    unordered_map<int, vector<int>> neighborMap;
    vector<int> dr = { -2, 2, 0, 0 };
    vector<int> dc = { 0, 0, 1, -1 };
    vector<int> nodes2 = {};
    int x, y, index;
    for (int i = 0; i < nodes.size(); i++) {
        for (int j = 0; j < dr.size(); j++) {
            x = nodes[i][0] + dr[j];
            y = nodes[i][1] + dc[j];
            index = find_index(x, y, nodes);
            if (index != -1)
                nodes2.push_back(index);
        }
        neighborMap[i] = nodes2;
        nodes2 = {};
    }
    return neighborMap;
}

vector<int> find_path(int s, int e, vector<int> nodes){
    vector<int> path = {};
    path.push_back(e);
    int i = e;
    while (i != s){
        path.push_back(nodes[i]);
        i = nodes[i];
    }
    reverse(path.begin(), path.end());
    return path;
}

int find_index(int x, int y, vector<vector<int>> nodes){
    for (int i = 0; i < nodes.size(); i++)
    {
        if (nodes[i][0] == x && nodes[i][1] == y)
            return i;
    }
    return -1;
}