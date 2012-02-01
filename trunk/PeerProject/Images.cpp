//
// Images.cpp
//
// This file is part of PeerProject (peerproject.org) © 2010-2012
// All work here is original and released as-is under Persistent Public Domain [PPD]
//

// Dynamic Watermarks  (Common volatile skinned buttons etc.)

#include "StdAfx.h"
#include "Settings.h"
#include "Images.h"
#include "Colors.h"
#include "Skin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif	// Filename

CImages Images;


//////////////////////////////////////////////////////////////////////
// CImages construction

//CImages::CImages()
//{
//}

//CImages::~CImages()
//{
//}

//////////////////////////////////////////////////////////////////////
// CImages Bitmap Objects

void CImages::DeleteObjects()
{
	if ( m_brDialogControl.m_hObject ) m_brDialogControl.DeleteObject();
	if ( m_brDialogPanelControl.m_hObject ) m_brDialogPanelControl.DeleteObject();
	if ( m_brMediaControl.m_hObject ) m_brMediaControl.DeleteObject();

	if ( m_bmBanner.m_hObject ) m_bmBanner.DeleteObject();
	if ( m_bmBannerEdge.m_hObject ) m_bmBannerEdge.DeleteObject();
	if ( m_bmDialog.m_hObject ) m_bmDialog.DeleteObject();
	if ( m_bmDialogPanel.m_hObject ) m_bmDialogPanel.DeleteObject();
	if ( m_bmPanelMark.m_hObject ) m_bmPanelMark.DeleteObject();

	if ( m_bmSelected.m_hObject ) m_bmSelected.DeleteObject();
	if ( m_bmMenuSelected.m_hObject ) m_bmMenuSelected.DeleteObject();
	if ( m_bmMenuSelectedEdge.m_hObject ) m_bmMenuSelectedEdge.DeleteObject();
	if ( m_bmMenuDisabled.m_hObject ) m_bmMenuDisabled.DeleteObject();
	if ( m_bmMenuDisabledEdge.m_hObject ) m_bmMenuDisabledEdge.DeleteObject();
	if ( m_bmProgress.m_hObject ) m_bmProgress.DeleteObject();
	if ( m_bmProgressEdge.m_hObject ) m_bmProgressEdge.DeleteObject();
	if ( m_bmProgressNone.m_hObject ) m_bmProgressNone.DeleteObject();
	if ( m_bmProgressShaded.m_hObject ) m_bmProgressShaded.DeleteObject();
	if ( m_bmToolTip.m_hObject ) m_bmToolTip.DeleteObject();

	if ( m_bmIconButton.m_hObject ) m_bmIconButton.DeleteObject();
	if ( m_bmIconButtonHover.m_hObject ) m_bmIconButtonHover.DeleteObject();
	if ( m_bmIconButtonPress.m_hObject ) m_bmIconButtonPress.DeleteObject();
	if ( m_bmIconButtonActive.m_hObject ) m_bmIconButtonActive.DeleteObject();
	if ( m_bmIconButtonDisabled.m_hObject ) m_bmIconButtonDisabled.DeleteObject();

	if ( m_bmRichButton.m_hObject ) m_bmRichButton.DeleteObject();
	if ( m_bmRichButtonEdge.m_hObject ) m_bmRichButtonEdge.DeleteObject();
	if ( m_bmRichButtonHover.m_hObject ) m_bmRichButtonHover.DeleteObject();
	if ( m_bmRichButtonHoverEdge.m_hObject ) m_bmRichButtonHoverEdge.DeleteObject();
	if ( m_bmRichButtonPress.m_hObject ) m_bmRichButtonPress.DeleteObject();
	if ( m_bmRichButtonPressEdge.m_hObject ) m_bmRichButtonPressEdge.DeleteObject();
	if ( m_bmRichButtonActive.m_hObject ) m_bmRichButtonActive.DeleteObject();
	if ( m_bmRichButtonActiveEdge.m_hObject ) m_bmRichButtonActiveEdge.DeleteObject();
	if ( m_bmRichButtonDisabled.m_hObject ) m_bmRichButtonDisabled.DeleteObject();
	if ( m_bmRichButtonDisabledEdge.m_hObject ) m_bmRichButtonDisabledEdge.DeleteObject();

	if ( m_bmToolbarButton.m_hObject ) m_bmToolbarButton.DeleteObject();
	if ( m_bmToolbarButtonEdge.m_hObject ) m_bmToolbarButtonEdge.DeleteObject();
	if ( m_bmToolbarButtonHover.m_hObject ) m_bmToolbarButtonHover.DeleteObject();
	if ( m_bmToolbarButtonHoverEdge.m_hObject ) m_bmToolbarButtonHoverEdge.DeleteObject();
	if ( m_bmToolbarButtonPress.m_hObject ) m_bmToolbarButtonPress.DeleteObject();
	if ( m_bmToolbarButtonPressEdge.m_hObject ) m_bmToolbarButtonPressEdge.DeleteObject();
	if ( m_bmToolbarButtonActive.m_hObject ) m_bmToolbarButtonActive.DeleteObject();
	if ( m_bmToolbarButtonActiveEdge.m_hObject ) m_bmToolbarButtonActiveEdge.DeleteObject();
	if ( m_bmToolbarButtonDisabled.m_hObject ) m_bmToolbarButtonDisabled.DeleteObject();
	if ( m_bmToolbarButtonDisabledEdge.m_hObject ) m_bmToolbarButtonDisabledEdge.DeleteObject();
	if ( m_bmToolbarSeparator.m_hObject ) m_bmToolbarSeparator.DeleteObject();

	if ( m_bmMenubarItem.m_hObject ) m_bmMenubarItem.DeleteObject();
	if ( m_bmMenubarItemEdge.m_hObject ) m_bmMenubarItemEdge.DeleteObject();
	if ( m_bmMenubarItemHover.m_hObject ) m_bmMenubarItemHover.DeleteObject();
	if ( m_bmMenubarItemHoverEdge.m_hObject ) m_bmMenubarItemHoverEdge.DeleteObject();
	if ( m_bmMenubarItemPress.m_hObject ) m_bmMenubarItemPress.DeleteObject();
	if ( m_bmMenubarItemPressEdge.m_hObject ) m_bmMenubarItemPressEdge.DeleteObject();
	if ( m_bmMenubarButton.m_hObject ) m_bmMenubarButton.DeleteObject();
	if ( m_bmMenubarButtonEdge.m_hObject ) m_bmMenubarButtonEdge.DeleteObject();
	if ( m_bmMenubarButtonHover.m_hObject ) m_bmMenubarButtonHover.DeleteObject();
	if ( m_bmMenubarButtonHoverEdge.m_hObject ) m_bmMenubarButtonHoverEdge.DeleteObject();
	if ( m_bmMenubarButtonPress.m_hObject ) m_bmMenubarButtonPress.DeleteObject();
	if ( m_bmMenubarButtonPressEdge.m_hObject ) m_bmMenubarButtonPressEdge.DeleteObject();
	if ( m_bmMenubarButtonActive.m_hObject ) m_bmMenubarButtonActive.DeleteObject();
	if ( m_bmMenubarButtonActiveEdge.m_hObject ) m_bmMenubarButtonActiveEdge.DeleteObject();

	if ( m_bmTaskbarButton.m_hObject ) m_bmTaskbarButton.DeleteObject();
	if ( m_bmTaskbarButtonEdge.m_hObject ) m_bmTaskbarButtonEdge.DeleteObject();
	if ( m_bmTaskbarButtonActive.m_hObject ) m_bmTaskbarButtonActive.DeleteObject();
	if ( m_bmTaskbarButtonActiveEdge.m_hObject ) m_bmTaskbarButtonActiveEdge.DeleteObject();
	if ( m_bmTaskbarButtonHover.m_hObject ) m_bmTaskbarButtonHover.DeleteObject();
	if ( m_bmTaskbarButtonHoverEdge.m_hObject ) m_bmTaskbarButtonHoverEdge.DeleteObject();
	if ( m_bmTaskbarButtonPress.m_hObject ) m_bmTaskbarButtonPress.DeleteObject();
	if ( m_bmTaskbarButtonPressEdge.m_hObject ) m_bmTaskbarButtonPressEdge.DeleteObject();

	if ( m_bmDownloadGroup.m_hObject ) m_bmDownloadGroup.DeleteObject();
	if ( m_bmDownloadGroupEdge.m_hObject ) m_bmDownloadGroupEdge.DeleteObject();
	if ( m_bmDownloadGroupActive.m_hObject ) m_bmDownloadGroupActive.DeleteObject();
	if ( m_bmDownloadGroupActiveEdge.m_hObject ) m_bmDownloadGroupActiveEdge.DeleteObject();
	if ( m_bmDownloadGroupHover.m_hObject ) m_bmDownloadGroupHover.DeleteObject();
	if ( m_bmDownloadGroupHoverEdge.m_hObject ) m_bmDownloadGroupHoverEdge.DeleteObject();
	if ( m_bmDownloadGroupPress.m_hObject ) m_bmDownloadGroupPress.DeleteObject();
	if ( m_bmDownloadGroupPressEdge.m_hObject ) m_bmDownloadGroupPressEdge.DeleteObject();

	if ( m_bmButtonMapIconbox.m_hObject ) m_bmButtonMapIconbox.DeleteObject();
	if ( m_bmButtonMapRichdoc.m_hObject ) m_bmButtonMapRichdoc.DeleteObject();
	if ( m_bmButtonMapToolbar.m_hObject ) m_bmButtonMapToolbar.DeleteObject();
	if ( m_bmButtonMapMenubar.m_hObject ) m_bmButtonMapMenubar.DeleteObject();
	if ( m_bmButtonMapMenutext.m_hObject ) m_bmButtonMapMenutext.DeleteObject();
	if ( m_bmButtonMapMenuselect.m_hObject ) m_bmButtonMapMenuselect.DeleteObject();
	if ( m_bmButtonMapProgressbar.m_hObject ) m_bmButtonMapProgressbar.DeleteObject();
	if ( m_bmButtonMapDownloadgroup.m_hObject ) m_bmButtonMapDownloadgroup.DeleteObject();
	if ( m_bmButtonMapTaskbar.m_hObject ) m_bmButtonMapTaskbar.DeleteObject();
}

