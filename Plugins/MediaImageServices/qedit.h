//
// qedit.h : Workaround for missing file in Microsoft Windows 7 SDK +
//

#pragma once

#ifndef __qedit_h__
#define __qedit_h__
#endif

#include <rpc.h>
#include <rpcndr.h>


// Forward Declarations 

#ifndef __IPropertySetter_FWD_DEFINED__
#define __IPropertySetter_FWD_DEFINED__
typedef interface IPropertySetter IPropertySetter;
#endif

#ifndef __IDxtCompositor_FWD_DEFINED__
#define __IDxtCompositor_FWD_DEFINED__
//typedef interface IDxtCompositor IDxtCompositor;
#endif

#ifndef __IDxtAlphaSetter_FWD_DEFINED__
#define __IDxtAlphaSetter_FWD_DEFINED__
//typedef interface IDxtAlphaSetter IDxtAlphaSetter;
#endif

#ifndef __IDxtJpeg_FWD_DEFINED__
#define __IDxtJpeg_FWD_DEFINED__
//typedef interface IDxtJpeg IDxtJpeg;
#endif

#ifndef __IDxtKey_FWD_DEFINED__
#define __IDxtKey_FWD_DEFINED__
//typedef interface IDxtKey IDxtKey;
#endif

#ifndef __IMediaLocator_FWD_DEFINED__
#define __IMediaLocator_FWD_DEFINED__
typedef interface IMediaLocator IMediaLocator;
#endif

#ifndef __IMediaDet_FWD_DEFINED__
#define __IMediaDet_FWD_DEFINED__
typedef interface IMediaDet IMediaDet;
#endif

#ifndef __IGrfCache_FWD_DEFINED__
#define __IGrfCache_FWD_DEFINED__
typedef interface IGrfCache IGrfCache;
#endif

#ifndef __IRenderEngine_FWD_DEFINED__
#define __IRenderEngine_FWD_DEFINED__
typedef interface IRenderEngine IRenderEngine;
#endif

#ifndef __IRenderEngine2_FWD_DEFINED__
#define __IRenderEngine2_FWD_DEFINED__
typedef interface IRenderEngine2 IRenderEngine2;
#endif

#ifndef __IFindCompressorCB_FWD_DEFINED__
#define __IFindCompressorCB_FWD_DEFINED__
typedef interface IFindCompressorCB IFindCompressorCB;
#endif

#ifndef __ISmartRenderEngine_FWD_DEFINED__
#define __ISmartRenderEngine_FWD_DEFINED__
typedef interface ISmartRenderEngine ISmartRenderEngine;
#endif

#ifndef __IAMTimelineObj_FWD_DEFINED__
#define __IAMTimelineObj_FWD_DEFINED__
typedef interface IAMTimelineObj IAMTimelineObj;
#endif

#ifndef __IAMTimelineEffectable_FWD_DEFINED__
#define __IAMTimelineEffectable_FWD_DEFINED__
typedef interface IAMTimelineEffectable IAMTimelineEffectable;
#endif

#ifndef __IAMTimelineEffect_FWD_DEFINED__
#define __IAMTimelineEffect_FWD_DEFINED__
typedef interface IAMTimelineEffect IAMTimelineEffect;
#endif

#ifndef __IAMTimelineTransable_FWD_DEFINED__
#define __IAMTimelineTransable_FWD_DEFINED__
typedef interface IAMTimelineTransable IAMTimelineTransable;
#endif

#ifndef __IAMTimelineSplittable_FWD_DEFINED__
#define __IAMTimelineSplittable_FWD_DEFINED__
typedef interface IAMTimelineSplittable IAMTimelineSplittable;
#endif

#ifndef __IAMTimelineTrans_FWD_DEFINED__
#define __IAMTimelineTrans_FWD_DEFINED__
typedef interface IAMTimelineTrans IAMTimelineTrans;
#endif

#ifndef __IAMTimelineSrc_FWD_DEFINED__
#define __IAMTimelineSrc_FWD_DEFINED__
typedef interface IAMTimelineSrc IAMTimelineSrc;
#endif

#ifndef __IAMTimelineTrack_FWD_DEFINED__
#define __IAMTimelineTrack_FWD_DEFINED__
typedef interface IAMTimelineTrack IAMTimelineTrack;
#endif

#ifndef __IAMTimelineVirtualTrack_FWD_DEFINED__
#define __IAMTimelineVirtualTrack_FWD_DEFINED__
typedef interface IAMTimelineVirtualTrack IAMTimelineVirtualTrack;
#endif

#ifndef __IAMTimelineComp_FWD_DEFINED__
#define __IAMTimelineComp_FWD_DEFINED__
typedef interface IAMTimelineComp IAMTimelineComp;
#endif

#ifndef __IAMTimelineGroup_FWD_DEFINED__
#define __IAMTimelineGroup_FWD_DEFINED__
typedef interface IAMTimelineGroup IAMTimelineGroup;
#endif

#ifndef __IAMTimeline_FWD_DEFINED__
#define __IAMTimeline_FWD_DEFINED__
typedef interface IAMTimeline IAMTimeline;
#endif

#ifndef __IXml2Dex_FWD_DEFINED__
#define __IXml2Dex_FWD_DEFINED__
typedef interface IXml2Dex IXml2Dex;
#endif

#ifndef __IAMErrorLog_FWD_DEFINED__
#define __IAMErrorLog_FWD_DEFINED__
typedef interface IAMErrorLog IAMErrorLog;
#endif

#ifndef __IAMSetErrorLog_FWD_DEFINED__
#define __IAMSetErrorLog_FWD_DEFINED__
typedef interface IAMSetErrorLog IAMSetErrorLog;
#endif

#ifndef __ISampleGrabberCB_FWD_DEFINED__
#define __ISampleGrabberCB_FWD_DEFINED__
typedef interface ISampleGrabberCB ISampleGrabberCB;
#endif

#ifndef __ISampleGrabber_FWD_DEFINED__
#define __ISampleGrabber_FWD_DEFINED__
typedef interface ISampleGrabber ISampleGrabber;
#endif

#ifndef __IResize_FWD_DEFINED__
#define __IResize_FWD_DEFINED__
typedef interface IResize IResize;
#endif


//#ifdef __cplusplus

#ifndef __AMTimeline_FWD_DEFINED__
#define __AMTimeline_FWD_DEFINED__
typedef class AMTimeline AMTimeline;
#endif

#ifndef __AMTimelineObj_FWD_DEFINED__
#define __AMTimelineObj_FWD_DEFINED__
typedef class AMTimelineObj AMTimelineObj;
#endif

#ifndef __AMTimelineSrc_FWD_DEFINED__
#define __AMTimelineSrc_FWD_DEFINED__
typedef class AMTimelineSrc AMTimelineSrc;
#endif

#ifndef __AMTimelineTrack_FWD_DEFINED__
#define __AMTimelineTrack_FWD_DEFINED__
typedef class AMTimelineTrack AMTimelineTrack;
#endif

#ifndef __AMTimelineComp_FWD_DEFINED__
#define __AMTimelineComp_FWD_DEFINED__
typedef class AMTimelineComp AMTimelineComp;
#endif

#ifndef __AMTimelineGroup_FWD_DEFINED__
#define __AMTimelineGroup_FWD_DEFINED__
typedef class AMTimelineGroup AMTimelineGroup;
#endif

