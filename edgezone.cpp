#include "sierrachart.h"
SCDLLName("Edge Zone")


void PlotBars(SCStudyInterfaceRef sc, int index,  COLORREF color, SCSubgraphRef subgraph0, SCSubgraphRef subgraph1, float &barwidth)
{
    float CustomOpen =sc.Open[index];
    float CustomClose =sc.Close[index];
    s_UseTool ToolBody;
    ToolBody.Clear();
    // Create a shape tool
    ToolBody.ChartNumber = sc.ChartNumber;
    ToolBody.Color = color;
    ToolBody.LineWidth=barwidth;
    ToolBody.Region = 1; // Draw in the subgraph region
    ToolBody.BeginDateTime = sc.BaseDateTimeIn[index] ; 
    ToolBody.EndDateTime = sc.BaseDateTimeIn[index];  
    ToolBody.BeginValue = CustomOpen; 
    ToolBody.EndValue = CustomClose;    
    ToolBody.DrawingType=DRAWING_RECTANGLEHIGHLIGHT;
    ToolBody.AddMethod = UTAM_ADD_OR_ADJUST;
    sc.UseTool(ToolBody);
    sc.Subgraph[0][index] =ToolBody.BeginValue;  // Open
    sc.Subgraph[1][index] =ToolBody.EndValue; // Close
    sc.Subgraph[0].DataColor[index]=ToolBody.Color;
    sc.Subgraph[1].DataColor[index]=ToolBody.Color;
    sc.Subgraph[0].LineWidth=1;
    sc.Subgraph[1].LineWidth=1;
   
}

