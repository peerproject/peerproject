//
// FictionBookReader.cs
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// Shareaza is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)

#region Using directives
using System;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using System.Security.Permissions;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.Xml.Serialization;
using Microsoft.Win32;
using Schemas;
using Enc = System.Drawing.Imaging.Encoder;

#endregion

namespace PeerProject
{
	[Guid("1B5C019B-CAD0-4590-9264-870284468393")]
	[ClassInterface(ClassInterfaceType.None)] // Explicit interface
	// [ProgId("PeerProject.FictionBookReader")]
	public class FictionBookReader : ILibraryBuilderPlugin, IImageServicePlugin
	{
		static readonly string builderPath = @"SOFTWARE\PeerProject\PeerProject\Plugins\LibraryBuilder";
		static readonly string imgServPath = @"SOFTWARE\PeerProject\PeerProject\Plugins\ImageService";
		static readonly string readerGuid = @"{1B5C019B-CAD0-4590-9264-870284468393}";

		#region ILibraryBuilderPlugin Members

		[SecurityPermission(SecurityAction.Demand)]
		public void Process(IntPtr hFile, string sFile, ISXMLElement pXML) {
			bool success = true;
			try {
				success = (Path.GetExtension(sFile) == @".fb2") && hFile != IntPtr.Zero;
			} catch { }
			if (!success)
				Marshal.ThrowExceptionForHR(Hresults.E_INVALIDARG);

			XmlReader reader = null;
			FictionBook book = null;
			Stream fs = null;
			try {
				XmlSerializer s = new XmlSerializer(typeof(FictionBook));
				s.UnknownNode += new XmlNodeEventHandler(OnUnknownNode);
				s.UnknownAttribute += new XmlAttributeEventHandler(OnUnknownAttribute);
				fs = new FileStream(sFile, FileMode.Open, FileAccess.Read);
				reader = new XmlTextReader(fs);
				book = (FictionBook)s.Deserialize(reader);
			} catch (Exception e) {
				Trace.WriteLine(e.Message);
				Exception inner = e.InnerException;
				while (inner != null) {
					Trace.WriteLine(inner.Message);
					inner = inner.InnerException;
				}
			} finally {
				if (reader != null) reader.Close();
				if (fs != null) fs.Close();
			}

			if (book == null) {
				Marshal.ThrowExceptionForHR(Hresults.E_FAIL);
			}

			int result = Hresults.S_OK;
			try {
				CreatePeerProjectXml(book, ref pXML);
			} catch (Exception e) {
				Trace.WriteLine(e.Message);
				Exception inner = e.InnerException;
				while (inner != null) {
					Trace.WriteLine(inner.Message);
					inner = inner.InnerException;
				}
				result = Marshal.GetHRForException(e);
			}

			if (result != 0) {
				Trace.WriteLine("Error HRESULT=" + result.ToString());
				Marshal.ThrowExceptionForHR(result);
			}
		}

		#endregion

		#region Routine of FictionBook XML convertion to PeerProject format

