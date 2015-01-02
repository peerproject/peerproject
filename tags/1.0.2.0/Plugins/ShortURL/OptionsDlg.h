//
// OptionsDlg.h : Declaration of the COptionsDlg
//
// This file is part of PeerProject (peerproject.org) � 2014
// Portions Copyright Nikolay Raspopov, 2014.
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

class CPlugin;

// COptionsDlg

class COptionsDlg
	: public CAxDialogImpl< COptionsDlg >
{
public:
	COptionsDlg(CPlugin* pOwner);
	virtual ~COptionsDlg();

	enum { IDD = IDD_OPTIONS };

BEGIN_MSG_MAP( COptionsDlg )
	MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
	COMMAND_HANDLER( IDOK, BN_CLICKED, OnClickedOK )
	COMMAND_HANDLER( IDCANCEL, BN_CLICKED, OnClickedCancel )
	COMMAND_HANDLER( IDC_DEFAULTS, BN_CLICKED, OnClickedDefaults )
	COMMAND_HANDLER( IDC_TEST, BN_CLICKED, OnClickedTest )
	COMMAND_HANDLER( IDC_NEW, BN_CLICKED, OnClickedNew )
	COMMAND_HANDLER( IDC_DELETE, BN_CLICKED, OnClickedDelete )
	COMMAND_HANDLER( IDC_UP, BN_CLICKED, OnClickedUp )
	COMMAND_HANDLER( IDC_DOWN, BN_CLICKED, OnClickedDown )
	NOTIFY_HANDLER( IDC_URL_LIST, LVN_ITEMCHANGED, OnItemChanged )
	NOTIFY_HANDLER( IDC_URL_LIST, LVN_ENDLABELEDIT, OnItemEdited )
	NOTIFY_HANDLER( IDC_URL_LIST, LVN_KEYDOWN, OnKeyDown )
	NOTIFY_HANDLER( IDC_URL_LIST, NM_DBLCLK, OnDblClick )
	CHAIN_MSG_MAP( CAxDialogImpl< COptionsDlg > )
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

protected:
	CPlugin* m_pOwner;

	void LoadList(CString sURLs);
	CString SaveList() const;
	int  GetSelectedURL() const;
	void UpdateInterface();
	void Edit();

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedDefaults( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnClickedTest( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnClickedNew( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnClickedDelete( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnClickedUp( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnClickedDown( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnDblClick( int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled );
	LRESULT OnKeyDown( int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled );
	LRESULT OnItemChanged( int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled );
	LRESULT OnItemEdited( int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled );
};
