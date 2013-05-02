//
// UPnPFinder.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software. You may redistribute and/or modify it
// under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// version 3 or later at your option. (AGPLv3)
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

typedef CComPtr< IUPnPDeviceFinder > FinderPointer;
typedef CComPtr< IUPnPDevice > DevicePointer;
typedef CComPtr< IUPnPService > ServicePointer;

CString translateUPnPResult(HRESULT hr);
HRESULT UPnPMessage(HRESULT hr);

class CUPnPFinder
{
// Construction
public:
	CUPnPFinder();
	~CUPnPFinder();

public:
	void StartDiscovery(bool bSecondTry=false);
	void StopAsyncFind();
	void DeletePorts();
	void AddDevice(DevicePointer pDevice, bool bAddChilds, int nLevel = 0);
	void RemoveDevice(CComBSTR bsUDN);
	bool OnSearchComplete();
	bool Init();

	inline bool IsAsyncFindRunning()
	{
		if ( m_pDeviceFinder && m_bAsyncFindRunning )
		{
			if ( GetTickCount() > m_tLastEvent + 16000 )	 // Timeout ~20 seconds
			{
				m_pDeviceFinder->CancelAsyncFind( m_nAsyncFindHandle );
				m_bAsyncFindRunning = false;
			}
			SafeMessageLoop();
		}

		return m_bAsyncFindRunning;
	}

// Implementation
private:
	static FinderPointer CreateFinderInstance() throw();

	struct FindDevice : public std::unary_function< DevicePointer, bool >
	{
		FindDevice(const CComBSTR& udn) : m_udn( udn ) {}
		result_type operator()(argument_type device) const
		{
			CComBSTR deviceName;
			HRESULT hr = device->get_UniqueDeviceName( &deviceName );

			if ( FAILED( hr ) )
				return UPnPMessage( hr ), false;

			return wcscmp( deviceName.m_str, m_udn ) == 0;
		}
		CComBSTR m_udn;
	};

	void	ProcessAsyncFind(BSTR bsSearchType) throw();
	HRESULT	GetDeviceServices(DevicePointer pDevice);
	void	StartPortMapping();
	HRESULT	MapPort(const ServicePointer& service);
	void	DeleteExistingPortMappings(ServicePointer pService);
	void	CreatePortMappings(ServicePointer pService);
	HRESULT SaveServices(CComPtr< IEnumUnknown >, const LONG nTotalItems);

	HRESULT InvokeAction(ServicePointer pService, CComBSTR action,
		LPCTSTR pszInArgString, CString& strResult);

	// Utility functions
	HRESULT CreateSafeArray(const VARTYPE vt, const ULONG nArgs, SAFEARRAY** ppsa);
	INT_PTR CreateVarFromString(const CString& strArgs, VARIANT*** pppVars);
	INT_PTR	GetStringFromOutArgs(const VARIANT* pvaOutArgs, CString& strArgs);
	void	DestroyVars(const INT_PTR nCount, VARIANT*** pppVars);
	HRESULT GetSafeArrayBounds(SAFEARRAY* psa, LONG* pLBound, LONG* pUBound);
	HRESULT GetVariantElement(SAFEARRAY* psa, LONG pos, VARIANT* pvar);
	CString	GetLocalRoutableIP(ServicePointer pService);

// Public members
public:
	DWORD	m_tLastEvent;	// When the last event was received?

// Private members
private:
	std::vector< CAdapt< DevicePointer > >	m_pDevices;
	std::vector< CAdapt< ServicePointer> >	m_pServices;
	FinderPointer m_pDeviceFinder;
	CComPtr< IUPnPDeviceFinderCallback >	m_pDeviceFinderCallback;
	CComPtr< IUPnPServiceCallback > 		m_pServiceCallback;

	LONG	m_nAsyncFindHandle;
	bool	m_bAsyncFindRunning;
	bool	m_bPortIsFree;
	CString m_sLocalIP;
	CString m_sExternalIP;
	bool	m_bADSL;		// Is the device ADSL?
	bool	m_bADSLFailed;	// Did port mapping failed for the ADSL device?
	bool	m_bInited;
	bool	m_bSecondTry;
	bool	m_bDisableWANIPSetup;
	bool	m_bDisableWANPPPSetup;
	ServicePointer m_pWANIPService;
};

// DeviceFinder Callback
class CDeviceFinderCallback
	: public IUnknownImplementation< IUPnPDeviceFinderCallback >
{
public:
	CDeviceFinderCallback(CUPnPFinder& instance)
		: m_instance( instance )
	{}

private:
	CUPnPFinder& m_instance;

// Implementation
private:
	HRESULT __stdcall DeviceAdded(LONG nFindData, IUPnPDevice* pDevice);
	HRESULT __stdcall DeviceRemoved(LONG nFindData, BSTR bsUDN);
	HRESULT __stdcall SearchComplete(LONG nFindData);
};

// Service Callback
class CServiceCallback
	: public IUnknownImplementation< IUPnPServiceCallback >
{
public:
	CServiceCallback(CUPnPFinder& instance)
		: m_instance( instance )
	{}

private:
	CUPnPFinder& m_instance;

// Implementation
private:
	HRESULT __stdcall StateVariableChanged(IUPnPService* pService, LPCWSTR pszStateVarName, VARIANT varValue);
	HRESULT __stdcall ServiceInstanceDied(IUPnPService* pService);
};
