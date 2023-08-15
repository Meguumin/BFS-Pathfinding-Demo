#include <iostream>
#include "raylib.h"
#include <vector>
#include <deque>
#include <time.h>
#include <chrono>

struct SuperRectangle
{
    Rectangle rect;
    int state = 0;
    bool visited = false;
    int x = 0;
    int y = 0;
    int parentx = 0;
    int parenty = 0;
    // 0 = empty, 1 equals wall, 2 equals starting and 3 equals end
    // parent cell
};

void starttimer(std::chrono::time_point<std::chrono::steady_clock>& begin, std::chrono::time_point<std::chrono::steady_clock>& end, float& elapsed_secs)
{
    begin = std::chrono::steady_clock::now();
    end = std::chrono::steady_clock::now();
    elapsed_secs = 0.0f;
}

void Setupcell(SuperRectangle& newCell, int x, int y, int i, int j)
{
    newCell.rect.x = float(x);
    newCell.rect.y = float(y);
    newCell.rect.width = 50;
    newCell.rect.height = 50;
    newCell.x = i;
    newCell.y = j;
}

void ChangeCellState(int t, Vector2 mousePoint, std::vector<std::vector<SuperRectangle>>& Cells, std::deque<SuperRectangle>& QueueofCells, bool& startplace, bool& endplace)
{
    for (int i = 0; i < Cells.size(); ++i) {
        for (int j = 0; j < Cells[i].size(); ++j) {
            if (CheckCollisionPointRec(mousePoint, Cells[i][j].rect))
            {
                if (t == 2)
                {
                    QueueofCells.push_back(Cells[i][j]);
                }
                if (Cells[i][j].state == 2)
                {
                    startplace = false;
                    QueueofCells.pop_front();
                }
                if (Cells[i][j].state == 3)
                {
                    endplace = false;
                }
                if (t == 1 && (Cells[i][j].state == 2 || Cells[i][j].state == 3))
                {
                    break;
                }
                Cells[i][j].state = t;
            }
        }
    }
}

void reset(bool& found, bool& startplace, bool& endplace, bool& timerstarted, std::vector<std::vector<SuperRectangle>>& Cells, std::deque<SuperRectangle>& QueueofCells)
{
    found = false;
    startplace = false;
    endplace = false;
    timerstarted = false;
    for (int i = 0; i < Cells.size(); ++i) {
        for (int j = 0; j < Cells[i].size(); ++j) {
            Cells[i][j].state = 0;
            Cells[i][j].visited = false;
        }
    }
    QueueofCells.clear();
}
// Add UI so you can dynamically change the map col and row realtime 
// make it so after 30 col and row it changes cell size and gap based on monitor size
// add extra header to remove functions
// Add emscripten auto build
// make ui sidebar so you can see path, time and stuff
// maze generation 
// 


