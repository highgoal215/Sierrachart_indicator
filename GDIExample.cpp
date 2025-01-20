#include "sierrachart.h"

// SCDLLName("GDI Example")

// This file demonstrates the functionality to use the Windows Graphics Device
// Interface (GDI) with ACSIL to freely draw inside of the chart window

// Windows GDI documentation can be found here: 
// http://msdn.microsoft.com/en-nz/library/windows/desktop/dd145203%28v=vs.85%29.aspx

/*==========================================================================*/
// Drawing function declaration
void DrawToChart(HWND WindowHandle, HDC DeviceContext, SCStudyInterfaceRef sc); 


/*==========================================================================*/
SCSFExport scsf_DrawToChartExample(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Draw To Chart Example";
		sc.GraphRegion = 0;

		sc.AutoLoop = 0;
		return;
	}


	// This is where we specify the drawing function. This function will be called
	// when the study graph is drawn on the chart. We are placing this after
	// if (sc.SetDefaults). So in case the study DLL is unloaded and reloaded,
	// this will continue to be set to the correct address.
	sc.p_GDIFunction = DrawToChart;


}
/*==========================================================================*/

// This is the actual drawing function. This function is specified by the
// "sc.p_GDIFunction" member in the main study function above. This drawing
// function is called when Sierra Chart draws the study on the chart. This
// will only occur after there has been a call to the main "scsf_" study
// function which is defined above.

// This drawing function has access to the ACSIL "sc." structure.
// However, any changes to the variable members will have no effect.

void DrawToChart(HWND WindowHandle, HDC DeviceContext, SCStudyInterfaceRef sc )
{
	// Yellow brush
	n_ACSIL::s_GraphicsBrush GraphicsBrush;
	GraphicsBrush.m_BrushType = n_ACSIL::s_GraphicsBrush::BRUSH_TYPE_SOLID;
	GraphicsBrush.m_BrushColor.SetRGB(255, 255, 0);

	sc.Graphics.SetBrush(GraphicsBrush);

	n_ACSIL::s_GraphicsPen GraphicsPenForRectangle;
	GraphicsPenForRectangle.m_PenColor.SetColorValue(COLOR_BLUE);
	GraphicsPenForRectangle.m_PenStyle = n_ACSIL::s_GraphicsPen::e_PenStyle::PEN_STYLE_SOLID;
	GraphicsPenForRectangle.m_Width = 5;

	sc.Graphics.SetPen(GraphicsPenForRectangle);

	//Draw a rectangle at the top left of the chart
	sc.Graphics.DrawRectangle( sc.StudyRegionLeftCoordinate + 5, sc.StudyRegionTopCoordinate + 5, sc.StudyRegionLeftCoordinate + 200, sc.StudyRegionTopCoordinate + 200);

	// Hollow Rectangle
	sc.Graphics.ResetBrush();

	//Using existing set pen

	n_ACSIL::s_GraphicsBrush HollowBrush;
	HollowBrush.m_BrushType = n_ACSIL::s_GraphicsBrush::BRUSH_TYPE_STOCK;
	HollowBrush.m_BrushStockType = NULL_BRUSH;

	sc.Graphics.SetBrush(HollowBrush);

	sc.Graphics.DrawRectangle(sc.StudyRegionLeftCoordinate + 300, sc.StudyRegionTopCoordinate + 300, sc.StudyRegionLeftCoordinate + 500, sc.StudyRegionTopCoordinate + 500);

	//int RightCoordinate = sc.StudyRegionRightCoordinate;


	sc.Graphics.SetTextAlign(TA_NOUPDATECP);

	n_ACSIL::s_GraphicsFont GraphicsFont;
	GraphicsFont.m_Height = 16;
	GraphicsFont.m_Weight = FW_BOLD;
	sc.Graphics.SetTextFont(GraphicsFont);

	n_ACSIL::s_GraphicsColor GraphicsColor;
	GraphicsColor.SetRGB(64, 128, 0);

	sc.Graphics.SetBackgroundColor(GraphicsColor);

	sc.Graphics.DrawTextAt("Hello. Sierra Chart is the best.", 350, 200);

	// Draw a line
	
	// Cyan Pen
	n_ACSIL::s_GraphicsPen GraphicsPen;
	GraphicsPen.m_PenColor.SetColorValue(COLOR_CYAN);
	GraphicsPen.m_PenStyle = n_ACSIL::s_GraphicsPen::e_PenStyle::PEN_STYLE_SOLID;
	GraphicsPen.m_Width = 5;

	sc.Graphics.SetPen(GraphicsPen);
	
	sc.Graphics.MoveTo(0, 0);
	sc.Graphics.LineTo(300, 300);

	return;
}

/*==========================================================================*/

