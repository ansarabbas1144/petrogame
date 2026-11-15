#include <iostream>
#include "terminal.h"
using namespace std;

// =====================================================
//                  CAVE & ROCK HELPERS
// =====================================================

void initCave(int caveTop[], int caveBottom[], int rockType[], int rockTop[], int rockCol[],
              int &direction, int &length, bool &isRockStretch)
{
    for (int c = 0; c < 114; c++)
    {
        rockType[c] = -1;
        rockTop[c] = 0;
        rockCol[c] = -1;
    }

    // Column 57 starts at centre 15, gap 20
    caveTop[57] = 5;
    caveBottom[57] = 25;

    // Populate right side (58 to 113)
    direction = randomInt(-1, 1);
    length = randomInt(6, 25);
    isRockStretch = false;

    for (int c = 58; c < 114; c++)
    {
        if (length <= 0)
        {
            direction = randomInt(-1, 1);
            length = randomInt(6, 25);
        }

        int oldTop = caveTop[c - 1];
        int oldBot = caveBottom[c - 1];
        int gap = oldBot - oldTop;
        int centre = oldTop + gap / 2;

        centre += direction;

        int roll = randomInt(0, 99);
        if (roll <= 7) gap++;
        else if (roll <= 15) gap--;

        int minGap = 18;
        if (gap < minGap) gap = minGap;
        if (gap > minGap + 4) gap = minGap + 4;

        int top = centre - gap / 2;
        if (top < 1)
        {
            top = 1;
            direction = 0;
        }
        if (top + gap > 29)
        {
            top = 29 - gap;
            direction = 0;
        }

        caveTop[c] = top;
        caveBottom[c] = top + gap;
        length--;
    }

    // Populate left side (56 down to 0)
    direction = randomInt(-1, 1);
    length = randomInt(6, 25);

    for (int c = 56; c >= 0; c--)
    {
        if (length <= 0)
        {
            direction = randomInt(-1, 1);
            length = randomInt(6, 25);
        }

        int oldTop = caveTop[c + 1];
        int oldBot = caveBottom[c + 1];
        int gap = oldBot - oldTop;
        int centre = oldTop + gap / 2;

        centre += direction;

        int roll = randomInt(0, 99);
        if (roll <= 7) gap++;
        else if (roll <= 15) gap--;

        int minGap = 18;
        if (gap < minGap) gap = minGap;
        if (gap > minGap + 4) gap = minGap + 4;

        int top = centre - gap / 2;
        if (top < 1)
        {
            top = 1;
            direction = 0;
        }
        if (top + gap > 29)
        {
            top = 29 - gap;
            direction = 0;
        }

        caveTop[c] = top;
        caveBottom[c] = top + gap;
        length--;
    }

    // Start fresh stretch when flying begins
    direction = randomInt(-1, 1);
    length = randomInt(6, 25);
    isRockStretch = false;
}