		void CreatePeerProjectXml(FictionBook book, ref ISXMLElement pXML) {
			PeerProjectBook sbook = new PeerProjectBook(book);
			Books sBooks = new Books();
			sBooks.books[0] = sbook;

			MemoryStream ms = null;
			XmlWriter writer = null;
			string finalXml = String.Empty;

			try {
				XmlSerializer s = new XmlSerializer(typeof(Books));
				s.UnknownNode += new XmlNodeEventHandler(OnUnknownNode);
				s.UnknownAttribute += new XmlAttributeEventHandler(OnUnknownAttribute);

				ms = new MemoryStream();

				UTF8Encoding utf8 = new UTF8Encoding(false, false);
				writer = new XmlTextWriter(ms, utf8);

				XmlSerializerNamespaces xsn = new XmlSerializerNamespaces();
				// Don't add any prefixes
				xsn.Add("xsi", "http://www.w3.org/2001/XMLSchema-instance");
				s.Serialize(writer, sBooks, xsn);

				// Start modifying the resulting XML
				XmlDocument doc = new XmlDocument();
				ms.Position = 0;
				doc.Load(ms);
				XmlAttribute schema = doc.CreateAttribute("xsi", "noNamespaceSchemaLocation",
														  "http://www.w3.org/2001/XMLSchema-instance");
				schema.Value = Books.URI;
				doc.DocumentElement.SetAttributeNode(schema);
				// Truncate the serialization result and overwrite it with our modified XML
				ms.SetLength(0);
				ms.Position = 0;
				writer = new XmlTextWriter(ms, utf8);
				doc.Save(writer);

				char[] buffer = Encoding.UTF8.GetChars(ms.ToArray());
				finalXml = new string(buffer);
			} catch (Exception e) {
				Trace.WriteLine(e.Message);
				Exception inner = e.InnerException;
				while (inner != null) {
					Trace.WriteLine(inner.Message);
					inner = inner.InnerException;
				}
			} finally {
				if (writer != null) writer.Close();
				if (ms != null) ms.Close();
			}
			if (!String.IsNullOrEmpty(finalXml)) {
				Trace.WriteLine(finalXml);
				ISXMLElement newXML = pXML.FromString(finalXml);
				if (newXML != null) {
					pXML.Elements.Attach(newXML);
				}
			}
		}

		#endregion

		#region XML reading events (Debug)

		protected void OnUnknownNode(object sender, XmlNodeEventArgs e) {
			Trace.WriteLine("Unknown Node:" + e.Name + "\t" + e.Text);
		}

		protected void OnUnknownAttribute(object sender, XmlAttributeEventArgs e) {
			XmlAttribute attr = e.Attr;
			Trace.WriteLine("Unknown attribute " + attr.Name + "='" + attr.Value + "'");
		}

		#endregion

		#region COM registration

