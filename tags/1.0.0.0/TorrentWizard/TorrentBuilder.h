//
// TorrentBuilder.h
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2007.
//
// PeerProject Torrent Wizard is free software; you can redistribute it
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// Torrent Wizard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#pragma once


class CTorrentBuilder : public CWinThread
{
// Construction
public:
	CTorrentBuilder();
	virtual ~CTorrentBuilder();

	DECLARE_DYNCREATE(CTorrentBuilder)

// Operations
public:
	BOOL	SetName(LPCTSTR pszName);
	BOOL	SetOutputFile(LPCTSTR pszPath);
	void	SetPieceSize(int nPieceIndex);
	void	Enable(BOOL bSHA1, BOOL bED2K, BOOL bMD5);
	BOOL	AddFile(LPCTSTR pszPath);
	BOOL	AddTrackerURL(LPCTSTR pszURL);
	BOOL	AddTrackerURL2(LPCTSTR pszURL);
	BOOL	SetComment(LPCTSTR pszComment);
public:
	BOOL	Start();
	void	Stop();
	BOOL	SetPriority(int nPriority);
	BOOL	IsRunning();
	BOOL	IsFinished();
	BOOL	GetTotalProgress(DWORD& nPosition, DWORD& nScale);
	BOOL	GetCurrentFile(CString& strFile);
	BOOL	GetMessageString(CString& strMessage);
protected:
	BOOL	ScanFiles();
	BOOL	ProcessFiles();
	BOOL	ProcessFile(DWORD nFile, LPCTSTR pszFile);
	BOOL	WriteOutput();

// Attributes
protected:
	CCriticalSection	m_pSection;
	BOOL				m_bActive;
	BOOL				m_bFinished;
	BOOL				m_bAbort;
	CString				m_sMessage;
	CString				m_sName;
	CString				m_sOutput;
	CString				m_sTracker;
	CString				m_sTracker2;
	CString				m_sComment;
	CStringList			m_pFiles;
	CString				m_sThisFile;
	QWORD				m_nTotalSize;
	QWORD				m_nTotalPos;
	BOOL				m_bSHA1;		// Enable SHA1 creation
	BOOL				m_bED2K;		// Enable MD4 creation
	BOOL				m_bMD5;			// Enable MD5 creation
	CSHA				m_oDataSHA1;	// Total SHA1
	CED2K				m_oDataED2K;	// Total MD4
	CMD5				m_oDataMD5;		// Total MD5
	QWORD*				m_pFileSize;
	CSHA*				m_pFileSHA1;	// SHA1 per file
	CED2K*				m_pFileED2K;	// MD4 per file
	CMD5*				m_pFileMD5;		// MD5 per file
	CSHA*				m_pPieceSHA1;	// BitTorrent SHA1 per piece
	CSHA				m_oPieceSHA1;	// BitTorrent piece SHA1 (temporary)
	DWORD				m_nPieceSize;
	DWORD				m_nPieceCount;
	DWORD				m_nPiecePos;
	DWORD				m_nPieceUsed;
	BOOL				m_bAutoPieces;
	BYTE*				m_pBuffer;
	DWORD				m_nBuffer;

// Overrides
public:
	virtual BOOL InitInstance() { return TRUE; }
	virtual int Run();

	DECLARE_MESSAGE_MAP()
};
