// Windows Template Library - WTL version 8.1
// Copyright (C) Microsoft Corporation. All rights reserved.
//
// This file is a part of the Windows Template Library.
// The use and distribution terms for this software are covered by the
// Common Public License 1.0 (http://opensource.org/licenses/cpl1.0.php)
// which can be found in the file CPL.TXT at the root of this distribution.
// By using this software in any fashion, you are agreeing to be bound by
// the terms of this license. You must not remove this notice,
// or any other, from this software.

#ifndef __ATLWINX_H__
#define __ATLWINX_H__

#pragma once

#ifndef __ATLAPP_H__
	#error atlwinx.h requires atlapp.h to be included first
#endif

//#if (_ATL_VER >= 0x0700)
  #include <atlwin.h>
//#endif // (_ATL_VER >= 0x0700)


///////////////////////////////////////////////////////////////////////////////
// Classes in this file:
//
// _U_RECT
// _U_MENUorID
// _U_STRINGorID


///////////////////////////////////////////////////////////////////////////////
// Command Chaining Macros

#define CHAIN_COMMANDS(theChainClass) \
	if(uMsg == WM_COMMAND) \
		CHAIN_MSG_MAP(theChainClass)

#define CHAIN_COMMANDS_ALT(theChainClass, msgMapID) \
	if(uMsg == WM_COMMAND) \
		CHAIN_MSG_MAP_ALT(theChainClass, msgMapID)

#define CHAIN_COMMANDS_MEMBER(theChainMember) \
	if(uMsg == WM_COMMAND) \
		CHAIN_MSG_MAP_MEMBER(theChainMember)

#define CHAIN_COMMANDS_ALT_MEMBER(theChainMember, msgMapID) \
	if(uMsg == WM_COMMAND) \
		CHAIN_MSG_MAP_ALT_MEMBER(theChainMember, msgMapID)


///////////////////////////////////////////////////////////////////////////////
// Macros for parent message map to selectively reflect control messages

// NOTE: ReflectNotifications is a member of ATL's CWindowImplRoot
//  (and overridden in 2 cases - CContainedWindowT and CAxHostWindow)
//  Since we can't modify ATL, we'll provide the needed additions
//  in a separate function (that is not a member of CWindowImplRoot)

