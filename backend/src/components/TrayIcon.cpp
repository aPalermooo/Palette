/***********************************
 *  Name:           TrayIcon.cpp
 *  Description:    Creates an Icon in the tray that can manage the state of the application without GUI open
 *  Author(s):      Xander Palermo <ajp2s@missouristate.edu>
 *  Date:           April 2026
 *
 * Course:          CSC450
 ***********************************/

#include "TrayIcon.h"

#include <windows.h>
#include <shellapi.h>
#include <conio.h>

/*******************************
 *
 *  PRIVATE FUNCTIONS
 *
 ******************************/


void TrayIcon::registerWindow() const {

    WNDCLASSW wndclass = {};

    wndclass.lpfnWndProc    = WndProc;
    wndclass.hInstance      = m_hInst;
    wndclass.lpszClassName  = APP_NAME;

    RegisterClassW(&wndclass);
}

int TrayIcon::createWindow() {
    m_hWnd = CreateWindowW(APP_NAME, APP_NAME, WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, m_hInst, this);
    if (!m_hWnd) return -1;
    return 0;
}

void TrayIcon::addIcon() {
    if (!m_hWnd) return; // No window to attach to

    m_nid.cbSize              = sizeof(m_nid);
    m_nid.hWnd                = m_hWnd;
    m_nid.uID                 = ID_TRAY;
    m_nid.uFlags              = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    m_nid.uCallbackMessage    = WM_TRAY;
    m_nid.hIcon               = LoadIcon(nullptr, IDI_APPLICATION);

    wcsncpy_s(m_nid.szTip, APP_NAME, _TRUNCATE);
    Shell_NotifyIconW(NIM_ADD, &m_nid);
}

void TrayIcon::showContextMenu() const {
    if (!m_hWnd) return;

    HMENU hMenu = CreatePopupMenu();

    // Context Menu Structure
    AppendMenuW(hMenu, MF_STRING, IDM_OPEN, L"Open Palette");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, IDM_EXIT, L"Exit Palette");

    // Get mouse location
    SetForegroundWindow(m_hWnd);
    POINT point;
    GetCursorPos(&point);

    // Style
    MENUINFO menuInfo   = {};
    menuInfo.cbSize     = sizeof(menuInfo);
    menuInfo.fMask      = MIM_STYLE;
    menuInfo.dwStyle    = MNS_AUTODISMISS;
    SetMenuInfo(hMenu, &menuInfo);

    // Listen for events
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON | TPM_BOTTOMALIGN, point.x, point.y, 0, m_hWnd, nullptr);
    DestroyMenu(hMenu);
}

void TrayIcon::processMessage() {
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void TrayIcon::removeIcon() {
    if (!m_hWnd) return;
    Shell_NotifyIconW(NIM_DELETE, &m_nid);
}

void TrayIcon::destroyWindow() {
    if (!m_hWnd) return;
    DestroyWindow(m_hWnd);
    m_hWnd = nullptr;
}

/*******************************
 *
 *  PUBLIC FUNCTIONS
 *
 ******************************/

TrayIcon::~TrayIcon() {
    removeIcon();
    destroyWindow();
}

void TrayIcon::run() {
    registerWindow();
    if (createWindow() == -1) return ;
    addIcon();

    processMessage();
}
