//
// DlgSplash.h
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#pragma once


class CSplashDlg : public CDialog
{
public:
	CSplashDlg(int nMax, bool bClosing);
	virtual ~CSplashDlg();

	enum { IDD = IDD_SPLASH };

	DECLARE_DYNAMIC(CSplashDlg)

public:
	void	Step(LPCTSTR pszText);
	void	Topmost();
	void	Hide();

protected:
	int			m_nPos;
	int			m_nMax;
	bool		m_bClosing;
	CString		m_sState;
	CBitmap		m_bmSplash;
	CBitmap		m_bmBuffer;
	CDC			m_dcBuffer1;
	CDC			m_dcBuffer2;

	void		DoPaint(CDC* pDC);
	BOOL		(WINAPI *m_pfnAnimateWindow)(HWND, DWORD, DWORD);

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnPaint();
	afx_msg LRESULT OnPrintClient(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};