void generateColumn(int newCol, int oldCol, int caveTop[], int caveBottom[],
                    int rockType[], int rockTop[], int rockCol[],
                    int &direction, int &length, bool &isRockStretch,
                    int &stretchRockType, int &stretchRockTop, int &stretchIndex,
                    int distance)
{
    if (length <= 0)
    {
        int level = distance / 250;
        int rockChance = 35 + 5 * level;
        if (rockChance > 70) rockChance = 70;

        if (distance >= 150 && randomInt(0, 99) < rockChance)
        {
            isRockStretch = true;
            direction = 0;
            length = 28;
            stretchIndex = 0;
            stretchRockType = randomInt(0, 1); // 0 = small (3x6), 1 = large (4x8)

            int height = (stretchRockType == 0) ? 3 : 4;
            int top = caveTop[oldCol];
            int bottom = caveBottom[oldCol];

            int maxTop = bottom - 1 - height;
            if (maxTop < top + 1)
                stretchRockTop = top + 1;
            else
                stretchRockTop = randomInt(top + 1, maxTop);

            // Ensure at least 4 free rows above and below if possible
            int freeAbove = stretchRockTop - top;
            int freeBelow = bottom - (stretchRockTop + height);
            if (freeAbove < 4 && freeBelow < 4)
            {
                stretchRockTop = top + 1;
            }
        }
        else
        {
            isRockStretch = false;
            direction = randomInt(-1, 1);
            length = randomInt(6, 25);
            stretchIndex = 0;
        }
    }

    rockType[newCol] = -1;
    rockTop[newCol] = 0;
    rockCol[newCol] = -1;

    if (isRockStretch)
    {
        caveTop[newCol] = caveTop[oldCol];
        caveBottom[newCol] = caveBottom[oldCol];

        int width = (stretchRockType == 0) ? 6 : 8;
        int startOffset = (28 - width) / 2;

        if (stretchIndex >= startOffset && stretchIndex < startOffset + width)
        {
            rockType[newCol] = stretchRockType;
            rockTop[newCol] = stretchRockTop;
            // Store relative column inside sprite (0 .. width-1)
            // When flying right, stretchIndex increases left-to-right
            // When flying left, stretchIndex increases right-to-left
            if (newCol == 113)
                rockCol[newCol] = stretchIndex - startOffset;
            else
                rockCol[newCol] = (width - 1) - (stretchIndex - startOffset);
        }

        stretchIndex++;
        length--;
    }
    else
    {
        int oldTop = caveTop[oldCol];
        int oldBot = caveBottom[oldCol];
        int gap = oldBot - oldTop;
        int centre = oldTop + gap / 2;

        centre += direction;

        int roll = randomInt(0, 99);
        if (roll <= 7) gap++;
        else if (roll <= 15) gap--;

        int minGap = 18 - distance / 250;
        if (minGap < 9) minGap = 9;

        if (gap < minGap) gap = minGap;
        if (gap > minGap + 4) gap = minGap + 4;

        int top = centre - gap / 2;
        if (top < 1)
        {
            top = 1;
            direction = 0;
        }
        if (top + gap > 29)
        {
            top = 29 - gap;
            direction = 0;
        }

        caveTop[newCol] = top;
        caveBottom[newCol] = top + gap;
        length--;
    }
}

void scrollCave(int birdDir, int caveTop[], int caveBottom[],
                int rockType[], int rockTop[], int rockCol[],
                int &direction, int &length, bool &isRockStretch,
                int &stretchRockType, int &stretchRockTop, int &stretchIndex,
                int distance)
{
    if (birdDir == 1) // Flying right: shift left, generate col 113 from 112
    {
        for (int c = 0; c < 113; c++)
        {
            caveTop[c] = caveTop[c + 1];
            caveBottom[c] = caveBottom[c + 1];
            rockType[c] = rockType[c + 1];
            rockTop[c] = rockTop[c + 1];
            rockCol[c] = rockCol[c + 1];
        }
        generateColumn(113, 112, caveTop, caveBottom, rockType, rockTop, rockCol,
                       direction, length, isRockStretch, stretchRockType,
                       stretchRockTop, stretchIndex, distance);
    }
    else // Flying left: shift right, generate col 0 from 1
    {
        for (int c = 113; c > 0; c--)
        {
            caveTop[c] = caveTop[c - 1];
            caveBottom[c] = caveBottom[c - 1];
            rockType[c] = rockType[c - 1];
            rockTop[c] = rockTop[c - 1];
            rockCol[c] = rockCol[c - 1];
        }
        generateColumn(0, 1, caveTop, caveBottom, rockType, rockTop, rockCol,
                       direction, length, isRockStretch, stretchRockType,
                       stretchRockTop, stretchIndex, distance);
    }
}

// =====================================================
//               SPRITE, COLLISION & RENDER
// =====================================================

char getRockChar(int type, int rOffset, int cOffset)
{
    if (type == 0) // Small 3x6
    {
        const char smallRock[3][7] = {
            " (##) ",
            "(####)",
            " (##) "
        };
        return smallRock[rOffset][cOffset];
    }
    else // Large 4x8
    {
        const char largeRock[4][9] = {
            " _/##\\_ ",
            "(######)",
            "(######)",
            " \\_##_/ "
        };
        return largeRock[rOffset][cOffset];
    }
}

