// CursorFinder.cpp : 定义应用程序的入口点。
//
#define OEMRESOURCE
#include <iostream>
#include <thread>
#include <windows.h>
#include <algorithm>

using namespace std;

#define SAMPLE_MAX 1000/50
#define SPEED_MIN 90
#define IGNORE_MAX 1000/50/2

//Lock
volatile bool isSetting = false;
static HCURSOR oldCursor;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
    oldCursor = CopyCursor(LoadCursor(NULL, IDC_ARROW));
    (new thread([] {
        //两点坐标
        POINT* ptold = new POINT(), * ptnew = new POINT();
        //连续符合的样本数
        int count = 0;
        //若存在连续的 则可以忽略两次采样
        int ignoredCount = 2;
        //初始化第一个坐标
        GetCursorPos(ptold);
        while (true) {
            //每100ms采样一次
            Sleep(50);
            GetCursorPos(ptnew);
            auto speed = sqrt((ptnew->x - ptold->x) * (ptnew->x - ptold->x) + (ptnew->y - ptold->y) * (ptnew->y - ptold->y));
            speed > SPEED_MIN ? count++ : ignoredCount < IGNORE_MAX ? ignoredCount++, count++ : ignoredCount = count = 0;
            //cout <<"速度: "<< speed<<"\t连续合格: "<<count<<"\t已忽略: "<<ignoredCount << endl;
            if (count > SAMPLE_MAX)
            {
                cout << "符合条件！" << endl;
                HCURSOR myCursor = LoadCursorFromFileA("cursor.cur");
                SetSystemCursor(CopyCursor(myCursor), OCR_NORMAL);
                (new thread([] {
                    //Only one thread can restore
                    if (isSetting) return;
                    isSetting = true;
                    Sleep(2000);
                    HCURSOR newOldCursor = CopyCursor(oldCursor);
                    SetSystemCursor(oldCursor, OCR_NORMAL);
                    oldCursor = newOldCursor;
                    cout << GetLastError() << endl;
                    isSetting = false;
                    }))->detach();
                    ignoredCount = count = 0;
            }
            //交换缓冲
            swap(ptnew, ptold);
        }
        }))->detach();
        while (true);
        std::cout << "Hello World!\n";
    return 0;
}