#ifndef __AMTimelineTrans_FWD_DEFINED__
#define __AMTimelineTrans_FWD_DEFINED__
typedef class AMTimelineTrans AMTimelineTrans;
#endif

#ifndef __AMTimelineEffect_FWD_DEFINED__
#define __AMTimelineEffect_FWD_DEFINED__
typedef class AMTimelineEffect AMTimelineEffect;
#endif

#ifndef __RenderEngine_FWD_DEFINED__
#define __RenderEngine_FWD_DEFINED__
typedef class RenderEngine RenderEngine;
#endif

#ifndef __SmartRenderEngine_FWD_DEFINED__
#define __SmartRenderEngine_FWD_DEFINED__
typedef class SmartRenderEngine SmartRenderEngine;
#endif

#ifndef __AudMixer_FWD_DEFINED__
#define __AudMixer_FWD_DEFINED__
typedef class AudMixer AudMixer;
#endif

#ifndef __Xml2Dex_FWD_DEFINED__
#define __Xml2Dex_FWD_DEFINED__
typedef class Xml2Dex Xml2Dex;
#endif

#ifndef __MediaLocator_FWD_DEFINED__
#define __MediaLocator_FWD_DEFINED__
typedef class MediaLocator MediaLocator;
#endif

#ifndef __PropertySetter_FWD_DEFINED__
#define __PropertySetter_FWD_DEFINED__
typedef class PropertySetter PropertySetter;
#endif

#ifndef __MediaDet_FWD_DEFINED__
#define __MediaDet_FWD_DEFINED__
typedef class MediaDet MediaDet;
#endif

#ifndef __SampleGrabber_FWD_DEFINED__
#define __SampleGrabber_FWD_DEFINED__
typedef class SampleGrabber SampleGrabber;
#endif

#ifndef __NullRenderer_FWD_DEFINED__
#define __NullRenderer_FWD_DEFINED__
typedef class NullRenderer NullRenderer;
#endif

#ifndef __ColorSource_FWD_DEFINED__
#define __ColorSource_FWD_DEFINED__
typedef class ColorSource ColorSource;
#endif



// Header files for imported files
#include "oaidl.h"
#include "ocidl.h"
#include "amstream.h"
//#include "dxtrans.h"

//#ifdef __cplusplus
extern "C"{


/* interface __MIDL_itf_qedit_0000_0000 */
/* [local] */ 


typedef /* [public] */ 
enum __MIDL___MIDL_itf_qedit_0000_0000_0001
	{	DEXTERF_JUMP	= 0,
	DEXTERF_INTERPOLATE	= ( DEXTERF_JUMP + 1 ) 
	} 	DEXTERF;

typedef /* [public][public][public][public] */ struct __MIDL___MIDL_itf_qedit_0000_0000_0002
	{
	BSTR Name;
	DISPID dispID;
	LONG nValues;
	} 	DEXTER_PARAM;

typedef /* [public][public][public][public] */ struct __MIDL___MIDL_itf_qedit_0000_0000_0003
	{
	VARIANT v;
	REFERENCE_TIME rt;
	DWORD dwInterp;
	} 	DEXTER_VALUE;


enum __MIDL___MIDL_itf_qedit_0000_0000_0004
	{	DEXTER_AUDIO_JUMP	= 0,
	DEXTER_AUDIO_INTERPOLATE	= ( DEXTER_AUDIO_JUMP + 1 ) 
	} ;
typedef /* [public] */ struct __MIDL___MIDL_itf_qedit_0000_0000_0005
	{
	REFERENCE_TIME rtEnd;
	double dLevel;
	BOOL bMethod;
	} 	DEXTER_AUDIO_VOLUMEENVELOPE;


enum __MIDL___MIDL_itf_qedit_0000_0000_0006
	{	TIMELINE_INSERT_MODE_INSERT	= 1,
	TIMELINE_INSERT_MODE_OVERLAY	= 2
	} ;
typedef /* [public][public][public][public][public][public][public][public] */ 
enum __MIDL___MIDL_itf_qedit_0000_0000_0007
	{	TIMELINE_MAJOR_TYPE_COMPOSITE	= 1,
	TIMELINE_MAJOR_TYPE_TRACK	= 2,
	TIMELINE_MAJOR_TYPE_SOURCE	= 4,
	TIMELINE_MAJOR_TYPE_TRANSITION	= 8,
	TIMELINE_MAJOR_TYPE_EFFECT	= 16,
	TIMELINE_MAJOR_TYPE_GROUP	= 128
	} 	TIMELINE_MAJOR_TYPE;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_qedit_0000_0000_0008
	{	DEXTERF_BOUNDING	= -1,
	DEXTERF_EXACTLY_AT	= 0,
	DEXTERF_FORWARDS	= 1
	} 	DEXTERF_TRACK_SEARCH_FLAGS;

typedef struct _SCompFmt0
	{
	long nFormatId;
	AM_MEDIA_TYPE MediaType;
	} 	SCompFmt0;


enum __MIDL___MIDL_itf_qedit_0000_0000_0009
	{	RESIZEF_STRETCH	= 0,
	RESIZEF_CROP	= ( RESIZEF_STRETCH + 1 ) ,
	RESIZEF_PRESERVEASPECTRATIO	= ( RESIZEF_CROP + 1 ) ,
	RESIZEF_PRESERVEASPECTRATIO_NOLETTERBOX	= ( RESIZEF_PRESERVEASPECTRATIO + 1 ) 
	} ;

enum __MIDL___MIDL_itf_qedit_0000_0000_0010
	{	CONNECTF_DYNAMIC_NONE	= 0,
	CONNECTF_DYNAMIC_SOURCES	= 0x1,
	CONNECTF_DYNAMIC_EFFECTS	= 0x2
	} ;

enum __MIDL___MIDL_itf_qedit_0000_0000_0011
	{	SFN_VALIDATEF_CHECK	= 0x1,
	SFN_VALIDATEF_POPUP	= 0x2,
	SFN_VALIDATEF_TELLME	= 0x4,
	SFN_VALIDATEF_REPLACE	= 0x8,
	SFN_VALIDATEF_USELOCAL	= 0x10,
	SFN_VALIDATEF_NOFIND	= 0x20,
	SFN_VALIDATEF_IGNOREMUTED	= 0x40,
	SFN_VALIDATEF_END	= ( SFN_VALIDATEF_IGNOREMUTED + 1 ) 
	} ;

enum __MIDL___MIDL_itf_qedit_0000_0000_0012
	{	DXTKEY_RGB	= 0,
	DXTKEY_NONRED	= ( DXTKEY_RGB + 1 ) ,
	DXTKEY_LUMINANCE	= ( DXTKEY_NONRED + 1 ) ,
	DXTKEY_ALPHA	= ( DXTKEY_LUMINANCE + 1 ) ,
	DXTKEY_HUE	= ( DXTKEY_ALPHA + 1 ) 
	} ;


extern RPC_IF_HANDLE __MIDL_itf_qedit_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_qedit_0000_0000_v0_0_s_ifspec;

#ifndef __IPropertySetter_INTERFACE_DEFINED__
#define __IPropertySetter_INTERFACE_DEFINED__

/* interface IPropertySetter */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IPropertySetter;
	
	MIDL_INTERFACE("AE9472BD-B0C3-11D2-8D24-00A0C9441E20")
	IPropertySetter : public IUnknown
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE LoadXML( 
			/* [in] */ __RPC__in_opt IUnknown *pxml) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE PrintXML( 
			/* [out] */ __RPC__out char *pszXML,
			/* [in] */ int cbXML,
			/* [out] */ __RPC__out int *pcbPrinted,
			/* [in] */ int indent) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE CloneProps( 
			/* [out] */ __RPC__deref_out_opt IPropertySetter **ppSetter,
			/* [in] */ REFERENCE_TIME rtStart,
			/* [in] */ REFERENCE_TIME rtStop) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE AddProp( 
			/* [in] */ DEXTER_PARAM Param,
			/* [in] */ __RPC__in DEXTER_VALUE *paValue) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetProps( 
			/* [out] */ __RPC__out LONG *pcParams,
			/* [out] */ __RPC__deref_out_opt DEXTER_PARAM **paParam,
			/* [out] */ __RPC__deref_out_opt DEXTER_VALUE **paValue) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE FreeProps( 
			/* [in] */ LONG cParams,
			/* [in] */ __RPC__in DEXTER_PARAM *paParam,
			/* [in] */ __RPC__in DEXTER_VALUE *paValue) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE ClearProps( void) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SaveToBlob( 
			/* [out] */ __RPC__out LONG *pcSize,
			/* [out] */ __RPC__deref_out_opt BYTE **ppb) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE LoadFromBlob( 
			/* [in] */ LONG cSize,
			/* [in] */ __RPC__in BYTE *pb) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetProps( 
			/* [in] */ __RPC__in_opt IUnknown *pTarget,
			/* [in] */ REFERENCE_TIME rtNow) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE PrintXMLW( 
			/* [out] */ __RPC__out WCHAR *pszXML,
			/* [in] */ int cchXML,
			/* [out] */ __RPC__out int *pcchPrinted,
			/* [in] */ int indent) = 0;
	};

