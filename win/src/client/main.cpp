﻿// basic.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "main.h"
#include "gui/login/login_form.h"
#include "gui/home/home_form.h"
#include "gui/login/login_setting_form.h"
#include "cim/cim.h"

HINSTANCE g_instance_;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // init chatkit
    cim::ChatKitConfig config;

    if (cim::initChatKit(config) != cim::Result::kSuccess) {
        LogWarn("init chatkit failed.");
        return -1;
    }

    cim::ConfigServerInfo info;

    if (cim::db::ConfigDao::getInstance()->query(info)) {
        LogInfo("serverIP:{},gatePort:{},httpPort:{}", info.ip, info.gatePort, info.httpPort);
        cim::setChatKitServerInfo(info);
    }


    // 创建主线程
    MainThread thread;
    g_instance_ = hInstance;

    // 执行主线程循环
    thread.RunOnCurrentThreadWithLoop(nbase::MessageLoop::kUIMessageLoop);

    return 0;
}

void MainThread::Init() {
    nbase::ThreadManager::RegisterThread(kThreadUI);

    // 获取资源路径，初始化全局参数
    std::wstring theme_dir = nbase::win32::GetCurrentModuleDirectory();
#ifdef _DEBUG
    // Debug 模式下使用本地文件夹作为资源
    // 默认皮肤使用 resources\\themes\\default
    // 默认语言使用 resources\\lang\\zh_CN
    // 如需修改请指定 Startup 最后两个参数
    ui::GlobalManager::Startup(theme_dir + L"resources\\", ui::CreateControlCallback(), false, L"themes\\macos");
#else
    // Release 模式下使用资源中的压缩包作为资源
    // 资源被导入到资源列表分类为 THEME，资源名称为 IDR_THEME
    // 如果资源使用的是本地的 zip 文件而非资源中的 zip 压缩包
    // 可以使用 OpenResZip 另一个重载函数打开本地的资源压缩包
    ui::GlobalManager::OpenResZip(MAKEINTRESOURCE(IDR_THEME), L"THEME", "");
    // ui::GlobalManager::OpenResZip(L"resources.zip", "");
    ui::GlobalManager::Startup(L"resources\\", ui::CreateControlCallback(), false);
#endif

    //LoginForm* window = new LoginForm();
    //HomeForm* window = new HomeForm();

    //设置程序默认图标
    /*HICON hIcon = ::LoadIcon(g_instance_, L"IDI_BASIC");
    ::SendMessage(window->GetHWND(), STM_SETICON, IMAGE_ICON, (LPARAM)(UINT)hIcon);*/

    /*window->Create(NULL, LoginForm::kClassName.c_str(), WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
    window->CenterWindow();
    window->ShowWindow();*/

    ShowHomeForm();
}

void MainThread::Cleanup() {
    // sdk dispose
    cim::cleanup();

    ui::GlobalManager::Shutdown();
    SetThreadWasQuitProperly(true);
    nbase::ThreadManager::UnregisterThread();

    // kill myself
    HANDLE hself = GetCurrentProcess();
    TerminateProcess(hself, 0);
}