		[ComRegisterFunction()]
		public static void RegisterFunction(Type t) {
			try {
				using (RegistryKey regHKLM = Registry.LocalMachine) {
					using (RegistryKey subKey = regHKLM.CreateSubKey(builderPath)) {
						subKey.SetValue(@".fb2", readerGuid);
					}
					using (RegistryKey subKey = regHKLM.CreateSubKey(imgServPath)) {
						subKey.SetValue(@".fb2", readerGuid);
					}
				}
			} catch (Exception ex) {
				MessageBox.Show(ex.Message, "Cannot Register",
								MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		[ComUnregisterFunction()]
		public static void UnregisterFunction(Type t) {
			try {
				using (RegistryKey regHKLM = Registry.LocalMachine) {
					using (RegistryKey subKey = regHKLM.OpenSubKey(builderPath)) {
						subKey.DeleteValue(@".fb2");
					}
					using (RegistryKey subKey = regHKLM.OpenSubKey(imgServPath)) {
						subKey.DeleteValue(@".fb2");
					}
				}
			} catch (Exception ex) {
				MessageBox.Show(ex.Message, "Cannot delete a value.",
								MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		#endregion

		#region IImageServicePlugin Members

		public void LoadFromFile(string sFile, ref IMAGESERVICEDATA pParams, out Array ppImage) {
			ppImage = null;
			bool success = true;
			try {
				success = (Path.GetExtension(sFile) == @".fb2");
			} catch { }
			if (!success)
				Marshal.ThrowExceptionForHR(Hresults.E_INVALIDARG);
			XmlReader reader = null;
			FictionBookImage image = null;
			Stream fs = null;
			try {
				XmlSerializer s = new XmlSerializer(typeof(FictionBookImage));
				s.UnknownNode += new XmlNodeEventHandler(OnUnknownNode);
				s.UnknownAttribute += new XmlAttributeEventHandler(OnUnknownAttribute);
				fs = new FileStream(sFile, FileMode.Open, FileAccess.Read);
				reader = new XmlTextReader(fs);
				image = (FictionBookImage)s.Deserialize(reader);
			} catch (Exception e) {
				Trace.WriteLine(e.Message);
				Exception inner = e.InnerException;
				while (inner != null) {
					Trace.WriteLine(inner.Message);
					inner = inner.InnerException;
				}
			} finally {
				if (reader != null) reader.Close();
				if (fs != null) fs.Close();
			}

			if (image == null || image.binary == null || image.binary.Length == 0 ||
				image.binary[0].Value == null ||
				(image.binary[0].ContentType != @"image/png" &&
				 image.binary[0].ContentType != @"image/jpeg" &&
				 image.binary[0].ContentType != @"image/bmp" &&
				 image.binary[0].ContentType != @"image/gif")) {
				Marshal.ThrowExceptionForHR(Hresults.E_FAIL);
			}

			MemoryStream msIn = new MemoryStream(image.binary[0].Value);
			try {
				using (Image inImage = Image.FromStream(msIn)) {
					MemoryStream msOut = new MemoryStream();
					try {
						Enc bmpCompression = Enc.Compression;
						Enc bmpColors = Enc.ColorDepth;
						// Save the bitmap with RLE compression and 24bit colors
						using (EncoderParameter parameter1 = 
												new EncoderParameter(bmpColors, 24))
						using (EncoderParameter parameter2 =
												new EncoderParameter(bmpCompression,
																	 (long)EncoderValue.CompressionNone))
						using (EncoderParameters parameters = new EncoderParameters(2)) {
							parameters.Param[0] = parameter1;
							parameters.Param[1] = parameter2;
							inImage.Save(msOut, GetEncoderInfo(@"image/bmp"), parameters);
						}

						msOut.Position = 0;
						using (Bitmap outBitmap = new Bitmap(msOut)) {
							Rectangle rect = new Rectangle(0, 0, outBitmap.Width, outBitmap.Height);
							BitmapData bmpData = outBitmap.LockBits(rect, ImageLockMode.ReadOnly,
																	outBitmap.PixelFormat);

							// Declare an array to hold the bytes of the bitmap.
							int nBytes = bmpData.Stride * outBitmap.Height;
							byte[] bytes = new byte[nBytes];
							
							// Swap Red and Blue bytes
							unsafe {
								byte* pData = (byte*)bmpData.Scan0;
								int offset = bmpData.Stride - outBitmap.Width * 3;
								byte red;

								for (int y = 0; y < outBitmap.Height; ++y) {
									for (int x = 0; x < outBitmap.Width; ++x) {
										red = pData[2];
										pData[2] = pData[0];
										pData[0] = red;
										pData += 3;
									}
									pData += offset;
								}
							}
							Marshal.Copy(bmpData.Scan0, bytes, 0, nBytes);
							ppImage = bytes;

							pParams.nComponents = 3;
							pParams.nWidth = outBitmap.Width;
							pParams.nHeight = outBitmap.Height;

							outBitmap.UnlockBits(bmpData);
						}
					} catch {
						Marshal.ThrowExceptionForHR(Hresults.E_FAIL);
					} finally {
						msOut.Close();
					}
				}
			} catch {
				Marshal.ThrowExceptionForHR(Hresults.E_FAIL);
			} finally {
				msIn.Close();
			}
		}

		public void LoadFromMemory(string sType, Array pMemory,
								   ref IMAGESERVICEDATA pParams, out Array ppImage) {
			ppImage = null;
			Marshal.ThrowExceptionForHR(Hresults.E_NOTIMPL);
		}

		public void SaveToFile(string sFile, ref IMAGESERVICEDATA pParams, Array pImage) {
			Marshal.ThrowExceptionForHR(Hresults.E_NOTIMPL);
		}

		public void SaveToMemory(string sType, out Array ppMemory,
								 ref IMAGESERVICEDATA pParams, Array pImage) {
			ppMemory = null;
			Marshal.ThrowExceptionForHR(Hresults.E_NOTIMPL);
		}

		#endregion

		private ImageCodecInfo GetEncoderInfo(string mimeType) {
			ImageCodecInfo[] codecs = ImageCodecInfo.GetImageEncoders();
			for (int i = 0; i < codecs.Length; i++)
				if (codecs[i].MimeType == mimeType)
					return codecs[i];
			return null;
		}
	}
}