char getBirdChar(int birdDir, int wingState, int rOffset, int cOffset)
{
    if (birdDir == 1) // Facing right (2 rows x 9 cols)
    {
        if (wingState == 0)
        {
            const char sprite[2][10] = {
                "   |\\    ",
                "~==|_\\o=>"
            };
            return sprite[rOffset][cOffset];
        }
        else
        {
            const char sprite[2][10] = {
                "~==|-/o=>",
                "   |/    "
            };
            return sprite[rOffset][cOffset];
        }
    }
    else // Facing left (2 rows x 9 cols)
    {
        if (wingState == 0)
        {
            const char sprite[2][10] = {
                "    /|   ",
                "<=o/_|==~"
            };
            return sprite[rOffset][cOffset];
        }
        else
        {
            const char sprite[2][10] = {
                "<=o\\-|==~",
                "    \\|   "
            };
            return sprite[rOffset][cOffset];
        }
    }
}

bool checkCollision(int caveTop[], int caveBottom[],
                    int rockType[], int rockTop[], int rockCol[],
                    int birdRow, int birdDir, int wingState)
{
    int birdCol = 53;
    for (int r = 0; r < 2; r++)
    {
        for (int c = 0; c < 9; c++)
        {
            char ch = getBirdChar(birdDir, wingState, r, c);
            if (ch == ' ')
                continue;

            int screenR = birdRow + r;
            int screenC = birdCol + c;

            if (screenR < 0 || screenR >= 30 || screenC < 0 || screenC >= 114)
                return true;

            // Check cave walls
            if (screenR < caveTop[screenC] || screenR >= caveBottom[screenC])
                return true;

            // Check floating rock
            if (rockType[screenC] != -1)
            {
                int rHeight = (rockType[screenC] == 0) ? 3 : 4;
                if (screenR >= rockTop[screenC] && screenR < rockTop[screenC] + rHeight)
                {
                    char rCh = getRockChar(rockType[screenC],
                                           screenR - rockTop[screenC],
                                           rockCol[screenC]);
                    if (rCh != ' ')
                        return true;
                }
            }
        }
    }
    return false;
}

void printFiveDigits(int val)
{
    if (val > 99999) val = 99999;
    int d1 = (val / 10000) % 10;
    int d2 = (val / 1000) % 10;
    int d3 = (val / 100) % 10;
    int d4 = (val / 10) % 10;
    int d5 = val % 10;
    cout << d1 << d2 << d3 << d4 << d5;
}

void drawScreen(int caveTop[], int caveBottom[],
                int rockType[], int rockTop[], int rockCol[],
                int birdRow, int birdDir, int wingState,
                int distance, int best, int waitTime, bool gameOver)
{
    char grid[30][114];

    // Fill cave and empty space
    for (int r = 0; r < 30; r++)
    {
        for (int c = 0; c < 114; c++)
        {
            if (r < caveTop[c] || r >= caveBottom[c])
                grid[r][c] = '#';
            else
                grid[r][c] = ' ';
        }
    }

    // Draw floating rocks
    for (int c = 0; c < 114; c++)
    {
        if (rockType[c] != -1)
        {
            int h = (rockType[c] == 0) ? 3 : 4;
            for (int r = 0; r < h; r++)
            {
                int sr = rockTop[c] + r;
                if (sr >= 1 && sr < 29)
                {
                    char rCh = getRockChar(rockType[c], r, rockCol[c]);
                    if (rCh != ' ')
                        grid[sr][c] = rCh;
                }
            }
        }
    }

    // Draw bird sprite at col 53
    for (int r = 0; r < 2; r++)
    {
        for (int c = 0; c < 9; c++)
        {
            char ch = getBirdChar(birdDir, wingState, r, c);
            int sr = birdRow + r;
            int sc = 53 + c;
            if (ch != ' ' && sr >= 0 && sr < 30 && sc >= 0 && sc < 114)
            {
                grid[sr][sc] = gameOver ? 'X' : ch;
            }
        }
    }

    // Overlay Game Over box if crashed
    if (gameOver)
    {
        const char box[5][33] = {
            "+------------------------------+",
            "|          GAME OVER!          |",
            "|   Press R / Space to Retry   |",
            "|        Press Q to Quit       |",
            "+------------------------------+"
        };
        int startR = 12;
        int startC = 41;
        for (int r = 0; r < 5; r++)
            for (int c = 0; c < 32; c++)
                grid[startR + r][startC + c] = box[r][c];
    }

    cursorHome();

    // Top border (116 chars)
    cout << "+";
    for (int c = 0; c < 114; c++) cout << "-";
    cout << "+\n";

    // Playfield (30 rows)
    for (int r = 0; r < 30; r++)
    {
        cout << "|";
        for (int c = 0; c < 114; c++)
            cout << grid[r][c];
        cout << "|\n";
    }

    // Bottom border (116 chars)
    cout << "+";
    for (int c = 0; c < 114; c++) cout << "-";
    cout << "+\n";

    // HUD (Line 33, 116 chars)
    int speedBars = (70 - waitTime) / 4;
    if (speedBars < 0) speedBars = 0;
    if (speedBars > 10) speedBars = 10;

    cout << " DIST: ";
    printFiveDigits(distance);
    cout << "   BEST: ";
    printFiveDigits(best);
    cout << "   SPEED: [";
    for (int i = 0; i < 10; i++)
        cout << (i < speedBars ? '>' : '-');
    cout << "]   FLYING: " << (birdDir == 1 ? ">>" : "<<")
         << "   [W/S: Move | A/D: Turn | Q: Quit]               ";
    cout.flush();
}