namespace WTL
{

inline LRESULT WtlReflectNotificationsFiltered(HWND hWndParent, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled,
                                               UINT uMsgFilter = WM_NULL, UINT_PTR idFromFilter = 0, HWND hWndChildFilter = NULL)
{
	if((uMsgFilter != WM_NULL) && (uMsgFilter != uMsg))
	{
		// The notification message doesn't match the filter.
		bHandled = FALSE;
		return 1;
	}

	HWND hWndChild = NULL;
	UINT_PTR idFrom = 0;

	switch(uMsg)
	{
	case WM_COMMAND:
		if(lParam != NULL)	// not from a menu
		{
			hWndChild = (HWND)lParam;
			idFrom = (UINT_PTR)LOWORD(wParam);
		}
		break;
	case WM_NOTIFY:
		hWndChild = ((LPNMHDR)lParam)->hwndFrom;
		idFrom = ((LPNMHDR)lParam)->idFrom;
		break;
#ifndef _WIN32_WCE
	case WM_PARENTNOTIFY:
		switch(LOWORD(wParam))
		{
		case WM_CREATE:
		case WM_DESTROY:
			hWndChild = (HWND)lParam;
			idFrom = (UINT_PTR)HIWORD(wParam);
			break;
		default:
			hWndChild = ::GetDlgItem(hWndParent, HIWORD(wParam));
			idFrom = (UINT_PTR)::GetDlgCtrlID(hWndChild);
			break;
		}
		break;
#endif // !_WIN32_WCE
	case WM_DRAWITEM:
		if(wParam)	// not from a menu
		{
			hWndChild = ((LPDRAWITEMSTRUCT)lParam)->hwndItem;
			idFrom = (UINT_PTR)wParam;
		}
		break;
	case WM_MEASUREITEM:
		if(wParam)	// not from a menu
		{
			hWndChild = ::GetDlgItem(hWndParent, ((LPMEASUREITEMSTRUCT)lParam)->CtlID);
			idFrom = (UINT_PTR)wParam;
		}
		break;
	case WM_COMPAREITEM:
		if(wParam)	// not from a menu
		{
			hWndChild = ((LPCOMPAREITEMSTRUCT)lParam)->hwndItem;
			idFrom = (UINT_PTR)wParam;
		}
		break;
	case WM_DELETEITEM:
		if(wParam)	// not from a menu
		{
			hWndChild = ((LPDELETEITEMSTRUCT)lParam)->hwndItem;
			idFrom = (UINT_PTR)wParam;
		}
		break;
	case WM_VKEYTOITEM:
	case WM_CHARTOITEM:
	case WM_HSCROLL:
	case WM_VSCROLL:
		hWndChild = (HWND)lParam;
		idFrom = (UINT_PTR)::GetDlgCtrlID(hWndChild);
		break;
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORMSGBOX:
	case WM_CTLCOLORSCROLLBAR:
	case WM_CTLCOLORSTATIC:
		hWndChild = (HWND)lParam;
		idFrom = (UINT_PTR)::GetDlgCtrlID(hWndChild);
		break;
	default:
		break;
	}

	if((hWndChild == NULL) ||
		((hWndChildFilter != NULL) && (hWndChildFilter != hWndChild)))
	{
		// Either hWndChild isn't valid, or
		// hWndChild doesn't match the filter.
		bHandled = FALSE;
		return 1;
	}

	if((idFromFilter != 0) && (idFromFilter != idFrom))
	{
		// The dialog control id doesn't match the filter.
		bHandled = FALSE;
		return 1;
	}

	ATLASSERT(::IsWindow(hWndChild));
	LRESULT lResult = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
	if((lResult == 0) && (uMsg >= WM_CTLCOLORMSGBOX) && (uMsg <= WM_CTLCOLORSTATIC))
	{
		// Try to prevent problems with WM_CTLCOLOR* messages when
		// the message wasn't really handled
		bHandled = FALSE;
	}

	return lResult;
}

}; // namespace WTL

// Try to prevent problems with WM_CTLCOLOR* messages when
// the message wasn't really handled
#define REFLECT_NOTIFICATIONS_EX() \
{ \
	bHandled = TRUE; \
	lResult = ReflectNotifications(uMsg, wParam, lParam, bHandled); \
	if((lResult == 0) && (uMsg >= WM_CTLCOLORMSGBOX) && (uMsg <= WM_CTLCOLORSTATIC)) \
		bHandled = FALSE; \
	if(bHandled) \
		return TRUE; \
}

#define REFLECT_NOTIFICATIONS_MSG_FILTERED(uMsgFilter) \
	{ \
		bHandled = TRUE; \
		lResult = WTL::WtlReflectNotificationsFiltered(m_hWnd, uMsg, wParam, lParam, bHandled, uMsgFilter, 0, NULL); \
		if(bHandled) \
			return TRUE; \
	}

#define REFLECT_NOTIFICATIONS_ID_FILTERED(idFromFilter) \
	{ \
		bHandled = TRUE; \
		lResult = WTL::WtlReflectNotificationsFiltered(m_hWnd, uMsg, wParam, lParam, bHandled, WM_NULL, idFromFilter, NULL); \
		if(bHandled) \
			return TRUE; \
	}

#define REFLECT_NOTIFICATIONS_HWND_FILTERED(hWndChildFilter) \
	{ \
		bHandled = TRUE; \
		lResult = WTL::WtlReflectNotificationsFiltered(m_hWnd, uMsg, wParam, lParam, bHandled, WM_NULL, 0, hWndChildFilter); \
		if(bHandled) \
			return TRUE; \
	}

#define REFLECT_NOTIFICATIONS_MSG_ID_FILTERED(uMsgFilter, idFromFilter) \
	{ \
		bHandled = TRUE; \
		lResult = WTL::WtlReflectNotificationsFiltered(m_hWnd, uMsg, wParam, lParam, bHandled, uMsgFilter, idFromFilter, NULL); \
		if(bHandled) \
			return TRUE; \
	}