int main(void)
{
    int mapcol = 0, maprow = 0;
    std::cout << "Enter number of rows: ";
    std::cin >> maprow;
    std::cout << "Enter number of columns: ";
    std::cin >> mapcol;
    int cellsize = 50;
    float gap = 1;
    bool found = false;
    bool startplace = false;
    bool endplace = false;
    bool timerstarted = false;
    const int screenWidth = mapcol * 50 + gap * mapcol;
    const int screenHeight = maprow  * 50 + gap * maprow;
   // if(screenWidth )
    std::deque<SuperRectangle> QueueofCells;
    std::vector<std::vector<SuperRectangle>> Cells; 
    SuperRectangle newcell;
    Vector2 mousePoint = GetMousePosition();
    std::chrono::time_point<std::chrono::steady_clock> begin, end;
    float elapsed_secs = 0.0f;
    for (int i = 0; i < maprow; ++i) 
    {
        std::vector<SuperRectangle> rowCells;
        for (int j = 0; j < mapcol; ++j) 
        { 
            int x = j * (cellsize + float(gap)); 
            int y = i * (cellsize + float(gap));
            Setupcell(newcell, x, y, i, j);
            rowCells.push_back(newcell);
        }
        Cells.push_back(rowCells);
    }
    InitWindow(screenWidth, screenHeight, "Pathfinding");
    SetTargetFPS(0);

    while (WindowShouldClose() == false)
    {
        if (timerstarted) {
            end = std::chrono::steady_clock::now();
            std::chrono::duration<float> elapsed_seconds = end - begin;
            elapsed_secs = elapsed_seconds.count();
        }

        mousePoint = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            ChangeCellState(1, mousePoint, Cells, QueueofCells, startplace, endplace);
        }
        if (IsKeyPressed(KEY_T))
        {
            reset(found, startplace, endplace, timerstarted, Cells, QueueofCells);
        }
        if (IsKeyDown(KEY_E) && !startplace)
        {
            ChangeCellState(2, mousePoint, Cells, QueueofCells, startplace, endplace);
            startplace = true;
        }

        if (IsKeyDown(KEY_R) && !endplace)
        {
            ChangeCellState(3, mousePoint, Cells, QueueofCells, startplace, endplace);
            endplace = true;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            ChangeCellState(0, mousePoint, Cells, QueueofCells, startplace, endplace);
        }

        // put x and y grid accessible in superrectangle make work
        if (IsKeyPressed(KEY_ENTER) && startplace && endplace) {
            starttimer(begin, end, elapsed_secs);
            timerstarted = true;
        }

       if (elapsed_secs > 0.02 && !found && timerstarted)
        {
            SuperRectangle currentCell = QueueofCells.front();
            int x = currentCell.x;
            int y = currentCell.y;
            Cells[x][y].visited = true;

            if (!QueueofCells.empty()) {
                QueueofCells.pop_front();
            }
          

            if (currentCell.state != 3) {
                // Check and push adjacent cells if they are not walls (state != 1) and not visited yet.
                if (x + 1 < maprow && !Cells[x + 1][y].visited && Cells[x + 1][y].state != 1) {
                    QueueofCells.push_back(Cells[x + 1][y]);
                    Cells[x + 1][y].parentx = x;
                    Cells[x + 1][y].parenty = y;
                    Cells[x + 1][y].visited = true;
                }

                if (x - 1 >= 0 && !Cells[x - 1][y].visited && Cells[x - 1][y].state != 1) {
                    QueueofCells.push_back(Cells[x - 1][y]);
                    Cells[x - 1][y].parentx = x;
                    Cells[x - 1][y].parenty = y;
                    Cells[x - 1][y].visited = true;
                }

                if (y + 1 < mapcol && !Cells[x][y + 1].visited && Cells[x][y + 1].state != 1) {
                    QueueofCells.push_back(Cells[x][y + 1]);
                    Cells[x][y + 1].parentx = x;
                    Cells[x][y + 1].parenty = y;
                    Cells[x][y + 1].visited = true;
                }

                if (y - 1 >= 0 && !Cells[x][y - 1].visited && Cells[x][y - 1].state != 1) {
                    QueueofCells.push_back(Cells[x][y - 1]);
                    Cells[x][y - 1].parentx = x;
                    Cells[x][y - 1].parenty = y;
                    Cells[x][y - 1].visited = true;
                }
            }

            if (currentCell.state == 3) {
                std::cout << "Found!";
                found = true;
                SuperRectangle pathCell = Cells[x][y];
                while (pathCell.state != 2) {
                    int parentX = pathCell.parentx;
                    int parentY = pathCell.parenty;
                    pathCell = Cells[parentX][parentY];
                    if (Cells[parentX][parentY].state != 2) {
                        Cells[parentX][parentY].state = 4;
                    }
                }
            }
            if (QueueofCells.empty()) {
                break;
            }
            begin = std::chrono::steady_clock::now();
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (int i = 0; i < Cells.size(); ++i) {
            for (int j = 0; j < Cells[i].size(); ++j) {
                if (Cells[i][j].state == 1) {
                    DrawRectangleRec(Cells[i][j].rect, BLUE);
                }
                else if (Cells[i][j].state == 0 && !Cells[i][j].visited) {
                    DrawRectangleRec(Cells[i][j].rect, GREEN);
                }
                else if (Cells[i][j].state == 2) {
                    DrawRectangleRec(Cells[i][j].rect, RED);
                }
                else if (Cells[i][j].state == 3) {
                    DrawRectangleRec(Cells[i][j].rect, BLACK);
                }
                else if (Cells[i][j].visited && Cells[i][j].state != 3 && Cells[i][j].state != 2 && Cells[i][j].state != 4) {
                    DrawRectangleRec(Cells[i][j].rect, ORANGE);
                }
                else if (Cells[i][j].state == 4) {
                    DrawRectangleRec(Cells[i][j].rect, YELLOW);
                }
            }
        }
        EndDrawing();
    }

    CloseWindow(); // Close window and OpenGL context

    return 0;
}