void CImages::Load()
{
	DeleteObjects();

	if ( HBITMAP hSelected = Skin.GetWatermark( _T("System.Highlight") ) )
		m_bmSelected.Attach( hSelected );
	else if ( HBITMAP hSelected = Skin.GetWatermark( _T("CTransfers.Selected") ) )
		m_bmSelected.Attach( hSelected );

	if ( HBITMAP hButton = Skin.GetWatermark( _T("System.MenuSelected") ) )
	{
		PreBlend( hButton );
		m_bmMenuSelected.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("System.MenuSelectedEdge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hButton );
			m_bmMenuSelectedEdge.Attach( hEdge );
			m_bmMenuSelectedEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmMenuSelectedEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CCoolMenu.Hover") ) )
	{
		PreBlend( hButton );
		m_bmMenuSelected.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CCoolMenu.Hover.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hButton );
			m_bmMenuSelectedEdge.Attach( hEdge );
			m_bmMenuSelectedEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmMenuSelectedEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hButton = Skin.GetWatermark( _T("System.MenuDisabled") ) )
	{
		PreBlend( hButton );
		m_bmMenuDisabled.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("System.MenuDisabledEdge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hButton );
			m_bmMenuDisabledEdge.Attach( hEdge );
			m_bmMenuDisabledEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmMenuDisabledEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CCoolMenu.Disabled") ) )
	{
		PreBlend( hButton );
		m_bmMenuDisabled.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CCoolMenu.Disabled.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hButton );
			m_bmMenuDisabledEdge.Attach( hEdge );
			m_bmMenuDisabledEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmMenuDisabledEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hProgress = Skin.GetWatermark( _T("System.Progress") ) )
	{
		m_bmProgress.Attach( hProgress );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("System.Progress.Edge") ) )
		{
			BITMAP bmInfo;
			m_bmProgressEdge.Attach( hEdge );
			m_bmProgressEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmProgressEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hProgress = Skin.GetWatermark( _T("ProgressBar") ) )
	{
		m_bmProgress.Attach( hProgress );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("ProgressBar.Edge") ) )
		{
			BITMAP bmInfo;
			m_bmProgressEdge.Attach( hEdge );
			m_bmProgressEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmProgressEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hProgress = Skin.GetWatermark( _T("System.Progress.None") ) )
		m_bmProgressNone.Attach( hProgress );
	else if ( HBITMAP hProgress = Skin.GetWatermark( _T("ProgressBar.None") ) )
		m_bmProgressNone.Attach( hProgress );

	if ( HBITMAP hProgress = Skin.GetWatermark( _T("System.Progress.Shaded") ) )
		m_bmProgressShaded.Attach( hProgress );
	else if ( HBITMAP hProgress = Skin.GetWatermark( _T("ProgressBar.Shaded") ) )
		m_bmProgressShaded.Attach( hProgress );


	if ( HBITMAP hToolTip = Skin.GetWatermark( _T("System.ToolTip") ) )
		m_bmToolTip.Attach( hToolTip );
	else if ( HBITMAP hToolTip = Skin.GetWatermark( _T("System.Tooltips") ) )
		m_bmToolTip.Attach( hToolTip );
	else if ( HBITMAP hToolTip = Skin.GetWatermark( _T("CToolTipCtrl") ) )
		m_bmToolTip.Attach( hToolTip );

	if ( HBITMAP hDialog = Skin.GetWatermark( _T("System.Dialogs") ) )
		m_bmDialog.Attach( hDialog );
	else if ( HBITMAP hDialog = Skin.GetWatermark( _T("CDialog") ) )
		m_bmDialog.Attach( hDialog );

	if ( HBITMAP hDialog = Skin.GetWatermark( _T("System.DialogPanels") ) )
		m_bmDialogPanel.Attach( hDialog );
	else if ( HBITMAP hDialog = Skin.GetWatermark( _T("CDialog.Panel") ) )
		m_bmDialogPanel.Attach( hDialog );

	// Related brushes:
	// (Note skinnable dialogs:  Brush applies to opaque text/control bg.  Body in DlgSkinDialog, WndSettingPage, etc.)

	if ( HBITMAP hControl = Skin.GetWatermark( _T("System.Dialogs.Control") ) )
	{
		CBitmap bmControl;
		bmControl.Attach( hControl );
		m_brDialogControl.CreatePatternBrush( &bmControl ); 	// Attach( (HBRUSH)GetStockObject( NULL_BRUSH ) ); ?
	}
	else if ( HBITMAP hControl = Skin.GetWatermark( _T("CDialog.Control") ) )
	{
		CBitmap bmControl;
		bmControl.Attach( hControl );
		m_brDialogControl.CreatePatternBrush( &bmControl );
	}
	else
		m_brDialogControl.CreateSolidBrush( Colors.m_crDialog );

	if ( HBITMAP hControl = Skin.GetWatermark( _T("System.DialogPanels.Control") ) )
	{
		CBitmap bmControl;
		bmControl.Attach( hControl );
		m_brDialogControl.CreatePatternBrush( &bmControl );
	}
	else if ( HBITMAP hControl = Skin.GetWatermark( _T("CDialog.Panel.Control") ) )
	{
		CBitmap bmControl;
		bmControl.Attach( hControl );
		m_brDialogPanelControl.CreatePatternBrush( &bmControl );
	}
	else
		m_brDialogPanelControl.CreateSolidBrush( Colors.m_crDialogPanel );

	if ( HBITMAP hControl = Skin.GetWatermark( _T("CMediaFrame.Slider") ) )
	{
		CBitmap bmControl;
		bmControl.Attach( hControl );
		m_brMediaControl.CreatePatternBrush( &bmControl );
	}
	else if ( HBITMAP hControl = Skin.GetWatermark( _T("CCoolbar.Control") ) )
	{
		CBitmap bmControl;
		bmControl.Attach( hControl );
		m_brMediaControl.CreatePatternBrush( &bmControl );
	}
	else
		m_brMediaControl.CreateSolidBrush( Colors.m_crMidtone );


	m_nBanner = 0;		// Height = Images.m_bmBanner.GetBitmapDimension().cy

	if ( HBITMAP hBanner = Skin.GetWatermark( _T("System.Header") ) )
	{
		BITMAP bmInfo;
		m_bmBanner.Attach( hBanner );
		m_bmBanner.GetObject( sizeof(BITMAP), &bmInfo );
		m_bmBanner.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		m_nBanner = bmInfo.bmHeight;

		if ( HBITMAP hEdge = Skin.GetWatermark( _T("System.Header.Edge") ) )
		{
			m_bmBannerEdge.Attach( hEdge );
			m_bmBannerEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmBannerEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hBanner = Skin.GetWatermark( _T("Banner") ) )
	{
		BITMAP bmInfo;
		m_bmBanner.Attach( hBanner );
		m_bmBanner.GetObject( sizeof(BITMAP), &bmInfo );
		m_bmBanner.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		m_nBanner = bmInfo.bmHeight;

		if ( HBITMAP hEdge = Skin.GetWatermark( _T("Banner.Edge") ) )
		{
			m_bmBannerEdge.Attach( hEdge );
			m_bmBannerEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmBannerEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hPanelMark = Skin.GetWatermark( _T("CPanelWnd.Caption") ) )
		m_bmPanelMark.Attach( hPanelMark );
	else if ( Colors.m_crPanelBack == RGB_DEFAULT_CASE )
		m_bmPanelMark.LoadBitmap( IDB_PANEL_MARK );				// Special-case default resource handling


	// Note "System.Toolbars" fallback handled at toolbar creation

	// Button states:

	// Dialog Single-Icon Buttons:

	if ( HBITMAP hButton = Skin.GetWatermark( _T("IconButton") ) )
	{
		BITMAP bmInfo;
		m_bmIconButton.Attach( hButton );
		m_bmIconButton.GetObject( sizeof(BITMAP), &bmInfo );
		m_bmIconButton.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
	}
	if ( HBITMAP hButton = Skin.GetWatermark( _T("IconButton.Hover") ) )
	{
		BITMAP bmInfo;
		m_bmIconButtonHover.Attach( hButton );
		m_bmIconButtonHover.GetObject( sizeof(BITMAP), &bmInfo );
		m_bmIconButtonHover.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
	}
	if ( HBITMAP hButton = Skin.GetWatermark( _T("IconButton.Press") ) )
	{
		BITMAP bmInfo;
		m_bmIconButtonPress.Attach( hButton );
		m_bmIconButtonPress.GetObject( sizeof(BITMAP), &bmInfo );
		m_bmIconButtonPress.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
	}
	if ( HBITMAP hButton = Skin.GetWatermark( _T("IconButton.Active") ) )
	{
		BITMAP bmInfo;
		m_bmIconButtonActive.Attach( hButton );
		m_bmIconButtonActive.GetObject( sizeof(BITMAP), &bmInfo );
		m_bmIconButtonActive.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
	}
	if ( HBITMAP hButton = Skin.GetWatermark( _T("IconButton.Disabled") ) )
	{
		BITMAP bmInfo;
		m_bmIconButtonDisabled.Attach( hButton );
		m_bmIconButtonDisabled.GetObject( sizeof(BITMAP), &bmInfo );
		m_bmIconButtonDisabled.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
	}

	// RichDoc Buttons (Search):

	if ( HBITMAP hButton = Skin.GetWatermark( _T("RichButton") ) )
	{
		PreBlend( hButton );
		m_bmRichButton.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("RichButtonEdge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmRichButtonEdge.Attach( hEdge );
			m_bmRichButtonEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmRichButtonEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	if ( HBITMAP hButton = Skin.GetWatermark( _T("RichButton.Hover") ) )
	{
		PreBlend( hButton );
		m_bmRichButtonHover.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("RichButtonEdge.Hover") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmRichButtonHoverEdge.Attach( hEdge );
			m_bmRichButtonHoverEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmRichButtonHoverEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	if ( HBITMAP hButton = Skin.GetWatermark( _T("RichButton.Press") ) )
	{
		PreBlend( hButton );
		m_bmRichButtonPress.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("RichButtonEdge.Press") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmRichButtonPressEdge.Attach( hEdge );
			m_bmRichButtonPressEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmRichButtonPressEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	if ( HBITMAP hButton = Skin.GetWatermark( _T("RichButton.Active") ) )
	{
		PreBlend( hButton );
		m_bmRichButtonActive.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("RichButtonEdge.Active") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmRichButtonActiveEdge.Attach( hEdge );
			m_bmRichButtonActiveEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmRichButtonActiveEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	if ( HBITMAP hButton = Skin.GetWatermark( _T("RichButton.Disabled") ) )
	{
		PreBlend( hButton );
		m_bmRichButtonDisabled.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("RichButtonEdge.Disabled") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmRichButtonDisabledEdge.Attach( hEdge );
			m_bmRichButtonDisabledEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmRichButtonDisabledEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}


	// Command Toolbar Buttons:

	if ( HBITMAP hButton = Skin.GetWatermark( _T("ToolbarButton") ) )
	{
		PreBlend( hButton );
		m_bmToolbarButton.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("ToolbarButtonEdge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmToolbarButtonEdge.Attach( hEdge );
			m_bmToolbarButtonEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmToolbarButtonEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CCoolbar.Up") ) )
	{
		PreBlend( hButton );
		m_bmToolbarButton.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CCoolbar.Up.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmToolbarButtonEdge.Attach( hEdge );
			m_bmToolbarButtonEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmToolbarButtonEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hButton = Skin.GetWatermark( _T("ToolbarButton.Hover") ) )
	{
		PreBlend( hButton );
		m_bmToolbarButtonHover.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("ToolbarButtonEdge.Hover") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmToolbarButtonHoverEdge.Attach( hEdge );
			m_bmToolbarButtonHoverEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmToolbarButtonHoverEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CCoolbar.Hover") ) )
	{
		PreBlend( hButton );
		m_bmToolbarButtonHover.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CCoolbar.Hover.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmToolbarButtonHoverEdge.Attach( hEdge );
			m_bmToolbarButtonHoverEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmToolbarButtonHoverEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hButton = Skin.GetWatermark( _T("ToolbarButton.Press") ) )
	{
		PreBlend( hButton );
		m_bmToolbarButtonPress.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("ToolbarButtonEdge.Press") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmToolbarButtonPressEdge.Attach( hEdge );
			m_bmToolbarButtonPressEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmToolbarButtonPressEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CCoolbar.Down") ) )
	{
		PreBlend( hButton );
		m_bmToolbarButtonPress.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CCoolbar.Down.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmToolbarButtonPressEdge.Attach( hEdge );
			m_bmToolbarButtonPressEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmToolbarButtonPressEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hButton = Skin.GetWatermark( _T("ToolbarButton.Active") ) )
	{
		PreBlend( hButton );
		m_bmToolbarButtonActive.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("ToolbarButtonEdge.Active") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmToolbarButtonActiveEdge.Attach( hEdge );
			m_bmToolbarButtonActiveEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmToolbarButtonActiveEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CCoolbar.Checked") ) )
	{
		PreBlend( hButton );
		m_bmToolbarButtonActive.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CCoolbar.Checked.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmToolbarButtonActiveEdge.Attach( hEdge );
			m_bmToolbarButtonActiveEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmToolbarButtonActiveEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hButton = Skin.GetWatermark( _T("ToolbarButton.Disabled") ) )
	{
		PreBlend( hButton );
		m_bmToolbarButtonDisabled.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("ToolbarButtonEdge.Disabled") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmToolbarButtonDisabledEdge.Attach( hEdge );
			m_bmToolbarButtonDisabledEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmToolbarButtonDisabledEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CCoolbar.Disabled") ) )
	{
		PreBlend( hButton );
		m_bmToolbarButtonDisabled.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CCoolbar.Disabled.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmToolbarButtonDisabledEdge.Attach( hEdge );
			m_bmToolbarButtonDisabledEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmToolbarButtonDisabledEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hImage = Skin.GetWatermark( _T("ToolbarSeparator") ) )
	{
		PreBlend( hImage );
		m_bmToolbarSeparator.Attach( hImage );
	}
	else if ( HBITMAP hImage = Skin.GetWatermark( _T("CCoolbar.Separator") ) )
	{
		PreBlend( hImage );
		m_bmToolbarSeparator.Attach( hImage );
	}


	// Main Menubar (Text) Buttons:

	if ( HBITMAP hButton = Skin.GetWatermark( _T("MenubarItem") ) )
	{
		PreBlend( hButton );
		m_bmMenubarItem.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("MenubarItemEdge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmMenubarItemEdge.Attach( hEdge );
			m_bmMenubarItemEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmMenubarItemEdge.SetBitmapDimension( bmInfo.bmWidth, 0 );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CCoolMenuItem.Up") ) )
	{
		PreBlend( hButton );
		m_bmMenubarItem.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CCoolMenuItem.Up.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmMenubarItemEdge.Attach( hEdge );
			m_bmMenubarItemEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmMenubarItemEdge.SetBitmapDimension( bmInfo.bmWidth, 0 );
		}
	}

	if ( HBITMAP hButton = Skin.GetWatermark( _T("MenubarItem.Hover") ) )
	{
		PreBlend( hButton );
		m_bmMenubarItemHover.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("MenubarItemEdge.Hover") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmMenubarItemHoverEdge.Attach( hEdge );
			m_bmMenubarItemHoverEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmMenubarItemHoverEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CCoolMenuItem.Hover") ) )
	{
		PreBlend( hButton );
		m_bmMenubarItemHover.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CCoolMenuItem.Hover.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmMenubarItemHoverEdge.Attach( hEdge );
			m_bmMenubarItemHoverEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmMenubarItemHoverEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hButton = Skin.GetWatermark( _T("MenubarItem.Press") ) )
	{
		PreBlend( hButton );
		m_bmMenubarItemPress.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("MenubarItemEdge.Press") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmMenubarItemPressEdge.Attach( hEdge );
			m_bmMenubarItemPressEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmMenubarItemPressEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CCoolMenuItem.Down") ) )
	{
		PreBlend( hButton );
		m_bmMenubarItemPress.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CCoolMenuItem.Down.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmMenubarItemPressEdge.Attach( hEdge );
			m_bmMenubarItemPressEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmMenubarItemPressEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hButton = Skin.GetWatermark( _T("MenubarButton") ) )
	{
		PreBlend( hButton );
		m_bmMenubarButton.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("MenubarButtonEdge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmMenubarButtonEdge.Attach( hEdge );
			m_bmMenubarButtonEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmMenubarButtonEdge.SetBitmapDimension( bmInfo.bmWidth, 0 );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CCoolMenuBar.Up") ) )
	{
		PreBlend( hButton );
		m_bmMenubarButton.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CCoolMenuBar.Up.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmMenubarButtonEdge.Attach( hEdge );
			m_bmMenubarButtonEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmMenubarButtonEdge.SetBitmapDimension( bmInfo.bmWidth, 0 );
		}
	}

	if ( HBITMAP hButton = Skin.GetWatermark( _T("MenubarButton.Hover") ) )
	{
		PreBlend( hButton );
		m_bmMenubarButtonHover.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("MenubarButtonEdge.Hover") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmMenubarButtonHoverEdge.Attach( hEdge );
			m_bmMenubarButtonHoverEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmMenubarButtonHoverEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CCoolMenuBar.Hover") ) )
	{
		PreBlend( hButton );
		m_bmMenubarButtonHover.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CCoolMenuBar.Hover.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmMenubarButtonHoverEdge.Attach( hEdge );
			m_bmMenubarButtonHoverEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmMenubarButtonHoverEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hButton = Skin.GetWatermark( _T("MenubarButton.Press") ) )
	{
		PreBlend( hButton );
		m_bmMenubarButtonPress.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("MenubarButtonEdge.Press") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmMenubarButtonPressEdge.Attach( hEdge );
			m_bmMenubarButtonPressEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmMenubarButtonPressEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CCoolMenuBar.Down") ) )
	{
		PreBlend( hButton );
		m_bmMenubarButtonPress.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CCoolMenuBar.Down.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmMenubarButtonPressEdge.Attach( hEdge );
			m_bmMenubarButtonPressEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmMenubarButtonPressEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hButton = Skin.GetWatermark( _T("MenubarButton.Active") ) )
	{
		PreBlend( hButton );
		m_bmMenubarButtonActive.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("MenubarButtonEdge.Active") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmMenubarButtonActiveEdge.Attach( hEdge );
			m_bmMenubarButtonActiveEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmMenubarButtonActiveEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CCoolMenuBar.Checked") ) )
	{
		PreBlend( hButton );
		m_bmMenubarButtonActive.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CCoolMenuBar.Checked.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmMenubarButtonActiveEdge.Attach( hEdge );
			m_bmMenubarButtonActiveEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmMenubarButtonActiveEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}


	// Taskbar Tabs:

	if ( HBITMAP hButton = Skin.GetWatermark( _T("TaskbarButton") ) )
	{
		PreBlend( hButton );
		m_bmTaskbarButton.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("TaskbarButtonEdge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmTaskbarButtonEdge.Attach( hEdge );
			m_bmTaskbarButtonEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmTaskbarButtonEdge.SetBitmapDimension( bmInfo.bmWidth, 0 );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CWndTabBar.Tab") ) )
	{
		PreBlend( hButton );
		m_bmTaskbarButton.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CWndTabBar.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmTaskbarButtonEdge.Attach( hEdge );
			m_bmTaskbarButtonEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmTaskbarButtonEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hButton = Skin.GetWatermark( _T("TaskbarButton.Active") ) )
	{
		PreBlend( hButton );
		m_bmTaskbarButtonActive.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("TaskbarButtonEdge.Active") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmTaskbarButtonActiveEdge.Attach( hEdge );
			m_bmTaskbarButtonActiveEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmTaskbarButtonActiveEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CWndTabBar.Active") ) )
	{
		PreBlend( hButton );
		m_bmTaskbarButtonActive.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CWndTabBar.Active.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmTaskbarButtonActiveEdge.Attach( hEdge );
			m_bmTaskbarButtonActiveEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmTaskbarButtonActiveEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hButton = Skin.GetWatermark( _T("TaskbarButton.Hover") ) )
	{
		PreBlend( hButton );
		m_bmTaskbarButtonHover.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("TaskbarButtonEdge.Hover") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmTaskbarButtonHoverEdge.Attach( hEdge );
			m_bmTaskbarButtonHoverEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmTaskbarButtonHoverEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CWndTabBar.Hover") ) )
	{
		PreBlend( hButton );
		m_bmTaskbarButtonHover.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CWndTabBar.Hover.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmTaskbarButtonHoverEdge.Attach( hEdge );
			m_bmTaskbarButtonHoverEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmTaskbarButtonHoverEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hButton = Skin.GetWatermark( _T("TaskbarButton.Press") ) )
	{
		PreBlend( hButton );
		m_bmTaskbarButtonPress.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("TaskbarButtonEdge.Press") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmTaskbarButtonPressEdge.Attach( hEdge );
			m_bmTaskbarButtonPressEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmTaskbarButtonPressEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CWndTabBar.Active.Hover") ) )
	{
		PreBlend( hButton );
		m_bmTaskbarButtonPress.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CWndTabBar.Active.Hover.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmTaskbarButtonPressEdge.Attach( hEdge );
			m_bmTaskbarButtonPressEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmTaskbarButtonPressEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	// Download Groups Bar Tabs:

	if ( HBITMAP hButton = Skin.GetWatermark( _T("DownloadGroup") ) )
	{
		PreBlend( hButton );
		m_bmDownloadGroup.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("DownloadGroupEdge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmDownloadGroupEdge.Attach( hEdge );
			m_bmDownloadGroupEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmDownloadGroupEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CDownloadTabBar.Up") ) )
	{
		PreBlend( hButton );
		m_bmDownloadGroup.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CDownloadTabBar.Up.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmDownloadGroupEdge.Attach( hEdge );
			m_bmDownloadGroupEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmDownloadGroupEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hButton = Skin.GetWatermark( _T("DownloadGroup.Active") ) )
	{
		PreBlend( hButton );
		m_bmDownloadGroupActive.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("DownloadGroupEdge.Active") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmDownloadGroupActiveEdge.Attach( hEdge );
			m_bmDownloadGroupActiveEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmDownloadGroupActiveEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CDownloadTabBar.Active") ) )
	{
		PreBlend( hButton );
		m_bmDownloadGroupActive.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CDownloadTabBar.Active.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmDownloadGroupActiveEdge.Attach( hEdge );
			m_bmDownloadGroupActiveEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmDownloadGroupActiveEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hButton = Skin.GetWatermark( _T("DownloadGroup.Hover") ) )
	{
		PreBlend( hButton );
		m_bmDownloadGroupHover.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("DownloadGroupEdge.Hover") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmDownloadGroupHoverEdge.Attach( hEdge );
			m_bmDownloadGroupHoverEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmDownloadGroupHoverEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CDownloadTabBar.Hover") ) )
	{
		PreBlend( hButton );
		m_bmDownloadGroupHover.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CDownloadTabBar.Hover.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmDownloadGroupHoverEdge.Attach( hEdge );
			m_bmDownloadGroupHoverEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmDownloadGroupHoverEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hButton = Skin.GetWatermark( _T("DownloadGroup.Press") ) )
	{
		PreBlend( hButton );
		m_bmDownloadGroupPress.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("DownloadGroupEdge.Press") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmDownloadGroupPressEdge.Attach( hEdge );
			m_bmDownloadGroupPressEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmDownloadGroupPressEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CDownloadTabBar.Active.Hover") ) )
	{
		PreBlend( hButton );
		m_bmDownloadGroupPress.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CDownloadTabBar.Active.Hover.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmDownloadGroupPressEdge.Attach( hEdge );
			m_bmDownloadGroupPressEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmDownloadGroupPressEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}

	if ( HBITMAP hButton = Skin.GetWatermark( _T("DownloadGroup.Disabled") ) )
	{
		PreBlend( hButton );
		m_bmDownloadGroupDisabled.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("DownloadGroupEdge.Disabled") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmDownloadGroupDisabledEdge.Attach( hEdge );
			m_bmDownloadGroupDisabledEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmDownloadGroupDisabledEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}
	else if ( HBITMAP hButton = Skin.GetWatermark( _T("CDownloadTabBar.Disabled") ) )
	{
		PreBlend( hButton );
		m_bmDownloadGroupDisabled.Attach( hButton );
		if ( HBITMAP hEdge = Skin.GetWatermark( _T("CDownloadTabBar.Disabled.Edge") ) )
		{
			BITMAP bmInfo;
			PreBlend( hEdge );
			m_bmDownloadGroupDisabledEdge.Attach( hEdge );
			m_bmDownloadGroupDisabledEdge.GetObject( sizeof(BITMAP), &bmInfo );
			m_bmDownloadGroupDisabledEdge.SetBitmapDimension( bmInfo.bmWidth, bmInfo.bmHeight );
		}
	}


	// Button Maps:

	if ( HBITMAP hButtonMap = Skin.GetWatermark( _T("ButtonMap.Iconbox") ) )
	{
		BITMAP bmInfo;
		PreBlend( hButtonMap );
		m_bmButtonMapIconbox.Attach( hButtonMap );
		m_bmButtonMapIconbox.GetObject( sizeof(BITMAP), &bmInfo );
		m_bmButtonMapIconbox.SetBitmapDimension( 0, bmInfo.bmHeight / STATE_COUNT );
	}

	if ( HBITMAP hButtonMap = Skin.GetWatermark( _T("ButtonMap.Richdoc") ) )
	{
		BITMAP bmInfo;
		PreBlend( hButtonMap );
		m_bmButtonMapRichdoc.Attach( hButtonMap );
		m_bmButtonMapRichdoc.GetObject( sizeof(BITMAP), &bmInfo );
		m_bmButtonMapRichdoc.SetBitmapDimension(
			bmInfo.bmWidth > Skin.m_nButtonEdge ? Skin.m_nButtonEdge : 0, bmInfo.bmHeight / STATE_COUNT );
	}

	if ( HBITMAP hButtonMap = Skin.GetWatermark( _T("ButtonMap.Toolbar") ) )
	{
		BITMAP bmInfo;
		PreBlend( hButtonMap );
		m_bmButtonMapToolbar.Attach( hButtonMap );
		m_bmButtonMapToolbar.GetObject( sizeof(BITMAP), &bmInfo );
		m_bmButtonMapToolbar.SetBitmapDimension(
			bmInfo.bmWidth > Skin.m_nButtonEdge ? Skin.m_nButtonEdge : 0, bmInfo.bmHeight / STATE_COUNT );
	}

	if ( HBITMAP hButtonMap = Skin.GetWatermark( _T("ButtonMap.Menubar") ) )
	{
		BITMAP bmInfo;
		PreBlend( hButtonMap );
		m_bmButtonMapMenubar.Attach( hButtonMap );
		m_bmButtonMapMenubar.GetObject( sizeof(BITMAP), &bmInfo );
		m_bmButtonMapMenubar.SetBitmapDimension(
			bmInfo.bmWidth > Skin.m_nButtonEdge ? Skin.m_nButtonEdge : 0, bmInfo.bmHeight / STATE_COUNT );
	}

	if ( HBITMAP hButtonMap = Skin.GetWatermark( _T("ButtonMap.MenuText") ) )
	{
		BITMAP bmInfo;
		PreBlend( hButtonMap );
		m_bmButtonMapMenutext.Attach( hButtonMap );
		m_bmButtonMapMenutext.GetObject( sizeof(BITMAP), &bmInfo );
		m_bmButtonMapMenutext.SetBitmapDimension(
			bmInfo.bmWidth > Skin.m_nButtonEdge ? Skin.m_nButtonEdge : 0, bmInfo.bmHeight / 3 );
	}

	if ( HBITMAP hButtonMap = Skin.GetWatermark( _T("ButtonMap.MenuSelect") ) )
	{
		BITMAP bmInfo;
		PreBlend( hButtonMap );
		m_bmButtonMapMenuselect.Attach( hButtonMap );
		m_bmButtonMapMenuselect.GetObject( sizeof(BITMAP), &bmInfo );
		m_bmButtonMapMenuselect.SetBitmapDimension(
			bmInfo.bmWidth > Skin.m_nButtonEdge ? Skin.m_nButtonEdge : 0, bmInfo.bmHeight / 3 );
	}

	if ( HBITMAP hButtonMap = Skin.GetWatermark( _T("ButtonMap.ProgressBar") ) )
	{
		BITMAP bmInfo;
		PreBlend( hButtonMap );
		m_bmButtonMapProgressbar.Attach( hButtonMap );
		m_bmButtonMapProgressbar.GetObject( sizeof(BITMAP), &bmInfo );
		m_bmButtonMapProgressbar.SetBitmapDimension(
			bmInfo.bmWidth > Skin.m_nButtonEdge ? Skin.m_nButtonEdge : 0, bmInfo.bmHeight / 3 );
	}

	if ( HBITMAP hButtonMap = Skin.GetWatermark( _T("ButtonMap.DownloadGroup") ) )
	{
		BITMAP bmInfo;
		PreBlend( hButtonMap );
		m_bmButtonMapDownloadgroup.Attach( hButtonMap );
		m_bmButtonMapDownloadgroup.GetObject( sizeof(BITMAP), &bmInfo );
		m_bmButtonMapDownloadgroup.SetBitmapDimension(
			bmInfo.bmWidth > Skin.m_nButtonEdge ? Skin.m_nButtonEdge : 0, bmInfo.bmHeight / STATE_COUNT );
	}

	if ( HBITMAP hButtonMap = Skin.GetWatermark( _T("ButtonMap.Taskbar") ) )
	{
		BITMAP bmInfo;
		PreBlend( hButtonMap );
		m_bmButtonMapTaskbar.Attach( hButtonMap );
		m_bmButtonMapTaskbar.GetObject( sizeof(BITMAP), &bmInfo );
		m_bmButtonMapTaskbar.SetBitmapDimension(
			bmInfo.bmWidth > Skin.m_nButtonEdge ? Skin.m_nButtonEdge : 0, bmInfo.bmHeight / STATE_COUNT );
	}
}

BOOL CImages::PreBlend(HBITMAP hButton)
{
	BITMAP pInfo;
	static CBitmap *spBitMap;
	spBitMap = CBitmap::FromHandle( hButton );
	spBitMap->GetBitmap( &pInfo );

	if ( pInfo.bmBitsPixel != 32 )
		return FALSE;

	// Pre-multiply for AlphaBlend transparency support (rgba from PNG)

	const int bufferSize = pInfo.bmWidthBytes * pInfo.bmHeight;
	BYTE* buffer = (BYTE*)malloc(bufferSize);
	GetBitmapBits( hButton, bufferSize, buffer );	// Get/SetBitmapBits() deprecated by MS, but useful here

	for ( int i = 0 ; i < bufferSize ; i += 4 )
	{
		if ( buffer[i + 3] == 255 ) continue;

		buffer[i + 0] = buffer[i + 0] * buffer[i + 3] / 255;
		buffer[i + 1] = buffer[i + 1] * buffer[i + 3] / 255;
		buffer[i + 2] = buffer[i + 2] * buffer[i + 3] / 255;
	}

	SetBitmapBits( hButton, bufferSize, buffer );
	delete buffer;

	return TRUE;
}

BOOL CImages::DrawIconButton(CDC* pDC, CRect rc, CBitmap* bmButton)
{
	if ( ! bmButton->m_hObject || pDC == NULL || rc == NULL )
		return FALSE;

	CSize szButton( bmButton->GetBitmapDimension() );
	if ( szButton.cx > 16 && szButton.cy > 16 && szButton.cx < rc.Width() + 2 && szButton.cy < rc.Height() + 2 )
	{
		if ( ! Skin.m_bmDialog.m_hObject )
			pDC->FillSolidRect( &rc, pDC->GetBkColor() );

		// Set button rect to centered image size
		rc.top += ( rc.Height() - szButton.cy ) / 2;
		rc.left += ( rc.Width() - szButton.cx ) / 2;
		rc.right = rc.left + szButton.cx;
		rc.bottom = rc.top + szButton.cy;
	}

	CDC dcMark;
	dcMark.CreateCompatibleDC( pDC );
	if ( Settings.General.LanguageRTL )
		SetLayout( dcMark.m_hDC, LAYOUT_BITMAPORIENTATIONPRESERVED );

	CBitmap* pOld;
	pOld = (CBitmap*)dcMark.SelectObject( bmButton );

	for ( int nY = rc.top ; nY < rc.bottom ; nY += szButton.cy )
	{
		for ( int nX = rc.left ; nX < rc.right ; nX += szButton.cx )
		{
			pDC->BitBlt( nX, nY,
				min( szButton.cx, rc.right - nX ),
				min( szButton.cy, rc.bottom - nY ),
				&dcMark, 0, 0, SRCCOPY );
		}
	}

	dcMark.SelectObject( pOld );
	dcMark.DeleteDC();

	return TRUE;
}

BOOL CImages::DrawButton(CDC* pDC, const CRect rc, CBitmap* bmButton, CBitmap* bmButtonEdge, BOOL bRTL)
{
	if ( ! bmButton->m_hObject || pDC == NULL || rc == NULL )
		return FALSE;

	CDC dcMark;
	dcMark.CreateCompatibleDC( pDC );
	if ( Settings.General.LanguageRTL )
		SetLayout( dcMark.m_hDC, bRTL ? LAYOUT_RTL : LAYOUT_BITMAPORIENTATIONPRESERVED );

	CBitmap* pOld;
	pOld = (CBitmap*)dcMark.SelectObject( bmButton );

	BLENDFUNCTION bf;				// Set now if needed
	bf.BlendFlags = 0;				// Must be zero
	bf.BlendOp = AC_SRC_OVER;		// Must be defined
	bf.AlphaFormat = AC_SRC_ALPHA;	// Use bitmap alpha
	bf.SourceConstantAlpha = 0xFF;	// Opaque (Bitmap alpha only)

	const int nEdge = ( bmButtonEdge && bmButtonEdge->m_hObject ) ? bmButtonEdge->GetBitmapDimension().cx : 0;

	if ( rc.Width() > nEdge )
	{
		BITMAP pInfo;
		bmButton->GetBitmap( &pInfo );

		for ( int nY = rc.top ; nY < rc.bottom ; nY += pInfo.bmHeight )
		{
			for ( int nX = rc.left ; nX < rc.right - nEdge ; nX += pInfo.bmWidth )
			{
				const int nWidth  = min( pInfo.bmWidth, rc.right - nX - nEdge );
				const int nHeight = min( pInfo.bmHeight, rc.bottom - nY ); 	// No repeat (+ 1 to allow 1px overdraw?)

				if ( pInfo.bmBitsPixel == 32 )		// (Pre-multiplied for AlphaBlend Transparency)
					pDC->AlphaBlend( nX, nY, nWidth, nHeight, &dcMark, 0, 0, nWidth, nHeight, bf );
				else
					pDC->BitBlt( nX, nY, nWidth, nHeight, &dcMark, 0, 0, SRCCOPY );
			}
		}
	}

	if ( nEdge > 0 )
	{
		BITMAP pInfo;
		bmButtonEdge->GetBitmap( &pInfo );
		dcMark.SelectObject( bmButtonEdge );

		const int nHeight = min( pInfo.bmHeight, rc.Height() ); 	// No repeat (+ 1 to allow 1px overdraw?)

		if ( pInfo.bmBitsPixel == 32 )
			pDC->AlphaBlend( rc.right - nEdge, rc.top, nEdge, nHeight, &dcMark, 0, 0, nEdge, nHeight, bf );
		else
			pDC->BitBlt( rc.right - nEdge, rc.top, nEdge, nHeight, &dcMark, 0, 0, SRCCOPY );
	}

	dcMark.SelectObject( pOld );
	dcMark.DeleteDC();

	return TRUE;
}

BOOL CImages::DrawButtonMap(CDC* pDC, const CRect rc, CBitmap* bmButtonMap, const int nState, BOOL bRTL)
{
	if ( ! bmButtonMap->m_hObject || pDC == NULL || rc == NULL )
		return FALSE;

	BITMAP pInfo;
	bmButtonMap->GetBitmap( &pInfo );

	const int nSourceHeight	= bmButtonMap->GetBitmapDimension().cy;
	const int nPosition 	= nSourceHeight * nState;
	const int nEdge			= bmButtonMap->GetBitmapDimension().cx;
	const int nSourceWidth	= pInfo.bmWidth - nEdge;

	if ( pInfo.bmBitsPixel == 32 && nState == STATE_DEFAULT )	// Test for empty inactive button
	{
		// ToDo: Faster/smarter way to check if available button state should be drawn now?
		BOOL bEmpty = TRUE;
		const int bufferSize = pInfo.bmWidthBytes * pInfo.bmHeight;
		BYTE* buffer = (BYTE*)malloc(bufferSize);
		GetBitmapBits( (HBITMAP)bmButtonMap->m_hObject, bufferSize, buffer );	// Deprecated function but useful

		for ( int i = 3 ; i < ( pInfo.bmWidthBytes * ( nSourceHeight - 1 ) ) ; i += 4 )
		{
			if ( buffer[i] > 1 )	// Non-zero alpha
			{
				bEmpty = FALSE;
				break;
			}
		}

		delete buffer;
		if ( bEmpty ) return FALSE;
	}

	CDC dcMark;
	dcMark.CreateCompatibleDC( pDC );
	if ( Settings.General.LanguageRTL )
		SetLayout( dcMark.m_hDC, bRTL ? LAYOUT_RTL : LAYOUT_BITMAPORIENTATIONPRESERVED );

	CBitmap* pOld;
	pOld = (CBitmap*)dcMark.SelectObject( bmButtonMap );

	BLENDFUNCTION bf;				// Set now if needed
	bf.BlendOp = AC_SRC_OVER;		// Must be defined
	bf.BlendFlags = 0;				// Must be zero
	bf.SourceConstantAlpha = 0xFF;	// Opaque (Bitmap alpha only)
	bf.AlphaFormat = AC_SRC_ALPHA;	// Use bitmap alpha

	if ( rc.Width() > nEdge )
	{
		for ( int nY = rc.top ; nY < rc.bottom ; nY += nSourceHeight )
		{
			for ( int nX = rc.left ; nX < rc.right - nEdge ; nX += nSourceWidth )
			{
				const int nWidth  = min( nSourceWidth, rc.right - nX - nEdge );
				const int nHeight = min( nSourceHeight, rc.bottom - nY ); 	// No repeat (+ 1 to allow 1px overdraw?)

				if ( pInfo.bmBitsPixel == 32 )		// (Pre-multiplied for AlphaBlend Transparency)
					pDC->AlphaBlend( nX, nY, nWidth, nHeight, &dcMark, 0, nPosition, nWidth, nHeight, bf );
				else
					pDC->BitBlt( nX, nY, nWidth, nHeight, &dcMark, 0, nPosition, SRCCOPY );
			}
		}
	}

	if ( nEdge > 0 )
	{
		const int nHeight = min( nSourceHeight, rc.Height() ); 	// No repeat (+ 1 to allow 1px overdraw?)

		if ( pInfo.bmBitsPixel == 32 )
			pDC->AlphaBlend( rc.right - nEdge, rc.top, nEdge, nHeight, &dcMark, nSourceWidth, nPosition, nEdge, nHeight, bf );
		else
			pDC->BitBlt( rc.right - nEdge, rc.top, nEdge, nHeight, &dcMark, nSourceWidth, nPosition, SRCCOPY );
	}

	dcMark.SelectObject( pOld );
	dcMark.DeleteDC();

	return TRUE;
}

BOOL CImages::DrawButtonState(CDC* pDC, const CRect rc, const int nResource)
{
	if ( pDC == NULL || rc == NULL )
		return FALSE;

	// Abstracted pass-through for convenience/consistency elsewhere:

	switch( nResource )
	{
	case IMAGE_BANNER:
		if ( m_nBanner < 2 ) return FALSE;
		return DrawButton(pDC, rc, &m_bmBanner, &m_bmBannerEdge);
	case IMAGE_DIALOG:
		return DrawButton( pDC, rc, &m_bmDialog );		// ToDo: m_bmDialogEdge?
	case IMAGE_DIALOGPANEL:
		return DrawButton( pDC, rc, &m_bmDialogPanel );	// ToDo: m_bmDialogPanelEdge?
	case IMAGE_PANELMARK:
		return DrawButton( pDC, rc, &m_bmPanelMark );
	case IMAGE_TOOLTIP:
		return DrawButton( pDC, rc, &m_bmToolTip );		// ToDo: &m_bmToolTipEdge?

	case IMAGE_SELECTED:	// + IMAGE_HIGHLIGHT
		return DrawButton( pDC, rc, &m_bmSelected );	// ToDo: &m_bmSelectedEdge?
	case IMAGE_MENUSELECTED:
		return DrawButton( pDC, rc, &m_bmMenuSelected, &m_bmMenuSelectedEdge, TRUE ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapMenuselect, STATE_HOVER, TRUE );
	case IMAGE_MENUDISABLED:
		return DrawButton( pDC, rc, &m_bmMenuDisabled, &m_bmMenuDisabledEdge, TRUE ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapMenuselect, STATE_PRESS, TRUE );
	case IMAGE_PROGRESSBAR:
		return DrawButton( pDC, rc, &m_bmProgress, &m_bmProgressEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapProgressbar, STATE_HOVER );
	case IMAGE_PROGRESSBAR_NONE:
		return DrawButton( pDC, rc, &m_bmProgressNone ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapProgressbar, STATE_DEFAULT );
	case IMAGE_PROGRESSBAR_SHADED:
		return DrawButton( pDC, rc, &m_bmProgressShaded ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapProgressbar, STATE_PRESS );

	case ICONBUTTON_DEFAULT:
		return DrawIconButton( pDC, rc, &m_bmIconButton ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapIconbox, STATE_DEFAULT );
	case ICONBUTTON_HOVER:
		return DrawIconButton( pDC, rc, &m_bmIconButtonHover ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapIconbox, STATE_HOVER );
	case ICONBUTTON_PRESS:
		return DrawIconButton( pDC, rc, &m_bmIconButtonPress ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapIconbox, STATE_PRESS );
	case ICONBUTTON_ACTIVE:
		return DrawIconButton( pDC, rc, &m_bmIconButtonActive ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapIconbox, STATE_ACTIVE );
	case ICONBUTTON_DISABLED:
		return DrawIconButton( pDC, rc, &m_bmIconButtonDisabled )||
			DrawButtonMap( pDC, rc, &m_bmButtonMapIconbox, STATE_DISABLED );

	case RICHBUTTON_DEFAULT:
		return DrawButton( pDC, rc, &m_bmRichButton, &m_bmRichButtonEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapRichdoc, STATE_DEFAULT );
	case RICHBUTTON_HOVER:
		return DrawButton( pDC, rc, &m_bmRichButtonHover, &m_bmRichButtonHoverEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapRichdoc, STATE_HOVER );
	case RICHBUTTON_PRESS:
		return DrawButton( pDC, rc, &m_bmRichButtonPress, &m_bmRichButtonPressEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapRichdoc, STATE_PRESS );
	case RICHBUTTON_ACTIVE:
		return DrawButton( pDC, rc, &m_bmRichButtonActive, &m_bmRichButtonActiveEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapRichdoc, STATE_ACTIVE );
	case RICHBUTTON_DISABLED:
		return DrawButton( pDC, rc, &m_bmRichButtonDisabled, &m_bmRichButtonDisabledEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapRichdoc, STATE_DISABLED );

	case TOOLBARBUTTON_DEFAULT:
		return DrawButton( pDC, rc, &m_bmToolbarButton, &m_bmToolbarButtonEdge, TRUE ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapToolbar, STATE_DEFAULT, TRUE );
	case TOOLBARBUTTON_HOVER:
		return DrawButton( pDC, rc, &m_bmToolbarButtonHover, &m_bmToolbarButtonHoverEdge, TRUE ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapToolbar, STATE_HOVER, TRUE );
	case TOOLBARBUTTON_PRESS:
		return DrawButton( pDC, rc, &m_bmToolbarButtonPress, &m_bmToolbarButtonPressEdge, TRUE ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapToolbar, STATE_PRESS, TRUE );
	case TOOLBARBUTTON_ACTIVE:
		return DrawButton( pDC, rc, &m_bmToolbarButtonActive, &m_bmToolbarButtonActiveEdge, TRUE ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapToolbar, STATE_ACTIVE, TRUE );
	case TOOLBARBUTTON_DISABLED:
		return DrawButton( pDC, rc, &m_bmToolbarButtonDisabled, &m_bmToolbarButtonDisabledEdge, TRUE ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapToolbar, STATE_DISABLED, TRUE );
	case TOOLBAR_SEPARATOR:
		return DrawButton( pDC, rc, &m_bmToolbarSeparator );

	case MENUBARITEM_DEFAULT:
		return DrawButton( pDC, rc, &m_bmMenubarItem, &m_bmMenubarItemEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapMenutext, STATE_DEFAULT );
	case MENUBARITEM_HOVER:
		return DrawButton( pDC, rc, &m_bmMenubarItemHover, &m_bmMenubarItemHoverEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapMenutext, STATE_HOVER );
	case MENUBARITEM_PRESS:
		return DrawButton( pDC, rc, &m_bmMenubarItemPress, &m_bmMenubarItemPressEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapMenutext, STATE_PRESS );
	case MENUBARBUTTON_DEFAULT:
		return DrawButton( pDC, rc, &m_bmMenubarButton, &m_bmMenubarButtonEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapMenubar, STATE_DEFAULT );
	case MENUBARBUTTON_HOVER:
		return DrawButton( pDC, rc, &m_bmMenubarButtonHover, &m_bmMenubarButtonHoverEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapMenubar, STATE_HOVER );
	case MENUBARBUTTON_PRESS:
		return DrawButton( pDC, rc, &m_bmMenubarButtonPress, &m_bmMenubarButtonPressEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapMenubar, STATE_PRESS );
	case MENUBARBUTTON_ACTIVE:
		return DrawButton( pDC, rc, &m_bmMenubarButtonActive, &m_bmMenubarButtonActiveEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapMenubar, STATE_ACTIVE );

	case TASKBARBUTTON_DEFAULT:
		return DrawButton( pDC, rc, &m_bmTaskbarButton, &m_bmTaskbarButtonEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapTaskbar, STATE_DEFAULT );
	case TASKBARBUTTON_HOVER:
		return DrawButton( pDC, rc, &m_bmTaskbarButtonHover, &m_bmTaskbarButtonHoverEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapTaskbar, STATE_HOVER );
	case TASKBARBUTTON_PRESS:
		return DrawButton( pDC, rc, &m_bmTaskbarButtonPress, &m_bmTaskbarButtonPressEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapTaskbar, STATE_PRESS );
	case TASKBARBUTTON_ACTIVE:
		return DrawButton( pDC, rc, &m_bmTaskbarButtonActive, &m_bmTaskbarButtonActiveEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapTaskbar, STATE_ACTIVE );

	case DOWNLOADGROUP_DEFAULT:
		return DrawButton( pDC, rc, &m_bmDownloadGroup, &m_bmDownloadGroupEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapDownloadgroup, STATE_DEFAULT );
	case DOWNLOADGROUP_HOVER:
		return DrawButton( pDC, rc, &m_bmDownloadGroupHover, &m_bmDownloadGroupHoverEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapDownloadgroup, STATE_HOVER );
	case DOWNLOADGROUP_PRESS:
		return DrawButton( pDC, rc, &m_bmDownloadGroupPress, &m_bmDownloadGroupPressEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapDownloadgroup, STATE_PRESS );
	case DOWNLOADGROUP_ACTIVE:
		return DrawButton( pDC, rc, &m_bmDownloadGroupActive, &m_bmDownloadGroupActiveEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapDownloadgroup, STATE_ACTIVE );
	case DOWNLOADGROUP_DISABLED:
		return DrawButton( pDC, rc, &m_bmDownloadGroupDisabled, &m_bmDownloadGroupDisabledEdge ) ||
			DrawButtonMap( pDC, rc, &m_bmButtonMapDownloadgroup, STATE_DISABLED );

	default:
		ASSERT( FALSE );
		return FALSE;
	}
}