void PlotLine(SCStudyInterfaceRef sc, int index, COLORREF color, SCSubgraphRef subgraph3, SCSubgraphRef subgraph2)
{

    {
        // Create a shape tool
        float CustomHigh = sc.High[index];
        float CustomLow =sc.Low[index];
        s_UseTool ToolWick;
        ToolWick.Clear();
        ToolWick.ChartNumber = sc.ChartNumber;
        ToolWick.Color = color;
        ToolWick.LineWidth=1;
        ToolWick.Region = 1; // Draw in the subgraph region
        ToolWick.BeginDateTime = sc.BaseDateTimeIn[index]; 
        ToolWick.EndDateTime = sc.BaseDateTimeIn[index];  
        ToolWick.BeginValue =(sc.BaseData[SC_LAST][index] >sc.BaseData[SC_OPEN][index])? CustomLow :CustomHigh;    
        ToolWick.EndValue =(sc.BaseData[SC_LAST][index] >sc.BaseData[SC_OPEN][index])?  CustomHigh :CustomLow ;      
        ToolWick.DrawingType = DRAWING_LINE;
        ToolWick.AddMethod = UTAM_ADD_OR_ADJUST;
        sc.UseTool(ToolWick);
        sc.Subgraph[2][index] = ToolWick.BeginValue;  // Open
        sc.Subgraph[3][index] =ToolWick.EndValue; // Close
        sc.Subgraph[2].DataColor[index]=ToolWick.Color;
        sc.Subgraph[3].DataColor[index]=ToolWick.Color;
        sc.Subgraph[3].LineWidth=1;
        sc.Subgraph[3].LineWidth=1;
    }
}
SCSFExport scsf_NetVolumeCalculation(SCStudyInterfaceRef sc)
{
    float open=0.0f;
    float high=0.0f; 
    float low=0.0f ;
    float close=0.0f;
    // Define input parameters
    SCInputRef LengthHARSI = sc.Input[0];
    SCInputRef Smoothing = sc.Input[1];
    SCInputRef Source = sc.Input[2];
    SCInputRef LengthRSI = sc.Input[3];
    SCInputRef ModeRSI = sc.Input[4];
    SCInputRef UpperOB = sc.Input[5];
    SCInputRef UpperOBExtreme = sc.Input[6];
    SCInputRef LowerOS = sc.Input[7];
    SCInputRef LowerOSExtreme = sc.Input[8];
    SCInputRef Length = sc.Input[9];
    SCInputRef ShowZones = sc.Input[10];
    SCInputRef BarSize=sc.Input[11];
    //color define
    COLORREF extra_extreme_buy_color = RGB(255, 0, 60); // #ff003c
    COLORREF extra_extreme_sell_color =RGB(0, 30, 255); // #001eff
    COLORREF sea_zone_upbar_color = RGB(0, 224, 60); // #00e040
    COLORREF sea_zone_downbar_color = RGB(224, 26, 0); // #e01a00
    COLORREF frontier_buy_sweep_color = RGB(0, 0, 0); // #000000
    COLORREF frontier_sell_sweep_color = RGB(0, 0, 0); // #000000
    COLORREF extreme_sell_1_color = RGB(214, 0, 51); // #d60033
    COLORREF extreme_sell_2_color=RGB(214,0,51);
    COLORREF extreme_sell_3_color=RGB(255,255,0);
    COLORREF extreme_sell_4_color=RGB(255,255,255);
    COLORREF extreme_buy_1_color = RGB(255, 0, 255); // #d60033
    COLORREF extreme_buy_2_color=RGB(126,0,126);
    COLORREF extreme_buy_3_color=RGB(0,255,255);
    COLORREF extreme_buy_4_color=RGB(255,255,255);
    COLORREF frontier_sell_color=RGB(33,150,243);
    COLORREF frontier_buy_color=RGB(76,175,80);

    if (sc.SetDefaults)
        {
        
            sc.GraphName = "Edge Zone";
            sc.StudyDescription = "Edge Zone";
            sc.AutoLoop = 1;

            LengthHARSI.Name = "Length HARSI";
            LengthHARSI.SetFloat(14);
            LengthHARSI.SetFloatLimits(1, 100);

            Smoothing.Name = "Open Smoothing";
            Smoothing.SetFloat(1);
            Smoothing.SetFloatLimits(1, 100);

            Source.Name = "Source";
            Source.SetInputDataIndex(SC_HLC_AVG);

            LengthRSI.Name = "Length RSI";
            LengthRSI.SetFloat(7);
            LengthRSI.SetFloatLimits(1, INT_MAX);

            ModeRSI.Name = "Smoothed Mode RSI?";
            ModeRSI.SetYesNo(1);

            UpperOB.Name = "OB";
            UpperOB.SetFloat(20);
            UpperOB.SetFloatLimits(1, 50);

            UpperOBExtreme.Name = "OB Extreme";
            UpperOBExtreme.SetFloat(30);
            UpperOBExtreme.SetFloatLimits(1, 50);

            LowerOS.Name = "OS";
            LowerOS.SetFloat(-20);
            LowerOS.SetFloatLimits(-50, -1);

            LowerOSExtreme.Name = "OS Extreme";
            LowerOSExtreme.SetFloat(-30);
            LowerOSExtreme.SetFloatLimits(-50, -1);

            Length.Name = "Length";
            Length.SetFloat(20);
            Length.SetFloatLimits(1, INT_MAX);

            BarSize.Name="Bar Size";
            BarSize.SetFloat(6);
            BarSize.SetFloatLimits(1,INT_MAX);

            // // Configure the subgraphs for open and close lines
            sc.Subgraph[0].Name = "Open";
            sc.Subgraph[0].DrawStyle = DRAWSTYLE_HIDDEN;
            sc.Subgraph[1].Name = "Close";
            sc.Subgraph[1].DrawStyle = DRAWSTYLE_HIDDEN;
            sc.Subgraph[2].Name = "High";
            sc.Subgraph[2].DrawStyle = DRAWSTYLE_HIDDEN;
            sc.Subgraph[3].Name = "Low";
            sc.Subgraph[3].DrawStyle = DRAWSTYLE_HIDDEN;
            sc.Subgraph[10].Name="UpperExtremline";
            sc.Subgraph[10].DrawStyle=DRAWSTYLE_DASH;
            sc.Subgraph[11].Name="Uppeline";
            sc.Subgraph[11].DrawStyle=DRAWSTYLE_DASH;
            sc.Subgraph[12].Name="Lowline";
            sc.Subgraph[12].DrawStyle=DRAWSTYLE_DASH;
            sc.Subgraph[13].Name="LowExtremline";
            sc.Subgraph[13].DrawStyle=DRAWSTYLE_DASH;

            return;
        }

    float smothingget=Smoothing.GetFloat();
    float candleBarSize=BarSize.GetFloat();

    // Subgraphs
    SCFloatArrayRef RSI_HA_Open = sc.Subgraph[12].Data;
    SCFloatArrayRef RSI_HA_High = sc.Subgraph[13].Data;
    SCFloatArrayRef RSI_HA_Low =sc.Subgraph[14].Data;
    SCFloatArrayRef RSI_HA_Close =sc.Subgraph[15].Data;

    // RSI calculation for Heikin-Ashi data
    SCFloatArrayRef HA_Close=sc.Subgraph[6].Data;
    SCFloatArrayRef HA_Open=sc.Subgraph[7].Data;
    SCFloatArrayRef HA_High= sc.Subgraph[8].Data;
    SCFloatArrayRef HA_Low= sc.Subgraph[9].Data;
    HA_Open[sc.Index] = (sc.Open[sc.Index ] + sc.Close[sc.Index]) / 2.0f;
    HA_Close[sc.Index] = (sc.Open[sc.Index] + sc.High[sc.Index] + sc.Low[sc.Index] + sc.Close[sc.Index]) / 4.0f;
    HA_High[sc.Index] = max(sc.High[sc.Index], max(HA_Open[sc.Index], HA_Close[sc.Index]));
    HA_Low[sc.Index] = min(sc.Low[sc.Index], min(HA_Open[sc.Index], HA_Close[sc.Index]));

    SCSubgraphRef RSI_Close=sc.Subgraph[16];
    SCSubgraphRef RSI_Open=sc.Subgraph[17]; 
    SCSubgraphRef RSI_High=sc.Subgraph[18]; 
    SCSubgraphRef RSI_Low=sc.Subgraph[19];

    sc.RSI(HA_Close, RSI_Close,sc.Index, LengthHARSI.GetFloat());
    sc.RSI(HA_Open, RSI_Open,sc.Index, LengthHARSI.GetFloat());
    sc.RSI(HA_High, RSI_High,sc.Index, LengthHARSI.GetFloat());
    sc.RSI(HA_Low, RSI_Low,sc.Index, LengthHARSI.GetFloat());

    float closeRSI = RSI_Close[sc.Index] - 50;
    float openRSI = sc.Index > 0 ? RSI_Close[sc.Index - 1] - 50 : closeRSI;
    float highRSI_raw = RSI_High[sc.Index] - 50;
    float lowRSI_raw = RSI_Low[sc.Index] - 50;

    float highRSI = max(highRSI_raw, lowRSI_raw);
    float lowRSI = min(highRSI_raw, lowRSI_raw);

    float HA_Close_Value = (openRSI + highRSI + lowRSI + closeRSI) / 4;

    static float HA_Open_Value = 0;
    if (sc.Index == 0)
        HA_Open_Value = (openRSI + closeRSI) / 2;
    else
        HA_Open_Value = (HA_Open_Value * smothingget + HA_Close_Value) / (smothingget + 1);

    float HA_High_Value = max(highRSI, max(HA_Open_Value, HA_Close_Value));
    float HA_Low_Value = min(lowRSI, min(HA_Open_Value, HA_Close_Value));



    // Main zones
    bool extreme_sell = HA_Close_Value > UpperOBExtreme.GetFloat();
    bool frontier_sell = HA_Close_Value < UpperOBExtreme.GetFloat() && HA_Close_Value > UpperOB.GetFloat();
    bool sea_zone = HA_Close_Value < UpperOB.GetFloat() && HA_Close_Value > LowerOS.GetFloat();
    bool frontier_buy = HA_Close_Value < LowerOS.GetFloat() && HA_Close_Value > LowerOSExtreme.GetFloat();
    bool extreme_buy = HA_Close_Value < LowerOSExtreme.GetFloat();

    bool fr_buy_sweep = sea_zone && HA_High_Value > UpperOB.GetFloat();
    bool fr_sel_sweep = sea_zone && HA_Low_Value < LowerOS.GetFloat();
    bool extreme_sell_1 = HA_Close_Value > 27.5 && close <= 30;
    bool extreme_sell_2 = HA_Close_Value > 25 && close <= 27.5;
    bool extreme_sell_3 = HA_Close_Value > 22.5 && HA_Close_Value <= 25;
    bool extreme_sell_4 = HA_Close_Value > 20 && HA_Close_Value <= 22.5;
    bool extreme_buy_1 = HA_Close_Value < -27.5 && HA_Close_Value >= -30;
    bool extreme_buy_2 = HA_Close_Value < -25 && HA_Close_Value >= -27.5;
    bool extreme_buy_3 = HA_Close_Value < -22.5 && HA_Close_Value >= -25;
    bool extreme_buy_4 = HA_Close_Value < -20 && HA_Close_Value >= -22.5;
    // Extra extreme zones
    bool extra_extreme_sell = HA_Close_Value > 30;
    bool extra_extreme_buy = HA_Close_Value < -30;

    COLORREF candle_color = RGB(128, 128, 128); // Default to gray
    if (extra_extreme_sell)
            candle_color = extra_extreme_sell_color;
    else if (extreme_sell_1)
        candle_color = extreme_sell_1_color;
    else if (extreme_sell_2)
        candle_color = extreme_sell_2_color; // Orange
    else if (extreme_sell_3)
        candle_color = extreme_sell_3_color; // Yellow
    else if (extreme_sell_4)
        candle_color = extreme_sell_4_color; // White
    else if (fr_sel_sweep)
        candle_color = frontier_sell_sweep_color;
    else if (frontier_sell)
            candle_color=frontier_sell_color;
    else if (extra_extreme_buy)
        candle_color = extra_extreme_buy_color;
    else if (extreme_buy_1)
        candle_color = extreme_buy_1_color; // Fuchsia
    else if (extreme_buy_2)
        candle_color = extreme_buy_2_color; // Purple
    else if (extreme_buy_3)
        candle_color =extreme_buy_3_color; // Aqua
    else if (extreme_buy_4)
        candle_color = extreme_buy_4_color; // White
    else if (fr_buy_sweep)
        candle_color = frontier_buy_sweep_color;
    else if (frontier_buy)
        candle_color=frontier_buy_color;
    else  (sc.Close[sc.Index]>sc.Open[sc.Index]) ?
    candle_color=sea_zone_upbar_color: candle_color=sea_zone_downbar_color;

    PlotBars(sc, sc.Index, candle_color, sc.Subgraph[0],sc.Subgraph[1], candleBarSize);
    PlotLine(sc, sc.Index, candle_color, sc.Subgraph[2],sc.Subgraph[3] );
    
    // //Upper Extreme
    // s_UseTool ToolUpperExtreme;
    // ToolUpperExtreme.Clear();
    // ToolUpperExtreme.ChartNumber = sc.ChartNumber;
    // ToolUpperExtreme.DrawingType = DRAWING_LINE;
    // ToolUpperExtreme.AddMethod = UTAM_ADD_OR_ADJUST;
    // ToolUpperExtreme.BeginValue = UpperOBExtreme.GetFloat();;
    // ToolUpperExtreme.Color = RGB(0, 0, 255); // Blue
    // ToolUpperExtreme.LineWidth = 2;
    // ToolUpperExtreme.TransparencyLevel = 50;
    // ToolUpperExtreme.Text = "Upper Extreme";
    // ToolUpperExtreme.Region=1;
    // sc.Subgraph[10][sc.Index]=ToolUpperExtreme.BeginValue;
    // sc.Subgraph[10].DataColor[sc.Index]=ToolUpperExtreme.Color;
    // sc.UseTool(ToolUpperExtreme);

    // // Upper
    // s_UseTool ToolUpper;
    // ToolUpper.Clear();
    // ToolUpper.ChartNumber = sc.ChartNumber;
    // ToolUpper.DrawingType = DRAWING_HORIZONTALLINE;
    // ToolUpper.AddMethod = UTAM_ADD_OR_ADJUST;
    // ToolUpper.BeginValue = UpperOB.GetFloat();
    // ToolUpper.Color = RGB(0, 0, 200); // Darker Blue
    // ToolUpper.LineWidth = 1;
    // ToolUpper.TransparencyLevel = 70;
    // ToolUpper.Text = "Upper";
    // ToolUpper.Region=1;
    // sc.Subgraph[11][sc.Index]=ToolUpper.BeginValue;
    // sc.Subgraph[11].DataColor[sc.Index]=ToolUpper.Color;
    // sc.UseTool(ToolUpper);

    // // Lower
    // s_UseTool ToolLower;
    // ToolLower.Clear();
    // ToolLower.ChartNumber = sc.ChartNumber;
    // ToolLower.DrawingType = DRAWING_HORIZONTALLINE;
    // ToolLower.AddMethod = UTAM_ADD_OR_ADJUST;
    // ToolLower.BeginValue = LowerOS.GetFloat();
    // ToolLower.Color = RGB(200, 0, 0); // Darker Red
    // ToolLower.LineWidth = 1;
    // ToolLower.TransparencyLevel = 70;
    // ToolLower.Text = "Lower";
    // ToolLower.Region=1;
    // sc.Subgraph[12][sc.Index]=ToolLower.BeginValue;
    // sc.Subgraph[12].DataColor[sc.Index]=ToolLower.Color;
    // sc.UseTool(ToolLower);

    // // Lower Extreme
    // s_UseTool ToolLowerExtreme;
    // ToolLowerExtreme.Clear();
    // ToolLowerExtreme.ChartNumber = sc.ChartNumber;
    // ToolLowerExtreme.DrawingType = DRAWING_HORIZONTALLINE;
    // ToolLowerExtreme.AddMethod = UTAM_ADD_OR_ADJUST;
    // ToolLowerExtreme.BeginValue = LowerOSExtreme.GetFloat();
    // ToolLowerExtreme.Color = RGB(255, 0, 0); // Red
    // ToolLowerExtreme.LineWidth = 2;
    // ToolLowerExtreme.TransparencyLevel = 50;
    // ToolLowerExtreme.Text = "Lower Extreme";
    // ToolLowerExtreme.Region=1;
    // sc.Subgraph[13][sc.Index]=ToolLowerExtreme.BeginValue;
    // sc.Subgraph[13].DataColor[sc.Index]=ToolLowerExtreme.Color;
    // sc.UseTool(ToolLowerExtreme);

}