#define REFLECT_NOTIFICATIONS_MSG_HWND_FILTERED(uMsgFilter, hWndChildFilter) \
	{ \
		bHandled = TRUE; \
		lResult = WTL::WtlReflectNotificationsFiltered(m_hWnd, uMsg, wParam, lParam, bHandled, uMsgFilter, 0, hWndChildFilter); \
		if(bHandled) \
			return TRUE; \
	}

#define REFLECT_COMMAND(id, code) \
	if(uMsg == WM_COMMAND && id == LOWORD(wParam) && code == HIWORD(wParam)) \
	{ \
		bHandled = TRUE; \
		lResult = ReflectNotifications(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define REFLECT_COMMAND_ID(id) \
	if(uMsg == WM_COMMAND && id == LOWORD(wParam)) \
	{ \
		bHandled = TRUE; \
		lResult = ReflectNotifications(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define REFLECT_COMMAND_CODE(code) \
	if(uMsg == WM_COMMAND && code == HIWORD(wParam)) \
	{ \
		bHandled = TRUE; \
		lResult = ReflectNotifications(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define REFLECT_COMMAND_RANGE(idFirst, idLast) \
	if(uMsg == WM_COMMAND && LOWORD(wParam) >= idFirst  && LOWORD(wParam) <= idLast) \
	{ \
		bHandled = TRUE; \
		lResult = ReflectNotifications(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define REFLECT_COMMAND_RANGE_CODE(idFirst, idLast, code) \
	if(uMsg == WM_COMMAND && code == HIWORD(wParam) && LOWORD(wParam) >= idFirst  && LOWORD(wParam) <= idLast) \
	{ \
		bHandled = TRUE; \
		lResult = ReflectNotifications(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define REFLECT_NOTIFY(id, cd) \
	if(uMsg == WM_NOTIFY && id == ((LPNMHDR)lParam)->idFrom && cd == ((LPNMHDR)lParam)->code) \
	{ \
		bHandled = TRUE; \
		lResult = ReflectNotifications(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define REFLECT_NOTIFY_ID(id) \
	if(uMsg == WM_NOTIFY && id == ((LPNMHDR)lParam)->idFrom) \
	{ \
		bHandled = TRUE; \
		lResult = ReflectNotifications(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define REFLECT_NOTIFY_CODE(cd) \
	if(uMsg == WM_NOTIFY && cd == ((LPNMHDR)lParam)->code) \
	{ \
		bHandled = TRUE; \
		lResult = ReflectNotifications(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define REFLECT_NOTIFY_RANGE(idFirst, idLast) \
	if(uMsg == WM_NOTIFY && ((LPNMHDR)lParam)->idFrom >= idFirst && ((LPNMHDR)lParam)->idFrom <= idLast) \
	{ \
		bHandled = TRUE; \
		lResult = ReflectNotifications(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define REFLECT_NOTIFY_RANGE_CODE(idFirst, idLast, cd) \
	if(uMsg == WM_NOTIFY && cd == ((LPNMHDR)lParam)->code && ((LPNMHDR)lParam)->idFrom >= idFirst && ((LPNMHDR)lParam)->idFrom <= idLast) \
	{ \
		bHandled = TRUE; \
		lResult = ReflectNotifications(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}


///////////////////////////////////////////////////////////////////////////////
// Reflected message handler macros for message maps (for ATL 3.0)

//#if (_ATL_VER < 0x0700)	(Removed)



///////////////////////////////////////////////////////////////////////////////
// Dual argument helper classes (for ATL 3.0)

//#if (_ATL_VER < 0x0700)	(Removed)



namespace WTL
{

///////////////////////////////////////////////////////////////////////////////
// Forward notifications support for message maps (for ATL 3.0)

//#if (_ATL_VER < 0x0700)	(Removed)

}; // namespace WTL

#endif // __ATLWINX_H__
