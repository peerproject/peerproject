//
// RegExpDlg.h
//

#pragma once

class CRegExpDlg : public CDialog
{
public:
	CRegExpDlg(CWnd* pParent = NULL);	// Standard constructor

	enum { IDD = IDD_REGEXP_DIALOG };

protected:
	HICON m_hIcon;
	CString m_strInput;
	CString m_strRegExp;
	CListBox m_oResult;

	void DoIt();
	void DoItSafe();

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnEnChangeRegexp();
	afx_msg void OnEnChangeInput();

	DECLARE_MESSAGE_MAP()
};
