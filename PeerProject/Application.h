//
// Application.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2008.
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


class CApplication : public CComObject
{
	DECLARE_DYNCREATE(CApplication)

public:
	CApplication();
	virtual ~CApplication();

	static HRESULT GetApp(IApplication** ppIApplication) throw();
	static HRESULT GetUI(IUserInterface** ppIUserInterface) throw();
	static HRESULT GetSettings(ISettings** ppISettings) throw();

// IApplication
protected:
	BEGIN_INTERFACE_PART(Application, IApplication)
		DECLARE_DISPATCH()
		STDMETHOD(get_Application)(IApplication FAR* FAR* ppApplication);
		STDMETHOD(get_Version)(BSTR FAR* psVersion);
		STDMETHOD(CheckVersion)(BSTR sVersion);
		STDMETHOD(CreateXML)(ISXMLElement FAR* FAR* ppXML);
		STDMETHOD(get_UserInterface)(IUserInterface FAR* FAR* ppUserInterface);
		STDMETHOD(get_Library)(ILibrary FAR* FAR* ppLibrary);
		STDMETHOD(get_Settings)(ISettings FAR* FAR* ppSettings);
		STDMETHOD(get_ImageService)(IImageServicePlugin FAR* FAR* ppImageService);
	END_INTERFACE_PART(Application)

	BEGIN_INTERFACE_PART(UserInterface, IUserInterface)
		DECLARE_DISPATCH()
		STDMETHOD(get_Application)(IApplication FAR* FAR* ppApplication);
		STDMETHOD(get_UserInterface)(IUserInterface FAR* FAR* ppUserInterface);
		STDMETHOD(NewWindow)(BSTR bsName, IPluginWindowOwner FAR* pOwner, IPluginWindow FAR* FAR* ppWindow);
		STDMETHOD(get_MainWindowHwnd)(HWND FAR* phWnd);
		STDMETHOD(get_ActiveView)(IGenericView FAR* FAR* ppView);
		STDMETHOD(RegisterCommand)(BSTR bsName, HICON hIcon, UINT* pnCommandID);
		STDMETHOD(AddFromString)(BSTR sXML);
		STDMETHOD(AddFromResource)(HINSTANCE hInstance, UINT nID);
		STDMETHOD(AddFromXML)(ISXMLElement FAR* pXML);
		STDMETHOD(GetMenu)(BSTR bsName, VARIANT_BOOL bCreate, ISMenu FAR* FAR* ppMenu);
		STDMETHOD(GetToolbar)(BSTR bsName, VARIANT_BOOL bCreate, ISToolbar FAR* FAR* ppToolbar);
	END_INTERFACE_PART(UserInterface)

	BEGIN_INTERFACE_PART(Settings, ISettings)
		DECLARE_DISPATCH()
		STDMETHOD(GetValue)(VARIANT* value);	// Pass as BSTR path (ex. Gnutella2.EnableAlways, get back the actual value
	END_INTERFACE_PART(Settings)

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CApplication)
	DECLARE_INTERFACE_MAP()
};
