//
// PageSettingsAdvanced.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version at your option.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 (AGPLv3) for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

#include "WndSettingsPage.h"

class CSettingEdit;


class CAdvancedSettingsPage : public CSettingsPage
{
public:
	CAdvancedSettingsPage();
	virtual ~CAdvancedSettingsPage();

	DECLARE_DYNCREATE(CAdvancedSettingsPage)

protected:
	enum { IDD = IDD_SETTINGS_ADVANCED };

	CSpinButtonCtrl	m_wndValueSpin;
	CEdit			m_wndValue;
	CListCtrl		m_wndList;

	void	AddSettings();				// Add settings to list
	void	UpdateListItem(int nItem);	// Update list item
	void	UpdateInputArea();			// Update edit box, spins and buttons
	void	CommitAll();				// Commit all data to settings
	void	UpdateAll();				// Update settings list
	bool	IsModified() const;			// Check if some of settings was modified

	class EditItem
	{
	public:
		EditItem(CSettings::Item* pItem);

		CSettings::Item*	m_pItem;	// Parent item
		CString				m_sName;	// Item name
		DWORD				m_nValue;	// Current value for DWORD
		bool				m_bValue;	// Current value for bool
		DWORD				m_nOriginalValue;	// Original value for DWORD
		bool				m_bOriginalValue;	// Original value for bool

		void	Update();				// Reload data from parent item
		void	Commit();				// Commit data to parent item
		bool	IsModified() const;		// Value was modified
		bool	IsDefault() const;		// Check if value is equal to default value
		void	Default();				// Restore default value
	};

	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	afx_msg void OnDestroy();
	afx_msg void OnItemChangedProperties(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeValue();
	afx_msg void OnColumnClickProperties(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedDefaultValue();

	DECLARE_MESSAGE_MAP()
};
