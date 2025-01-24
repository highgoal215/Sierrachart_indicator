#include "sierrachart.h"

SCDLLName("Horizontal Lines with UseTool");

SCSFExport scsf_HorizontalLinesWithUseTool(SCStudyInterfaceRef sc) {
    if (sc.SetDefaults) {
        sc.GraphName = "Horizontal Lines with UseTool";
        sc.StudyDescription = "Plots horizontal lines for given thresholds using sc.UseTool.";
        sc.AutoLoop = 0; // Draw lines only once unless updated
        sc.Subgraph[0].Name="UpperExtremline";
        sc.Subgraph[0].DrawStyle=DRAWSTYLE_DASH;
        sc.Subgraph[1].Name="Uppeline";
        sc.Subgraph[1].DrawStyle=DRAWSTYLE_DASH;
        sc.Subgraph[2].Name="Lowline";
        sc.Subgraph[2].DrawStyle=DRAWSTYLE_DASH;
        sc.Subgraph[3].Name="LowExtremline";
        sc.Subgraph[3].DrawStyle=DRAWSTYLE_DASH;

        return;
    }

    // Define your threshold levels
    float i_upperx = 105.0f;  // Upper Extreme
    float i_upper = 100.0f;   // Upper
    float i_lower = 95.0f;    // Lower
    float i_lowerx = 90.0f;   // Lower Extreme

    // Upper Extreme
    s_UseTool ToolUpperExtreme;
    ToolUpperExtreme.Clear();
    ToolUpperExtreme.ChartNumber = sc.ChartNumber;
    ToolUpperExtreme.DrawingType = DRAWING_LINE;
    ToolUpperExtreme.AddMethod = UTAM_ADD_OR_ADJUST;
    ToolUpperExtreme.BeginValue = i_upperx;
    ToolUpperExtreme.Color = RGB(0, 0, 255); // Blue
    ToolUpperExtreme.LineWidth = 2;
    ToolUpperExtreme.TransparencyLevel = 50;
    ToolUpperExtreme.Text = "Upper Extreme";
    ToolUpperExtreme.Region=1;
    sc.Subgraph[0][sc.Index]=ToolUpperExtreme.BeginValue;
    sc.Subgraph[0].DataColor[sc.Index]=ToolUpperExtreme.Color;
    sc.UseTool(ToolUpperExtreme);

    // Upper
    s_UseTool ToolUpper;
    ToolUpper.Clear();
    ToolUpper.ChartNumber = sc.ChartNumber;
    ToolUpper.DrawingType = DRAWING_HORIZONTALLINE;
    ToolUpper.AddMethod = UTAM_ADD_OR_ADJUST;
    ToolUpper.BeginValue = i_upper;
    ToolUpper.Color = RGB(0, 0, 200); // Darker Blue
    ToolUpper.LineWidth = 1;
    ToolUpper.TransparencyLevel = 70;
    ToolUpper.Text = "Upper";
    ToolUpper.Region=1;
    sc.Subgraph[1][sc.Index]=ToolUpper.BeginValue;
    sc.Subgraph[1].DataColor[sc.Index]=ToolUpper.Color;
    sc.UseTool(ToolUpper);

    // Lower
    s_UseTool ToolLower;
    ToolLower.Clear();
    ToolLower.ChartNumber = sc.ChartNumber;
    ToolLower.DrawingType = DRAWING_HORIZONTALLINE;
    ToolLower.AddMethod = UTAM_ADD_OR_ADJUST;
    ToolLower.BeginValue = i_lower;
    ToolLower.Color = RGB(200, 0, 0); // Darker Red
    ToolLower.LineWidth = 1;
    ToolLower.TransparencyLevel = 70;
    ToolLower.Text = "Lower";
    ToolLower.Region=1;
    sc.Subgraph[2][sc.Index]=ToolLower.BeginValue;
    sc.Subgraph[2].DataColor[sc.Index]=ToolLower.Color;
    sc.UseTool(ToolLower);

    // Lower Extreme
    s_UseTool ToolLowerExtreme;
    ToolLowerExtreme.Clear();
    ToolLowerExtreme.ChartNumber = sc.ChartNumber;
    ToolLowerExtreme.DrawingType = DRAWING_HORIZONTALLINE;
    ToolLowerExtreme.AddMethod = UTAM_ADD_OR_ADJUST;
    ToolLowerExtreme.BeginValue = i_lowerx;
    ToolLowerExtreme.Color = RGB(255, 0, 0); // Red
    ToolLowerExtreme.LineWidth = 2;
    ToolLowerExtreme.TransparencyLevel = 50;
    ToolLowerExtreme.Text = "Lower Extreme";
    ToolLowerExtreme.Region=1;
    sc.Subgraph[3][sc.Index]=ToolLowerExtreme.BeginValue;
    sc.Subgraph[3].DataColor[sc.Index]=ToolLowerExtreme.Color;
    sc.UseTool(ToolLowerExtreme);
}
