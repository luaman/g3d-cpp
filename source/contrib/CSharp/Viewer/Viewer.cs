using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace Viewer
{
	/// <summary>
	/// Summary description for Viewer.
	/// </summary>
	public class Viewer : System.Windows.Forms.UserControl
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		RendererWrapper wrapper;

		public Viewer(int width, int height)
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();
			this.Width = width;
			this.Height = height;

			// The next few calls *should* set the window up to be double-buffered and painted by the user.
			// Actually, though, it seems to make the first frame not draw properly.
//						this.SetStyle(ControlStyles.DoubleBuffer, true);
//						this.SetStyle(ControlStyles.AllPaintingInWmPaint, true);
//						this.SetStyle(ControlStyles.UserPaint, true);

			initialize3D();
		}

		public void initialize3D() 
		{
			wrapper = new RendererWrapper();
			wrapper.initHWND(this.Handle, this.Width, this.Height);
		
			this.Paint +=new PaintEventHandler(Viewer_Paint);
		}

		public void setModel(MolecularModel mm) 
		{
			wrapper.setModel(mm);
			Invalidate();
		}

		public void spinY(float spinRadians) 
		{
			wrapper.spinY(spinRadians);
			Invalidate();
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if( components != null )
					components.Dispose();
			}
			base.Dispose( disposing );
		}

		#region Component Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			// 
			// Viewer
			// 
			this.BackColor = System.Drawing.Color.Teal;
			this.Name = "Viewer";
			this.Size = new System.Drawing.Size(296, 288);

		}
		#endregion

		private void Viewer_Paint(object sender, PaintEventArgs e)
		{
			DateTime startTime = System.DateTime.Now;

			// Calling this *twice* forces a buffer swap. 
			wrapper.renderScene();
			wrapper.renderScene();

			DateTime endTime = System.DateTime.Now;
			System.TimeSpan diff = endTime - startTime;
			Console.WriteLine("render time: " + diff.Milliseconds);

		}
	}
}
