
// WowFisher.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CWowFisherApp: 
// �йش����ʵ�֣������ WowFisher.cpp
//

class CWowFisherApp : public CWinApp
{
public:
    CWowFisherApp();

    // ��д
public:
    virtual BOOL InitInstance();

    // ʵ��

    DECLARE_MESSAGE_MAP()
};

extern CWowFisherApp theApp;