#endif 	// __IPropertySetter_INTERFACE_DEFINED__


#ifndef __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtCompositor_INTERFACE_DEFINED__
	// Missing dxtrans.h
#endif 	// __IDxtCompositor_INTERFACE_DEFINED__


#ifndef __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
	// Missing dxtrans.h
#endif 	// __IDxtAlphaSetter_INTERFACE_DEFINED__


#ifndef __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
	// Missing dxtrans.h
#endif 	// __IDxtJpeg_INTERFACE_DEFINED__


#ifndef __IDxtKey_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
	// Missing dxtrans.h
#endif 	// __IDxtKey_INTERFACE_DEFINED__


#ifndef __IMediaLocator_INTERFACE_DEFINED__
#define __IMediaLocator_INTERFACE_DEFINED__

/* interface IMediaLocator */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IMediaLocator;
	
	MIDL_INTERFACE("288581E0-66CE-11d2-918F-00C0DF10D434")
	IMediaLocator : public IUnknown
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE FindMediaFile( 
			__RPC__in BSTR Input,
			__RPC__in BSTR FilterString,
			__RPC__deref_in_opt BSTR *pOutput,
			long Flags) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE AddFoundLocation( 
			__RPC__in BSTR DirectoryName) = 0;
	};

#endif 	// __IMediaLocator_INTERFACE_DEFINED__


#ifndef __IMediaDet_INTERFACE_DEFINED__
#define __IMediaDet_INTERFACE_DEFINED__

/* interface IMediaDet */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IMediaDet;
	
	MIDL_INTERFACE("65BD0710-24D2-4ff7-9324-ED2E5D3ABAFA")
	IMediaDet : public IUnknown
	{
	public:
		virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Filter( 
			/* [retval][out] */ __RPC__deref_out_opt IUnknown **pVal) = 0;
		
		virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Filter( 
			/* [in] */ __RPC__in_opt IUnknown *newVal) = 0;
		
		virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OutputStreams( 
			/* [retval][out] */ __RPC__out long *pVal) = 0;
		
		virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentStream( 
			/* [retval][out] */ __RPC__out long *pVal) = 0;
		
		virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CurrentStream( 
			/* [in] */ long newVal) = 0;
		
		virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StreamType( 
			/* [retval][out] */ __RPC__out GUID *pVal) = 0;
		
		virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StreamTypeB( 
			/* [retval][out] */ __RPC__deref_out_opt BSTR *pVal) = 0;
		
		virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StreamLength( 
			/* [retval][out] */ __RPC__out double *pVal) = 0;
		
		virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Filename( 
			/* [retval][out] */ __RPC__deref_out_opt BSTR *pVal) = 0;
		
		virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Filename( 
			/* [in] */ __RPC__in BSTR newVal) = 0;
		
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetBitmapBits( 
			double StreamTime,
			__RPC__in long *pBufferSize,
			__RPC__in char *pBuffer,
			long Width,
			long Height) = 0;
		
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteBitmapBits( 
			double StreamTime,
			long Width,
			long Height,
			__RPC__in BSTR Filename) = 0;
		
		virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StreamMediaType( 
			/* [retval][out] */ __RPC__out AM_MEDIA_TYPE *pVal) = 0;
		
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSampleGrabber( 
			/* [out] */ __RPC__deref_out_opt ISampleGrabber **ppVal) = 0;
		
		virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FrameRate( 
			/* [retval][out] */ __RPC__out double *pVal) = 0;
		
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EnterBitmapGrabMode( 
			double SeekTime) = 0;
	};

#endif 	// __IMediaDet_INTERFACE_DEFINED__


#ifndef __IGrfCache_INTERFACE_DEFINED__
#define __IGrfCache_INTERFACE_DEFINED__

/* interface IGrfCache */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IGrfCache;
	
	MIDL_INTERFACE("AE9472BE-B0C3-11D2-8D24-00A0C9441E20")
	IGrfCache : public IDispatch
	{
	public:
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddFilter( 
			__RPC__in_opt IGrfCache *ChainedCache,
			LONGLONG ID,
			__RPC__in_opt const IBaseFilter *pFilter,
			__RPC__in LPCWSTR pName) = 0;
		
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ConnectPins( 
			__RPC__in_opt IGrfCache *ChainedCache,
			LONGLONG PinID1,
			__RPC__in_opt const IPin *pPin1,
			LONGLONG PinID2,
			__RPC__in_opt const IPin *pPin2) = 0;
		
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetGraph( 
			__RPC__in_opt const IGraphBuilder *pGraph) = 0;
		
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DoConnectionsNow( void) = 0;
	};

#endif 	// __IGrfCache_INTERFACE_DEFINED__


#ifndef __IRenderEngine_INTERFACE_DEFINED__
#define __IRenderEngine_INTERFACE_DEFINED__

