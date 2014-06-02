namespace Updater.Common
{
	using System;
	using System.Drawing;
	using System.Windows.Forms;
	using System.ComponentModel;

	partial class XmlViewBox
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing) {
			if (disposing && (components != null)) {
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Component Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent() {
			this.SuspendLayout();
			//
			// XmlViewBox
			//
			this.AcceptsTab = true;
			this.DetectUrls = false;
			this.ResumeLayout(false);

		}

		#endregion
	}
}