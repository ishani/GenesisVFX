using System;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Windows.Forms;

using Asphalt.Controls;
using Asphalt.Controls.Theme;

using PaintDotNet.Effects;

namespace GenesisVFX
{
    public class AsphaltEffectConfigDialog : EffectConfigDialog, IPropertyEvents, IHasBackgroundShader
    {
        #region Properties_Shading

        private Shader _PanelShader = null;
        private Shader _EdgeShader  = null;

        [Browsable(true), EditorBrowsable(EditorBrowsableState.Always), Category("Asphalt.Panel")]
        public Shader PanelShader
        {
            get => _PanelShader;
            set => Prop.Exchange(ref _PanelShader, value, this);
        }

        [Browsable(true), EditorBrowsable(EditorBrowsableState.Always), Category("Asphalt.Panel")]
        public Shader EdgeShader
        {
            get => _EdgeShader;
            set => Prop.Exchange(ref _EdgeShader, value, this);
        }

        void IPropertyEvents.OnChange(object sender, PropertyChangedEventArgs e)
        {
            Invalidate();
        }

        #endregion


        public AsphaltEffectConfigDialog()
        {
            SetStyle(
                ControlStyles.AllPaintingInWmPaint |
                ControlStyles.OptimizedDoubleBuffer |
                ControlStyles.ResizeRedraw |
                ControlStyles.UserPaint, true );

            FormBorderStyle = FormBorderStyle.None;
            StartPosition   = FormStartPosition.CenterScreen;
            Padding         = new Padding( 4 );

            PanelShader = new Shader(Scheme.Base, Pigment.Primary, Shade.Default);
            EdgeShader  = new Shader(Scheme.Base, Pigment.Analogous, Shade.Darker);
        }

        protected override CreateParams CreateParams
        {
            get {
                CreateParams cp = base.CreateParams;
                if ( !DesignMode )
                {
                    cp.ExStyle |= 0x02000000;  // WS_EX_COMPOSITED
                }
                return cp;
            }
        }

        Shader IHasBackgroundShader.GetCurrentBackgroundShader()
        {
            return PanelShader;
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);

            if ( !DesignMode )
                AsphaltForm.ForwardMouseDownToNonClient(Handle);
        }

        protected override void OnPaintBackground( PaintEventArgs e )
        {
            try // without ControlStyles.AllPaintingInWmPaint, we need our own error handling
            {
                var background      = Current.Instance.SolidBrush( _PanelShader );
                Color backgroundD   = Current.Instance.Color( _EdgeShader );

                using ( var borderPen = new Pen( backgroundD, 6 ) )
                {
                    e.Graphics.FillRectangle( background, 0, 0, Width, Height );
                    e.Graphics.DrawRectangle( borderPen,  0, 0, Width, Height );
                }
            }
            catch ( Exception ex )
            {
                Debug.WriteLine( ex );
                Invalidate();
            }
        }
    }
}