/* interface IRenderEngine */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IRenderEngine;
	
	MIDL_INTERFACE("6BEE3A81-66C9-11d2-918F-00C0DF10D434")
	IRenderEngine : public IUnknown
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE SetTimelineObject( 
			__RPC__in_opt IAMTimeline *pTimeline) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetTimelineObject( 
			/* [out] */ __RPC__deref_out_opt IAMTimeline **ppTimeline) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetFilterGraph( 
			/* [out] */ __RPC__deref_out_opt IGraphBuilder **ppFG) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetFilterGraph( 
			__RPC__in_opt IGraphBuilder *pFG) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetInterestRange( 
			REFERENCE_TIME Start,
			REFERENCE_TIME Stop) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetInterestRange2( 
			double Start,
			double Stop) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetRenderRange( 
			REFERENCE_TIME Start,
			REFERENCE_TIME Stop) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetRenderRange2( 
			double Start,
			double Stop) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetGroupOutputPin( 
			long Group,
			/* [out] */ __RPC__deref_out_opt IPin **ppRenderPin) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE ScrapIt( void) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE RenderOutputPins( void) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetVendorString( 
			/* [retval][out] */ __RPC__deref_out_opt BSTR *pVendorID) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE ConnectFrontEnd( void) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetSourceConnectCallback( 
			__RPC__in_opt IGrfCache *pCallback) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetDynamicReconnectLevel( 
			long Level) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE DoSmartRecompression( void) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE UseInSmartRecompressionGraph( void) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetSourceNameValidation( 
			__RPC__in BSTR FilterString,
			__RPC__in_opt IMediaLocator *pOverride,
			LONG Flags) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE Commit( void) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE Decommit( void) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetCaps( 
			long Index,
			__RPC__in long *pReturn) = 0;
	};

#endif 	// __IRenderEngine_INTERFACE_DEFINED__


#ifndef __IRenderEngine2_INTERFACE_DEFINED__
#define __IRenderEngine2_INTERFACE_DEFINED__

/* interface IRenderEngine2 */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IRenderEngine2;
	
	MIDL_INTERFACE("6BEE3A82-66C9-11d2-918F-00C0DF10D434")
	IRenderEngine2 : public IUnknown
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE SetResizerGUID( 
			GUID ResizerGuid) = 0;
	};

#endif 	// __IRenderEngine2_INTERFACE_DEFINED__


#ifndef __IFindCompressorCB_INTERFACE_DEFINED__
#define __IFindCompressorCB_INTERFACE_DEFINED__

/* interface IFindCompressorCB */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IFindCompressorCB;
	
	MIDL_INTERFACE("F03FA8DE-879A-4d59-9B2C-26BB1CF83461")
	IFindCompressorCB : public IUnknown
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE GetCompressor( 
			__RPC__in AM_MEDIA_TYPE *pType,
			__RPC__in AM_MEDIA_TYPE *pCompType,
			/* [out] */ __RPC__deref_out_opt IBaseFilter **ppFilter) = 0;
	};

#endif 	// __IFindCompressorCB_INTERFACE_DEFINED__


#ifndef __ISmartRenderEngine_INTERFACE_DEFINED__
#define __ISmartRenderEngine_INTERFACE_DEFINED__

/* interface ISmartRenderEngine */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_ISmartRenderEngine;
	
	MIDL_INTERFACE("F03FA8CE-879A-4d59-9B2C-26BB1CF83461")
	ISmartRenderEngine : public IUnknown
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE SetGroupCompressor( 
			long Group,
			__RPC__in_opt IBaseFilter *pCompressor) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetGroupCompressor( 
			long Group,
			__RPC__deref_in_opt IBaseFilter **pCompressor) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetFindCompressorCB( 
			__RPC__in_opt IFindCompressorCB *pCallback) = 0;
		
	};

#endif 	// __ISmartRenderEngine_INTERFACE_DEFINED__


#ifndef __IAMTimelineObj_INTERFACE_DEFINED__
#define __IAMTimelineObj_INTERFACE_DEFINED__

/* interface IAMTimelineObj */
/* [unique][helpstring][uuid][local][object] */ 

