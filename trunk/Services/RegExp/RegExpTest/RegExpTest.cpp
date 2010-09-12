//
// RegExpTest.cpp : Defines class behaviors for the application.
//

#include "StdAfx.h"
#include "RegExpTest.h"
#include "RegExpTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CRegExpApp, CWinApp)
END_MESSAGE_MAP()

CRegExpApp::CRegExpApp()
{
}

CRegExpApp theApp;

BOOL CRegExpApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	SetRegistryKey( _T("PeerProject") );

	CRegExpDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	return FALSE;
}