// =====================================================
//                        MAIN
// =====================================================
int main()
{
    termInit();

    int caveTop[114];
    int caveBottom[114];
    int rockType[114];
    int rockTop[114];
    int rockCol[114];

    int best = 0;
    bool running = true;

    while (running)
    {
        int birdRow = 14;
        int birdDir = 1;
        int distance = 0;
        int frame = 0;
        bool gameOver = false;

        int direction = 0;
        int length = 0;
        bool isRockStretch = false;
        int stretchRockType = 0;
        int stretchRockTop = 0;
        int stretchIndex = 0;

        initCave(caveTop, caveBottom, rockType, rockTop, rockCol,
                 direction, length, isRockStretch);

        while (!gameOver && running)
        {
            while (keyPressed())
            {
                int key = readKey();
                if (key == 'q')
                {
                    running = false;
                    break;
                }
                else if (key == 'w' || key == KEY_ARROW_UP)
                {
                    if (birdRow > 1) birdRow--;
                }
                else if (key == 's' || key == KEY_ARROW_DOWN)
                {
                    if (birdRow < 27) birdRow++;
                }
                else if (key == 'a' || key == KEY_ARROW_LEFT)
                {
                    if (birdDir != -1)
                    {
                        birdDir = -1;
                        isRockStretch = false;
                        direction = 0;
                        length = 10;
                    }
                }
                else if (key == 'd' || key == KEY_ARROW_RIGHT)
                {
                    if (birdDir != 1)
                    {
                        birdDir = 1;
                        isRockStretch = false;
                        direction = 0;
                        length = 10;
                    }
                }
            }

            if (!running) break;

            scrollCave(birdDir, caveTop, caveBottom, rockType, rockTop, rockCol,
                       direction, length, isRockStretch, stretchRockType,
                       stretchRockTop, stretchIndex, distance);

            distance++;
            if (distance > best) best = distance;

            int wingState = (frame / 3) % 2;

            if (checkCollision(caveTop, caveBottom, rockType, rockTop, rockCol,
                               birdRow, birdDir, wingState))
            {
                gameOver = true;
                flushKeys();
            }

            int waitTime = 70 - (distance / 40);
            if (waitTime < 30) waitTime = 30;

            drawScreen(caveTop, caveBottom, rockType, rockTop, rockCol,
                       birdRow, birdDir, wingState, distance, best, waitTime, gameOver);

            if (!gameOver)
            {
                sleepMs(waitTime);
                frame++;
            }
        }

        // Game Over wait loop
        while (gameOver && running)
        {
            if (keyPressed())
            {
                int key = readKey();
                if (key == 'q')
                {
                    running = false;
                }
                else if (key == 'r' || key == ' ')
                {
                    gameOver = false; // Restarts outer loop
                }
            }
            sleepMs(30);
        }
    }

    termRestore();
    return 0;
}