EXTERN_C const IID IID_IAMTimelineObj;
	
	MIDL_INTERFACE("78530B77-61F9-11D2-8CAD-00A024580902")
	IAMTimelineObj : public IUnknown
	{
	public:
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetStartStop( 
			REFERENCE_TIME *pStart,
			REFERENCE_TIME *pStop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetStartStop2( 
			REFTIME *pStart,
			REFTIME *pStop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE FixTimes( 
			REFERENCE_TIME *pStart,
			REFERENCE_TIME *pStop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE FixTimes2( 
			REFTIME *pStart,
			REFTIME *pStop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetStartStop( 
			REFERENCE_TIME Start,
			REFERENCE_TIME Stop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetStartStop2( 
			REFTIME Start,
			REFTIME Stop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetPropertySetter( 
			/* [retval][out] */ IPropertySetter **pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetPropertySetter( 
			IPropertySetter *newVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetSubObject( 
			/* [retval][out] */ IUnknown **pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetSubObject( 
			IUnknown *newVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetSubObjectGUID( 
			GUID newVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetSubObjectGUIDB( 
			BSTR newVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetSubObjectGUID( 
			GUID *pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetSubObjectGUIDB( 
			/* [retval][out] */ BSTR *pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetSubObjectLoaded( 
			BOOL *pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetTimelineType( 
			TIMELINE_MAJOR_TYPE *pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetTimelineType( 
			TIMELINE_MAJOR_TYPE newVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetUserID( 
			long *pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetUserID( 
			long newVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetGenID( 
			long *pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetUserName( 
			/* [retval][out] */ BSTR *pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetUserName( 
			BSTR newVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetUserData( 
			BYTE *pData,
			long *pSize) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetUserData( 
			BYTE *pData,
			long Size) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetMuted( 
			BOOL *pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetMuted( 
			BOOL newVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetLocked( 
			BOOL *pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetLocked( 
			BOOL newVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDirtyRange( 
			REFERENCE_TIME *pStart,
			REFERENCE_TIME *pStop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDirtyRange2( 
			REFTIME *pStart,
			REFTIME *pStop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetDirtyRange( 
			REFERENCE_TIME Start,
			REFERENCE_TIME Stop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetDirtyRange2( 
			REFTIME Start,
			REFTIME Stop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ClearDirty( void) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Remove( void) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RemoveAll( void) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetTimelineNoRef( 
			IAMTimeline **ppResult) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetGroupIBelongTo( 
			/* [out] */ IAMTimelineGroup **ppGroup) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetEmbedDepth( 
			long *pVal) = 0;
	};

#endif 	// __IAMTimelineObj_INTERFACE_DEFINED__


#ifndef __IAMTimelineEffectable_INTERFACE_DEFINED__
#define __IAMTimelineEffectable_INTERFACE_DEFINED__

/* interface IAMTimelineEffectable */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IAMTimelineEffectable;
	
	MIDL_INTERFACE("EAE58537-622E-11d2-8CAD-00A024580902")
	IAMTimelineEffectable : public IUnknown
	{
	public:
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EffectInsBefore( 
			__RPC__in_opt IAMTimelineObj *pFX,
			long priority) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EffectSwapPriorities( 
			long PriorityA,
			long PriorityB) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EffectGetCount( 
			__RPC__in long *pCount) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetEffect( 
			/* [out] */ __RPC__deref_out_opt IAMTimelineObj **ppFx,
			long Which) = 0;
	};

#endif 	// __IAMTimelineEffectable_INTERFACE_DEFINED__


#ifndef __IAMTimelineEffect_INTERFACE_DEFINED__
#define __IAMTimelineEffect_INTERFACE_DEFINED__

/* interface IAMTimelineEffect */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IAMTimelineEffect;
	
	MIDL_INTERFACE("BCE0C264-622D-11d2-8CAD-00A024580902")
	IAMTimelineEffect : public IUnknown
	{
	public:
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EffectGetPriority( 
			__RPC__in long *pVal) = 0;
	};

#endif 	// __IAMTimelineEffect_INTERFACE_DEFINED__


#ifndef __IAMTimelineTransable_INTERFACE_DEFINED__
#define __IAMTimelineTransable_INTERFACE_DEFINED__

/* interface IAMTimelineTransable */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IAMTimelineTransable;
	
	MIDL_INTERFACE("378FA386-622E-11d2-8CAD-00A024580902")
	IAMTimelineTransable : public IUnknown
	{
	public:
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE TransAdd( 
			__RPC__in_opt IAMTimelineObj *pTrans) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE TransGetCount( 
			__RPC__in long *pCount) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetNextTrans( 
			/* [out] */ __RPC__deref_out_opt IAMTimelineObj **ppTrans,
			__RPC__in REFERENCE_TIME *pInOut) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetNextTrans2( 
			/* [out] */ __RPC__deref_out_opt IAMTimelineObj **ppTrans,
			__RPC__in REFTIME *pInOut) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetTransAtTime( 
			/* [out] */ __RPC__deref_out_opt IAMTimelineObj **ppObj,
			REFERENCE_TIME Time,
			long SearchDirection) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetTransAtTime2( 
			/* [out] */ __RPC__deref_out_opt IAMTimelineObj **ppObj,
			REFTIME Time,
			long SearchDirection) = 0;
	};

#endif 	// __IAMTimelineTransable_INTERFACE_DEFINED__


#ifndef __IAMTimelineSplittable_INTERFACE_DEFINED__
#define __IAMTimelineSplittable_INTERFACE_DEFINED__

/* interface IAMTimelineSplittable */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IAMTimelineSplittable;
	
	MIDL_INTERFACE("A0F840A0-D590-11d2-8D55-00A0C9441E20")
	IAMTimelineSplittable : public IUnknown
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE SplitAt( 
			REFERENCE_TIME Time) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SplitAt2( 
			REFTIME Time) = 0;
	};

#endif 	// __IAMTimelineSplittable_INTERFACE_DEFINED__


#ifndef __IAMTimelineTrans_INTERFACE_DEFINED__
#define __IAMTimelineTrans_INTERFACE_DEFINED__

/* interface IAMTimelineTrans */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IAMTimelineTrans;
	
	MIDL_INTERFACE("BCE0C265-622D-11d2-8CAD-00A024580902")
	IAMTimelineTrans : public IUnknown
	{
	public:
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetCutPoint( 
			__RPC__in REFERENCE_TIME *pTLTime) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetCutPoint2( 
			__RPC__in REFTIME *pTLTime) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetCutPoint( 
			REFERENCE_TIME TLTime) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetCutPoint2( 
			REFTIME TLTime) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetSwapInputs( 
			__RPC__in BOOL *pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetSwapInputs( 
			BOOL pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetCutsOnly( 
			__RPC__in BOOL *pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetCutsOnly( 
			BOOL pVal) = 0;
	};

#endif 	// __IAMTimelineTrans_INTERFACE_DEFINED__


#ifndef __IAMTimelineSrc_INTERFACE_DEFINED__
#define __IAMTimelineSrc_INTERFACE_DEFINED__

/* interface IAMTimelineSrc */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IAMTimelineSrc;
	
	MIDL_INTERFACE("78530B79-61F9-11D2-8CAD-00A024580902")
	IAMTimelineSrc : public IUnknown
	{
	public:
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetMediaTimes( 
			__RPC__in REFERENCE_TIME *pStart,
			__RPC__in REFERENCE_TIME *pStop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetMediaTimes2( 
			__RPC__in REFTIME *pStart,
			__RPC__in REFTIME *pStop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ModifyStopTime( 
			REFERENCE_TIME Stop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ModifyStopTime2( 
			REFTIME Stop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE FixMediaTimes( 
			__RPC__in REFERENCE_TIME *pStart,
			__RPC__in REFERENCE_TIME *pStop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE FixMediaTimes2( 
			__RPC__in REFTIME *pStart,
			__RPC__in REFTIME *pStop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetMediaTimes( 
			REFERENCE_TIME Start,
			REFERENCE_TIME Stop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetMediaTimes2( 
			REFTIME Start,
			REFTIME Stop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetMediaLength( 
			REFERENCE_TIME Length) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetMediaLength2( 
			REFTIME Length) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetMediaLength( 
			__RPC__in REFERENCE_TIME *pLength) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetMediaLength2( 
			__RPC__in REFTIME *pLength) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetMediaName( 
			/* [retval][out] */ __RPC__deref_out_opt BSTR *pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetMediaName( 
			__RPC__in BSTR newVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SpliceWithNext( 
			__RPC__in_opt IAMTimelineObj *pNext) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetStreamNumber( 
			__RPC__in long *pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetStreamNumber( 
			long Val) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE IsNormalRate( 
			__RPC__in BOOL *pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDefaultFPS( 
			__RPC__in double *pFPS) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetDefaultFPS( 
			double FPS) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetStretchMode( 
			__RPC__in int *pnStretchMode) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetStretchMode( 
			int nStretchMode) = 0;
	};

#endif 	// __IAMTimelineSrc_INTERFACE_DEFINED__


#ifndef __IAMTimelineTrack_INTERFACE_DEFINED__
#define __IAMTimelineTrack_INTERFACE_DEFINED__

/* interface IAMTimelineTrack */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IAMTimelineTrack;
	
	MIDL_INTERFACE("EAE58538-622E-11d2-8CAD-00A024580902")
	IAMTimelineTrack : public IUnknown
	{
	public:
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SrcAdd( 
			__RPC__in_opt IAMTimelineObj *pSource) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetNextSrc( 
			/* [out] */ __RPC__deref_out_opt IAMTimelineObj **ppSrc,
			__RPC__in REFERENCE_TIME *pInOut) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetNextSrc2( 
			/* [out] */ __RPC__deref_out_opt IAMTimelineObj **ppSrc,
			__RPC__in REFTIME *pInOut) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE MoveEverythingBy( 
			REFERENCE_TIME Start,
			REFERENCE_TIME MoveBy) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE MoveEverythingBy2( 
			REFTIME Start,
			REFTIME MoveBy) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetSourcesCount( 
			__RPC__in long *pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AreYouBlank( 
			__RPC__in long *pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetSrcAtTime( 
			/* [out] */ __RPC__deref_out_opt IAMTimelineObj **ppSrc,
			REFERENCE_TIME Time,
			long SearchDirection) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetSrcAtTime2( 
			/* [out] */ __RPC__deref_out_opt IAMTimelineObj **ppSrc,
			REFTIME Time,
			long SearchDirection) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE InsertSpace( 
			REFERENCE_TIME rtStart,
			REFERENCE_TIME rtEnd) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE InsertSpace2( 
			REFTIME rtStart,
			REFTIME rtEnd) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE ZeroBetween( 
			REFERENCE_TIME rtStart,
			REFERENCE_TIME rtEnd) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE ZeroBetween2( 
			REFTIME rtStart,
			REFTIME rtEnd) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetNextSrcEx( 
			__RPC__in_opt IAMTimelineObj *pLast,
			/* [out] */ __RPC__deref_out_opt IAMTimelineObj **ppNext) = 0;
	};

#endif 	// __IAMTimelineTrack_INTERFACE_DEFINED__


#ifndef __IAMTimelineVirtualTrack_INTERFACE_DEFINED__
#define __IAMTimelineVirtualTrack_INTERFACE_DEFINED__

/* interface IAMTimelineVirtualTrack */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IAMTimelineVirtualTrack;
	
	MIDL_INTERFACE("A8ED5F80-C2C7-11d2-8D39-00A0C9441E20")
	IAMTimelineVirtualTrack : public IUnknown
	{
	public:
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE TrackGetPriority( 
			__RPC__in long *pPriority) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetTrackDirty( void) = 0;
	};

#endif 	// __IAMTimelineVirtualTrack_INTERFACE_DEFINED__


#ifndef __IAMTimelineComp_INTERFACE_DEFINED__
#define __IAMTimelineComp_INTERFACE_DEFINED__

/* interface IAMTimelineComp */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IAMTimelineComp;
	
	MIDL_INTERFACE("EAE58536-622E-11d2-8CAD-00A024580902")
	IAMTimelineComp : public IUnknown
	{
	public:
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE VTrackInsBefore( 
			__RPC__in_opt IAMTimelineObj *pVirtualTrack,
			long Priority) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE VTrackSwapPriorities( 
			long VirtualTrackA,
			long VirtualTrackB) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE VTrackGetCount( 
			__RPC__in long *pVal) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetVTrack( 
			/* [out] */ __RPC__deref_out_opt IAMTimelineObj **ppVirtualTrack,
			long Which) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetCountOfType( 
			__RPC__in long *pVal,
			__RPC__in long *pValWithComps,
			TIMELINE_MAJOR_TYPE MajorType) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetRecursiveLayerOfType( 
			/* [out] */ __RPC__deref_out_opt IAMTimelineObj **ppVirtualTrack,
			long WhichLayer,
			TIMELINE_MAJOR_TYPE Type) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetRecursiveLayerOfTypeI( 
			/* [out] */ __RPC__deref_out_opt IAMTimelineObj **ppVirtualTrack,
			/* [out][in] */ __RPC__inout long *pWhichLayer,
			TIMELINE_MAJOR_TYPE Type) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetNextVTrack( 
			__RPC__in_opt IAMTimelineObj *pVirtualTrack,
			/* [out] */ __RPC__deref_out_opt IAMTimelineObj **ppNextVirtualTrack) = 0;
	};

#endif 	// __IAMTimelineComp_INTERFACE_DEFINED__


#ifndef __IAMTimelineGroup_INTERFACE_DEFINED__
#define __IAMTimelineGroup_INTERFACE_DEFINED__

/* interface IAMTimelineGroup */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IAMTimelineGroup;
	
	MIDL_INTERFACE("9EED4F00-B8A6-11d2-8023-00C0DF10D434")
	IAMTimelineGroup : public IUnknown
	{
	public:
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetTimeline( 
			__RPC__in_opt IAMTimeline *pTimeline) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetTimeline( 
			/* [out] */ __RPC__deref_out_opt IAMTimeline **ppTimeline) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetPriority( 
			__RPC__in long *pPriority) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetMediaType( 
			/* [out] */ __RPC__out AM_MEDIA_TYPE *__MIDL__IAMTimelineGroup0000) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetMediaType( 
			/* [in] */ __RPC__in AM_MEDIA_TYPE *__MIDL__IAMTimelineGroup0001) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetOutputFPS( 
			double FPS) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetOutputFPS( 
			__RPC__in double *pFPS) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetGroupName( 
			__RPC__in BSTR pGroupName) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetGroupName( 
			/* [retval][out] */ __RPC__deref_out_opt BSTR *pGroupName) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetPreviewMode( 
			BOOL fPreview) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetPreviewMode( 
			__RPC__in BOOL *pfPreview) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetMediaTypeForVB( 
			/* [in] */ long Val) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetOutputBuffering( 
			/* [out] */ __RPC__out int *pnBuffer) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetOutputBuffering( 
			/* [in] */ int nBuffer) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetSmartRecompressFormat( 
			__RPC__in long *pFormat) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetSmartRecompressFormat( 
			__RPC__deref_in_opt long **ppFormat) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE IsSmartRecompressFormatSet( 
			__RPC__in BOOL *pVal) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE IsRecompressFormatDirty( 
			__RPC__in BOOL *pVal) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE ClearRecompressFormatDirty( void) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetRecompFormatFromSource( 
			__RPC__in_opt IAMTimelineSrc *pSource) = 0;
	};

#endif 	// __IAMTimelineGroup_INTERFACE_DEFINED__


#ifndef __IAMTimeline_INTERFACE_DEFINED__
#define __IAMTimeline_INTERFACE_DEFINED__

/* interface IAMTimeline */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IAMTimeline;
	
	MIDL_INTERFACE("78530B74-61F9-11D2-8CAD-00A024580902")
	IAMTimeline : public IUnknown
	{
	public:
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreateEmptyNode( 
			/* [out] */ __RPC__deref_out_opt IAMTimelineObj **ppObj,
			TIMELINE_MAJOR_TYPE Type) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE AddGroup( 
			__RPC__in_opt IAMTimelineObj *pGroup) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE RemGroupFromList( 
			__RPC__in_opt IAMTimelineObj *pGroup) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetGroup( 
			/* [out] */ __RPC__deref_out_opt IAMTimelineObj **ppGroup,
			long WhichGroup) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetGroupCount( 
			__RPC__in long *pCount) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE ClearAllGroups( void) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetInsertMode( 
			__RPC__in long *pMode) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetInsertMode( 
			long Mode) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnableTransitions( 
			BOOL fEnabled) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE TransitionsEnabled( 
			__RPC__in BOOL *pfEnabled) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnableEffects( 
			BOOL fEnabled) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EffectsEnabled( 
			__RPC__in BOOL *pfEnabled) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetInterestRange( 
			REFERENCE_TIME Start,
			REFERENCE_TIME Stop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDuration( 
			__RPC__in REFERENCE_TIME *pDuration) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDuration2( 
			__RPC__in double *pDuration) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetDefaultFPS( 
			double FPS) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDefaultFPS( 
			__RPC__in double *pFPS) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE IsDirty( 
			__RPC__in BOOL *pDirty) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDirtyRange( 
			__RPC__in REFERENCE_TIME *pStart,
			__RPC__in REFERENCE_TIME *pStop) = 0;
		
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetCountOfType( 
			long Group,
			__RPC__in long *pVal,
			__RPC__in long *pValWithComps,
			TIMELINE_MAJOR_TYPE MajorType) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE ValidateSourceNames( 
			long ValidateFlags,
			__RPC__in_opt IMediaLocator *pOverride,
			LONG_PTR NotifyEventHandle) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetDefaultTransition( 
			__RPC__in GUID *pGuid) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetDefaultTransition( 
			__RPC__in GUID *pGuid) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetDefaultEffect( 
			__RPC__in GUID *pGuid) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetDefaultEffect( 
			__RPC__in GUID *pGuid) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetDefaultTransitionB( 
			__RPC__in BSTR pGuid) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetDefaultTransitionB( 
			/* [retval][out] */ __RPC__deref_out_opt BSTR *pGuid) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetDefaultEffectB( 
			__RPC__in BSTR pGuid) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetDefaultEffectB( 
			/* [retval][out] */ __RPC__deref_out_opt BSTR *pGuid) = 0;
	};

#endif 	// __IAMTimeline_INTERFACE_DEFINED__


#ifndef __IXml2Dex_INTERFACE_DEFINED__
#define __IXml2Dex_INTERFACE_DEFINED__

/* interface IXml2Dex */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IXml2Dex;
	
	MIDL_INTERFACE("18C628ED-962A-11D2-8D08-00A0C9441E20")
	IXml2Dex : public IDispatch
	{
	public:
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateGraphFromFile( 
			/* [out] */ __RPC__deref_out_opt IUnknown **ppGraph,
			__RPC__in_opt IUnknown *pTimeline,
			__RPC__in BSTR Filename) = 0;
		
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteGrfFile( 
			__RPC__in_opt IUnknown *pGraph,
			__RPC__in BSTR FileName) = 0;
		
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteXMLFile( 
			__RPC__in_opt IUnknown *pTimeline,
			__RPC__in BSTR FileName) = 0;
		
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadXMLFile( 
			__RPC__in_opt IUnknown *pTimeline,
			__RPC__in BSTR XMLName) = 0;
		
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Delete( 
			__RPC__in_opt IUnknown *pTimeline,
			double dStart,
			double dEnd) = 0;
		
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteXMLPart( 
			__RPC__in_opt IUnknown *pTimeline,
			double dStart,
			double dEnd,
			__RPC__in BSTR FileName) = 0;
		
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PasteXMLFile( 
			__RPC__in_opt IUnknown *pTimeline,
			double dStart,
			__RPC__in BSTR FileName) = 0;
		
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CopyXML( 
			__RPC__in_opt IUnknown *pTimeline,
			double dStart,
			double dEnd) = 0;
		
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PasteXML( 
			__RPC__in_opt IUnknown *pTimeline,
			double dStart) = 0;
		
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
		
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadXML( 
			__RPC__in_opt IUnknown *pTimeline,
			__RPC__in_opt IUnknown *pXML) = 0;
		
		virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteXML( 
			__RPC__in_opt IUnknown *pTimeline,
			__RPC__deref_in_opt BSTR *pbstrXML) = 0;
	};

#endif 	// __IXml2Dex_INTERFACE_DEFINED__


#ifndef __IAMErrorLog_INTERFACE_DEFINED__
#define __IAMErrorLog_INTERFACE_DEFINED__

/* interface IAMErrorLog */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IAMErrorLog;
	
	MIDL_INTERFACE("E43E73A2-0EFA-11d3-9601-00A0C9441E20")
	IAMErrorLog : public IUnknown
	{
	public:
		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE LogError( 
			long Severity,
			__RPC__in BSTR pErrorString,
			long ErrorCode,
			long hresult,
			/* [in] */ __RPC__in VARIANT *pExtraInfo) = 0;
		
	};

#endif 	// __IAMErrorLog_INTERFACE_DEFINED__


#ifndef __IAMSetErrorLog_INTERFACE_DEFINED__
#define __IAMSetErrorLog_INTERFACE_DEFINED__

/* interface IAMSetErrorLog */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IAMSetErrorLog;
	
	MIDL_INTERFACE("963566DA-BE21-4eaf-88E9-35704F8F52A1")
	IAMSetErrorLog : public IUnknown
	{
	public:
		virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ErrorLog( 
			/* [retval][out] */ __RPC__deref_out_opt IAMErrorLog **pVal) = 0;
		
		virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ErrorLog( 
			/* [in] */ __RPC__in_opt IAMErrorLog *newVal) = 0;
		
	};

#endif // __IAMSetErrorLog_INTERFACE_DEFINED__


#ifndef __ISampleGrabberCB_INTERFACE_DEFINED__
#define __ISampleGrabberCB_INTERFACE_DEFINED__

/* interface ISampleGrabberCB */
/* [unique][helpstring][local][uuid][object] */ 

EXTERN_C const IID IID_ISampleGrabberCB;
	
	MIDL_INTERFACE("0579154A-2B53-4994-B0D0-E773148EFF85")
	ISampleGrabberCB : public IUnknown
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE SampleCB( 
			double SampleTime,
			IMediaSample *pSample) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE BufferCB( 
			double SampleTime,
			BYTE *pBuffer,
			long BufferLen) = 0;
	};

#endif 	// __ISampleGrabberCB_INTERFACE_DEFINED__


#ifndef __ISampleGrabber_INTERFACE_DEFINED__
#define __ISampleGrabber_INTERFACE_DEFINED__

/* interface ISampleGrabber */
/* [unique][helpstring][local][uuid][object] */ 

EXTERN_C const IID IID_ISampleGrabber;
	
	MIDL_INTERFACE("6B652FFF-11FE-4fce-92AD-0266B5D7C78F")
	ISampleGrabber : public IUnknown
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE SetOneShot( 
			BOOL OneShot) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetMediaType( 
			const AM_MEDIA_TYPE *pType) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType( 
			AM_MEDIA_TYPE *pType) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetBufferSamples( 
			BOOL BufferThem) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer( 
			/* [out][in] */ long *pBufferSize,
			/* [out] */ long *pBuffer) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE GetCurrentSample( 
			/* [retval][out] */ IMediaSample **ppSample) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetCallback( 
			ISampleGrabberCB *pCallback,
			long WhichMethodToCallback) = 0;
	};

#endif 	// __ISampleGrabber_INTERFACE_DEFINED__



#ifndef __DexterLib_LIBRARY_DEFINED__
#define __DexterLib_LIBRARY_DEFINED__

/* library DexterLib */
/* [helpstring][version][uuid] */ 

EXTERN_C const IID LIBID_DexterLib;


#ifndef __IResize_INTERFACE_DEFINED__
#define __IResize_INTERFACE_DEFINED__

/* interface IResize */
/* [unique][helpstring][uuid][object] */ 

EXTERN_C const IID IID_IResize;
	
	MIDL_INTERFACE("4ada63a0-72d5-11d2-952a-0060081840bc")
	IResize : public IUnknown
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE get_Size( 
			/* [out] */ __RPC__out int *piHeight,
			/* [out] */ __RPC__out int *piWidth,
			/* [out] */ __RPC__out long *pFlag) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE get_InputSize( 
			/* [out] */ __RPC__out int *piHeight,
			/* [out] */ __RPC__out int *piWidth) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE put_Size( 
			/* [in] */ int Height,
			/* [in] */ int Width,
			/* [in] */ long Flag) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE get_MediaType( 
			/* [out] */ __RPC__out AM_MEDIA_TYPE *pmt) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE put_MediaType( 
			/* [in] */ __RPC__in const AM_MEDIA_TYPE *pmt) = 0;
	};

#endif 	// __IResize_INTERFACE_DEFINED__


EXTERN_C const CLSID CLSID_AMTimeline;

//#ifdef __cplusplus

class DECLSPEC_UUID("78530B75-61F9-11D2-8CAD-00A024580902")
AMTimeline;

EXTERN_C const CLSID CLSID_AMTimelineObj;

class DECLSPEC_UUID("78530B78-61F9-11D2-8CAD-00A024580902")
AMTimelineObj;

EXTERN_C const CLSID CLSID_AMTimelineSrc;

class DECLSPEC_UUID("78530B7A-61F9-11D2-8CAD-00A024580902")
AMTimelineSrc;

EXTERN_C const CLSID CLSID_AMTimelineTrack;

class DECLSPEC_UUID("8F6C3C50-897B-11d2-8CFB-00A0C9441E20")
AMTimelineTrack;

EXTERN_C const CLSID CLSID_AMTimelineComp;

class DECLSPEC_UUID("74D2EC80-6233-11d2-8CAD-00A024580902")
AMTimelineComp;

EXTERN_C const CLSID CLSID_AMTimelineGroup;

class DECLSPEC_UUID("F6D371E1-B8A6-11d2-8023-00C0DF10D434")
AMTimelineGroup;

EXTERN_C const CLSID CLSID_AMTimelineTrans;

class DECLSPEC_UUID("74D2EC81-6233-11d2-8CAD-00A024580902")
AMTimelineTrans;

EXTERN_C const CLSID CLSID_AMTimelineEffect;

class DECLSPEC_UUID("74D2EC82-6233-11d2-8CAD-00A024580902")
AMTimelineEffect;

EXTERN_C const CLSID CLSID_RenderEngine;

class DECLSPEC_UUID("64D8A8E0-80A2-11d2-8CF3-00A0C9441E20")
RenderEngine;

EXTERN_C const CLSID CLSID_SmartRenderEngine;

class DECLSPEC_UUID("498B0949-BBE9-4072-98BE-6CCAEB79DC6F")
SmartRenderEngine;

EXTERN_C const CLSID CLSID_AudMixer;

class DECLSPEC_UUID("036A9790-C153-11d2-9EF7-006008039E37")
AudMixer;

EXTERN_C const CLSID CLSID_Xml2Dex;

class DECLSPEC_UUID("18C628EE-962A-11D2-8D08-00A0C9441E20")
Xml2Dex;

EXTERN_C const CLSID CLSID_MediaLocator;

class DECLSPEC_UUID("CC1101F2-79DC-11D2-8CE6-00A0C9441E20")
MediaLocator;

EXTERN_C const CLSID CLSID_PropertySetter;

class DECLSPEC_UUID("ADF95821-DED7-11d2-ACBE-0080C75E246E")
PropertySetter;

EXTERN_C const CLSID CLSID_MediaDet;

class DECLSPEC_UUID("65BD0711-24D2-4ff7-9324-ED2E5D3ABAFA")
MediaDet;

EXTERN_C const CLSID CLSID_SampleGrabber;

class DECLSPEC_UUID("C1F400A0-3F08-11d3-9F0B-006008039E37")
SampleGrabber;

EXTERN_C const CLSID CLSID_NullRenderer;

class DECLSPEC_UUID("C1F400A4-3F08-11d3-9F0B-006008039E37")
NullRenderer;

EXTERN_C const CLSID CLSID_DxtCompositor;

class DECLSPEC_UUID("BB44391D-6ABD-422f-9E2E-385C9DFF51FC")
DxtCompositor;

EXTERN_C const CLSID CLSID_DxtAlphaSetter;

class DECLSPEC_UUID("506D89AE-909A-44f7-9444-ABD575896E35")
DxtAlphaSetter;

EXTERN_C const CLSID CLSID_DxtJpeg;

class DECLSPEC_UUID("DE75D012-7A65-11D2-8CEA-00A0C9441E20")
DxtJpeg;

EXTERN_C const CLSID CLSID_ColorSource;

class DECLSPEC_UUID("0cfdd070-581a-11d2-9ee6-006008039e37")
ColorSource;

EXTERN_C const CLSID CLSID_DxtKey;

class DECLSPEC_UUID("C5B19592-145E-11d3-9F04-006008039E37")
DxtKey;

#endif	// __DexterLib_LIBRARY_DEFINED__


/* interface __MIDL_itf_qedit_0001_0097 */
/* [local] */ 


enum __MIDL___MIDL_itf_qedit_0001_0097_0001
	{	E_NOTINTREE	= 0x80040400,
	E_RENDER_ENGINE_IS_BROKEN	= 0x80040401,
	E_MUST_INIT_RENDERER	= 0x80040402,
	E_NOTDETERMINED	= 0x80040403,
	E_NO_TIMELINE	= 0x80040404,
	S_WARN_OUTPUTRESET	= 40404
	} ;
#define DEX_IDS_BAD_SOURCE_NAME 	1400
#define DEX_IDS_BAD_SOURCE_NAME2	1401
#define DEX_IDS_MISSING_SOURCE_NAME	1402
#define DEX_IDS_UNKNOWN_SOURCE		1403
#define DEX_IDS_INSTALL_PROBLEM 	1404
#define DEX_IDS_NO_SOURCE_NAMES 	1405
#define DEX_IDS_BAD_MEDIATYPE		1406
#define DEX_IDS_STREAM_NUMBER		1407
#define DEX_IDS_OUTOFMEMORY			1408
#define DEX_IDS_DIBSEQ_NOTALLSAME	1409
#define DEX_IDS_CLIPTOOSHORT		1410
#define DEX_IDS_INVALID_DXT 		1411
#define DEX_IDS_INVALID_DEFAULT_DXT	1412
#define DEX_IDS_NO_3D				1413
#define DEX_IDS_BROKEN_DXT			1414
#define DEX_IDS_NO_SUCH_PROPERTY	1415
#define DEX_IDS_ILLEGAL_PROPERTY_VAL 1416
#define DEX_IDS_INVALID_XML 		1417
#define DEX_IDS_CANT_FIND_FILTER	1418
#define DEX_IDS_DISK_WRITE_ERROR	1419
#define DEX_IDS_INVALID_AUDIO_FX	1420
#define DEX_IDS_CANT_FIND_COMPRESSOR 1421
#define DEX_IDS_TIMELINE_PARSE		1426
#define DEX_IDS_GRAPH_ERROR 		1427
#define DEX_IDS_GRID_ERROR		1428
#define DEX_IDS_INTERFACE_ERROR 	1429
EXTERN_GUID(CLSID_VideoEffects1Category, 0xcc7bfb42, 0xf175, 0x11d1, 0xa3, 0x92, 0x0, 0xe0, 0x29, 0x1f, 0x39, 0x59);
EXTERN_GUID(CLSID_VideoEffects2Category, 0xcc7bfb43, 0xf175, 0x11d1, 0xa3, 0x92, 0x0, 0xe0, 0x29, 0x1f, 0x39, 0x59);
EXTERN_GUID(CLSID_AudioEffects1Category, 0xcc7bfb44, 0xf175, 0x11d1, 0xa3, 0x92, 0x0, 0xe0, 0x29, 0x1f, 0x39, 0x59);
EXTERN_GUID(CLSID_AudioEffects2Category, 0xcc7bfb45, 0xf175, 0x11d1, 0xa3, 0x92, 0x0, 0xe0, 0x29, 0x1f, 0x39, 0x59);


extern RPC_IF_HANDLE __MIDL_itf_qedit_0001_0097_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_qedit_0001_0097_v0_0_s_ifspec;

// Additional Prototypes for all interfaces

unsigned long   __RPC_USER  BSTR_UserSize(	 unsigned long *, unsigned long, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void			__RPC_USER  BSTR_UserFree(	 unsigned long *, BSTR * ); 

unsigned long   __RPC_USER  VARIANT_UserSize(	 unsigned long *, unsigned long, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void			__RPC_USER  VARIANT_UserFree(	 unsigned long *, VARIANT * ); 

unsigned long   __RPC_USER  BSTR_UserSize64(	 unsigned long *, unsigned long, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal64(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal64(unsigned long *, unsigned char *, BSTR * ); 
void			__RPC_USER  BSTR_UserFree64(	 unsigned long *, BSTR * ); 

unsigned long   __RPC_USER  VARIANT_UserSize64(	 unsigned long *, unsigned long, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal64(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal64(unsigned long *, unsigned char *, VARIANT * ); 
void			__RPC_USER  VARIANT_UserFree64(	 unsigned long *, VARIANT * ); 

// end Additional Prototypes

} // extern C
