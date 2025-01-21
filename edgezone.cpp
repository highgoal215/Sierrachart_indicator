#include "sierrachart.h"
SCDLLName("Edge Zone")

// Function to calculate zero-centered RSI
float f_zrsi(SCFloatArrayRef source, float length, SCStudyInterfaceRef sc)
{   
    SCString message;
    SCSubgraphRef RSIArray = sc.Subgraph[5]; // Assuming Subgraph[5] is used for RSI values
    sc.RSI(source, RSIArray, sc.Index ,MOVAVGTYPE_SIMPLE, length);
    message.Format("RSIArray: %.5f", RSIArray[sc.Index]);
	sc.AddMessageToLog(message, 1);
    return RSIArray[sc.Index];

}
// Function to calculate Heikin Ashi RSI
void f_rsiHeikinAshi(float length, SCStudyInterfaceRef sc, float* open, float* high, float* low, float* close, float smothingget)
{
    SCString message;
    SCFloatArrayRef haOpen = sc.Subgraph[0].Data; // Assuming Subgraph[6] is used for Heikin Ashi open values
    SCFloatArrayRef haHigh = sc.Subgraph[1].Data; // Assuming Subgraph[7] is used for Heikin Ashi high values
    SCFloatArrayRef haLow = sc.Subgraph[2].Data; // Assuming Subgraph[8] is used for Heikin Ashi low values
    SCFloatArrayRef haClose = sc.Subgraph[3].Data; // Assuming Subgraph[9] is used for Heikin Ashi close values
    haOpen[sc.Index] = (sc.Open[sc.Index ] + sc.Close[sc.Index]) / 2;
    haClose[sc.Index] = (sc.Open[sc.Index] + sc.High[sc.Index] + sc.Low[sc.Index] + sc.Close[sc.Index]) / 4;
    haHigh[sc.Index] = max(sc.High[sc.Index], max(haOpen[sc.Index], haClose[sc.Index]));
    haLow[sc.Index] = min(sc.Low[sc.Index], min(haOpen[sc.Index], haClose[sc.Index]));
    float Open1 = sc.BaseData[SC_OPEN][sc.Index];
    message.Format("Haopen: %.5f", haOpen[sc.Index]);
	sc.AddMessageToLog(message, 1);
    float closeRSI = f_zrsi(haClose, length, sc);
    message.Format("Open1: %.5f", Open1);
	sc.AddMessageToLog(message, 1);
    float openRSI = closeRSI;
    float highRSI_raw = f_zrsi(haHigh, length, sc);
    float lowRSI_raw = f_zrsi(haLow, length, sc);
    float highRSI = max(highRSI_raw, lowRSI_raw);
    float lowRSI = min(highRSI_raw, lowRSI_raw);
    *close = (openRSI + highRSI + lowRSI + closeRSI) / 4;

    static float prev_open = 0;
    if (sc.Index == 0)
    {
        *open = (openRSI + closeRSI) / 2;
    }
    else
    {
        *open = (prev_open * smothingget+ (*close)) / (smothingget + 1);
    }
    prev_open = *open;
    *high = max(highRSI, max((*open), (*close)));
    *low = min(lowRSI, min((*open), (*close)));

}
void PlotBars(SCStudyInterfaceRef sc, int index, bool condition, COLORREF color, SCSubgraphRef subgraph , float &barwidth)
{
    SCString message; // Missing semicolon added here

    if (condition)
    {
        // Create a shape tool
        s_UseTool tool;
        tool.BeginDateTime = sc.BaseDateTimeIn[index]; // Use BaseDateTimeIn for time
        tool.EndDateTime = sc.BaseDateTimeIn[index]; // Same for end time
        tool.ChartNumber = sc.ChartNumber;
        tool.DrawingType = DRAWING_RETRACEMENT; // Example of a shape type
        // tool.MarkerType=MARKER_SQUARE;

        tool.BeginValue = sc.Open[index]; // Use the close price for the Y value
        tool.EndValue=sc.Close[index];
        tool.AddMethod = UTAM_ADD_OR_ADJUST;
        tool.Color = color;
        tool.LineWidth = barwidth;
        tool.Region =0;
        // Plot the shape at the specified index
        // sc.Subgraph[10].DataColor[sc.Index] = tool.Color;
        // sc.Subgraph[10].DrawStyle = tool.DrawingType;
        // sc.Subgraph[10][sc.Index] = tool.BeginValue;
        sc.UseTool(tool);
    }
}
void PlotLine(SCStudyInterfaceRef sc, int index, bool condition, COLORREF color, SCSubgraphRef subgraph)
{
    SCString message; // Missing semicolon added here

    if (condition)
    {
        // Create a shape tool
        s_UseTool tool;
        tool.BeginDateTime = sc.BaseDateTimeIn[index]; // Use BaseDateTimeIn for time
        tool.EndDateTime = sc.BaseDateTimeIn[index]; // Same for end time
        tool.ChartNumber = sc.ChartNumber;
        tool.DrawingType = DRAWING_RETRACEMENT; // Example of a shape type
        tool.BeginValue = sc.High[index]; // Use the close price for the Y value
        tool.EndValue=sc.Low[index];
        tool.AddMethod = UTAM_ADD_OR_ADJUST;
        tool.Color = color;
        tool.LineWidth = 1;
        tool.Region = 0;
        // Plot the shape at the specified index
        // sc.Subgraph[10].DataColor[sc.Index] = tool.Color;
        // sc.Subgraph[10].DrawStyle = tool.DrawingType;
        // sc.Subgraph[10][sc.Index] = tool.BeginValue;
        sc.UseTool(tool);
    }
}
SCSFExport scsf_NetVolumeCalculation(SCStudyInterfaceRef sc)
{
    SCString message;
   
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
    COLORREF extreme_buy_2_color=RGB(128,0,128);
    COLORREF extreme_buy_3_color=RGB(0,255,255);
    COLORREF extreme_buy_4_color=RGB(255,255,255);
    COLORREF frontier_sell_color=RGB(33,150,243);
    COLORREF frontier_buy_color=RGB(76,175,80);


    // Set default input values
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
        Source.SetInputDataIndex(SC_OHLC);

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
        BarSize.SetFloat(12);
        BarSize.SetFloatLimits(1,INT_MAX);

//   // Subgraphs for candle properties
//         sc.Subgraph[0].Name = "Open";
//         sc.Subgraph[1].Name = "High";
//         sc.Subgraph[2].Name = "Low";
//         sc.Subgraph[3].Name = "Close";                  // Line width for candle borders

  
        return;
    }
    float smothingget=Smoothing.GetFloat();
    float candleBarSize=BarSize.GetFloat();
    float open=0.0f;
    float high=0.0f; 
    float low=0.0f ;
    float close=0.0f;
    f_rsiHeikinAshi(LengthHARSI.GetFloat(), sc, &open, &high, &low, &close , smothingget);

       // Main zones
    bool extreme_sell = close > UpperOBExtreme.GetFloat();
    bool frontier_sell = close < UpperOBExtreme.GetFloat() && close > UpperOB.GetFloat();
    bool sea_zone = close < UpperOB.GetFloat() && close > LowerOS.GetFloat();
    bool frontier_buy = close < LowerOS.GetFloat() && close > LowerOSExtreme.GetFloat();
    bool extreme_buy = close < LowerOSExtreme.GetFloat();

    bool fr_buy_sweep = sea_zone && high > UpperOB.GetFloat();
    bool fr_sel_sweep = sea_zone && low < LowerOS.GetFloat();
    bool extreme_sell_1 = close > 27.5 && close <= 30;
    bool extreme_sell_2 = close > 25 && close <= 27.5;
    bool extreme_sell_3 = close > 22.5 && close <= 25;
    bool extreme_sell_4 = close > 20 && close <= 22.5;
    bool extreme_buy_1 = close < -27.5 && close >= -30;
    bool extreme_buy_2 = close < -25 && close >= -27.5;
    bool extreme_buy_3 = close < -22.5 && close >= -25;
    bool extreme_buy_4 = close < -20 && close >= -22.5;

    // Extra extreme zones
    bool extra_extreme_sell = close > 30;
    bool extra_extreme_buy = close < -30;
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

    PlotLine(sc, sc.Index, extra_extreme_sell, extra_extreme_sell_color, sc.Subgraph[10]);
    PlotLine(sc, sc.Index, extreme_sell_1, extreme_sell_1_color, sc.Subgraph[10]);
    PlotLine(sc, sc.Index, extreme_sell_2, extreme_sell_2_color, sc.Subgraph[10]);
    PlotLine(sc, sc.Index, extreme_sell_3, extreme_sell_3_color, sc.Subgraph[10]);
    PlotLine(sc, sc.Index, extreme_sell_4, extreme_sell_4_color, sc.Subgraph[10]);
    PlotLine(sc, sc.Index, fr_sel_sweep, frontier_sell_sweep_color, sc.Subgraph[10]);
    PlotLine(sc, sc.Index, frontier_sell, frontier_sell_color, sc.Subgraph[10]);
    // Plotting shapes for Extreme Buy Zones
    PlotLine(sc, sc.Index, extra_extreme_buy, extra_extreme_buy_color, sc.Subgraph[10]);
    PlotLine(sc, sc.Index, extreme_buy_1, extreme_buy_1_color, sc.Subgraph[10]);
    PlotLine(sc, sc.Index, extreme_buy_2, extreme_buy_2_color, sc.Subgraph[10]);
    PlotLine(sc, sc.Index, extreme_buy_3, extreme_buy_3_color, sc.Subgraph[10]);
    PlotLine(sc, sc.Index, extreme_buy_4, extreme_buy_4_color , sc.Subgraph[10]);
    PlotLine(sc, sc.Index, frontier_buy, frontier_buy_color, sc.Subgraph[10]);
    PlotLine(sc,sc.Index ,fr_buy_sweep ,candle_color,sc.Subgraph[10]);
    PlotLine(sc,sc.Index ,(sc.Close[sc.Index]>sc.Open[sc.Index]) ,sea_zone_upbar_color,sc.Subgraph[10]);
    PlotLine(sc,sc.Index ,(sc.Close[sc.Index]<=sc.Open[sc.Index]) ,candle_color,sc.Subgraph[10]);

    PlotBars(sc, sc.Index, extra_extreme_sell, extra_extreme_sell_color, sc.Subgraph[10], candleBarSize);
    PlotBars(sc, sc.Index, extreme_sell_1, extreme_sell_1_color, sc.Subgraph[10], candleBarSize);
    PlotBars(sc, sc.Index, extreme_sell_2, extreme_sell_2_color, sc.Subgraph[10] , candleBarSize);
    PlotBars(sc, sc.Index, extreme_sell_3, extreme_sell_3_color, sc.Subgraph[10], candleBarSize);
    PlotBars(sc, sc.Index, extreme_sell_4, extreme_sell_4_color, sc.Subgraph[10], candleBarSize);
    PlotBars(sc, sc.Index, fr_sel_sweep, frontier_sell_sweep_color, sc.Subgraph[10], candleBarSize);
    PlotBars(sc, sc.Index, frontier_sell, frontier_sell_color, sc.Subgraph[10], candleBarSize);
    // Plotting shapes for Extreme Buy Zones
    PlotBars(sc, sc.Index, extra_extreme_buy, extra_extreme_buy_color, sc.Subgraph[10], candleBarSize);
    PlotBars(sc, sc.Index, extreme_buy_1, extreme_buy_1_color, sc.Subgraph[10], candleBarSize);
    PlotBars(sc, sc.Index, extreme_buy_2, extreme_buy_2_color, sc.Subgraph[10], candleBarSize);
    PlotBars(sc, sc.Index, extreme_buy_3, extreme_buy_3_color, sc.Subgraph[10], candleBarSize);
    PlotBars(sc, sc.Index, extreme_buy_4, extreme_buy_4_color , sc.Subgraph[10], candleBarSize);
    PlotBars(sc, sc.Index, frontier_buy, frontier_buy_color, sc.Subgraph[10], candleBarSize);
    PlotBars(sc,sc.Index ,fr_buy_sweep ,candle_color,sc.Subgraph[10], candleBarSize);
    PlotBars(sc,sc.Index ,(sc.Close[sc.Index]>sc.Open[sc.Index]) ,sea_zone_upbar_color,sc.Subgraph[10], candleBarSize);
    PlotBars(sc,sc.Index ,(sc.Close[sc.Index]<=sc.Open[sc.Index]) ,candle_color,sc.Subgraph[10], candleBarSize);
    
    s_UseTool hline_tool;

	hline_tool.Clear();
	hline_tool.ChartNumber= sc.ChartNumber;

	hline_tool.DrawingType= DRAWING_HORIZONTALLINE;

	hline_tool.BeginIndex= sc.Index; 
	hline_tool.BeginValue= UpperOBExtreme.GetFloat(); 
	hline_tool.Color= RGB(0,0,255); 
	hline_tool.LineStyle= LINESTYLE_SOLID; 
    hline_tool.Region=0;
	sc.UseTool(hline_tool);

	hline_tool.BeginIndex=  sc.Index; 
	hline_tool.BeginValue= UpperOB.GetFloat(); 
	hline_tool.Color= RGB(0,0,128);
    hline_tool.Region=0;
	sc.UseTool(hline_tool);

	hline_tool.BeginIndex=  sc.Index; 
	hline_tool.BeginValue= LowerOS.GetFloat(); 
	hline_tool.Color= RGB(255,0,0);
    hline_tool.Region=0;
	sc.UseTool(hline_tool);

	hline_tool.BeginIndex=  sc.Index; 
	hline_tool.BeginValue= LowerOSExtreme.GetFloat(); 
	hline_tool.Color= RGB(128,0,0);
    hline_tool.Region=0;
	sc.UseTool(hline_tool);
     message.Format("CurrentPrice==================>: %.5f", sc.Close[sc.Index]);
	sc.AddMessageToLog(message, 1);
    

}