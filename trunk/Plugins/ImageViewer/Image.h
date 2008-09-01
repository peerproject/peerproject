//
// Image.h
//
// This file is part of PeerProject (peerproject.org) � 2008
// Original author Michael Stokes released portions into the public domain. 
// You are free to redistribute and modify this page without any restrictions.
//

#if !defined(AFX_IMAGE_H__7C2E1D58_91A7_4C3B_A9FA_34A5768C82DC__INCLUDED_)
#define AFX_IMAGE_H__7C2E1D58_91A7_4C3B_A9FA_34A5768C82DC__INCLUDED_

#pragma once


class CImage  
{
// Construction
public:
	CImage();
	virtual ~CImage();
	
// Attributes
public:
	BYTE*	m_pImage;		// Pointer to image data
	int		m_nWidth;		// Width
	int		m_nHeight;		// Height
	int		m_nComponents;	// Components (1=mono, 3=RGB, 4=RGBA)
	BOOL	m_bPartial;		// Is it partially loaded?
	
// Operations
public:
	BOOL	Load(LPCTSTR pszFile);
	void	Clear();
	BOOL	EnsureRGB(COLORREF crFill = 0xFFFFFFFF);
	BOOL	MonoToRGB();
	BOOL	AlphaToRGB(COLORREF crFill);
	HBITMAP	Resample(int nWidth, int nHeight);
	
// Internal Helpers
protected:
	IImageServicePlugin*	LoadService(LPCTSTR pszFile);
	
};

#endif // !defined(AFX_IMAGE_H__7C2E1D58_91A7_4C3B_A9FA_34A5768C82DC__INCLUDED_)
